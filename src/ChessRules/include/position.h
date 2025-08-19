/*
    Position class for chess engine - represents a complete chess position
    including piece placement, side to move, castling rights, en passant, etc.
        
    Author: Nicolas Miller
    Date: 06/11/2025
*/

#ifndef POSITION_H
#define POSITION_H

#include "types.h"
#include "bitboard.h"
#include <string>
#include <vector>

class MoveGenerator;

class Position
{
public:
    // Constructor - creates starting position by default
    Position();
    
    // Load position from FEN string
    bool load_fen(const std::string& fen);
    
    // Get FEN string representation of current position
    std::string to_fen() const;
    
    // Move generation; delegates to MoveGenerator
    std::vector<Move> generate_legal_moves() const;
    
    // Make a move (updates the position and adds to history)
    void make_move(const Move& move);
    
    // Undo the last move (reverts to previous position)
    void undo_move();
    
    // Undo multiple moves
    void undo_moves(int count);
    
    // Check if a square is attacked by a given color; delegates to MoveGenerator
    bool is_square_attacked(Square square, Color by_color) const;
    
    // Check if current side is in check
    bool is_in_check() const;
    
    // Get piece on a given square
    Piece piece_on(Square square) const;
    
    // Utility functions
    Color side_to_move() const { return side_to_move_; }
    void print_board() const;
    
    // Helper to get king square
    Square king_square(Color color) const;
    
    // Get move history
    const std::vector<Move>& move_history() const { return move_history_; }
    size_t move_count() const { return move_history_.size(); }
    
    // Clear move history
    void clear_history() { move_history_.clear(); }
    
    // Hash key access
    uint64_t hash_key() const { return hash_key_; }
    
    // Getters for MoveGenerator
    Bitboard pieces(Color color, PieceType type) const 
    {
        return pieces_[static_cast<int>(color)][static_cast<int>(type)];
    }
    
    Bitboard occupied() const { return occupied_; }
    
    Bitboard occupied_by_color(Color color) const 
    { 
        return occupied_by_color_[static_cast<int>(color)]; 
    }
    
    Square en_passant_square() const { return en_passant_square_; }
    uint8_t castling_rights() const { return castling_rights_; }
    int halfmove_clock() const { return halfmove_clock_; }
    int fullmove_number() const { return fullmove_number_; }
    
    // Friend class
    friend class MoveGenerator;
    
private:
    // Bitboards for each piece type and color
    Bitboard pieces_[static_cast<int>(Color::NB)][static_cast<int>(PieceType::NB)];
    
    // Bitboards for all pieces by color
    Bitboard occupied_by_color_[static_cast<int>(Color::NB)];
    
    // Bitboard for all occupied squares
    Bitboard occupied_;
    
    // Array representation for faster piece lookup
    Piece board_[static_cast<int>(Square::NB)];
    
    // Game state
    Color side_to_move_;
    uint8_t castling_rights_;  // Bit flags for castling rights
    Square en_passant_square_;
    int halfmove_clock_;       // For 50-move rule
    int fullmove_number_;
    
    // Move history stack
    std::vector<Move> move_history_;
    
    // Zobrist hash key for current position
    uint64_t hash_key_;
    
    // Update bitboards after a move
    void update_bitboards();
    
    // Helper function for opposite color
    static Color opposite_color(Color c) 
    {
        return (c == Color::White) ? Color::Black : Color::White;
    }
};

#endif // POSITION_H
