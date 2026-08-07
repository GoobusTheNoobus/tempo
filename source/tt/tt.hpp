#pragma once

#include "core/types.hpp"
#include "move/move.hpp"
#include <iostream>

namespace Tempo {

    struct TranspositionTable {
        enum EntryType : u8 { Exact, Lower, Upper };

        struct Entry {
            u64 key = 0;
            int score = 0;
            u16 bestMove;
            u8 depth = 0;
            EntryType flag = Exact;
        };

      private:
        inline static usize TableMB = 64;
        inline static usize EntryNB = (TableMB * 1024 * 1024) / sizeof(Entry);

        inline static Vector<Entry> data;

      public:
        static void write(const Entry& entry);
        static Entry* probe(u64 key);
        static void clear();

        static int hashfull();
        static void init();
        static void resize(usize mb);
    };
} // namespace Tempo