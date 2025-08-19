#include <iostream>
#include "core/position.h"
#include "bitboard/bitboard.h"
#include "zobrist/zobrist.h"
#include "movegen/attacks.h"

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
    // 1. Initialize engine subsystems
    aetherchess::Zobrist::init();
    aetherchess::Attacks::init();

    std::cout << "AetherChess Engine" << std::endl;
    std::cout << "------------------" << std::endl;

    // 2. Set up a custom position to demonstrate functionality
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

    std::cout << "Successfully set up custom position." << std::endl;
    std::cout << "Zobrist hash for this position: " << pos.hash_key << std::endl;

    return 0;
}
