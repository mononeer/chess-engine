#pragma once

#include "../core/position.h"
#include <array>
#include <cstdint>

namespace aetherchess {

// A container for generated moves.
// To avoid performance overhead from dynamic memory allocation during the search,
// this list uses a fixed-size array. The maximum number of legal moves from any
// chess position is 218, so 256 is a safe upper bound.
struct MoveList {
    std::array<Move, 256> moves;
    int count = 0;

    // Adds a move to the list.
    void add(Move m) {
        if (count < 256) {
            moves[count++] = m;
        }
    }
};

// The MoveGenerator namespace contains functions for generating moves.
namespace MoveGenerator {

// Generates all pseudo-legal moves for the given position and adds them
// to the provided MoveList.
void generate_moves(const Position& pos, MoveList& move_list);

// (Future work: Private helper functions for generating moves for each piece type)
// void generate_pawn_moves(const Position& pos, MoveList& move_list);
// void generate_knight_moves(const Position& pos, MoveList& move_list);
// void generate_bishop_moves(const Position& pos, MoveList& move_list);
// void generate_rook_moves(const Position& pos, MoveList& move_list);
// void generate_queen_moves(const Position& pos, MoveList& move_list);
// void generate_king_moves(const Position& pos, MoveList& move_list);

} // namespace MoveGenerator
} // namespace aetherchess
