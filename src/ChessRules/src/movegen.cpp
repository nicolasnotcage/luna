/*
    Implementation of static utility MoveGenerator class for 
    use in a chess engine.

    Author: Nicolas Miller
    Date: 06/11/2025
*/

#include "movegen.h"
#include "position.h"
#include "types.h"
#include <vector>

// Generate all pseudo-legal moves for a color
void MoveGenerator::generate_all_moves(const Position& pos, std::vector<Move>& moves, Color color)
{
    generate_pawn_moves(pos, moves, color);
    generate_knight_moves(pos, moves, color);
    generate_bishop_moves(pos, moves, color);
    generate_rook_moves(pos, moves, color);
    generate_queen_moves(pos, moves, color);
    generate_king_moves(pos, moves, color);
    generate_castling_moves(pos, moves, color);
}

// Generate legal moves by filtering pseudo-legal moves
std::vector<Move> MoveGenerator::generate_legal_moves(const Position& pos)
{
    // Special case: if there are only two kings on the board, it's an automatic stalemate
    bool only_kings = true;
    for (int c = 0; c < static_cast<int>(Color::NB); ++c)
    {
        for (int pt = 0; pt < static_cast<int>(PieceType::NB); ++pt)
        {
            if (pt != static_cast<int>(PieceType::King) && pos.pieces(static_cast<Color>(c), static_cast<PieceType>(pt)).count_bits() > 0)
            {
                only_kings = false;
                break;
            }
        }
        if (!only_kings) break;
    }
    
    // If there are only two kings, return an empty list (stalemate)
    if (only_kings)
    {
        return std::vector<Move>();
    }
    
    std::vector<Move> pseudo_legal;
    std::vector<Move> legal;
    
    // Generate all pseudo-legal moves
    generate_all_moves(pos, pseudo_legal, pos.side_to_move());
    
    // Filter out illegal moves (those that leave king in check)
    for (const Move& move : pseudo_legal)
    {
        // Make move on a copy
        Position test_pos = pos;
        test_pos.make_move(move);
        
        // Determine if we're in check after move
        Color enemy_color = (pos.side_to_move() == Color::White) ? Color::Black : Color::White;
        
        // Get king square from the test position after the move
        Square our_king = test_pos.king_square(pos.side_to_move());
        
        // Check if our king is still on board and not in check
        if (!is_square_attacked(test_pos, our_king, enemy_color))
        {
            legal.push_back(move);
        }
    }
    
    return legal;
}

// Knight move generation
void MoveGenerator::generate_knight_moves(const Position& pos, std::vector<Move>& moves, Color color)
{
    Bitboard knights = pos.pieces(color, PieceType::Knight);
    Bitboard own_pieces = pos.occupied_by_color(color);
    
    while (knights.count_bits() > 0)
    {
        Square from = static_cast<Square>(knights.pop_lsb());
        Bitboard attacks = Bitboard::knight_attacks(from);
        
        // Remove squares occupied by own pieces
        attacks &= ~own_pieces;
        
        while (attacks.count_bits() > 0)
        {
            Square to = static_cast<Square>(attacks.pop_lsb());
            
            // Determine move type
            MoveType type = MoveType::Normal;
            if (pos.piece_on(to) != Piece::None)
            {
                type = MoveType::Capture;
            }
            
            moves.push_back(Move(from, to, type));
        }
    }
}

// Bishop move generation
void MoveGenerator::generate_bishop_moves(const Position& pos, std::vector<Move>& moves, Color color)
{
    Bitboard bishops = pos.pieces(color, PieceType::Bishop);
    Bitboard own_pieces = pos.occupied_by_color(color);
    
    while (bishops.count_bits() > 0)
    {
        Square from = static_cast<Square>(bishops.pop_lsb());
        Bitboard attacks = Bitboard::bishop_attacks(from, pos.occupied());
        
        // Remove squares occupied by own pieces
        attacks &= ~own_pieces;
        
        while (attacks.count_bits() > 0)
        {
            Square to = static_cast<Square>(attacks.pop_lsb());
            
            // Determine move type
            MoveType type = MoveType::Normal;
            if (pos.piece_on(to) != Piece::None)
            {
                type = MoveType::Capture;
            }
            
            moves.push_back(Move(from, to, type));
        }
    }
}

// Rook move generation
void MoveGenerator::generate_rook_moves(const Position& pos, std::vector<Move>& moves, Color color)
{
    Bitboard rooks = pos.pieces(color, PieceType::Rook);
    Bitboard own_pieces = pos.occupied_by_color(color);
    
    while (rooks.count_bits() > 0)
    {
        Square from = static_cast<Square>(rooks.pop_lsb());
        Bitboard attacks = Bitboard::rook_attacks(from, pos.occupied());
        
        // Remove squares occupied by own pieces
        attacks &= ~own_pieces;
        
        while (attacks.count_bits() > 0)
        {
            Square to = static_cast<Square>(attacks.pop_lsb());
            
            // Determine move type
            MoveType type = MoveType::Normal;
            if (pos.piece_on(to) != Piece::None)
            {
                type = MoveType::Capture;
            }
            
            moves.push_back(Move(from, to, type));
        }
    }
}

