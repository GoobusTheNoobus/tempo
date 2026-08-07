#pragma once

#include "board/position.hpp"
#include "move/move.hpp"


namespace Tempo::MoveGen {
    int generatePseudoLegalMoves(const Position& pos, u16 moves[]);
}
