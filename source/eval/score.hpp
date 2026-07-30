#pragma once

namespace Tempo {

    constexpr int DrawScore = 0,
        MaxCentipawn = 10000,
        MinCentipawn = -10000,
        MateScore = 11000,
        Infinity = 11001,
        NegativeInfinity = -11001,
        Timeout = 11002,
        KnownWin = 6000;

    inline String score_string(int score) {
        if (std::abs(score) <= MaxCentipawn) return "cp " + std::to_string(score);

        int mate_dist = MateScore - std::abs(score);

        mate_dist = score > 0 ? mate_dist : -mate_dist;
        mate_dist = (int)std::ceil(mate_dist / 2.0);

        return "mate " + std::to_string(mate_dist);
    }
}