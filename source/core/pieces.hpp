#pragma once
#include "core/types.hpp"

namespace Tempo {

    constexpr int ColorNB = 2, PieceTypeNB = 6, PieceNB = ColorNB * PieceTypeNB;

    enum Color : u8 { White, Black };

    enum PieceType : u8 { Pawn, Knight, Bishop, Rook, Queen, King };

    enum Piece : u8 {
        WhitePawn,
        WhiteKnight,
        WhiteBishop,
        WhiteRook,
        WhiteQueen,
        WhiteKing,
        BlackPawn,
        BlackKnight,
        BlackBishop,
        BlackRook,
        BlackQueen,
        BlackKing,
        NoPiece
    };

    inline Piece makePiece(PieceType pt, Color c) {
        return Piece(c * 6 + pt);
    }

    inline PieceType typeOf(Piece p) {
        return PieceType(p % 6);
    }

    inline Color colorOf(Piece p) {
        return Color(p / 6);
    }

    inline Color opposite(Color c) {
        return Color(c ^ 1);
    }

} // namespace Tempo