#pragma once

#include "move/move.hpp"
#include "board/position.hpp"

namespace Tempo::MoveGen {
    int generatePseudoLegalMoves(const Position& pos, u16 moves[]);
}
