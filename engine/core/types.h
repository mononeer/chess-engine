#pragma once

#include <cstdint>

namespace aetherchess {

// Using 'enum class' for type safety and to avoid polluting the global namespace.

enum class Color : int {
    WHITE,
    BLACK,
    NONE // For invalid/empty squares
};

enum class PieceType : int {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NONE // For invalid/empty squares
};

// Squares are numbered from 0 (A1) to 63 (H8).
enum Square : int {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    SQ_NONE // Represents an invalid or off-board square
};

// Files and Ranks for easier logic and readability.
enum File : int { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H };
enum Rank : int { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8 };

// Castling rights are stored as a bitmask.
enum CastlingRights : uint8_t {
    NO_CASTLING = 0,
    WHITE_KINGSIDE  = 1,
    WHITE_QUEENSIDE = 2,
    BLACK_KINGSIDE  = 4,
    BLACK_QUEENSIDE = 8,
    ANY_CASTLING = WHITE_KINGSIDE | WHITE_QUEENSIDE | BLACK_KINGSIDE | BLACK_QUEENSIDE
};

// A move is packed into a 32-bit integer for efficiency.
//
// Bits | Description
// -----|--------------------------------------------------
// 0-5  | "From" square (0-63)
// 6-11 | "To" square (0-63)
// 12-15| Move flags (see MoveType enum)
//
// The promotion piece is encoded within the flags.
using Move = uint32_t;

// The MoveType enum defines all possible types of moves.
// The 4-bit value is stored in the move's flags.
enum MoveType : uint32_t {
    QUIET            = 0b0000,
    DOUBLE_PAWN_PUSH = 0b0001,
    KING_CASTLE      = 0b0010,
    QUEEN_CASTLE     = 0b0011,
    CAPTURE          = 0b0100,
    EN_PASSANT       = 0b0101,
    // Promotions are encoded in the last 4 bits
    PROMO_KNIGHT     = 0b1000,
    PROMO_BISHOP     = 0b1001,
    PROMO_ROOK       = 0b1010,
    PROMO_QUEEN      = 0b1011,
    PROMO_CAPTURE_KNIGHT = 0b1100,
    PROMO_CAPTURE_BISHOP = 0b1101,
    PROMO_CAPTURE_ROOK   = 0b1110,
    PROMO_CAPTURE_QUEEN  = 0b1111
};

// Helper functions for creating and decoding moves.
namespace Moves {
    inline Move create(Square from, Square to, MoveType type = QUIET) {
        return (type << 12) | (to << 6) | from;
    }

    inline Square get_from(Move m) { return static_cast<Square>(m & 0x3F); }
    inline Square get_to(Move m) { return static_cast<Square>((m >> 6) & 0x3F); }
    inline MoveType get_type(Move m) { return static_cast<MoveType>((m >> 12) & 0xF); }
}

} // namespace aetherchess
