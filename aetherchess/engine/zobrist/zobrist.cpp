#include "zobrist.h"
#include <cstdint>

namespace aetherchess {
namespace Zobrist {

// Define the storage for the Zobrist keys.
uint64_t piece_keys[2][6][64];
uint64_t black_to_move_key;
uint64_t castling_keys[16];
uint64_t en_passant_keys[8];

// A simple pseudo-random number generator to ensure that the keys are
// the same every time the engine is run.
class PRNG {
public:
    explicit PRNG(uint64_t seed) : state(seed) {}

    uint64_t next() {
        uint64_t z = (state += 0x9e3779b97f4a7c15);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
        z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
        return z ^ (z >> 31);
    }

private:
    uint64_t state;
};

// Initialize all Zobrist keys.
void init() {
    PRNG prng(1070372); // A fixed seed for reproducibility.

    for (int c = 0; c < 2; ++c) {
        for (int pt = 0; pt < 6; ++pt) {
            for (int sq = 0; sq < 64; ++sq) {
                piece_keys[c][pt][sq] = prng.next();
            }
        }
    }

    black_to_move_key = prng.next();

    for (int i = 0; i < 16; ++i) {
        castling_keys[i] = prng.next();
    }

    for (int i = 0; i < 8; ++i) {
        en_passant_keys[i] = prng.next();
    }
}

} // namespace Zobrist
} // namespace aetherchess
