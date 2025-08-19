#pragma once

#include <cstdint>
#include "../core/types.h"

namespace aetherchess {

// Zobrist hashing is used for efficiently hashing board positions.
// A unique 64-bit key is generated for each game state component.
// The board's hash is the XOR sum of the keys for its components.
namespace Zobrist {

// Keys for each piece type on each square.
// Indexing: [color * 6 + piece_type][square]
extern uint64_t piece_keys[2][6][64];

// Key for when it is black's turn to move.
extern uint64_t black_to_move_key;

// Keys for each of the 16 possible castling right states.
extern uint64_t castling_keys[16];

// Keys for each possible en passant file (A-H).
extern uint64_t en_passant_keys[8];

// Initializes all Zobrist keys with pseudo-random 64-bit numbers.
// Must be called once at program startup.
void init();

} // namespace Zobrist
} // namespace aetherchess
