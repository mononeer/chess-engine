#include "movegen.h"
#include "../core/position.h"

namespace aetherchess {
namespace MoveGenerator {

// Main function to generate all pseudo-legal moves in a position.
// A pseudo-legal move is a move that follows the rules of movement for a piece,
// but may leave the king in check. A final legality check is required.
void generate_moves(const Position& pos, MoveList& move_list) {
    // This function will orchestrate the move generation process.
    // It will call helper functions for each piece type.

    // Example structure:
    // generate_pawn_moves(pos, move_list);
    // generate_knight_moves(pos, move_list);
    // generate_bishop_moves(pos, move_list);
    // generate_rook_moves(pos, move_list);
    // generate_queen_moves(pos, move_list);
    // generate_king_moves(pos, move_list);
}

} // namespace MoveGenerator
} // namespace aetherchess
