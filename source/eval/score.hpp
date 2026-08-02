#pragma once

namespace Tempo {

    inline constexpr int DrawScore              = 0;
    inline constexpr int MaxCentipawn           = 10000;
    inline constexpr int MinCentipawn           = -10000;
    inline constexpr int MateScore              = 11000;
    inline constexpr int Infinity               = 11001;
    inline constexpr int NegativeInfinity       = -11001;
    inline constexpr int Timeout                = 11002;
    inline constexpr int KnownWin               = 6000;

    inline String scoreString(int score) {
        if (std::abs(score) <= MaxCentipawn) return "cp " + std::to_string(score);

        int mateDist = MateScore - std::abs(score);

        mateDist = score > 0 ? mateDist : -mateDist;
        mateDist = (int)std::ceil(mateDist / 2.0);

        return "mate " + std::to_string(mateDist);
    }
}