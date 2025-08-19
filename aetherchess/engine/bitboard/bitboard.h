#pragma once

#include <cstdint>
#include "../core/types.h"

// For __builtin_ctzll and __builtin_popcountll on GCC/Clang.
// On MSVC, use <intrin.h> for _BitScanForward64 and __popcnt64.

namespace aetherchess {

using Bitboard = uint64_t;

namespace Bitboards {
    constexpr Bitboard EMPTY = 0ULL;
    constexpr Bitboard ALL = ~EMPTY;

    constexpr Bitboard FILE_A = 0x0101010101010101ULL;
    constexpr Bitboard FILE_B = FILE_A << 1;
    constexpr Bitboard FILE_C = FILE_A << 2;
    constexpr Bitboard FILE_D = FILE_A << 3;
    constexpr Bitboard FILE_E = FILE_A << 4;
    constexpr Bitboard FILE_F = FILE_A << 5;
    constexpr Bitboard FILE_G = FILE_A << 6;
    constexpr Bitboard FILE_H = FILE_A << 7;

    constexpr Bitboard RANK_1 = 0xFFULL;
    constexpr Bitboard RANK_2 = RANK_1 << (8 * 1);
    constexpr Bitboard RANK_3 = RANK_1 << (8 * 2);
    constexpr Bitboard RANK_4 = RANK_1 << (8 * 3);
    constexpr Bitboard RANK_5 = RANK_1 << (8 * 4);
    constexpr Bitboard RANK_6 = RANK_1 << (8 * 5);
    constexpr Bitboard RANK_7 = RANK_1 << (8 * 6);
    constexpr Bitboard RANK_8 = RANK_1 << (8 * 7);
} // namespace Bitboards

namespace BB {
    // Sets the bit corresponding to the given square.
    inline void set_bit(Bitboard& bb, Square s) {
        bb |= (1ULL << s);
    }

    // Returns true if the bit corresponding to the given square is set.
    inline bool get_bit(Bitboard bb, Square s) {
        return (bb >> s) & 1;
    }

    // Clears the bit corresponding to the given square.
    inline void clear_bit(Bitboard& bb, Square s) {
        bb &= ~(1ULL << s);
    }

    // Finds the least significant bit (LSB), clears it, and returns its index.
    // Assumes the bitboard is not empty.
    inline Square pop_lsb(Bitboard& bb) {
        Square s = static_cast<Square>(__builtin_ctzll(bb));
        bb &= bb - 1; // Clears the LSB
        return s;
    }

    // Counts the number of set bits (population count).
    inline int count_bits(Bitboard bb) {
        return __builtin_popcountll(bb);
    }
} // namespace BB

} // namespace aetherchess
