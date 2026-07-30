#pragma once

#include "chess/types.hpp"
#include "chess/move/move.hpp"
#include "eval/eval.hpp"

#include <string>
#include <iostream>
#include <random>

namespace Tempo {

    constexpr const char* StartingPositionFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    constexpr int CastlingWK = 1, CastlingWQ = 2, CastlingBK = 4, CastlingBQ = 8;

    struct GameState {
        Square enPassantSquare = NoSquare;
        int castlingRights = 0;
        int rule50Clock = 0;
    };

    struct MoveUndoInfo {
        u64 key;
        int castlingRights;
        int rule50Clock;
        u16 move;
        Square enPassantSquare;
        Piece capturedPiece;
    };

    class Position {

    public:

        Position() = default;
        
        void parseFen(const String& fen);
        void setUpStartpos();
        String toString() const;

        inline Piece getPieceOn(Square s) const { return board[int(s)]; }
        inline u64 getBitboard(Color c) const { return colorBitboards[int(c)]; }
        inline u64 getBitboard(Piece p) const { return pieceBitboards[int(p)]; }
        inline u64 getBitboard(PieceType pt, Color c) const { return pieceBitboards[int(makePiece(pt, c))]; }
        inline const u64* getBitboards() const { return pieceBitboards; }

        inline Color getSideToMove() const { return sideToMove; }
        inline Square getEnPassant() const { return state.enPassantSquare; }
        inline bool hasCastlingRight(int mask) const { return state.castlingRights & mask; }
        inline bool isRule50() const { return state.rule50Clock >= 100; }
        inline u64 getKey() const { return hash; }

        bool isAttacked(Square, Color by) const;
        bool isInCheck(Color) const;
        bool isInCheck() const;

        void makeMove(const u16 move);
        void makeMove(const String&);
        bool attemptMove(const u16 move);
        void undoMove();

        bool hasNonPawnMaterial() const;

        int evaluate() const;
        bool isRepetition() const;

    private:

        void clear();
        void clearSquare(Square square);
        void placePiece(Square square, Piece piece);

        void pushMoveStacks(u64 key, u16 move, int castlingRights, int rule50Clock, Square enPassantSquare, Piece capturedPiece);
        MoveUndoInfo& popUndoInfo();

        Piece board[SquareNB];
        u64 pieceBitboards[PieceNB];
        u64 colorBitboards[ColorNB];
        u64 occupancy = 0;

        Color sideToMove;
        GameState state;
        u64 hash = 0;

        MoveUndoInfo moveUndoStack[1024];
        int ply = 0;

        Evaluation::TaperedScore psqtScores;

    };

    std::ostream& operator<<(std::ostream& os, const Position& pos);
}