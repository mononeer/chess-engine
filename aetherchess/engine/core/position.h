#pragma once

#include "types.h"
#include "../bitboard/bitboard.h"
#include "../zobrist/zobrist.h"

namespace aetherchess {

// The Position struct represents a single, static chess position.
// It contains all the information needed to generate legal moves, evaluate the
// position, and continue the game.
struct Position {
    // Bitboards for piece locations.
    // Indexed by [Color][PieceType].
    Bitboard piece_bbs[2][6] = {};

    // Bitboards for all pieces of a given color.
    // Indexed by [Color].
    Bitboard color_bbs[2] = {};

    // Mailbox representation for quick piece lookup on a square.
    // Indexed by [Square].
    PieceType piece_on_sq[64] = {};
    Color color_on_sq[64] = {};

    // Game state variables.
    Color side_to_move = Color::WHITE;
    Square en_passant_sq = Square::SQ_NONE;
    CastlingRights castling_rights = CastlingRights::NO_CASTLING;
    int halfmove_clock = 0; // For the 50-move rule.
    int fullmove_number = 1;

    // The Zobrist hash key for the current position.
    uint64_t hash_key = 0;

    // --- Member Functions ---

    // (Future work: implement these)
    // void set_from_fen(const std::string& fen_string);
    // void make_move(Move m);
    // void unmake_move(Move m);

    // Calculates the Zobrist hash from scratch based on the current board state.
    uint64_t calculate_hash() const {
        uint64_t hash = 0;

        // Hash pieces
        for (int sq_idx = 0; sq_idx < 64; ++sq_idx) {
            if (color_on_sq[sq_idx] != Color::NONE) {
                int color_idx = static_cast<int>(color_on_sq[sq_idx]);
                int piece_type_idx = static_cast<int>(piece_on_sq[sq_idx]);
                hash ^= Zobrist::piece_keys[color_idx][piece_type_idx][sq_idx];
            }
        }

        // Hash side to move
        if (side_to_move == Color::BLACK) {
            hash ^= Zobrist::black_to_move_key;
        }

        // Hash castling rights
        hash ^= Zobrist::castling_keys[static_cast<uint8_t>(castling_rights)];

        // Hash en passant square
        if (en_passant_sq != Square::SQ_NONE) {
            // We only need to hash the file, as the rank is determined by the side to move.
            File ep_file = static_cast<File>(en_passant_sq % 8);
            hash ^= Zobrist::en_passant_keys[ep_file];
        }

        return hash;
    }
};

} // namespace aetherchess
