#pragma once

#include "../core/position.h"

namespace aetherchess {
namespace Eval {

// Evaluates the given position from the perspective of the side to move.
// A positive score favors the side to move, a negative score favors the opponent.
// The score is in centipawns.
int evaluate(const Position& pos);

} // namespace Eval
} // namespace aetherchess
