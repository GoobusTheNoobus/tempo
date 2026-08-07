#include "board/position.hpp"
#include "bitboards/attacks.hpp"
#include "bitboards/bitboards.hpp"
#include "board/zobrist.hpp"
#include "eval/score.hpp"
#include "move/movelist.hpp"

#include <charconv>
#include <sstream>

namespace Tempo {
    constexpr const char* PieceCharacters = "PNBRQKpnbrqk";

    std::ostream& operator<<(std::ostream& os, const Position& pos) {
        os << pos.toString();
        return os;
    }

    void Position::setUpStartpos() {
        parseFen(StartingPositionFen);
    }

    void Position::parseFen(const String& fen) {
        clear();

        std::istringstream iss(fen);

        String fenBoardPart;
        if (!(iss >> fenBoardPart))
            return;

        int r = 7, f = 0;
        for (char c : fenBoardPart) {
            if (c == '/') {
                --r;
                f = 0;
                continue;
            }

            if (std::isdigit(static_cast<unsigned char>(c))) {
                f += c - '0';
                continue;
            }

            Piece p = NoPiece;

            for (int i = 0; i < 13; ++i) {
                if (PieceCharacters[i] == c) {
                    p = Piece(i);
                    break;
                }
            }

            placePiece(makeSquare(r, f), p);
            ++f;
        }

        String fenSidePart;
        if (!(iss >> fenSidePart))
            return;
        sideToMove = (fenSidePart == "w") ? White : Black;

        String fenCastlingPart;
        if (!(iss >> fenCastlingPart))
            return;
        state.castlingRights = 0;

        if (fenCastlingPart != "-") {
            for (char c : fenCastlingPart) {
                switch (c) {
                case 'K':
                    state.castlingRights |= CastlingWK;
                    break;
                case 'Q':
                    state.castlingRights |= CastlingWQ;
                    break;
                case 'k':
                    state.castlingRights |= CastlingBK;
                    break;
                case 'q':
                    state.castlingRights |= CastlingBQ;
                    break;
                }
            }
        }

        String fenEpPart;
        if (!(iss >> fenEpPart))
            return;

        if (fenEpPart == "-")
            state.enPassantSquare = NoSquare;
        else
            state.enPassantSquare = makeSquare(fenEpPart);

        String fenRule50Part;
        if (!(iss >> fenRule50Part))
            return;
        std::from_chars(fenRule50Part.data(), fenRule50Part.data() + fenRule50Part.size(), state.rule50Clock);

        if (sideToMove == Black)
            hash ^= Zobrist::SideKey;

        if (state.castlingRights != 0)
            hash ^= Zobrist::CastlingKeys[state.castlingRights];

        if (state.enPassantSquare != NoSquare)
            hash ^= Zobrist::EnPassantKeys[fileOf(state.enPassantSquare)];
    }

    String Position::toString() const {
        std::ostringstream oss;

        oss << '\n';
        for (int r = 7; r >= 0; --r) {
            oss << "  +---+---+---+---+---+---+---+---+\n";
            oss << (r + 1) << ' ';
            for (int f = 0; f < 8; ++f) {
                oss << "| ";

                Piece piece = getPieceOn(makeSquare(r, f));

                if (piece != NoPiece)
                    oss << PieceCharacters[piece];
                else
                    oss << ' ';

                oss << ' ';
            }
            oss << "|\n";
        }
        oss << "  +---+---+---+---+---+---+---+---+\n";
        oss << "    a   b   c   d   e   f   g   h  \n\n";

        return oss.str();
    }

    void Position::clear() {
        for (int i = 0; i < SquareNB; ++i)
            board[i] = NoPiece;
        for (int i = 0; i < PieceNB; ++i)
            pieceBitboards[i] = 0;
        for (int i = 0; i < ColorNB; ++i)
            colorBitboards[i] = 0;
        occupancy = 0;

        sideToMove = White;
        state.castlingRights = 0;
        state.enPassantSquare = NoSquare;
        state.rule50Clock = 0;
        ply = 0;
        hash = 0;
        psqtScores = {0, 0};
    }

