#pragma once
#include "core/pieces.hpp"
#include "core/squares.hpp"
#include "core/types.hpp"


namespace Tempo::Zobrist {

    inline u64 PieceSquareKeys[PieceNB][SquareNB];
    inline u64 CastlingKeys[16];
    inline u64 EnPassantKeys[8];
    inline u64 SideKey;

    void init();
} // namespace Tempo::Zobrist