// Queen move generation
void MoveGenerator::generate_queen_moves(const Position& pos, std::vector<Move>& moves, Color color)
{
    Bitboard queens = pos.pieces(color, PieceType::Queen);
    Bitboard own_pieces = pos.occupied_by_color(color);
    
    while (queens.count_bits() > 0)
    {
        Square from = static_cast<Square>(queens.pop_lsb());
        Bitboard attacks = Bitboard::queen_attacks(from, pos.occupied());
        
        // Remove squares occupied by own pieces
        attacks &= ~own_pieces;
        
        while (attacks.count_bits() > 0)
        {
            Square to = static_cast<Square>(attacks.pop_lsb());
            
            // Determine move type
            MoveType type = MoveType::Normal;
            if (pos.piece_on(to) != Piece::None)
            {
                type = MoveType::Capture;
            }
            
            moves.push_back(Move(from, to, type));
        }
    }
}

// King move generation (excluding castling)
void MoveGenerator::generate_king_moves(const Position& pos, std::vector<Move>& moves, Color color)
{
    Bitboard kings = pos.pieces(color, PieceType::King);
    Bitboard own_pieces = pos.occupied_by_color(color);
    
    // There should be exactly one king
    if (kings.count_bits() > 0)
    {
        Square from = static_cast<Square>(kings.pop_lsb());
        Bitboard attacks = Bitboard::king_attacks(from);
        
        // Remove squares occupied by own pieces
        attacks &= ~own_pieces;
        
        while (attacks.count_bits() > 0)
        {
            Square to = static_cast<Square>(attacks.pop_lsb());
            
            // Determine move type
            MoveType type = MoveType::Normal;
            if (pos.piece_on(to) != Piece::None)
            {
                type = MoveType::Capture;
            }
            
            moves.push_back(Move(from, to, type));
        }
    }
}

// Castling move generation
void MoveGenerator::generate_castling_moves(const Position& pos, std::vector<Move>& moves, Color color)
{
    // Check if king is in check - can't castle out of check
    if (pos.is_in_check())
        return;
    
    // Get king square
    Square king_sq = pos.king_square(color);
    
    // Check if king is on the correct starting square
    if ((color == Color::White && king_sq != Square::E1) ||
        (color == Color::Black && king_sq != Square::E8))
        return;
    
    // Check castling rights
    uint8_t castling_rights = pos.castling_rights();
    
    if (color == Color::White)
    {
        // Kingside castling
        if (castling_rights & (1 << static_cast<int>(CastlingRights::White_OO)))
        {
            // Check if squares between king and rook are empty
            if (!pos.occupied().is_bit_set(Square::F1) && !pos.occupied().is_bit_set(Square::G1))
            {
                // Check if king passes through or ends up on an attacked square
                Color enemy = Color::Black;
                if (!is_square_attacked(pos, Square::F1, enemy) && !is_square_attacked(pos, Square::G1, enemy))
                {
                    // Check if rook is actually on H1
                    if (pos.piece_on(Square::H1) == Piece::WhiteRook)
                    {
                        moves.push_back(Move(Square::E1, Square::G1, MoveType::Castle));
                    }
                }
            }
        }
        
        // Queenside castling
        if (castling_rights & (1 << static_cast<int>(CastlingRights::White_OOO)))
        {
            // Check if squares between king and rook are empty
            if (!pos.occupied().is_bit_set(Square::D1) && !pos.occupied().is_bit_set(Square::C1) && !pos.occupied().is_bit_set(Square::B1))
            {
                // Check if king passes through or ends up on an attacked square
                Color enemy = Color::Black;
                if (!is_square_attacked(pos, Square::D1, enemy) && !is_square_attacked(pos, Square::C1, enemy))
                {
                    // Check if rook is actually on A1
                    if (pos.piece_on(Square::A1) == Piece::WhiteRook)
                    {
                        moves.push_back(Move(Square::E1, Square::C1, MoveType::Castle));
                    }
                }
            }
        }
    }
    else  // Black
    {
        // Kingside castling
        if (castling_rights & (1 << static_cast<int>(CastlingRights::Black_OO)))
        {
            // Check if squares between king and rook are empty
            if (!pos.occupied().is_bit_set(Square::F8) && !pos.occupied().is_bit_set(Square::G8))
            {
                // Check if king passes through or ends up on an attacked square
                Color enemy = Color::White;
                if (!is_square_attacked(pos, Square::F8, enemy) && !is_square_attacked(pos, Square::G8, enemy))
                {
                    // Check if rook is actually on H8
                    if (pos.piece_on(Square::H8) == Piece::BlackRook)
                    {
                        moves.push_back(Move(Square::E8, Square::G8, MoveType::Castle));
                    }
                }
            }
        }
        
        // Queenside castling
        if (castling_rights & (1 << static_cast<int>(CastlingRights::Black_OOO)))
        {
            // Check if squares between king and rook are empty
            if (!pos.occupied().is_bit_set(Square::D8) && !pos.occupied().is_bit_set(Square::C8) && !pos.occupied().is_bit_set(Square::B8))
            {
                // Check if king passes through or ends up on an attacked square
                Color enemy = Color::White;
                if (!is_square_attacked(pos, Square::D8, enemy) && !is_square_attacked(pos, Square::C8, enemy))
                {
                    // Check if rook is actually on A8
                    if (pos.piece_on(Square::A8) == Piece::BlackRook)
                    {
                        moves.push_back(Move(Square::E8, Square::C8, MoveType::Castle));
                    }
                }
            }
        }
    }
}

