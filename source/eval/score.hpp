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

    inline String scoreString(int score) {
        if (std::abs(score) <= MaxCentipawn) return "cp " + std::to_string(score);

        int mateDist = MateScore - std::abs(score);

        mateDist = score > 0 ? mateDist : -mateDist;
        mateDist = (int)std::ceil(mateDist / 2.0);

        return "mate " + std::to_string(mateDist);
    }
}