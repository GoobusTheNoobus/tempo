#include "search/search.hpp"
#include "uci/uci.hpp"
#include "chess/move/movelist.hpp"
#include "search/tt/tt.hpp"
#include "search/timer.hpp"
#include "search/history.hpp"
#include "search/killer.hpp"
#include "eval/score.hpp"

#include <chrono>
#include <atomic>
#include <algorithm>
#include <cstring>

namespace Tempo::Search {

    namespace {

        int reduction_table[2][256][MaxSearchDepth];

        constexpr int AspirationWindow = 130;
        constexpr int MaxQSearchDepth = 15;
        constexpr int MinNMPDepth = 3;
        constexpr int NMPReduction = 2;

        inline bool is_noisy(const Position& pos, const u16 move) {
            return Move::type(move) >= Move::EnPassant || pos.get_piece_on(Move::dest(move)) != NoPiece;
        }

        inline void update_pv(SearchInfo& info, int ply, u16 move) {
            info.pv_table[ply][ply] = move;
            
            int child_length = std::max(info.pv_lengths[ply + 1], ply + 1);
            for (int i = ply + 1; i < child_length; ++i)
                info.pv_table[ply][i] = info.pv_table[ply + 1][i];
            
            info.pv_lengths[ply] = child_length;
        }
    }

    void init() {
        for (int m = 1; m < 256; ++m) {
            for (int d = 1; d < MaxSearchDepth; ++d) {
                reduction_table[false][m][d] = (int)(0.5 * std::log(m) * std::log(d) + 0.75);
                reduction_table[true][m][d]  = (int)(0.25 * std::log(m) * std::log(d) + 0.5);
            }
        }
    }

    constexpr int AspirationExpansion = 2;

    void start(Position pos, int max_depth, int movetime) {

        // History::clear();
        TranspositionTable::clear();
        Killer::clear();
        Timer::start(movetime);

        int previous_score = 0;
        u16 best_move = 0;

        SearchInfo info;
        for (int depth = 1; depth <= max_depth; ++depth) {

            bool log_currmove = Timer::elapsed() > 500;

            info.seldepth = 0;

            int delta = AspirationWindow;

            int alpha = previous_score - delta;
            int beta = previous_score + delta;

            int score;
            if (depth == 1) {
                score = search<RootNode>(info, pos, depth, 0, alpha, beta, false);
            }
            else {
                while (true) {
                    score = search<RootNode>(info, pos, depth, 0, alpha, beta, false);

                    if (Timer::should_stop_search()) break;

                    if (score <= alpha) {
                        alpha -= delta;
                        delta *= AspirationExpansion;
                        continue;
                    }

                    if (score >= beta) {
                        beta += delta;
                        delta *= AspirationExpansion;
                        continue;
                    }

                    break;
                }
            }

            if (Timer::should_stop_search()) break;

            previous_score = score;
            best_move = info.pv_table[0][0];

            UCI::info_depth(depth, info.seldepth, score, Timer::elapsed(), info.nodes_searched, info.pv_table[0], info.pv_lengths[0]);
        }

        std::cout << "bestmove " << Move::to_string(best_move) << std::endl;
    }

