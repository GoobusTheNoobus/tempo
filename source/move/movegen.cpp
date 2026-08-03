#include "move/movegen.hpp"
#include "bitboards/attacks.hpp"
#include "bitboards/bitboards.hpp"

#include <iostream>

namespace Tempo::MoveGen {

    namespace {
        void add(int& i, u16* arr, u16 m) {
            arr[i++] = m;
        }

        void extractPawn(int& i, u16* arr, u64 bb, int offset, Move::Type type) {
            while (bb) {
                int lsb = popLsb(bb);
                add(i, arr, Move::create(Square(lsb - offset), Square(lsb), type));
            }
        }

        void extractPawnPromo(int& i, u16* arr, u64 bb, int offset) {
            while (bb) {
                int lsb = popLsb(bb);
                add(i, arr, Move::create(Square(lsb - offset), Square(lsb), Move::PromoQ));
                add(i, arr, Move::create(Square(lsb - offset), Square(lsb), Move::PromoR));
                add(i, arr, Move::create(Square(lsb - offset), Square(lsb), Move::PromoB));
                add(i, arr, Move::create(Square(lsb - offset), Square(lsb), Move::PromoN));
            }
        }
    }

    int generatePseudoLegalMoves(const Position& pos, u16 moves[]) {
        int size = 0;
        Color us = pos.getSideToMove();
        Color them = opposite(us);
        bool isWhite = us == White;

        u64 pawns = pos.getBitboard(Pawn, us);
        u64 occ = pos.getBitboard(White) | pos.getBitboard(Black);
        u64 enemy = pos.getBitboard(them);

        u64 rank3FromBottom = isWhite ? Bitboards::RankBB[2] : Bitboards::RankBB[5];
        u64 rank8FromBottom = isWhite ? Bitboards::RankBB[7] : Bitboards::RankBB[0];

        int singlePushOffset = isWhite ? 8 : -8;
        int doublePushOffset = singlePushOffset * 2;
        int leftCaptureOffset = isWhite ? 7 : -9;
        int rightCaptureOffset = isWhite ? 9 : -7;

        u64 singlePushBb = (isWhite ? pawns << 8 : pawns >> 8) & ~occ;
        u64 doublePushBb = (isWhite ? (singlePushBb & rank3FromBottom) << 8 : (singlePushBb & rank3FromBottom) >> 8) & ~occ;
        u64 leftCaptureBb = (isWhite ? (pawns & ~Bitboards::FileBB[0]) << 7 : (pawns & ~Bitboards::FileBB[0]) >> 9) & enemy;
        u64 rightCaptureBb = (isWhite ? (pawns & ~Bitboards::FileBB[7]) << 9 : (pawns & ~Bitboards::FileBB[7]) >> 7) & enemy;

        u64 singlePushPromoBb = singlePushBb & rank8FromBottom;
        u64 singlePushNormalBb = singlePushBb & ~rank8FromBottom;
        u64 leftPromoBb = leftCaptureBb & rank8FromBottom;
        u64 leftNormalBb = leftCaptureBb & ~rank8FromBottom;
        u64 rightPromoBb = rightCaptureBb & rank8FromBottom;
        u64 rightNormalBb = rightCaptureBb & ~rank8FromBottom;

        extractPawn(size, moves, singlePushNormalBb, singlePushOffset, Move::Normal);
        extractPawn(size, moves, doublePushBb, doublePushOffset, Move::DoublePawnPush);
        extractPawn(size, moves, rightNormalBb, rightCaptureOffset, Move::Normal);
        extractPawn(size, moves, leftNormalBb, leftCaptureOffset, Move::Normal);

        extractPawnPromo(size, moves, singlePushPromoBb, singlePushOffset);
        extractPawnPromo(size, moves, leftPromoBb, leftCaptureOffset);
        extractPawnPromo(size, moves, rightPromoBb, rightCaptureOffset);

        Square ep = pos.getEnPassant();
        if (ep != NoSquare) {
            u64 epPawns = pawns & Attacks::pawnAttacks(ep, opposite(us));
            while (epPawns) {
                int lsb = popLsb(epPawns);
                add(size, moves, Move::create(Square(lsb), ep, Move::EnPassant));
            }
        }

        u64 knights = pos.getBitboard(Knight, us);
        while (knights) {
            Square from = Square(popLsb(knights));
            u64 attacks = Attacks::knightAttacks(from) & ~pos.getBitboard(us);
            while (attacks) {
                Square to = Square(popLsb(attacks));
                add(size, moves, Move::create(from, to, Move::Normal));
            }
        }

        u64 bishops = pos.getBitboard(Bishop, us);
        while (bishops) {
            Square from = Square(popLsb(bishops));
            u64 attacks = Attacks::bishopAttack(from, occ) & ~pos.getBitboard(us);
            while (attacks) {
                Square to = Square(popLsb(attacks));
                add(size, moves, Move::create(from, to, Move::Normal));
            }
        }

        u64 rooks = pos.getBitboard(Rook, us);
        while (rooks) {
            Square from = Square(popLsb(rooks));
            u64 attacks = Attacks::rookAttack(from, occ) & ~pos.getBitboard(us);
            while (attacks) {
                Square to = Square(popLsb(attacks));
                add(size, moves, Move::create(from, to, Move::Normal));
            }
        }

        u64 queens = pos.getBitboard(Queen, us);
        while (queens) {
            Square from = Square(popLsb(queens));
            u64 attacks = Attacks::queenAttack(from, occ) & ~pos.getBitboard(us);
            while (attacks) {
                Square to = Square(popLsb(attacks));
                add(size, moves, Move::create(from, to, Move::Normal));
            }
        }

        u64 king = pos.getBitboard(King, us);
        if (king) {
            Square from = Square(popLsb(king));
            u64 attacks = Attacks::kingAttacks(from) & ~pos.getBitboard(us);
            while (attacks) {
                Square to = Square(popLsb(attacks));
                add(size, moves, Move::create(from, to, Move::Normal));
            }
        }

        constexpr u64 WKCastleEmpty = Bitboards::SquareBB[F1] | Bitboards::SquareBB[G1];
        constexpr u64 WQCastleEmpty = Bitboards::SquareBB[D1] | Bitboards::SquareBB[C1] | Bitboards::SquareBB[B1];
        constexpr u64 BKCastleEmpty = Bitboards::SquareBB[F8] | Bitboards::SquareBB[G8];
        constexpr u64 BQCastleEmpty = Bitboards::SquareBB[D8] | Bitboards::SquareBB[C8] | Bitboards::SquareBB[B8];

        if (isWhite && !pos.isAttacked(E1, them)) {

            if (pos.hasCastlingRight(CastlingWK) && !(occ & WKCastleEmpty) && !pos.isAttacked(F1, them) && !pos.isAttacked(G1, them)) 
                add(size, moves, Move::create(E1, G1, Move::Castling));
            
            if (pos.hasCastlingRight(CastlingWQ) && !(occ & WQCastleEmpty) && !pos.isAttacked(D1, them) && !pos.isAttacked(C1, them)) 
                add(size, moves, Move::create(E1, C1, Move::Castling));
            
        } else if (!isWhite && !pos.isAttacked(E8, them)) {

            if (pos.hasCastlingRight(CastlingBK) && !(occ & BKCastleEmpty) && !pos.isAttacked(F8, them) && !pos.isAttacked(G8, them)) 
                add(size, moves, Move::create(E8, G8, Move::Castling));
            
            if (pos.hasCastlingRight(CastlingBQ) && !(occ & BQCastleEmpty) && !pos.isAttacked(D8, them) && !pos.isAttacked(C8, them)) 
                add(size, moves, Move::create(E8, C8, Move::Castling));
            
        }

        return size;
    }
}