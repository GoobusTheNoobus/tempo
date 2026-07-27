#pragma once
#include "position.hpp"

namespace Tempo::Attacks {
    
    bool is_attacked(const Position& pos, Square square, Color by);

    u64 knight_attacks(Square);
    u64 king_attacks(Square);
    u64 pawn_attacks(Square, Color);
    u64 bishop_attack(Square, u64 occ);
    u64 rook_attack(Square, u64 occ); 
    u64 queen_attack(Square, u64 occ);

    void init();
}
