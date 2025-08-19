#pragma once

#include "../core/types.h"
#include "../bitboard/bitboard.h"

namespace aetherchess {
namespace Attacks {

// Attack Tables
// These tables are pre-computed at startup to provide fast lookups for piece attacks.

// Pawn attacks [Color][Square]
extern Bitboard pawn_attacks[2][64];

// Knight attacks [Square]
extern Bitboard knight_attacks[64];

// King attacks [Square]
extern Bitboard king_attacks[64];

// (Future work: Sliding piece attacks will be handled here, likely with magic bitboards)
// extern Bitboard bishop_attacks[64][512];
// extern Bitboard rook_attacks[64][4096];

// Initializes all attack tables. Must be called at program startup.
void init();

} // namespace Attacks
} // namespace aetherchess
