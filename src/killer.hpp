#pragma once
#include "types.hpp"
#include "search.hpp"
#include <cstring>

namespace Crystall::Search::Killer {
    inline u16 table[MaxSearchDepth][2];

    inline void clear() {
        std::memset(table, 0, sizeof(table));
    }

    inline void add(int plies_from_root, u16 move) {
        table[plies_from_root][1] = table[plies_from_root][0];
        table[plies_from_root][0] = move;
    }
}