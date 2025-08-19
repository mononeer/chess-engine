# AetherChess Engine

This directory contains the core C++ source code for the AetherChess engine. It is designed to be a high-performance, UCI-compliant chess engine built with C++20.

## Architecture Overview

The engine follows a modern, bitboard-based architecture. Key components are organized as follows:

- **`core/`**: Defines the most fundamental data structures, including `Position`, `Move`, `PieceType`, and `Color`. This is the heart of the board representation.
- **`bitboard/`**: Contains the `Bitboard` type (`uint64_t`) and a set of highly optimized functions for bit manipulation, which are crucial for performance.
- **`zobrist/`**: Implements Zobrist hashing, allowing for efficient hashing of board positions for use in transposition tables.
- **`movegen/`**: (Future work) Will contain the logic for generating pseudo-legal and legal moves for any given position.
- **`eval/`**: (Future work) Will house the position evaluation functions, including classical handcrafted terms and eventually an NNUE model.
- **`search/`**: (Future work) Will implement the core search algorithms (e.g., Alpha-Beta, Principal Variation Search).
- **`uci/`**: (Future work) Will handle communication with chess GUIs via the Universal Chess Interface (UCI) protocol.

## Building the Engine

The engine uses CMake for a cross-platform build system.

### Prerequisites
- A C++20 compliant compiler (e.g., GCC 10+, Clang 12+, MSVC v19.29+)
- CMake (version 3.16 or higher)
- A build tool like `make`, `ninja`, or Visual Studio.

### Build Steps

From the repository root, execute the following commands:

1.  **Navigate to the engine directory:**
    ```bash
    cd engine
    ```
2.  **Configure the project with CMake:**
    ```bash
    cmake -B build
    ```
    *This creates a `build` directory for all the build artifacts.*

3.  **Compile the source code:**
    ```bash
    cmake --build build
    ```
4.  The compiled binary, `aetherchess`, will be located in the `engine/build` directory. You can run it directly:
    ```bash
    ./build/aetherchess
    ```

## Example Usage

Below is a simple example of how to use the engine's data structures to set up a custom position and calculate its Zobrist hash.

```cpp
#include <iostream>
#include "core/position.h"
#include "bitboard/bitboard.h"
#include "zobrist/zobrist.h"

// Helper function to set a piece on the board in a position object.
void set_piece(aetherchess::Position& pos, aetherchess::Square s, aetherchess::Color c, aetherchess::PieceType pt) {
    // Set the piece on the mailbox arrays
    pos.color_on_sq[s] = c;
    pos.piece_on_sq[s] = pt;

    // Set the bit on the corresponding bitboards
    int color_idx = static_cast<int>(c);
    int piece_type_idx = static_cast<int>(pt);
    aetherchess::BB::set_bit(pos.piece_bbs[color_idx][piece_type_idx], s);
    aetherchess::BB::set_bit(pos.color_bbs[color_idx], s);
}

int main() {
    // 1. Initialize engine subsystems (like Zobrist keys)
    aetherchess::Zobrist::init();

    // 2. Set up a custom position
    aetherchess::Position pos = {}; // Zero-initialize the position
    pos.side_to_move = aetherchess::Color::WHITE;
    pos.castling_rights = aetherchess::CastlingRights::ANY_CASTLING;

    // Place a few pieces on the board
    set_piece(pos, aetherchess::E1, aetherchess::Color::WHITE, aetherchess::PieceType::KING);
    set_piece(pos, aetherchess::E8, aetherchess::Color::BLACK, aetherchess::PieceType::KING);
    set_piece(pos, aetherchess::A1, aetherchess::Color::WHITE, aetherchess::PieceType::ROOK);
    set_piece(pos, aetherchess::H8, aetherchess::Color::BLACK, aetherchess::PieceType::ROOK);
    set_piece(pos, aetherchess::D4, aetherchess::Color::WHITE, aetherchess::PieceType::PAWN);
    set_piece(pos, aetherchess::E5, aetherchess::Color::BLACK, aetherchess::PieceType::PAWN);

    // 3. Calculate the Zobrist hash for this position
    pos.hash_key = pos.calculate_hash();

    std::cout << "Custom position Zobrist hash: " << pos.hash_key << std::endl;

    return 0;
}
```
