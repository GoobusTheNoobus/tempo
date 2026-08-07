#pragma once
#include "core/types.hpp"
#include "search/search.hpp"
#include <cstring>

namespace Tempo::Search {

    class Killer {
      private:
        static inline u16 table[MaxSearchDepth][2];

      public:
        static inline void clear() {
            std::memset(table, 0, sizeof(table));
        }

        static inline void add(int pliesFromRoot, u16 move) {
            table[pliesFromRoot][1] = table[pliesFromRoot][0];
            table[pliesFromRoot][0] = move;
        }
    };

} // namespace Tempo::Search