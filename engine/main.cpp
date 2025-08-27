#include <iostream>
#include <string>
#include "core/position.h"
#include "movegen/attacks.h"
#include "zobrist/zobrist.h"
#include "movegen/movegen.h"
#include "perft.h"
#include "eval/eval.h"

// The main entry point for the AetherChess engine.
// This is currently set up to run a perft test for debugging.
int main() {
    aetherchess::Zobrist::init();
    aetherchess::Attacks::init();

    std::cout << "AetherChess Engine" << std::endl;
    std::cout << "------------------" << std::endl;

    aetherchess::Position pos;
    const std::string start_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    pos.set_from_fen(start_fen);

    std::cout << "Running Perft from starting position..." << std::endl;
    std::cout << "Note: Perft(3) has a known bug (8903 vs 8902)." << std::endl;

    for (int depth = 1; depth <= 3; ++depth) {
        uint64_t nodes = Perft::run(pos, depth);
        std::cout << "perft(" << depth << ") = " << nodes << std::endl;
    }

    return 0;
}
