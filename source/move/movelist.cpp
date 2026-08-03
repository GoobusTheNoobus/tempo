#include "move/movelist.hpp"
#include "search/search.hpp"
#include "search/history.hpp"
#include "tt/tt.hpp"

namespace Tempo {

    namespace {

        constexpr int MVVLVATable[PieceTypeNB][PieceTypeNB] = {
            { 809000, 831000, 832000, 849000, 889000, 999000 },
            { 806800, 828800, 829800, 846800, 886800, 996800 },
            { 806700, 828700, 829700, 846700, 886700, 996700 },
            { 805000, 827000, 828000, 845000, 885000, 995000 },
            { 801000, 823000, 824000, 841000, 881000, 991000 },
            { 799000, 821000, 822000, 839000, 879000, 989000 }
        };

        constexpr int PromotionScoreTable[4] = { 790000, 750000, 732000, 731000 };
        
        int scoreMove(const Position& pos, const u16& move, const u16& specialMove) {
            if (move == specialMove) return 1200000;
            
            Square from = Move::from(move);
            Square dest = Move::dest(move);
            Move::Type flag = Move::type(move);

            if (pos.getPieceOn(dest) != NoPiece) {
                int mvvlvaScore = MVVLVATable[typeOf(pos.getPieceOn(from))][typeOf(pos.getPieceOn(dest))];
                return mvvlvaScore;
            }

            if (flag >= Move::PromoQ) {
                int promoScore = PromotionScoreTable[flag - Move::PromoQ];
                return promoScore;
            }

            int historyScore = Search::History::table[pos.getSideToMove()][from][dest];
            return historyScore;
        }

        int scoreMove(const Position& pos, const u16 move, const u16 specialMove, const u16 killer0, const u16 killer1) {
            if (move == specialMove) return 1200000;
            
            Square from = Move::from(move);
            Square dest = Move::dest(move);
            Move::Type flag = Move::type(move);

            if (pos.getPieceOn(dest) != NoPiece) {
                int mvvlvaScore = MVVLVATable[typeOf(pos.getPieceOn(from))][typeOf(pos.getPieceOn(dest))];
                return mvvlvaScore;
            }

            if (flag >= Move::PromoQ) {
                int promoScore = PromotionScoreTable[flag - Move::PromoQ];
                return promoScore;
            }

            if (move == killer0 || move == killer1) {
                return 600000;
            }

            int historyScore = Search::History::table[pos.getSideToMove()][from][dest];
            return historyScore;
        }
    }

    void MoveList::calculateScores(const u16 specialMove) {
        for (int i = 0; i < size_; ++i) {
            scores[i] = scoreMove(pos, moves[i], specialMove);
        }
    }

    void MoveList::calculateScores() {
        for (int i = 0; i < size_; ++i) {
            scores[i] = scoreMove(pos, moves[i], Move::NullMove);
        }
    }
}