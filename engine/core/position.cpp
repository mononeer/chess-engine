#include "position.h"
#include "../movegen/attacks.h"
#include <vector>
#include <string>
#include <sstream>

namespace aetherchess {

// Forward declaration for helper
static void set_piece(Position& pos, Square s, PieceType pt, Color c, bool is_add);

// --- Public Member Functions ---

void Position::set_from_fen(const std::string& fen_string) {
    for(int i=0; i<2; ++i) for(int j=0; j<6; ++j) piece_bbs[i][j] = 0;
    for(int i=0; i<2; ++i) color_bbs[i] = 0;
    for(int i=0; i<64; ++i) { piece_on_sq[i] = PieceType::NONE; color_on_sq[i] = Color::NONE; }

    std::istringstream ss(fen_string);
    std::string board, side, castling, enpassant, half, full;
    ss >> board >> side >> castling >> enpassant >> half >> full;

    int rank = 7, file = 0;
    for (char symbol : board) {
        if (symbol == '/') { rank--; file = 0; }
        else if (isdigit(symbol)) { file += symbol - '0'; }
        else {
            Color c = isupper(symbol) ? Color::WHITE : Color::BLACK;
            PieceType pt;
            switch (tolower(symbol)) {
                case 'p': pt = PieceType::PAWN; break; case 'n': pt = PieceType::KNIGHT; break;
                case 'b': pt = PieceType::BISHOP; break; case 'r': pt = PieceType::ROOK; break;
                case 'q': pt = PieceType::QUEEN; break; case 'k': pt = PieceType::KING; break;
                default: continue;
            }
            set_piece(*this, static_cast<Square>(rank * 8 + file), pt, c, true);
            file++;
        }
    }
    side_to_move = (side == "w") ? Color::WHITE : Color::BLACK;
    castling_rights = NO_CASTLING;
    for(char c : castling) {
        if (c == 'K') castling_rights = static_cast<CastlingRights>(castling_rights | WHITE_KINGSIDE);
        if (c == 'Q') castling_rights = static_cast<CastlingRights>(castling_rights | WHITE_QUEENSIDE);
        if (c == 'k') castling_rights = static_cast<CastlingRights>(castling_rights | BLACK_KINGSIDE);
        if (c == 'q') castling_rights = static_cast<CastlingRights>(castling_rights | BLACK_QUEENSIDE);
    }
    if (enpassant != "-") {
        int ep_file = enpassant[0] - 'a';
        int ep_rank = enpassant[1] - '1';
        en_passant_sq = static_cast<Square>(ep_rank * 8 + ep_file);
    } else {
        en_passant_sq = SQ_NONE;
    }
    halfmove_clock = std::stoi(half);
    fullmove_number = std::stoi(full);
    hash_key = calculate_hash();
}

bool Position::make_move(Move m) {
    history[history_ply].castling_rights = castling_rights;
    history[history_ply].en_passant_sq = en_passant_sq;
    history[history_ply].halfmove_clock = halfmove_clock;
    history[history_ply].captured_piece = PieceType::NONE;
    history[history_ply].hash_key = hash_key;
    history_ply++;

    const Square from = Moves::get_from(m);
    const Square to = Moves::get_to(m);
    const MoveType type = Moves::get_type(m);
    const Color us = side_to_move;
    const Color them = (us == Color::WHITE) ? Color::BLACK : Color::WHITE;
    const PieceType moved_piece = piece_on_sq[from];

    side_to_move = them;
    en_passant_sq = SQ_NONE;
    if (moved_piece == PieceType::PAWN || type == CAPTURE || type == EN_PASSANT || type >= PROMO_CAPTURE_KNIGHT) halfmove_clock = 0;
    else halfmove_clock++;

    set_piece(*this, from, moved_piece, us, false);
    if (type == CAPTURE || type >= PROMO_CAPTURE_KNIGHT) {
        const PieceType captured = piece_on_sq[to];
        history[history_ply - 1].captured_piece = captured;
        set_piece(*this, to, captured, them, false);
    } else if (type == EN_PASSANT) {
        const Square capture_sq = (us == Color::WHITE) ? static_cast<Square>(to - 8) : static_cast<Square>(to + 8);
        history[history_ply - 1].captured_piece = PieceType::PAWN;
        set_piece(*this, capture_sq, PieceType::PAWN, them, false);
    }
    set_piece(*this, to, moved_piece, us, true);

    if (type == DOUBLE_PAWN_PUSH) en_passant_sq = static_cast<Square>((from + to) / 2);
    else if (type == KING_CASTLE) {
        const Square rook_from = (us == Color::WHITE) ? H1 : H8;
        const Square rook_to = (us == Color::WHITE) ? F1 : F8;
        set_piece(*this, rook_from, PieceType::ROOK, us, false);
        set_piece(*this, rook_to, PieceType::ROOK, us, true);
    } else if (type == QUEEN_CASTLE) {
        const Square rook_from = (us == Color::WHITE) ? A1 : A8;
        const Square rook_to = (us == Color::WHITE) ? D1 : D8;
        set_piece(*this, rook_from, PieceType::ROOK, us, false);
        set_piece(*this, rook_to, PieceType::ROOK, us, true);
    } else if (type >= PROMO_KNIGHT) {
        const PieceType promo_piece = static_cast<PieceType>(((type - PROMO_KNIGHT) % 4) + 1);
        set_piece(*this, to, moved_piece, us, false);
        set_piece(*this, to, promo_piece, us, true);
    }

    if (castling_rights) {
        if (moved_piece == PieceType::KING) {
            if (us == Color::WHITE) castling_rights = static_cast<CastlingRights>(castling_rights & ~(WHITE_KINGSIDE | WHITE_QUEENSIDE));
            else castling_rights = static_cast<CastlingRights>(castling_rights & ~(BLACK_KINGSIDE | BLACK_QUEENSIDE));
        }
        if (from == H1 || to == H1) castling_rights = static_cast<CastlingRights>(castling_rights & ~WHITE_KINGSIDE);
        if (from == A1 || to == A1) castling_rights = static_cast<CastlingRights>(castling_rights & ~WHITE_QUEENSIDE);
        if (from == H8 || to == H8) castling_rights = static_cast<CastlingRights>(castling_rights & ~BLACK_KINGSIDE);
        if (from == A8 || to == A8) castling_rights = static_cast<CastlingRights>(castling_rights & ~BLACK_QUEENSIDE);
    }
    if (is_in_check(us)) { unmake_move(m); return false; }
    return true;
}

void Position::unmake_move(Move m) {
    history_ply--;
    const Square from = Moves::get_from(m);
    const Square to = Moves::get_to(m);
    const MoveType type = Moves::get_type(m);
    const Color us = side_to_move == Color::WHITE ? Color::BLACK : Color::WHITE;
    const Color them = side_to_move;

    castling_rights = history[history_ply].castling_rights;
    en_passant_sq = history[history_ply].en_passant_sq;
    halfmove_clock = history[history_ply].halfmove_clock;
    side_to_move = us;

    PieceType moved_piece = piece_on_sq[to];
    if (type >= PROMO_KNIGHT) {
        moved_piece = PieceType::PAWN;
        set_piece(*this, to, piece_on_sq[to], us, false);
        set_piece(*this, to, PieceType::PAWN, us, true);
    }

    set_piece(*this, to, moved_piece, us, false);
    set_piece(*this, from, moved_piece, us, true);

    if (type == CAPTURE || type >= PROMO_CAPTURE_KNIGHT) {
        set_piece(*this, to, history[history_ply].captured_piece, them, true);
    } else if (type == EN_PASSANT) {
        const Square capture_sq = (us == Color::WHITE) ? static_cast<Square>(to - 8) : static_cast<Square>(to + 8);
        set_piece(*this, capture_sq, PieceType::PAWN, them, true);
    } else if (type == KING_CASTLE) {
        const Square rook_from = (us == Color::WHITE) ? H1 : H8;
        const Square rook_to = (us == Color::WHITE) ? F1 : F8;
        set_piece(*this, rook_to, PieceType::ROOK, us, false);
        set_piece(*this, rook_from, PieceType::ROOK, us, true);
    } else if (type == QUEEN_CASTLE) {
        const Square rook_from = (us == Color::WHITE) ? A1 : A8;
        const Square rook_to = (us == Color::WHITE) ? D1 : D8;
        set_piece(*this, rook_to, PieceType::ROOK, us, false);
        set_piece(*this, rook_from, PieceType::ROOK, us, true);
    }
}

bool Position::is_in_check(Color c) const {
    const Color them = (c == Color::WHITE) ? Color::BLACK : Color::WHITE;
    Bitboard king_bb = piece_bbs[static_cast<int>(c)][static_cast<int>(PieceType::KING)];
    if (!king_bb) return true; // King is captured, which is an invalid state
    Bitboard temp_king_bb = king_bb;
    const Square king_sq = static_cast<Square>(BB::pop_lsb(temp_king_bb));
    return is_square_attacked(king_sq, them);
}

bool Position::is_square_attacked(Square s, Color attacker_color) const {
    const Color defender_color = (attacker_color == Color::WHITE) ? Color::BLACK : Color::WHITE;
    if (Attacks::pawn_attacks[static_cast<int>(defender_color)][s] & piece_bbs[static_cast<int>(attacker_color)][static_cast<int>(PieceType::PAWN)]) return true;
    if (Attacks::knight_attacks[s] & piece_bbs[static_cast<int>(attacker_color)][static_cast<int>(PieceType::KNIGHT)]) return true;
    if (Attacks::king_attacks[s] & piece_bbs[static_cast<int>(attacker_color)][static_cast<int>(PieceType::KING)]) return true;
    const Bitboard occupied = color_bbs[0] | color_bbs[1];
    if (Attacks::get_rook_attacks(s, occupied) & (piece_bbs[static_cast<int>(attacker_color)][static_cast<int>(PieceType::ROOK)] | piece_bbs[static_cast<int>(attacker_color)][static_cast<int>(PieceType::QUEEN)])) return true;
    if (Attacks::get_bishop_attacks(s, occupied) & (piece_bbs[static_cast<int>(attacker_color)][static_cast<int>(PieceType::BISHOP)] | piece_bbs[static_cast<int>(attacker_color)][static_cast<int>(PieceType::QUEEN)])) return true;
    return false;
}

static void set_piece(Position& pos, Square s, PieceType pt, Color c, bool is_add) {
    Bitboard s_bb = 1ULL << s;
    pos.piece_on_sq[s] = is_add ? pt : PieceType::NONE;
    pos.color_on_sq[s] = is_add ? c : Color::NONE;
    if (is_add) {
        pos.piece_bbs[static_cast<int>(c)][static_cast<int>(pt)] |= s_bb;
        pos.color_bbs[static_cast<int>(c)] |= s_bb;
    } else {
        pos.piece_bbs[static_cast<int>(c)][static_cast<int>(pt)] &= ~s_bb;
        pos.color_bbs[static_cast<int>(c)] &= ~s_bb;
    }
}

} // namespace aetherchess