// Generate pawn moves
void MoveGenerator::generate_pawn_moves(const Position& pos, std::vector<Move>& moves, Color color)
{
    Bitboard pawns = pos.pieces(color, PieceType::Pawn);
    Bitboard own_pieces = pos.occupied_by_color(color);
    Bitboard enemy_pieces = pos.occupied_by_color(color == Color::White ? Color::Black : Color::White);
    
    // Direction pawns move
    Direction push_dir = (color == Color::White) ? Direction::North : Direction::South;
    int push_offset = static_cast<int>(push_dir);
    
    // Rank where pawns start and promote
    Rank start_rank = (color == Color::White) ? Rank::Two : Rank::Seven;
    Rank promo_rank = (color == Color::White) ? Rank::Eight : Rank::One;
    
    while (pawns.count_bits() > 0)
    {
        Square from = static_cast<Square>(pawns.pop_lsb());
        Rank from_rank = rank_of(from);
        
        // Single push
        Square to = static_cast<Square>(static_cast<int>(from) + push_offset);
        if (to >= Square::A1 && to <= Square::H8 && !pos.occupied().is_bit_set(to))
        {
            if (rank_of(to) == promo_rank)
            {
                // Promotion moves (generate all 4 promotion pieces)
                moves.push_back(Move(from, to, MoveType::Promotion, make_piece(color, PieceType::Queen)));
                moves.push_back(Move(from, to, MoveType::Promotion, make_piece(color, PieceType::Rook)));
                moves.push_back(Move(from, to, MoveType::Promotion, make_piece(color, PieceType::Bishop)));
                moves.push_back(Move(from, to, MoveType::Promotion, make_piece(color, PieceType::Knight)));
            }
            else
            {
                moves.push_back(Move(from, to, MoveType::Normal));
                
                // Double push from starting position
                if (from_rank == start_rank)
                {
                    Square double_to = static_cast<Square>(static_cast<int>(from) + 2 * push_offset);
                    if (!pos.occupied().is_bit_set(double_to))
                    {
                        moves.push_back(Move(from, double_to, MoveType::Normal));
                    }
                }
            }
        }
        
        // Captures
        Bitboard attacks = Bitboard::pawn_attacks(from, color);
        attacks &= enemy_pieces;  // Can only capture enemy pieces
        
        while (attacks.count_bits() > 0)
        {
            Square cap_sq = static_cast<Square>(attacks.pop_lsb());
            
            if (rank_of(cap_sq) == promo_rank)
            {
                // Capture with promotion
                moves.push_back(Move(from, cap_sq, MoveType::Promotion, make_piece(color, PieceType::Queen)));
                moves.push_back(Move(from, cap_sq, MoveType::Promotion, make_piece(color, PieceType::Rook)));
                moves.push_back(Move(from, cap_sq, MoveType::Promotion, make_piece(color, PieceType::Bishop)));
                moves.push_back(Move(from, cap_sq, MoveType::Promotion, make_piece(color, PieceType::Knight)));
            }
            else
            {
                moves.push_back(Move(from, cap_sq, MoveType::Capture));
            }
        }
        
        // En passant captures
        if (pos.en_passant_square() != Square::None)
        {
            Bitboard ep_attacks = Bitboard::pawn_attacks(from, color);
            if (ep_attacks.is_bit_set(pos.en_passant_square()))
            {
                moves.push_back(Move(from, pos.en_passant_square(), MoveType::EnPassant));
            }
        }
    }
}

