#pragma once
#include "board/position.hpp"
#include "core/pieces.hpp"
#include "core/squares.hpp"

namespace Tempo::Attacks {

    bool isAttacked(const Position& pos, Square square, Color by);

    u64 knightAttacks(Square);
    u64 kingAttacks(Square);
    u64 pawnAttacks(Square, Color);
    u64 bishopAttack(Square, u64 occ);
    u64 rookAttack(Square, u64 occ);
    u64 queenAttack(Square, u64 occ);

    void init();
} // namespace Tempo::Attacks