#pragma once

#include "chess/move/move.hpp"
#include "chess/board/position.hpp"

namespace Tempo::MoveGen {
    int generatePseudoLegalMoves(const Position& pos, u16 moves[]);
}