    template <NodeType NT>
    int search(SearchInfo& info, Position& pos, int depth, int plies_from_root, int alpha, int beta, bool allow_nmp) {
        
        ++info.nodes_searched;

        if (pos.is_repetition() || pos.is_rule_50()) return DrawScore;
        if (depth == 0 && NT != RootNode) {

            // Quiescence search
            // Determine the depth by how far we are from root.
            int qsearch_depth = plies_from_root * 2 + 2;
            return qsearch(info, pos, std::min(qsearch_depth, MaxQSearchDepth), plies_from_root, alpha, beta);
        }

        info.seldepth = std::max(info.seldepth, plies_from_root);
        info.pv_lengths[plies_from_root] = plies_from_root;

        if (Timer::should_stop_search()) return Timeout;

        // TT probe
        u64 key = pos.get_key();
        auto& bucket = TranspositionTable::probe(key);
        const TranspositionTable::Entry* best_entry = nullptr;
        int highest_depth = -1;

        for (int i = 0; i < TranspositionTable::BucketSize; ++i) {
            const TranspositionTable::Entry* current = &bucket.entries[i];

            if (current->key == key && highest_depth < current->depth) {
                highest_depth = current->depth;
                best_entry = current;
            }
        }

        if (best_entry && best_entry->depth >= depth) {

            if (NT == NonPVNode && best_entry->flag == TranspositionTable::Exact) {
                return best_entry->score;
            }

            else if (best_entry->flag == TranspositionTable::Lower) alpha = std::max(alpha, best_entry->score);
            else if (best_entry->flag == TranspositionTable::Upper) beta = std::min(beta, best_entry->score);

            if (NT == NonPVNode && alpha >= beta) {
                return best_entry->score;
            }
        }

        bool in_check = pos.is_in_check();
        int static_eval = pos.evaluate();

        if (NT == NonPVNode && !in_check && depth <= 3 && static_eval - 80 * depth >= beta) {
            return static_eval;
        }

        if (NT == NonPVNode && !in_check && allow_nmp && depth >= MinNMPDepth && static_eval >= beta && pos.has_non_pawn_material()) {

            int reduction = NMPReduction;

            pos.make_move(Move::NullMove);
            int null_score = -search<NonPVNode>(info, pos, depth - 1 - reduction, plies_from_root + 1, -beta, -beta + 1, false);
            pos.undo_move();

            if (null_score >= beta) {
                return beta;
            }
        }

        int best_score = NegativeInfinity;
        u16 best_move = Move::NullMove;
        int move_count = 0;
        int original_alpha = alpha;

        MoveList moves(pos);
        moves.calculate_scores(bucket);

        int i = 0;
        while (moves.next(i)) {
            u16 move = moves[i];
            ++i;

            bool noisy = is_noisy(pos, move);

            bool is_legal = pos.attempt_move(move);
            if (!is_legal) continue;

            ++move_count;

            if (NT == RootNode && Timer::elapsed() >= 1500) {
                UCI::info_depth(depth, Timer::elapsed(), info.nodes_searched, move, move_count);
            }

            // late move pruning
            if (NT == NonPVNode && move_count >= 12 && depth <= 3 && !in_check && !noisy) {
                pos.undo_move();
                continue;
            }

            int score;
            if (NT == NonPVNode && move_count >= 8 && depth >= 3 && !in_check) {
                int reduction = reduction_table[noisy][move_count][depth];

                score = -search<NonPVNode>(info, pos, depth - 1 - reduction, plies_from_root + 1, -alpha - 1, -alpha);

                if (score > alpha) {
                    score = -search<NonPVNode>(info, pos, depth - 1, plies_from_root + 1, -alpha - 1, -alpha);
                }

            } else if (NT == NonPVNode || move_count > 1) {
                score = -search<NonPVNode>(info, pos, depth - 1, plies_from_root + 1, -alpha - 1, -alpha);
            }

            if (NT != NonPVNode && (move_count == 1 || score > alpha)) {
                score = -search<PVNode>(info, pos, depth - 1, plies_from_root + 1, -beta, -alpha);
            }

            pos.undo_move();

            if (Timer::should_stop_search()) break;

            if (score > alpha) {
                alpha = score;

                if constexpr (NT != NonPVNode) {
                    info.pv_table[plies_from_root][plies_from_root] = move;

                    update_pv(info, plies_from_root, move);
                }
            }

            if (score > best_score) {
                best_score = score;
                best_move = move;
            }

            if (alpha >= beta) {
                if (!noisy) {
                    History::update(pos.get_side_to_move(), Move::from(move), Move::dest(move), std::min(300 * depth - 300, 2500));
                    Killer::add(plies_from_root, move);

                    // Loop all previously searched moves to penalise
                    for (int j = 0; j < i; ++j) {
                        u16 m = moves[j];
                        if (!is_noisy(pos, m))
                            History::update(pos.get_side_to_move(), Move::from(m), Move::dest(m), -std::min(300 * depth - 300, 2500));
                    }
                }

                break;
            }
        }

        if (move_count == 0) {

            if (pos.is_in_check())
                return -MateScore + plies_from_root + 1;

            return DrawScore;
        }

        TranspositionTable::EntryType store_flag;
        if (best_score <= original_alpha) store_flag = TranspositionTable::Upper;
        else if (best_score >= beta) store_flag = TranspositionTable::Lower;
        else store_flag = TranspositionTable::Exact;

        if (best_move != 0) {
            TranspositionTable::write(pos.get_key(), best_move, std::clamp(best_score, -KnownWin, KnownWin), depth, store_flag);
        }

        return best_score;
    }

    int qsearch(SearchInfo& info, Position& pos, int depth, int plies_from_root, int alpha, int beta) {
        ++info.nodes_searched;

        if (pos.is_repetition() || pos.is_rule_50()) return DrawScore;
        if (Timer::should_stop_search()) return Timeout;

        bool in_check = pos.is_in_check();
        int static_eval = pos.evaluate();

        if (depth <= 0 && !in_check) return static_eval;

        if (!in_check && static_eval >= beta) {
            return static_eval;
        }

        alpha = std::max(alpha, static_eval);

        info.seldepth = std::max(info.seldepth, plies_from_root);

        MoveList moves(pos);
        for (int i = 0; i < moves.size(); ++i) {
            u16 move = moves[i];

            bool search_move = in_check || is_noisy(pos, move);
            if (!search_move) continue;

            bool is_legal = pos.attempt_move(move);
            if (!is_legal) continue;

            int score = -qsearch(info, pos, depth - 1, plies_from_root + 1, -beta, -alpha);
            pos.undo_move();

            alpha = std::max(score, alpha);

            if (alpha >= beta) 
                return beta;
                
        }

        return alpha;
    }
}