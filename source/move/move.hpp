#pragma once
#include "core/types.hpp"
#include "core/squares.hpp"

namespace Tempo::Move {

    enum Type : u8 {
        Normal,
        DoublePawnPush, 
        Castling,
        EnPassant,
        PromoQ,
        PromoR,
        PromoB,
        PromoN,
        None
    };

    inline u16 create(Square from, Square dest, Type type) {
        return from | (dest << 6) | (type << 12);
    }

    inline Square from(const u16 move) { return Square(move & 0x3F); }
    inline Square dest(const u16 move) { return Square((move >> 6) & 0x3F); }
    inline Type type(const u16 move) { return Type(move >> 12); }

    constexpr char PromoCharacters[] = {'q', 'r', 'b', 'n'};

    inline String toString(const u16 move) {
        String moveStr = squareToString(from(move)) + squareToString(dest(move));

        if (type(move) >= PromoQ) {
            return moveStr + PromoCharacters[type(move) - PromoQ];
        }

        return moveStr;
    }

    constexpr u16 NullMove = 0;

}