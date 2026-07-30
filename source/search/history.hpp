#pragma once

#include "chess/types.hpp"
#include <cstring>
#include <algorithm>

namespace Tempo::Search {

    class History {
        
        public:

        static inline int table[ColorNB][SquareNB][SquareNB];

        static inline void clear() {
            std::memset(table, 0, sizeof(table));
        }

        static inline void update(Color c, Square f, Square d, int bonus) {
            int clamped = std::clamp(bonus, -500000, 500000);
            table[c][f][d] += clamped - table[c][f][d] * abs(clamped) / 500000;
        }

    };
    
}