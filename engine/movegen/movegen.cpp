#include "movegen.h"
#include "../core/position.h"
#include "attacks.h"
#include <iostream>

namespace aetherchess {
namespace MoveGenerator {

// Forward declarations for static helper functions
static void generate_pawn_moves(const Position& pos, MoveList& move_list);
static void generate_knight_moves(const Position& pos, MoveList& move_list);
static void generate_king_moves(const Position& pos, MoveList& move_list);
static void generate_rook_moves(const Position& pos, MoveList& move_list);
static void generate_bishop_moves(const Position& pos, MoveList& move_list);
static void generate_queen_moves(const Position& pos, MoveList& move_list);

// Main function to generate all pseudo-legal moves in a position.
void generate_moves(const Position& pos, MoveList& move_list) {
    generate_pawn_moves(pos, move_list);
    generate_knight_moves(pos, move_list);
    generate_king_moves(pos, move_list);
    generate_rook_moves(pos, move_list);
    generate_bishop_moves(pos, move_list);
    generate_queen_moves(pos, move_list);
}

// --- Sliding Piece Move Generation ---

static void generate_rook_moves(const Position& pos, MoveList& move_list) {
    const int us = static_cast<int>(pos.side_to_move);
    Bitboard rooks = pos.piece_bbs[us][static_cast<int>(PieceType::ROOK)];
    const Bitboard occupied = pos.color_bbs[0] | pos.color_bbs[1];

    while (rooks) {
        Square from_sq = BB::pop_lsb(rooks);
        Bitboard attacks = Attacks::get_rook_attacks(from_sq, occupied);
        Bitboard quiet_moves = attacks & ~occupied;
        Bitboard capture_moves = attacks & pos.color_bbs[1 - us];

        while (quiet_moves) {
            Square to_sq = BB::pop_lsb(quiet_moves);
            move_list.add(Moves::create(from_sq, to_sq, QUIET));
        }
        while (capture_moves) {
            Square to_sq = BB::pop_lsb(capture_moves);
            move_list.add(Moves::create(from_sq, to_sq, CAPTURE));
        }
    }
}

static void generate_bishop_moves(const Position& pos, MoveList& move_list) {
    const int us = static_cast<int>(pos.side_to_move);
    Bitboard bishops = pos.piece_bbs[us][static_cast<int>(PieceType::BISHOP)];
    const Bitboard occupied = pos.color_bbs[0] | pos.color_bbs[1];

    while (bishops) {
        Square from_sq = BB::pop_lsb(bishops);
        Bitboard attacks = Attacks::get_bishop_attacks(from_sq, occupied);
        Bitboard quiet_moves = attacks & ~occupied;
        Bitboard capture_moves = attacks & pos.color_bbs[1 - us];

        while (quiet_moves) {
            Square to_sq = BB::pop_lsb(quiet_moves);
            move_list.add(Moves::create(from_sq, to_sq, QUIET));
        }
        while (capture_moves) {
            Square to_sq = BB::pop_lsb(capture_moves);
            move_list.add(Moves::create(from_sq, to_sq, CAPTURE));
        }
    }
}

static void generate_queen_moves(const Position& pos, MoveList& move_list) {
    const int us = static_cast<int>(pos.side_to_move);
    Bitboard queens = pos.piece_bbs[us][static_cast<int>(PieceType::QUEEN)];
    const Bitboard occupied = pos.color_bbs[0] | pos.color_bbs[1];

    while (queens) {
        Square from_sq = BB::pop_lsb(queens);
        Bitboard attacks = Attacks::get_queen_attacks(from_sq, occupied);
        Bitboard quiet_moves = attacks & ~occupied;
        Bitboard capture_moves = attacks & pos.color_bbs[1 - us];

        while (quiet_moves) {
            Square to_sq = BB::pop_lsb(quiet_moves);
            move_list.add(Moves::create(from_sq, to_sq, QUIET));
        }
        while (capture_moves) {
            Square to_sq = BB::pop_lsb(capture_moves);
            move_list.add(Moves::create(from_sq, to_sq, CAPTURE));
        }
    }
}

// Helper function to generate knight moves.
static void generate_knight_moves(const Position& pos, MoveList& move_list) {
    const int us = static_cast<int>(pos.side_to_move);
    Bitboard knights = pos.piece_bbs[us][static_cast<int>(PieceType::KNIGHT)];
    const Bitboard their_pieces = pos.color_bbs[1 - us];
    const Bitboard our_pieces = pos.color_bbs[us];

    while (knights) {
        Square from_sq = BB::pop_lsb(knights);
        Bitboard attacks = Attacks::knight_attacks[from_sq];
        Bitboard quiet_moves = attacks & ~our_pieces & ~their_pieces;
        Bitboard capture_moves = attacks & their_pieces;

        while (quiet_moves) {
            Square to_sq = BB::pop_lsb(quiet_moves);
            move_list.add(Moves::create(from_sq, to_sq, QUIET));
        }
        while (capture_moves) {
            Square to_sq = BB::pop_lsb(capture_moves);
            move_list.add(Moves::create(from_sq, to_sq, CAPTURE));
        }
    }
}

// Helper function to generate king moves.
static void generate_king_moves(const Position& pos, MoveList& move_list) {
    const int us = static_cast<int>(pos.side_to_move);
    const Bitboard occupied = pos.color_bbs[0] | pos.color_bbs[1];

    // We assume there is exactly one king for the side to move.
    Bitboard king_bb = pos.piece_bbs[us][static_cast<int>(PieceType::KING)];
    if (!king_bb) return;

    Square from_sq = BB::pop_lsb(king_bb);
    Bitboard attacks = Attacks::king_attacks[from_sq];
    Bitboard quiet_moves = attacks & ~pos.color_bbs[us];
    Bitboard capture_moves = attacks & pos.color_bbs[1 - us];

    while (quiet_moves) {
        Square to_sq = BB::pop_lsb(quiet_moves);
        move_list.add(Moves::create(from_sq, to_sq, QUIET));
    }
    while (capture_moves) {
        Square to_sq = BB::pop_lsb(capture_moves);
        move_list.add(Moves::create(from_sq, to_sq, CAPTURE));
    }

    // Castling
    if (!pos.is_in_check(static_cast<Color>(us))) {
        if (us == static_cast<int>(Color::WHITE)) {
            if ((pos.castling_rights & WHITE_KINGSIDE) && !(occupied & 0x6000000000000000ULL)) {
                if (!pos.is_square_attacked(F1, Color::BLACK)) {
                    move_list.add(Moves::create(E1, G1, KING_CASTLE));
                }
            }
            if ((pos.castling_rights & WHITE_QUEENSIDE) && !(occupied & 0xE00000000000000ULL)) {
                if (!pos.is_square_attacked(D1, Color::BLACK)) {
                    move_list.add(Moves::create(E1, C1, QUEEN_CASTLE));
                }
            }
        } else { // BLACK
            if ((pos.castling_rights & BLACK_KINGSIDE) && !(occupied & 0x60ULL)) {
                if (!pos.is_square_attacked(F8, Color::WHITE)) {
                    move_list.add(Moves::create(E8, G8, KING_CASTLE));
                }
            }
            if ((pos.castling_rights & BLACK_QUEENSIDE) && !(occupied & 0xEULL)) {
                if (!pos.is_square_attacked(D8, Color::WHITE)) {
                    move_list.add(Moves::create(E8, C8, QUEEN_CASTLE));
                }
            }
        }
    }
}

// Helper function to generate pawn moves.
static void generate_pawn_moves(const Position& pos, MoveList& move_list) {
    const int us = static_cast<int>(pos.side_to_move);
    const int them = 1 - us;
    const Bitboard their_pieces = pos.color_bbs[them];
    const Bitboard occupied = pos.color_bbs[0] | pos.color_bbs[1];

    Bitboard pawns = pos.piece_bbs[us][static_cast<int>(PieceType::PAWN)];

    if (us == static_cast<int>(Color::WHITE)) {
        Bitboard single_pushes = (pawns << 8) & ~occupied;
        Bitboard double_pushes = ((single_pushes & Bitboards::RANK_3) << 8) & ~occupied;
        Bitboard left_captures = ((pawns & ~Bitboards::FILE_A) << 9) & their_pieces;
        Bitboard right_captures = ((pawns & ~Bitboards::FILE_H) << 7) & their_pieces;

        Bitboard promo_rank = Bitboards::RANK_8;

        while (single_pushes) {
            Square to_sq = BB::pop_lsb(single_pushes);
            Square from_sq = static_cast<Square>(to_sq - 8);
            if ((1ULL << to_sq) & promo_rank) {
                move_list.add(Moves::create(from_sq, to_sq, PROMO_QUEEN));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_ROOK));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_BISHOP));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_KNIGHT));
            } else {
                move_list.add(Moves::create(from_sq, to_sq, QUIET));
            }
        }
        while (double_pushes) {
            Square to_sq = BB::pop_lsb(double_pushes);
            move_list.add(Moves::create(static_cast<Square>(to_sq - 16), to_sq, DOUBLE_PAWN_PUSH));
        }
        while (left_captures) {
            Square to_sq = BB::pop_lsb(left_captures);
            Square from_sq = static_cast<Square>(to_sq - 9);
            if ((1ULL << to_sq) & promo_rank) {
                move_list.add(Moves::create(from_sq, to_sq, PROMO_CAPTURE_QUEEN));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_CAPTURE_ROOK));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_CAPTURE_BISHOP));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_CAPTURE_KNIGHT));
            } else {
                move_list.add(Moves::create(from_sq, to_sq, CAPTURE));
            }
        }
        while (right_captures) {
            Square to_sq = BB::pop_lsb(right_captures);
            Square from_sq = static_cast<Square>(to_sq - 7);
            if ((1ULL << to_sq) & promo_rank) {
                move_list.add(Moves::create(from_sq, to_sq, PROMO_CAPTURE_QUEEN));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_CAPTURE_ROOK));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_CAPTURE_BISHOP));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_CAPTURE_KNIGHT));
            } else {
                move_list.add(Moves::create(from_sq, to_sq, CAPTURE));
            }
        }
        if (pos.en_passant_sq != SQ_NONE) {
            std::cout << "DEBUG: White EP check, square is " << pos.en_passant_sq << std::endl;
            Bitboard ep_attacks = Attacks::pawn_attacks[them][pos.en_passant_sq] & pawns;
            if (ep_attacks) std::cout << "DEBUG: White found potential EP attacker!" << std::endl;
            while(ep_attacks) {
                Square from_sq = BB::pop_lsb(ep_attacks);
                move_list.add(Moves::create(from_sq, pos.en_passant_sq, EN_PASSANT));
            }
        }
    } else { // BLACK
        Bitboard single_pushes = (pawns >> 8) & ~occupied;
        Bitboard double_pushes = ((single_pushes & Bitboards::RANK_6) >> 8) & ~occupied;
        Bitboard left_captures = ((pawns & ~Bitboards::FILE_H) >> 7) & their_pieces;
        Bitboard right_captures = ((pawns & ~Bitboards::FILE_A) >> 9) & their_pieces;

        Bitboard promo_rank = Bitboards::RANK_1;

        while (single_pushes) {
            Square to_sq = BB::pop_lsb(single_pushes);
            Square from_sq = static_cast<Square>(to_sq + 8);
            if ((1ULL << to_sq) & promo_rank) {
                move_list.add(Moves::create(from_sq, to_sq, PROMO_QUEEN));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_ROOK));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_BISHOP));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_KNIGHT));
            } else {
                move_list.add(Moves::create(from_sq, to_sq, QUIET));
            }
        }
        while (double_pushes) {
            Square to_sq = BB::pop_lsb(double_pushes);
            move_list.add(Moves::create(static_cast<Square>(to_sq + 16), to_sq, DOUBLE_PAWN_PUSH));
        }
        while (left_captures) {
            Square to_sq = BB::pop_lsb(left_captures);
            Square from_sq = static_cast<Square>(to_sq + 7);
            if ((1ULL << to_sq) & promo_rank) {
                move_list.add(Moves::create(from_sq, to_sq, PROMO_CAPTURE_QUEEN));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_CAPTURE_ROOK));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_CAPTURE_BISHOP));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_CAPTURE_KNIGHT));
            } else {
                move_list.add(Moves::create(from_sq, to_sq, CAPTURE));
            }
        }
        while (right_captures) {
            Square to_sq = BB::pop_lsb(right_captures);
            Square from_sq = static_cast<Square>(to_sq + 9);
            if ((1ULL << to_sq) & promo_rank) {
                move_list.add(Moves::create(from_sq, to_sq, PROMO_CAPTURE_QUEEN));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_CAPTURE_ROOK));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_CAPTURE_BISHOP));
                move_list.add(Moves::create(from_sq, to_sq, PROMO_CAPTURE_KNIGHT));
            } else {
                move_list.add(Moves::create(from_sq, to_sq, CAPTURE));
            }
        }
        if (pos.en_passant_sq != SQ_NONE) {
            Bitboard ep_attacks = Attacks::pawn_attacks[them][pos.en_passant_sq] & pawns;
            while(ep_attacks) {
                Square from_sq = BB::pop_lsb(ep_attacks);
                move_list.add(Moves::create(from_sq, pos.en_passant_sq, EN_PASSANT));
            }
        }
    }
}

} // namespace MoveGenerator
} // namespace aetherchess
