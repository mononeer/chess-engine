#pragma once

#include "../core/types.h"
#include "../bitboard/bitboard.h"

namespace aetherchess {
namespace Attacks {

// Attack Tables
extern Bitboard pawn_attacks[2][64];
extern Bitboard knight_attacks[64];
extern Bitboard king_attacks[64];

// --- Magic Bitboards for Sliding Pieces ---

// The Magic struct holds the data needed for magic bitboard lookups for a single square.
struct Magic {
    Bitboard* attacks; // Pointer to the start of the attack table for this square
    Bitboard mask;     // Mask of relevant blocker squares
    Bitboard magic;    // The magic number used for hashing
    int shift;         // Shift to apply after multiplication to get the index
};

extern Magic rook_magics[64];
extern Magic bishop_magics[64];

// Initializes all attack tables, including magic bitboards.
// Must be called once at program startup.
void init();

// Inline functions to get sliding piece attacks using the generated tables.
inline Bitboard get_rook_attacks(Square s, Bitboard occupied) {
    occupied &= rook_magics[s].mask;
    occupied *= rook_magics[s].magic;
    occupied >>= rook_magics[s].shift;
    return rook_magics[s].attacks[occupied];
}

inline Bitboard get_bishop_attacks(Square s, Bitboard occupied) {
    occupied &= bishop_magics[s].mask;
    occupied *= bishop_magics[s].magic;
    occupied >>= bishop_magics[s].shift;
    return bishop_magics[s].attacks[occupied];
}

inline Bitboard get_queen_attacks(Square s, Bitboard occupied) {
    return get_rook_attacks(s, occupied) | get_bishop_attacks(s, occupied);
}

} // namespace Attacks
} // namespace aetherchess
