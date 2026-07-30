#pragma once

#include "chess/types.hpp"
#include "chess/board/position.hpp"
#include "search/timer.hpp"

namespace Tempo::Search {
    constexpr int MaxSearchDepth = 32;

    struct SearchInfo {
        u64 nodes_searched = 0;
        int seldepth = 0;

        // pv table
        u16 pv_table[MaxSearchDepth][MaxSearchDepth];
        int pv_lengths[MaxSearchDepth];
    };

    struct RootSearchResult {
        int score = 0;
        u16 move;
    };

    enum NodeType : u8 {
        RootNode,
        PVNode,
        NonPVNode
    };

    void start(Position pos, int depth, int movetime);
    inline void stop() { Timer::request_stop(); }

    template <NodeType NT>
    int search(SearchInfo& info, Position& pos, int depth, int plies_from_root, int alpha, int beta, bool allow_nmp = true);
    int qsearch(SearchInfo& info, Position& pos, int depth, int plies_from_root, int alpha, int beta);

    void init();
}