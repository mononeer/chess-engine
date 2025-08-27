#include "perft.h"
#include "movegen/movegen.h"
#include <string>

namespace Perft {

// Helper function to convert a move to a string in UCI format (e.g., "e2e4", "e7e8q")
std::string move_to_string(aetherchess::Move m) {
    aetherchess::Square from = aetherchess::Moves::get_from(m);
    aetherchess::Square to = aetherchess::Moves::get_to(m);
    aetherchess::MoveType type = aetherchess::Moves::get_type(m);

    std::string str;
    str += static_cast<char>('a' + (from % 8));
    str += static_cast<char>('1' + (from / 8));
    str += static_cast<char>('a' + (to % 8));
    str += static_cast<char>('1' + (to / 8));

    if (type >= aetherchess::PROMO_KNIGHT) {
        if (type == aetherchess::PROMO_QUEEN || type == aetherchess::PROMO_CAPTURE_QUEEN) str += 'q';
        else if (type == aetherchess::PROMO_ROOK || type == aetherchess::PROMO_CAPTURE_ROOK) str += 'r';
        else if (type == aetherchess::PROMO_BISHOP || type == aetherchess::PROMO_CAPTURE_BISHOP) str += 'b';
        else if (type == aetherchess::PROMO_KNIGHT || type == aetherchess::PROMO_CAPTURE_KNIGHT) str += 'n';
    }
    return str;
}

uint64_t run(aetherchess::Position& pos, int depth) {
    if (depth == 0) {
        return 1;
    }

    aetherchess::MoveList move_list;
    aetherchess::MoveGenerator::generate_moves(pos, move_list);

    uint64_t nodes = 0;
    for (int i = 0; i < move_list.count; ++i) {
        aetherchess::Move move = move_list.moves[i];

        if (pos.make_move(move)) {
            nodes += run(pos, depth - 1);
            pos.unmake_move(move);
        }
    }

    return nodes;
}

} // namespace Perft
