#include "attacks.h"
#include "../bitboard/bitboard.h"

namespace aetherchess {
namespace Attacks {

// Define storage for the attack tables.
Bitboard pawn_attacks[2][64];
Bitboard knight_attacks[64];
Bitboard king_attacks[64];

// Helper function to calculate knight attacks for a given square.
static Bitboard generate_knight_attacks(Square s) {
    Bitboard attacks = Bitboards::EMPTY;
    Bitboard from_bb = 1ULL << s;

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
static Bitboard generate_king_attacks(Square s) {
    Bitboard attacks = Bitboards::EMPTY;
    Bitboard from_bb = 1ULL << s;

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
static Bitboard generate_pawn_attacks(Square s, Color c) {
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

// --- Magic Bitboard structures ---
Magic rook_magics[64];
Magic bishop_magics[64];
Bitboard rook_attacks[102400];
Bitboard bishop_attacks[5248];

// --- PRNG for magic number generation ---
class PRNG {
public:
    explicit PRNG(uint64_t seed) : state(seed) {}
    uint64_t next() {
        uint64_t z = (state += 0x9e3779b97f4a7c15);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
        z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
        return z ^ (z >> 31);
    }
    uint64_t next_sparse() { return next() & next() & next(); }
private:
    uint64_t state;
};

// --- Static Helper Functions for Magic Bitboards ---
static Bitboard generate_rook_mask(Square s) {
    Bitboard result = 0ULL;
    int r = s / 8, f = s % 8;
    for (int i = r + 1; i < 7; i++) result |= (1ULL << (i * 8 + f));
    for (int i = r - 1; i > 0; i--) result |= (1ULL << (i * 8 + f));
    for (int i = f + 1; i < 7; i++) result |= (1ULL << (r * 8 + i));
    for (int i = f - 1; i > 0; i--) result |= (1ULL << (r * 8 + i));
    return result;
}

static Bitboard generate_bishop_mask(Square s) {
    Bitboard result = 0ULL;
    int r = s / 8, f = s % 8;
    for (int i = r + 1, j = f + 1; i < 7 && j < 7; i++, j++) result |= (1ULL << (i * 8 + j));
    for (int i = r + 1, j = f - 1; i < 7 && j > 0; i++, j--) result |= (1ULL << (i * 8 + j));
    for (int i = r - 1, j = f + 1; i > 0 && j < 7; i--, j++) result |= (1ULL << (i * 8 + j));
    for (int i = r - 1, j = f - 1; i > 0 && j > 0; i--, j--) result |= (1ULL << (i * 8 + j));
    return result;
}

static Bitboard generate_sliding_attacks(Square s, Bitboard blockers, bool is_bishop) {
    Bitboard attacks = 0ULL;
    int r = s / 8, f = s % 8;
    int dirs[8][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    int start_dir = is_bishop ? 4 : 0;
    int end_dir = is_bishop ? 8 : 4;

    for (int i = start_dir; i < end_dir; i++) {
        int dr = dirs[i][0];
        int df = dirs[i][1];
        for (int j = 1; j < 8; j++) {
            int nr = r + j * dr;
            int nf = f + j * df;
            if (nr >= 0 && nr < 8 && nf >= 0 && nf < 8) {
                Square to_sq = static_cast<Square>(nr * 8 + nf);
                attacks |= (1ULL << to_sq);
                if (blockers & (1ULL << to_sq)) break;
            } else {
                break;
            }
        }
    }
    return attacks;
}

static void find_magic(Square s, bool is_bishop, PRNG& prng) {
    static Bitboard* p_rook_attacks = rook_attacks;
    static Bitboard* p_bishop_attacks = bishop_attacks;

    Magic& magic = is_bishop ? bishop_magics[s] : rook_magics[s];
    magic.mask = is_bishop ? generate_bishop_mask(s) : generate_rook_mask(s);
    magic.attacks = is_bishop ? p_bishop_attacks : p_rook_attacks;

    int num_mask_bits = BB::count_bits(magic.mask);
    magic.shift = 64 - num_mask_bits;
    int num_occupancies = 1 << num_mask_bits;

    Bitboard occupancies[4096];
    Bitboard attacks[4096];
    Bitboard b = 0ULL;
    for(int i=0; i < num_occupancies; ++i) {
        occupancies[i] = b;
        attacks[i] = generate_sliding_attacks(s, b, is_bishop);
        b = (b - magic.mask) & magic.mask;
    }

    for (int i = 0; i < 100000000; i++) {
        magic.magic = prng.next_sparse();
        if (BB::count_bits((magic.mask * magic.magic) & 0xFF00000000000000ULL) < 6) continue;

        bool fail = false;
        for (int j = 0; j < num_occupancies; j++) {
            int idx = (occupancies[j] * magic.magic) >> magic.shift;
            if (magic.attacks[idx] == 0ULL) {
                magic.attacks[idx] = attacks[j];
            } else if (magic.attacks[idx] != attacks[j]) {
                fail = true;
                for(int k=0; k<j; ++k) magic.attacks[(occupancies[k] * magic.magic) >> magic.shift] = 0ULL;
                break;
            }
        }
        if (!fail) {
             if (is_bishop) p_bishop_attacks += num_occupancies;
             else p_rook_attacks += num_occupancies;
             return;
        }
    }
    // This is a fatal error, the engine cannot run without magic numbers.
}


void init() {
    // 1. Initialize non-sliding piece attacks
    for (int sq = 0; sq < 64; ++sq) {
        pawn_attacks[static_cast<int>(Color::WHITE)][sq] = generate_pawn_attacks(static_cast<Square>(sq), Color::WHITE);
        pawn_attacks[static_cast<int>(Color::BLACK)][sq] = generate_pawn_attacks(static_cast<Square>(sq), Color::BLACK);
        knight_attacks[sq] = generate_knight_attacks(static_cast<Square>(sq));
        king_attacks[sq] = generate_king_attacks(static_cast<Square>(sq));
    }

    // 2. Initialize sliding piece attacks using magic bitboards
    PRNG prng(1070372);
    for (int sq = 0; sq < 64; ++sq) {
        find_magic(static_cast<Square>(sq), false, prng); // Rooks
        find_magic(static_cast<Square>(sq), true, prng);  // Bishops
    }
}

} // namespace Attacks
} // namespace aetherchess
