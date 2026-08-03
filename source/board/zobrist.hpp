#pragma once
#include "core/types.hpp"
#include "core/pieces.hpp"
#include "core/squares.hpp"

namespace Tempo::Zobrist {

    inline u64 PieceSquareKeys[PieceNB][SquareNB];
    inline u64 CastlingKeys[16];
    inline u64 EnPassantKeys[8];
    inline u64 SideKey;

    void init();
}