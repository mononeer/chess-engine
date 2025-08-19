#include "attacks.h"
#include "../bitboard/bitboard.h"

namespace aetherchess {
namespace Attacks {

// Define storage for the attack tables.
Bitboard pawn_attacks[2][64];
Bitboard knight_attacks[64];
Bitboard king_attacks[64];

// Helper function to calculate knight attacks for a given square.
Bitboard generate_knight_attacks(Square s) {
    Bitboard attacks = Bitboards::EMPTY;
    Bitboard from_bb = 1ULL << s;

    // Possible knight moves (jumps)
    if ((from_bb >> 17) & ~Bitboards::FILE_H) attacks |= (from_bb >> 17);
    if ((from_bb >> 15) & ~Bitboards::FILE_A) attacks |= (from_bb >> 15);
    if ((from_bb >> 10) & ~Bitboards::FILE_G & ~Bitboards::FILE_H) attacks |= (from_bb >> 10);
    if ((from_bb >> 6)  & ~Bitboards::FILE_A & ~Bitboards::FILE_B) attacks |= (from_bb >> 6);
    if ((from_bb << 6)  & ~Bitboards::FILE_G & ~Bitboards::FILE_H) attacks |= (from_bb << 6);
    if ((from_bb << 10) & ~Bitboards::FILE_A & ~Bitboards::FILE_B) attacks |= (from_bb << 10);
    if ((from_bb << 15) & ~Bitboards::FILE_H) attacks |= (from_bb << 15);
    if ((from_bb << 17) & ~Bitboards::FILE_A) attacks |= (from_bb << 17);

    return attacks;
}

// Helper function to calculate king attacks for a given square.
Bitboard generate_king_attacks(Square s) {
    Bitboard attacks = Bitboards::EMPTY;
    Bitboard from_bb = 1ULL << s;

    // Possible king moves (one square in any direction)
    if ((from_bb >> 9) & ~Bitboards::FILE_H) attacks |= (from_bb >> 9);
    if ((from_bb >> 8)) attacks |= (from_bb >> 8);
    if ((from_bb >> 7) & ~Bitboards::FILE_A) attacks |= (from_bb >> 7);
    if ((from_bb >> 1) & ~Bitboards::FILE_H) attacks |= (from_bb >> 1);
    if ((from_bb << 1) & ~Bitboards::FILE_A) attacks |= (from_bb << 1);
    if ((from_bb << 7) & ~Bitboards::FILE_H) attacks |= (from_bb << 7);
    if ((from_bb << 8)) attacks |= (from_bb << 8);
    if ((from_bb << 9) & ~Bitboards::FILE_A) attacks |= (from_bb << 9);

    return attacks;
}

// Helper function to calculate pawn attacks for a given square and color.
Bitboard generate_pawn_attacks(Square s, Color c) {
    Bitboard attacks = Bitboards::EMPTY;
    Bitboard from_bb = 1ULL << s;

    if (c == Color::WHITE) {
        if ((from_bb << 7) & ~Bitboards::FILE_H) attacks |= (from_bb << 7);
        if ((from_bb << 9) & ~Bitboards::FILE_A) attacks |= (from_bb << 9);
    } else { // BLACK
        if ((from_bb >> 9) & ~Bitboards::FILE_H) attacks |= (from_bb >> 9);
        if ((from_bb >> 7) & ~Bitboards::FILE_A) attacks |= (from_bb >> 7);
    }

    return attacks;
}

void init() {
    for (int sq = 0; sq < 64; ++sq) {
        // Initialize for both colors
        pawn_attacks[static_cast<int>(Color::WHITE)][sq] = generate_pawn_attacks(static_cast<Square>(sq), Color::WHITE);
        pawn_attacks[static_cast<int>(Color::BLACK)][sq] = generate_pawn_attacks(static_cast<Square>(sq), Color::BLACK);

        knight_attacks[sq] = generate_knight_attacks(static_cast<Square>(sq));
        king_attacks[sq] = generate_king_attacks(static_cast<Square>(sq));
    }
}

} // namespace Attacks
} // namespace aetherchess
