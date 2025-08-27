#include "eval.h"
#include "../core/position.h"

namespace aetherchess {
namespace Eval {

// Piece-Square Tables (PSTs)
// These tables score a piece based on its position on the board.
// The values are in centipawns. They are defined from white's perspective.
// For black, the square index is flipped.

const int pawn_pst[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

const int knight_pst[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
};

const int bishop_pst[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
};

const int rook_pst[64] = {
      0,  0,  0,  0,  0,  0,  0,  0,
      5, 10, 10, 10, 10, 10, 10,  5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
      0,  0,  0,  5,  5,  0,  0,  0
};

const int queen_pst[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

const int king_pst[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

const int material_values[6] = {100, 320, 330, 500, 900, 20000}; // PAWN to KING

static int calculate_material(const Position& pos) {
    int score = 0;
    for (int pt_idx = 0; pt_idx < 6; ++pt_idx) {
        score += material_values[pt_idx] * (BB::count_bits(pos.piece_bbs[static_cast<int>(Color::WHITE)][pt_idx]) - BB::count_bits(pos.piece_bbs[static_cast<int>(Color::BLACK)][pt_idx]));
    }
    return score;
}


int evaluate(const Position& pos) {
    int score = calculate_material(pos);

    const int* psts[] = {pawn_pst, knight_pst, bishop_pst, rook_pst, queen_pst, king_pst};

    for (int pt_idx = 0; pt_idx < 6; ++pt_idx) {
        Bitboard white_pieces = pos.piece_bbs[static_cast<int>(Color::WHITE)][pt_idx];
        while (white_pieces) {
            Square s = BB::pop_lsb(white_pieces);
            score += psts[pt_idx][s];
        }

        Bitboard black_pieces = pos.piece_bbs[static_cast<int>(Color::BLACK)][pt_idx];
        while (black_pieces) {
            Square s = BB::pop_lsb(black_pieces);
            score -= psts[pt_idx][s ^ 56]; // Flip square for black
        }
    }

    // Return score from the perspective of the side to move
    return (pos.side_to_move == Color::WHITE) ? score : -score;
}

} // namespace Eval
} // namespace aetherchess