// Check if a square is attacked by a given color
bool MoveGenerator::is_square_attacked(const Position& pos, Square square, Color by_color)
{
    // For pawns, we need to check squares from which enemy pawns can attack this square
    // White pawns attack diagonally upward (NE, NW), Black pawns attack diagonally downward (SE, SW)
    if (by_color == Color::White)
    {
        // Check if white pawns can attack this square
        // White pawns would be on SW or SE of this square
        if (rank_of(square) > Rank::One)  // Can't be attacked by white pawns on rank 1
        {
            // Check SW square
            if (file_of(square) > File::A)
            {
                Square sw = static_cast<Square>(static_cast<int>(square) - 9);
                if (pos.piece_on(sw) == Piece::WhitePawn)
                    return true;
            }
            // Check SE square
            if (file_of(square) < File::H)
            {
                Square se = static_cast<Square>(static_cast<int>(square) - 7);
                if (pos.piece_on(se) == Piece::WhitePawn)
                    return true;
            }
        }
    }
    else  // Black
    {
        // Check if black pawns can attack this square
        // Black pawns would be on NW or NE of this square
        if (rank_of(square) < Rank::Eight)  // Can't be attacked by black pawns on rank 8
        {
            // Check NW square
            if (file_of(square) > File::A)
            {
                Square nw = static_cast<Square>(static_cast<int>(square) + 7);
                if (pos.piece_on(nw) == Piece::BlackPawn)
                    return true;
            }
            // Check NE square
            if (file_of(square) < File::H)
            {
                Square ne = static_cast<Square>(static_cast<int>(square) + 9);
                if (pos.piece_on(ne) == Piece::BlackPawn)
                    return true;
            }
        }
    }
    
    // Check knight attacks
    Bitboard knight_attackers = Bitboard::knight_attacks(square);
    if ((knight_attackers & pos.pieces(by_color, PieceType::Knight)).count_bits() > 0)
        return true;
    
    // Check king attacks
    Bitboard king_attackers = Bitboard::king_attacks(square);
    if ((king_attackers & pos.pieces(by_color, PieceType::King)).count_bits() > 0)
        return true;
    
    // Check bishop/queen attacks (diagonal)
    Bitboard bishop_attackers = Bitboard::bishop_attacks(square, pos.occupied());
    if ((bishop_attackers & (pos.pieces(by_color, PieceType::Bishop) | pos.pieces(by_color, PieceType::Queen))).count_bits() > 0)
        return true;
    
    // Check rook/queen attacks (straight)
    Bitboard rook_attackers = Bitboard::rook_attacks(square, pos.occupied());
    if ((rook_attackers & (pos.pieces(by_color, PieceType::Rook) | pos.pieces(by_color, PieceType::Queen))).count_bits() > 0)
        return true;
    
    return false;
}

// Get all attackers to a square
Bitboard MoveGenerator::get_attackers_to(const Position& pos, Square square, Color by_color)
{
    Bitboard attackers;
    
    // Pawn attackers
    if (by_color == Color::White)
    {
        if (rank_of(square) > Rank::One)
        {
            if (file_of(square) > File::A)
            {
                Square sw = static_cast<Square>(static_cast<int>(square) - 9);
                if (pos.piece_on(sw) == Piece::WhitePawn)
                    attackers.set_bit(sw);
            }
            if (file_of(square) < File::H)
            {
                Square se = static_cast<Square>(static_cast<int>(square) - 7);
                if (pos.piece_on(se) == Piece::WhitePawn)
                    attackers.set_bit(se);
            }
        }
    }
    else
    {
        if (rank_of(square) < Rank::Eight)
        {
            if (file_of(square) > File::A)
            {
                Square nw = static_cast<Square>(static_cast<int>(square) + 7);
                if (pos.piece_on(nw) == Piece::BlackPawn)
                    attackers.set_bit(nw);
            }
            if (file_of(square) < File::H)
            {
                Square ne = static_cast<Square>(static_cast<int>(square) + 9);
                if (pos.piece_on(ne) == Piece::BlackPawn)
                    attackers.set_bit(ne);
            }
        }
    }
    
    // Knight attackers
    attackers |= Bitboard::knight_attacks(square) & pos.pieces(by_color, PieceType::Knight);
    
    // King attackers
    attackers |= Bitboard::king_attacks(square) & pos.pieces(by_color, PieceType::King);
    
    // Bishop/Queen attackers (diagonal)
    Bitboard bishop_attacks = Bitboard::bishop_attacks(square, pos.occupied());
    attackers |= bishop_attacks & (pos.pieces(by_color, PieceType::Bishop) | pos.pieces(by_color, PieceType::Queen));
    
    // Rook/Queen attackers (straight)
    Bitboard rook_attacks = Bitboard::rook_attacks(square, pos.occupied());
    attackers |= rook_attacks & (pos.pieces(by_color, PieceType::Rook) | pos.pieces(by_color, PieceType::Queen));
    
    return attackers;
}