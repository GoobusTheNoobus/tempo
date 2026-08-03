#pragma once

#include "core/types.hpp"
#include "board/position.hpp"
#include "search/timer.hpp"

namespace Tempo::Search {
    constexpr int MaxSearchDepth = 32;

    struct SearchInfo {
        u64 nodesSearched = 0;
        int seldepth = 0;

        // pv table
        u16 pvTable[MaxSearchDepth][MaxSearchDepth]{};
        int pvLengths[MaxSearchDepth]{};
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
    inline void stop() { Timer::requestStop(); }

    template <NodeType NT>
    int search(SearchInfo& info, Position& pos, int depth, int pliesFromRoot, int alpha, int beta, bool allowNmp = true);
    int qsearch(SearchInfo& info, Position& pos, int depth, int pliesFromRoot, int alpha, int beta);

    void init();
}