    void Position::clearSquare(Square square) {
        if (getPieceOn(square) == NoPiece)
            return;

        Piece pieceAlreadyThere = getPieceOn(square);
        Color color = colorOf(pieceAlreadyThere);

        u64 mask = ~(1ULL << square);

        board[square] = NoPiece;

        pieceBitboards[pieceAlreadyThere] &= mask;
        colorBitboards[color] &= mask;
        occupancy &= mask;

        PieceType pt = typeOf(pieceAlreadyThere);
        if (color == White) {
            psqtScores.mgScore -= Evaluation::MGTables[pt][square ^ 56];
            psqtScores.egScore -= Evaluation::EGTables[pt][square ^ 56];
        } else {
            psqtScores.mgScore += Evaluation::MGTables[pt][square];
            psqtScores.egScore += Evaluation::EGTables[pt][square];
        }

        hash ^= Zobrist::PieceSquareKeys[pieceAlreadyThere][square];
    }

    void Position::placePiece(Square square, Piece piece) {
        if (piece == NoPiece) {
            clearSquare(square);
            return;
        }

        Color color = colorOf(piece);

        u64 mask = 1ULL << square;

        board[square] = piece;
        pieceBitboards[piece] |= mask;
        colorBitboards[color] |= mask;
        occupancy |= mask;

        PieceType pt = typeOf(piece);
        if (color == White) {
            psqtScores.mgScore += Evaluation::MGTables[pt][square ^ 56];
            psqtScores.egScore += Evaluation::EGTables[pt][square ^ 56];
        } else {
            psqtScores.mgScore -= Evaluation::MGTables[pt][square];
            psqtScores.egScore -= Evaluation::EGTables[pt][square];
        }

        hash ^= Zobrist::PieceSquareKeys[piece][square];
    }

    bool Position::isAttacked(Square square, Color by) const {
        return Attacks::isAttacked(*this, square, by);
    }

    bool Position::isInCheck(Color color) const {
        return isAttacked(Square(ctz(getBitboard(King, color))), opposite(color));
    }

    bool Position::isInCheck() const {
        return isInCheck(sideToMove);
    }

    void Position::pushMoveStacks(
        u64 key, u16 move, int castlingRights, int rule50Clock, Square enPassantSquare, Piece capturedPiece) {
        moveUndoStack[ply++] = {key, castlingRights, rule50Clock, move, enPassantSquare, capturedPiece};
    }

    MoveUndoInfo& Position::popUndoInfo() {
        return moveUndoStack[--ply];
    }

    void Position::makeMove(const u16 move) {
        Color us = sideToMove;
        bool isWhite = us == White;

        Square from = Move::from(move);
        Square dest = Move::dest(move);
        Move::Type flag = Move::type(move);

        Piece movingPiece = NoPiece;
        PieceType movingPt = Pawn;
        Piece capturedPiece = NoPiece;

        if (!(move == Move::NullMove)) {
            movingPiece = getPieceOn(from);
            movingPt = typeOf(movingPiece);
            capturedPiece = flag == Move::EnPassant ? makePiece(Pawn, opposite(us)) : getPieceOn(dest);
        }

        u64 hashBefore = hash;
        pushMoveStacks(hash, move, state.castlingRights, state.rule50Clock, state.enPassantSquare, capturedPiece);

        hash ^= Zobrist::SideKey;

        if (state.enPassantSquare != NoSquare)
            hash ^= Zobrist::EnPassantKeys[fileOf(state.enPassantSquare)];

        if (state.castlingRights != 0)
            hash ^= Zobrist::CastlingKeys[state.castlingRights];

        sideToMove = opposite(sideToMove);
        state.enPassantSquare = NoSquare;

        if (move == Move::NullMove) {
            return;
        }

        switch (flag) {
        case Move::Normal: {
            clearSquare(dest);
            clearSquare(from);
            placePiece(dest, movingPiece);
            break;
        }

        case Move::Castling: {
            bool kingSide = dest == G1 || dest == G8;

            Square rookFrom = isWhite ? (kingSide ? H1 : A1) : (kingSide ? H8 : A8);
            Square rookDest = isWhite ? (kingSide ? F1 : D1) : (kingSide ? F8 : D8);

            clearSquare(rookFrom);
            clearSquare(from);

            placePiece(dest, movingPiece);
            placePiece(rookDest, makePiece(Rook, us));
            break;
        }

        case Move::EnPassant: {
            Square captureSquare = isWhite ? Square(dest - 8) : Square(dest + 8);

            clearSquare(captureSquare);
            clearSquare(from);
            placePiece(dest, movingPiece);
            break;
        }

        case Move::DoublePawnPush: {
            state.enPassantSquare = isWhite ? Square(dest - 8) : Square(dest + 8);

            clearSquare(from);
            placePiece(dest, movingPiece);
            break;
        }

        default: {
            constexpr static PieceType PromoPieces[] = {Queen, Rook, Bishop, Knight};

            clearSquare(from);
            clearSquare(dest);
            placePiece(dest, makePiece(PromoPieces[flag - Move::PromoQ], us));
            break;
        }
        }

        if (from == A1 || dest == A1)
            state.castlingRights &= ~CastlingWQ;
        if (from == A8 || dest == A8)
            state.castlingRights &= ~CastlingBQ;
        if (from == H1 || dest == H1)
            state.castlingRights &= ~CastlingWK;
        if (from == H8 || dest == H8)
            state.castlingRights &= ~CastlingBK;

        if (from == E1)
            state.castlingRights &= ~(CastlingWK | CastlingWQ);
        else if (from == E8)
            state.castlingRights &= ~(CastlingBK | CastlingBQ);

        if (state.enPassantSquare != NoSquare)
            hash ^= Zobrist::EnPassantKeys[fileOf(state.enPassantSquare)];

        if (state.castlingRights != 0)
            hash ^= Zobrist::CastlingKeys[state.castlingRights];

        if (capturedPiece != NoPiece || movingPt == Pawn)
            state.rule50Clock = 0;
        else
            state.rule50Clock++;
    }

