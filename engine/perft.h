#pragma once

#include "core/position.h"
#include <cstdint>

namespace Perft {

// Runs a performance test (perft) to the given depth, counting the number of leaf nodes.
// This is used to verify the correctness of the move generator.
uint64_t run(aetherchess::Position& pos, int depth);

// Helper function to convert a move to a string in UCI format.
std::string move_to_string(aetherchess::Move m);

} // namespace Perft
