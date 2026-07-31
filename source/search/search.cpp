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

        int reductionTable[2][256][MaxSearchDepth];

        constexpr int AspirationWindow = 130;
        constexpr int MaxQSearchDepth = 15;
        constexpr int MinNMPDepth = 3;
        constexpr int NMPReduction = 2;

        inline bool isNoisy(const Position& pos, const u16 move) {
            return Move::type(move) >= Move::EnPassant || pos.getPieceOn(Move::dest(move)) != NoPiece;
        }

        inline void updatePv(SearchInfo& info, int ply, u16 move) {
            info.pvTable[ply][ply] = move;
            
            int childLength = std::max(info.pvLengths[ply + 1], ply + 1);
            for (int i = ply + 1; i < childLength; ++i)
                info.pvTable[ply][i] = info.pvTable[ply + 1][i];
            
            info.pvLengths[ply] = childLength;
        }

        inline int getFutilityMargin(int depth) {
            return (depth - 1) * 100;
        }
    }

    void init() {
        for (int m = 1; m < 256; ++m) {
            for (int d = 1; d < MaxSearchDepth; ++d) {
                reductionTable[false][m][d] = (int)(0.5 * std::log(m) * std::log(d) + 0.75);
                reductionTable[true][m][d]  = (int)(0.25 * std::log(m) * std::log(d) + 0.5);
            }
        }
    }

    constexpr int AspirationExpansion = 2;

    void start(Position pos, int maxDepth, int movetime) {

        // History::clear();
        TranspositionTable::clear();
        Killer::clear();
        Timer::start(movetime);

        int previousScore = 0;
        u16 bestMove = 0;

        SearchInfo info;
        for (int depth = 1; depth <= maxDepth; ++depth) {

            bool logCurrmove = Timer::elapsed() > 500;

            info.seldepth = 0;

            int delta = AspirationWindow;

            int alpha = previousScore - delta;
            int beta = previousScore + delta;

            int score;
            if (depth == 1) {
                score = search<RootNode>(info, pos, depth, 0, alpha, beta, false);
            }
            else {
                while (true) {
                    score = search<RootNode>(info, pos, depth, 0, alpha, beta, false);

                    if (Timer::shouldStopSearch()) break;

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

            if (Timer::shouldStopSearch()) break;

            previousScore = score;
            bestMove = info.pvTable[0][0];

            UCI::infoDepth(depth, info.seldepth, score, Timer::elapsed(), info.nodesSearched, info.pvTable[0], info.pvLengths[0]);
        }

        std::cout << "bestmove " << Move::toString(bestMove) << std::endl;
    }

    template <NodeType NT>
    int search(SearchInfo& info, Position& pos, int depth, int pliesFromRoot, int alpha, int beta, bool allowNmp) {
        
        ++info.nodesSearched;
        info.pvLengths[pliesFromRoot] = pliesFromRoot;

        if (pos.isRepetition() || pos.isRule50()) return DrawScore;
        if (depth == 0 && NT != RootNode) {

            // quiescence search
            // determine the depth by how far we are from root.
            int qsearchDepth = pliesFromRoot * 2 + 2;
            return qsearch(info, pos, std::min(qsearchDepth, MaxQSearchDepth), pliesFromRoot, alpha, beta);
        }

        info.seldepth = std::max(info.seldepth, pliesFromRoot);

        if (Timer::shouldStopSearch()) return Timeout;

        // TT probe
        u64 key = pos.getKey();
        auto& bucket = TranspositionTable::probe(key);
        const TranspositionTable::Entry* bestEntry = nullptr;
        int highestDepth = -1;

        for (int i = 0; i < TranspositionTable::BucketSize; ++i) {
            const TranspositionTable::Entry* current = &bucket.entries[i];

            if (current->key == key && highestDepth < current->depth) {
                highestDepth = current->depth;
                bestEntry = current;
            }
        }

        u16 ttMove = 0;

        if (bestEntry && bestEntry->depth >= depth) {

            ttMove = bestEntry->bestMove;

            if (NT == NonPVNode && bestEntry->flag == TranspositionTable::Exact) {
                return bestEntry->score;
            }

            else if (bestEntry->flag == TranspositionTable::Lower) alpha = std::max(alpha, bestEntry->score);
            else if (bestEntry->flag == TranspositionTable::Upper) beta = std::min(beta, bestEntry->score);

            if (NT == NonPVNode && alpha >= beta) {
                return bestEntry->score;
            }
        }

        bool inCheck = pos.isInCheck();
        int staticEval = pos.evaluate();

        if (NT == NonPVNode && !inCheck && depth <= 3 && staticEval - 80 * depth >= beta) {
            return staticEval;
        }

        if (NT == NonPVNode && !inCheck && allowNmp && depth >= MinNMPDepth && staticEval >= beta && pos.hasNonPawnMaterial()) {

            int reduction = NMPReduction;

            pos.makeMove(Move::NullMove);
            int nullScore = -search<NonPVNode>(info, pos, depth - 1 - reduction, pliesFromRoot + 1, -beta, -beta + 1, false);
            pos.undoMove();

            if (nullScore >= beta) {
                return beta;
            }
        }

        int bestScore = NegativeInfinity;
        u16 bestMove = Move::NullMove;
        int moveCount = 0;
        int originalAlpha = alpha;

        MoveList moves(pos);
        moves.calculateScores(ttMove);

        int i = 0;
        while (moves.next(i)) {
            u16 move = moves[i];
            ++i;

            bool noisy = isNoisy(pos, move);

            bool isLegal = pos.attemptMove(move);
            if (!isLegal) continue;

            ++moveCount;

            // futility pruning
            if (!noisy && !inCheck && depth <= 2 && bestScore >= -MaxCentipawn && NT == NonPVNode && staticEval + getFutilityMargin(depth) <= alpha) {
                pos.undoMove();
                 continue;
            }

            if (NT == RootNode && Timer::elapsed() >= 1500) {
                UCI::infoDepth(depth, Timer::elapsed(), info.nodesSearched, move, moveCount);
            }

            // late move pruning
            if (NT == NonPVNode && moveCount >= 12 && depth <= 3 && !inCheck && !noisy) {
                pos.undoMove();
                continue;
            }

            int score;
            if (NT == NonPVNode && moveCount >= 8 && depth >= 3 && !inCheck) {
                int reduction = reductionTable[noisy][moveCount][depth];

                score = -search<NonPVNode>(info, pos, depth - 1 - reduction, pliesFromRoot + 1, -alpha - 1, -alpha);

                if (score > alpha) {
                    score = -search<NonPVNode>(info, pos, depth - 1, pliesFromRoot + 1, -alpha - 1, -alpha);
                }

            } else if (NT == NonPVNode || moveCount > 1) {
                score = -search<NonPVNode>(info, pos, depth - 1, pliesFromRoot + 1, -alpha - 1, -alpha);
            }

            if (NT != NonPVNode && (moveCount == 1 || score > alpha)) {
                score = -search<PVNode>(info, pos, depth - 1, pliesFromRoot + 1, -beta, -alpha);
            }

            pos.undoMove();

            if (Timer::shouldStopSearch()) break;

            if (score > alpha) {
                alpha = score;

                if constexpr (NT != NonPVNode) {
                    info.pvTable[pliesFromRoot][pliesFromRoot] = move;

                    updatePv(info, pliesFromRoot, move);
                }
            }

            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }

            // cutoff
            if (alpha >= beta) {
                if (!noisy) {
                    History::update(pos.getSideToMove(), Move::from(move), Move::dest(move), std::min(300 * depth - 300, 2500));
                    Killer::add(pliesFromRoot, move);

                    // Loop all previously searched moves to penalise
                    for (int j = 0; j < i; ++j) {
                        u16 m = moves[j];
                        if (!isNoisy(pos, m))
                            History::update(pos.getSideToMove(), Move::from(m), Move::dest(m), -std::min(300 * depth - 300, 2500));
                    }
                }

                break;
            }
        }

        if (moveCount == 0) {

            if (pos.isInCheck())
                return -MateScore + pliesFromRoot + 1;

            return DrawScore;
        }

        TranspositionTable::EntryType storeFlag;
        if (bestScore <= originalAlpha) storeFlag = TranspositionTable::Upper;
        else if (bestScore >= beta) storeFlag = TranspositionTable::Lower;
        else storeFlag = TranspositionTable::Exact;

        if (bestMove != 0) {
            TranspositionTable::write(pos.getKey(), bestMove, std::clamp(bestScore, -KnownWin, KnownWin), depth, storeFlag);
        }

        return bestScore;
    }

    int qsearch(SearchInfo& info, Position& pos, int depth, int pliesFromRoot, int alpha, int beta) {
        ++info.nodesSearched;

        if (pos.isRepetition() || pos.isRule50()) return DrawScore;
        if (Timer::shouldStopSearch()) return Timeout;

        bool inCheck = pos.isInCheck();
        int staticEval = pos.evaluate();

        if (depth <= 0 && !inCheck) return staticEval;

        if (!inCheck && staticEval >= beta) {
            return staticEval;
        }

        alpha = std::max(alpha, staticEval);

        info.seldepth = std::max(info.seldepth, pliesFromRoot);

        MoveList moves(pos);
        for (int i = 0; i < moves.size(); ++i) {
            u16 move = moves[i];

            bool searchMove = inCheck || isNoisy(pos, move);
            if (!searchMove) continue;

            bool isLegal = pos.attemptMove(move);
            if (!isLegal) continue;

            int score = -qsearch(info, pos, depth - 1, pliesFromRoot + 1, -beta, -alpha);
            pos.undoMove();

            alpha = std::max(score, alpha);

            if (alpha >= beta) 
                return beta;
                
        }

        return alpha;
    }
}