    void Position::makeMove(const String& moveStr) {
        MoveList list(*this);

        for (int i = 0; i < list.size(); ++i) {
            if (Move::toString(list[i]) == moveStr) {
                makeMove(list[i]);
                return;
            }
        }
    }

    bool Position::attemptMove(const u16 move) {
        Color us = sideToMove;

        makeMove(move);

        if (isInCheck(us)) {
            undoMove();
            return false;
        }

        return true;
    }

    void Position::undoMove() {
        sideToMove = opposite(sideToMove);

        Color us = sideToMove;
        bool isWhite = us == White;

        MoveUndoInfo& info = popUndoInfo();

        u16 move = info.move;
        state.castlingRights = info.castlingRights;
        state.enPassantSquare = info.enPassantSquare;
        state.rule50Clock = info.rule50Clock;

        Piece capturedPiece = info.capturedPiece;

        Square from = Move::from(move);
        Square dest = Move::dest(move);
        Move::Type flag = Move::type(move);

        if (move == Move::NullMove) {
            hash = info.key;
            return;
        }

        Piece movingPiece = flag >= Move::PromoQ ? makePiece(Pawn, us) : getPieceOn(dest);

        switch (flag) {
        case Move::DoublePawnPush:
        case Move::Normal: {
            clearSquare(dest);
            placePiece(from, movingPiece);
            if (capturedPiece != NoPiece)
                placePiece(dest, capturedPiece);
            break;
        }

        case Move::Castling: {
            bool kingSide = dest == G1 || dest == G8;

            Square rookFrom = isWhite ? (kingSide ? H1 : A1) : (kingSide ? H8 : A8);
            Square rookDest = isWhite ? (kingSide ? F1 : D1) : (kingSide ? F8 : D8);

            clearSquare(dest);
            clearSquare(rookDest);
            placePiece(from, movingPiece);
            placePiece(rookFrom, makePiece(Rook, us));
            break;
        }

        case Move::EnPassant: {
            clearSquare(dest);
            placePiece(from, movingPiece);
            placePiece(Square(isWhite ? dest - 8 : dest + 8), capturedPiece);
            break;
        }

        default: {
            clearSquare(dest);
            placePiece(from, movingPiece);
            placePiece(dest, capturedPiece);
            break;
        }
        }

        hash = info.key;
    }

    int Position::evaluate() const {
        int phase = Evaluation::calculatePhase(pieceBitboards);

        int score;
        score = psqtScores.getScore(phase);

        return std::min(std::max((sideToMove == White ? score : -score) + Evaluation::TempoBonus, MinCentipawn),
                        MaxCentipawn);
    }

    bool Position::isRepetition() const {
        u64 key = hash;

        if (ply < 2)
            return false;

        int count = 0;

        for (int i = ply - 2; i >= std::max(0, ply - state.rule50Clock); i -= 2) {
            if (moveUndoStack[i].key == key) {
                ++count;
            }
            if (count >= 2) {
                return true;
            }
        }

        return false;
    }

    bool Position::hasNonPawnMaterial() const {
        return colorBitboards[sideToMove] &
               ~(pieceBitboards[makePiece(Pawn, sideToMove)] | pieceBitboards[makePiece(King, sideToMove)]);
    }

} // namespace Tempo