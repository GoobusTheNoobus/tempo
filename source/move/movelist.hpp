#pragma once

#include "board/position.hpp"
#include "move/move.hpp"
#include "move/movegen.hpp"


namespace Tempo {

    class MoveList {
      private:
        u16 moves[256];
        int scores[256];
        int size_ = 0;
        const Position& pos;

      public:
        inline MoveList(const Position& pos) : size_(MoveGen::generatePseudoLegalMoves(pos, moves)), pos(pos) {}

        inline u16 operator[](int i) {
            return moves[i];
        }

        inline const u16 operator[](int i) const {
            return moves[i];
        }

        inline int size() {
            return size_;
        }

        void calculateScores(const u16 specialMove);
        void calculateScores();

        inline bool next(int i) {
            if (i >= size_)
                return false;

            int highestScoreIndex = i;
            int highestScore = scores[i];

            for (int j = i; j < size_; ++j) {
                if (scores[j] > highestScore) {
                    highestScore = scores[j];
                    highestScoreIndex = j;
                }
            }

            std::swap(moves[i], moves[highestScoreIndex]);
            std::swap(scores[i], scores[highestScoreIndex]);

            return true;
        }
    };
} // namespace Tempo