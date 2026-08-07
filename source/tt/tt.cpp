#include "tt/tt.hpp"
#include <cstring>

namespace Tempo {
    void TranspositionTable::clear() {
        std::memset(data.data(), 0, data.size() * sizeof(Entry));
    }

    int TranspositionTable::hashfull() {
        int hashfull = 0;
        for (int i = 0; i < 1000; ++i) {
            if (data[i].bestMove != 0)
                ++hashfull;
        }

        return hashfull;
    }

    void TranspositionTable::init() {
        data.resize(EntryNB);
    }

    void TranspositionTable::resize(usize mb) {
        TableMB = mb;
        EntryNB = (TableMB * 1024 * 1024) / sizeof(Entry);

        data.resize(EntryNB);
    }

    TranspositionTable::Entry* TranspositionTable::probe(u64 key) {
        int index = key % EntryNB;

        return data[index].key == key ? &data[index] : nullptr;
    }

    void TranspositionTable::write(const Entry& entry) {
        int index = entry.key % EntryNB;

        if (data[index].depth <= entry.depth || data[index].key != entry.key) {
            data[index] = entry;
        }
    }
} // namespace Tempo