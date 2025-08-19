/*
    Implementation of chess position evaluation.

    Author: Nicolas Miller
    Date: 07/09/2025
*/

#include "evaluator.h"
#include "constants.h"
#include <algorithm>

namespace luna {

// Piece-square tables (from white's perspective)
// Positive values are good for the piece, negative are bad
// Values are mirrored for black pieces.
//
// Note: These PSTs are taken from the chess programming wiki and are considered a 
// starting point; there's a lot of opportunity here for fine tuning

const int Evaluator::pawn_table[64] = 
{
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

const int Evaluator::knight_table[64] = 
{
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

const int Evaluator::bishop_table[64] = 
{
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

const int Evaluator::rook_table[64] = 
{
     0,  0,  0,  0,  0,  0,  0,  0,
     5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     0,  0,  0,  5,  5,  0,  0,  0
};

const int Evaluator::queen_table[64] = 
{
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

const int Evaluator::king_middlegame_table[64] = 
{
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

const int Evaluator::king_endgame_table[64] = 
{
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
};

Evaluator::Evaluator() {}

int Evaluator::evaluate(const Position& position) 
{
    // Return 0 for drawn positions (only kings)
    if (is_only_kings(position)) return 0;
    
    int score = 0;
    
    // Evaluate all components from white's perspective
    score += evaluate_material(position);
    score += evaluate_piece_squares(position);
    score += evaluate_pawn_structure(position);
    score += evaluate_king_safety(position);
    score += evaluate_mobility(position);
    score += evaluate_piece_bonuses(position);
    
    // IMPORTANT: Return score from side to move's perspective for negamax
    return position.side_to_move() == Color::White ? score : -score;
}

int Evaluator::evaluate_material(const Position& pos) 
{
    // Initialize material count
    int white_material = 0;
    int black_material = 0;
    
    // Count material for white
    white_material += pos.pieces(Color::White, PieceType::Pawn).count_bits() * PAWN_VALUE;
    white_material += pos.pieces(Color::White, PieceType::Knight).count_bits() * KNIGHT_VALUE;
    white_material += pos.pieces(Color::White, PieceType::Bishop).count_bits() * BISHOP_VALUE;
    white_material += pos.pieces(Color::White, PieceType::Rook).count_bits() * ROOK_VALUE;
    white_material += pos.pieces(Color::White, PieceType::Queen).count_bits() * QUEEN_VALUE;
    
    // Count material for black
    black_material += pos.pieces(Color::Black, PieceType::Pawn).count_bits() * PAWN_VALUE;
    black_material += pos.pieces(Color::Black, PieceType::Knight).count_bits() * KNIGHT_VALUE;
    black_material += pos.pieces(Color::Black, PieceType::Bishop).count_bits() * BISHOP_VALUE;
    black_material += pos.pieces(Color::Black, PieceType::Rook).count_bits() * ROOK_VALUE;
    black_material += pos.pieces(Color::Black, PieceType::Queen).count_bits() * QUEEN_VALUE;
    
    return white_material - black_material;
}

int Evaluator::evaluate_piece_squares(const Position& pos) 
{
    // Initialize scores
    int white_score = 0;
    int black_score = 0;
    
    // Determine game phase. Just using material count right now but could attempt some
    // other ways of determining phase and see how that impacts engine behavior.
    int total_material = 0;
    total_material += (pos.pieces(Color::White, PieceType::Queen).count_bits() + 
                      pos.pieces(Color::Black, PieceType::Queen).count_bits()) * QUEEN_VALUE;
    total_material += (pos.pieces(Color::White, PieceType::Rook).count_bits() + 
                      pos.pieces(Color::Black, PieceType::Rook).count_bits()) * ROOK_VALUE;
    total_material += (pos.pieces(Color::White, PieceType::Bishop).count_bits() + 
                      pos.pieces(Color::Black, PieceType::Bishop).count_bits()) * BISHOP_VALUE;
    total_material += (pos.pieces(Color::White, PieceType::Knight).count_bits() + 
                      pos.pieces(Color::Black, PieceType::Knight).count_bits()) * KNIGHT_VALUE;
    
    // Determine if we're in endgame based on material count vs. threshold 
    bool is_endgame = total_material < ENDGAME_MATERIAL_THRESHOLD;
    
    // Evaluate each piece type for white
    evaluate_piece_type_squares(pos, Color::White, PieceType::Pawn, pawn_table, white_score);
    evaluate_piece_type_squares(pos, Color::White, PieceType::Knight, knight_table, white_score);
    evaluate_piece_type_squares(pos, Color::White, PieceType::Bishop, bishop_table, white_score);
    evaluate_piece_type_squares(pos, Color::White, PieceType::Rook, rook_table, white_score);
    evaluate_piece_type_squares(pos, Color::White, PieceType::Queen, queen_table, white_score);
    
    // Evaluate each piece type for black
    evaluate_piece_type_squares(pos, Color::Black, PieceType::Pawn, pawn_table, black_score);
    evaluate_piece_type_squares(pos, Color::Black, PieceType::Knight, knight_table, black_score);
    evaluate_piece_type_squares(pos, Color::Black, PieceType::Bishop, bishop_table, black_score);
    evaluate_piece_type_squares(pos, Color::Black, PieceType::Rook, rook_table, black_score);
    evaluate_piece_type_squares(pos, Color::Black, PieceType::Queen, queen_table, black_score);
    
    // Evaluate kings based on game state
    const int* king_table = is_endgame ? king_endgame_table : king_middlegame_table;
    evaluate_piece_type_squares(pos, Color::White, PieceType::King, king_table, white_score);
    evaluate_piece_type_squares(pos, Color::Black, PieceType::King, king_table, black_score);
    
    return white_score - black_score;
}

void Evaluator::evaluate_piece_type_squares(const Position& pos, Color color, PieceType type,
                                            const int table[64], int& score) 
{
    // Get copy of bitboard for colored piece
    Bitboard pieces = pos.pieces(color, type);
    
    // Add scores for each piece position on the board
    while (pieces.count_bits() > 0) 
    {
        Square sq = static_cast<Square>(pieces.pop_lsb());
        int table_index = static_cast<int>(sq);
        
        // For black pieces, mirror the table vertically
        if (color == Color::Black) 
        {
            int rank = table_index / 8;
            int file = table_index % 8;
            table_index = (7 - rank) * 8 + file;
        }
        
        score += table[table_index];
    }
}

int Evaluator::evaluate_pawn_structure(const Position& pos) 
{
    // Initialize scores
    int white_score = 0;
    int black_score = 0;
    
    // Evaluate white pawns
    Bitboard white_pawns = pos.pieces(Color::White, PieceType::Pawn);
    Bitboard white_pawns_copy = white_pawns;
    
    while (white_pawns_copy.count_bits() > 0) {
        Square pawn_sq = static_cast<Square>(white_pawns_copy.pop_lsb());
        File pawn_file = file_of(pawn_sq);
        
        // Doubled pawns penalty
        Bitboard file_pawns = white_pawns & Bitboard(pawn_file);
        if (file_pawns.count_bits() > 1) white_score -= DOUBLED_PAWN_PENALTY;
        
        // Isolated pawns penalty
        bool has_neighbor = false;
        if (static_cast<int>(pawn_file) > 0) 
        {
            File left_file = static_cast<File>(static_cast<int>(pawn_file) - 1);
            if ((white_pawns & Bitboard(left_file)).count_bits() > 0) 
                has_neighbor = true;
        }
        if (static_cast<int>(pawn_file) < 7) 
        {
            File right_file = static_cast<File>(static_cast<int>(pawn_file) + 1);
            if ((white_pawns & Bitboard(right_file)).count_bits() > 0)
                has_neighbor = true;
        }
        if (!has_neighbor)
            white_score -= ISOLATED_PAWN_PENALTY;
        
        // Passed pawns bonus
        Rank pawn_rank = rank_of(pawn_sq);
        bool is_passed = true;
        for (int r = static_cast<int>(pawn_rank) + 1; r < 8; r++) 
        {
            Square check_sq = make_square(pawn_file, static_cast<Rank>(r));
            if (pos.piece_on(check_sq) != Piece::None && 
                type_of(pos.piece_on(check_sq)) == PieceType::Pawn &&
                color_of(pos.piece_on(check_sq)) == Color::Black) 
                {
                    is_passed = false;
                    break;
                }
        }
        if (is_passed) 
            white_score += PASSED_PAWN_BONUS * static_cast<int>(pawn_rank);
    }
    
    // Evaluate black pawns
    Bitboard black_pawns = pos.pieces(Color::Black, PieceType::Pawn);
    Bitboard black_pawns_copy = black_pawns;
    
    while (black_pawns_copy.count_bits() > 0) 
    {
        Square pawn_sq = static_cast<Square>(black_pawns_copy.pop_lsb());
        File pawn_file = file_of(pawn_sq);
        
        // Doubled pawns penalty
        Bitboard file_pawns = black_pawns & Bitboard(pawn_file);
        if (file_pawns.count_bits() > 1) black_score -= DOUBLED_PAWN_PENALTY;
        
        // Isolated pawns penalty
        bool has_neighbor = false;
        if (static_cast<int>(pawn_file) > 0) 
        {
            File left_file = static_cast<File>(static_cast<int>(pawn_file) - 1);
            if ((black_pawns & Bitboard(left_file)).count_bits() > 0) 
                has_neighbor = true;    
        }
        if (static_cast<int>(pawn_file) < 7) 
        {
            File right_file = static_cast<File>(static_cast<int>(pawn_file) + 1);
            if ((black_pawns & Bitboard(right_file)).count_bits() > 0)
                has_neighbor = true;
        }
        if (!has_neighbor)
            black_score -= ISOLATED_PAWN_PENALTY;
        
        // Passed pawns bonus (for black, check ranks below current position)
        Rank pawn_rank = rank_of(pawn_sq);
        bool is_passed = true;
        for (int r = static_cast<int>(pawn_rank) - 1; r >= 0; r--) 
        {
            Square check_sq = make_square(pawn_file, static_cast<Rank>(r));
            if (pos.piece_on(check_sq) != Piece::None && 
                type_of(pos.piece_on(check_sq)) == PieceType::Pawn &&
                color_of(pos.piece_on(check_sq)) == Color::White) 
                {
                    is_passed = false;
                    break;
                }
        }
        if (is_passed) 
        {
            // For black pawns, bonus increases as they get closer to rank 1 (promotion)
            black_score += PASSED_PAWN_BONUS * (7 - static_cast<int>(pawn_rank));
        }
    }
    
    return white_score - black_score;
}

int Evaluator::evaluate_king_safety(const Position& pos) 
{
    // Initialize scores
    int white_score = 0;
    int black_score = 0;
    
    // Simple king safety: bonus for castling rights and pawn shield
    if (pos.castling_rights() & ((1 << static_cast<int>(CastlingRights::White_OO)) | 
                                 (1 << static_cast<int>(CastlingRights::White_OOO)))) 
    {
        white_score += CASTLING_RIGHTS_BONUS;
    }
    
    if (pos.castling_rights() & ((1 << static_cast<int>(CastlingRights::Black_OO)) | 
                                 (1 << static_cast<int>(CastlingRights::Black_OOO)))) 
    {
        black_score += CASTLING_RIGHTS_BONUS;
    }
    
    // Check for pawn shield
    Square white_king = pos.king_square(Color::White);
    Square black_king = pos.king_square(Color::Black);
    
    // Bonus for pawns in front of king
    if (rank_of(white_king) == Rank::One) 
    {
        File king_file = file_of(white_king);
        for (int f = std::max(0, static_cast<int>(king_file) - 1); f <= std::min(7, static_cast<int>(king_file) + 1); f++) 
        {
            Square pawn_sq = make_square(static_cast<File>(f), Rank::Two);
            if (pos.piece_on(pawn_sq) == Piece::WhitePawn)
                white_score += KING_PAWN_SHIELD_BONUS;
        }
    }
    
    // Similar for black king (on rank 8)
    if (rank_of(black_king) == Rank::Eight) 
    {
        File king_file = file_of(black_king);
        for (int f = std::max(0, static_cast<int>(king_file) - 1); f <= std::min(7, static_cast<int>(king_file) + 1); f++) 
        {
            Square pawn_sq = make_square(static_cast<File>(f), Rank::Seven);
            if (pos.piece_on(pawn_sq) == Piece::BlackPawn) 
                black_score += KING_PAWN_SHIELD_BONUS;  
        }
    }
    
    return white_score - black_score;
}

// We approximate mobility by counting piece attacks/moves rather
// than generating all legal moves. 
int Evaluator::evaluate_mobility(const Position& pos) 
{
    // Initialize mobility scores
    int white_mobility = 0;
    int black_mobility = 0;
    
    // Count knight mobility
    Bitboard white_knights = pos.pieces(Color::White, PieceType::Knight);
    Bitboard black_knights = pos.pieces(Color::Black, PieceType::Knight);
    
    while (white_knights.count_bits() > 0) 
    {
        Square sq = static_cast<Square>(white_knights.pop_lsb());
        white_mobility += count_knight_moves(pos, sq);
    }
    
    while (black_knights.count_bits() > 0) 
    {
        Square sq = static_cast<Square>(black_knights.pop_lsb());
        black_mobility += count_knight_moves(pos, sq);
    }
    
    // Count bishop mobility
    Bitboard white_bishops = pos.pieces(Color::White, PieceType::Bishop);
    Bitboard black_bishops = pos.pieces(Color::Black, PieceType::Bishop);
    
    while (white_bishops.count_bits() > 0) 
    {
        Square sq = static_cast<Square>(white_bishops.pop_lsb());
        white_mobility += count_bishop_moves(pos, sq);
    }
    
    while (black_bishops.count_bits() > 0) 
    {
        Square sq = static_cast<Square>(black_bishops.pop_lsb());
        black_mobility += count_bishop_moves(pos, sq);
    }
    
    // Count rook mobility
    Bitboard white_rooks = pos.pieces(Color::White, PieceType::Rook);
    Bitboard black_rooks = pos.pieces(Color::Black, PieceType::Rook);
    
    while (white_rooks.count_bits() > 0) 
    {
        Square sq = static_cast<Square>(white_rooks.pop_lsb());
        white_mobility += count_rook_moves(pos, sq);
    }
    
    while (black_rooks.count_bits() > 0) 
    {
        Square sq = static_cast<Square>(black_rooks.pop_lsb());
        black_mobility += count_rook_moves(pos, sq);
    }
    
    // Weight mobility
    int mobility_score = (white_mobility - black_mobility) * MOBILITY_SCORE_MULTIPLIER;
    
    // Center control bonus
    const Square center_squares[] = {Square::D4, Square::E4, Square::D5, Square::E5};
    for (Square sq : center_squares) 
    {
        Piece p = pos.piece_on(sq);
        if (p != Piece::None) 
        {
            if (color_of(p) == Color::White) mobility_score += CENTER_CONTROL_BONUS;
            else mobility_score -= CENTER_CONTROL_BONUS;
        }
    }
    
    return mobility_score;
}

int Evaluator::evaluate_piece_bonuses(const Position& pos)
{
    int white_score = 0;
    int black_score = 0;

    // Bishop pair bonus
    if (pos.pieces(Color::White, PieceType::Bishop).count_bits() >= 2)
        white_score += BISHOP_PAIR_BONUS;
    if (pos.pieces(Color::Black, PieceType::Bishop).count_bits() >= 2)
        black_score += BISHOP_PAIR_BONUS;

    // Rook on seventh rank bonus
    Bitboard white_rooks = pos.pieces(Color::White, PieceType::Rook);
    while (white_rooks.count_bits() > 0)
    {
        Square rook_sq = static_cast<Square>(white_rooks.pop_lsb());
        if (rank_of(rook_sq) == Rank::Seven)
            white_score += ROOK_ON_SEVENTH_BONUS;
    }

    Bitboard black_rooks = pos.pieces(Color::Black, PieceType::Rook);
    while (black_rooks.count_bits() > 0)
    {
        Square rook_sq = static_cast<Square>(black_rooks.pop_lsb());
        if (rank_of(rook_sq) == Rank::Two)
            black_score += ROOK_ON_SEVENTH_BONUS;
    }

    // Rook on open file bonus
    white_rooks = pos.pieces(Color::White, PieceType::Rook);
    while (white_rooks.count_bits() > 0)
    {
        Square rook_sq = static_cast<Square>(white_rooks.pop_lsb());
        File rook_file = file_of(rook_sq);
        if ((pos.pieces(Color::White, PieceType::Pawn) & Bitboard(rook_file)).count_bits() == 0 &&
            (pos.pieces(Color::Black, PieceType::Pawn) & Bitboard(rook_file)).count_bits() == 0)
        {
            white_score += ROOK_ON_OPEN_FILE_BONUS;
        }
    }

    black_rooks = pos.pieces(Color::Black, PieceType::Rook);
    while (black_rooks.count_bits() > 0)
    {
        Square rook_sq = static_cast<Square>(black_rooks.pop_lsb());
        File rook_file = file_of(rook_sq);
        if ((pos.pieces(Color::White, PieceType::Pawn) & Bitboard(rook_file)).count_bits() == 0 &&
            (pos.pieces(Color::Black, PieceType::Pawn) & Bitboard(rook_file)).count_bits() == 0)
        {
            black_score += ROOK_ON_OPEN_FILE_BONUS;
        }
    }

    return white_score - black_score;
}

bool Evaluator::is_only_kings(const Position& pos) 
{
    // Check if there are only two kings on the board
    for (int c = 0; c < static_cast<int>(Color::NB); ++c) 
    {
        for (int pt = 0; pt < static_cast<int>(PieceType::NB); ++pt) 
        {
            if (pt != static_cast<int>(PieceType::King) && 
                pos.pieces(static_cast<Color>(c), static_cast<PieceType>(pt)).count_bits() > 0) 
                {
                    return false;
                }
        }
    }
    return true;
}

int Evaluator::count_knight_moves(const Position& pos, Square sq) 
{
    // Use existing Bitboard knight attack function
    Bitboard attacks = Bitboard::knight_attacks(sq);
    
    // Remove squares occupied by own pieces
    Color piece_color = color_of(pos.piece_on(sq));
    Bitboard own_pieces = pos.occupied_by_color(piece_color);
    attacks &= ~own_pieces;
    
    return attacks.count_bits();
}

int Evaluator::count_bishop_moves(const Position& pos, Square sq) 
{
    // Use existing Bitboard bishop attack function
    Bitboard attacks = Bitboard::bishop_attacks(sq, pos.occupied());
    
    // Remove squares occupied by own pieces
    Color piece_color = color_of(pos.piece_on(sq));
    Bitboard own_pieces = pos.occupied_by_color(piece_color);
    attacks &= ~own_pieces;
    
    return attacks.count_bits();
}

int Evaluator::count_rook_moves(const Position& pos, Square sq) 
{
    // Use existing Bitboard rook attack function
    Bitboard attacks = Bitboard::rook_attacks(sq, pos.occupied());
    
    // Remove squares occupied by own pieces
    Color piece_color = color_of(pos.piece_on(sq));
    Bitboard own_pieces = pos.occupied_by_color(piece_color);
    attacks &= ~own_pieces;
    
    return attacks.count_bits();
}

} // namespace luna