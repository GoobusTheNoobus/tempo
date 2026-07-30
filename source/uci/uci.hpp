#pragma once

#include "chess/types.hpp"
#include "chess/move/move.hpp"
#include "search/tt/tt.hpp"

#include <iostream>
#include <vector>

namespace Tempo {
    namespace UCI {

        void info_depth(int depth, int seldepth, int score, u64 elapsed, u64 total_nodes, const u16 pv_arr[], int pv_len);
        void info_depth(int depth, u64 elapsed, u64 total_nodes, const u16 currmove, int currmovenumber);
        void info_string(const String& msg);

        void loop();
    }
}