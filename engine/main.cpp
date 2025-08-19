#include <iostream>
#include <cstdint>

#include "core/position.h"
#include "zobrist/zobrist.h"

int main() {
    // All engine subsystems that require one-time setup should be initialized here.
    aetherchess::Zobrist::init();

    std::cout << "AetherChess Engine" << std::endl;
    std::cout << "------------------" << std::endl;
    std::cout << "Initialization complete." << std::endl;

    // --- Placeholder for future UCI loop ---
    // The main application logic will eventually be a loop that handles
    // commands from a GUI or other interface via the UCI protocol.
    // For now, we'll perform a simple test.

    // Create a position object. It's an empty board by default.
    aetherchess::Position pos;

    // Calculate the Zobrist hash of the empty board.
    // This is a good sanity check to ensure the hashing and position
    // structures are working together.
    uint64_t initial_hash = pos.calculate_hash();

    std::cout << "Test: Zobrist hash of an empty board is " << initial_hash << "." << std::endl;

    // In the future, we would parse the standard starting position FEN here,
    // and the hash would be for that position.

    std::cout << "Engine is ready. (Waiting for UCI loop implementation)" << std::endl;

    return 0;
}
