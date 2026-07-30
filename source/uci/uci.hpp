#pragma once

#include "chess/types.hpp"
#include "chess/move/move.hpp"
#include "search/tt/tt.hpp"

#include <iostream>
#include <vector>

namespace Tempo {
    namespace UCI {

        void infoDepth(int depth, int seldepth, int score, u64 elapsed, u64 totalNodes, const u16 pvArr[], int pvLen);
        void infoDepth(int depth, u64 elapsed, u64 totalNodes, const u16 currmove, int currmovenumber);
        void infoString(const String& msg);

        void loop();
    }
}