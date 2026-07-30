#pragma once

#include <string>
#include <cstdint>
#include <cmath>

namespace Tempo {

    using u64 = uint64_t;
    using u32 = uint32_t;
    using u16 = uint16_t;
    using u8  = uint8_t;
    using i64 = int64_t;
    using i32 = int32_t;
    using i16 = int16_t;
    using i8  = int8_t;

    using usize = size_t;

    using String = std::string;
    using StringView = std::string_view;

    constexpr int FileNB = 8, RankNB = 8, SquareNB = FileNB * RankNB;
    enum Square : u8 {
        A1, B1, C1, D1, E1, F1, G1, H1,
        A2, B2, C2, D2, E2, F2, G2, H2,
        A3, B3, C3, D3, E3, F3, G3, H3,
        A4, B4, C4, D4, E4, F4, G4, H4,
        A5, B5, C5, D5, E5, F5, G5, H5,
        A6, B6, C6, D6, E6, F6, G6, H6,
        A7, B7, C7, D7, E7, F7, G7, H7,
        A8, B8, C8, D8, E8, F8, G8, H8,

        NoSquare
    };

    constexpr int ColorNB = 2, PieceTypeNB = 6, PieceNB = ColorNB * PieceTypeNB;
    enum Color : u8 { White, Black };
    enum PieceType : u8 { Pawn, Knight, Bishop, Rook, Queen, King };
    enum Piece : u8 {
        WhitePawn, WhiteKnight, WhiteBishop, WhiteRook, WhiteQueen, WhiteKing,
        BlackPawn, BlackKnight, BlackBishop, BlackRook, BlackQueen, BlackKing,
        NoPiece
    };

    inline Piece makePiece(PieceType pt, Color c) { return Piece(c * 6 + pt); }
    inline PieceType typeOf(Piece p) { return PieceType(p % 6); }
    inline Color colorOf(Piece p) { return Color(p / 6); }
    inline Color opposite(Color c) { return Color(c ^ 1); }

    inline Square makeSquare(int r, int f) { return Square(r * 8 + f); }
    inline int fileOf(Square s) { return s % 8; }
    inline int rankOf(Square s) { return s / 8; }

    inline Square makeSquare(const String& str) {

        char rc = str[1];
        char fc = str[0];

        int rank = rc - '1';
        int file = fc - 'a';

        if (unsigned(rank) > 7 || unsigned(file) > 7) return NoSquare;

        return makeSquare(rank, file);
    }

    inline String squareToString(Square square) {
        char rc = rankOf(square) + '1';
        char fc = fileOf(square) + 'a';

        return {fc, rc};
    }
}