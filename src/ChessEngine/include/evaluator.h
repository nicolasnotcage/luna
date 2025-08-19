/*
    Chess position evaluator interface.
    Evaluates chess positions based on material, piece placement, 
    pawn structure, king safety, and mobility.

    Author: Nicolas Miller
    Date: 07/09/2025
*/

#ifndef CHESS_ENGINE_EVALUATOR_H
#define CHESS_ENGINE_EVALUATOR_H

#include "types.h"
#include "position.h"

namespace luna 
{

class Evaluator 
{
public:
    Evaluator();
    
    // Main evaluation function
    int evaluate(const Position& position);
    
private:
    // Component evaluations
    int evaluate_material(const Position& pos);
    int evaluate_piece_squares(const Position& pos);
    int evaluate_pawn_structure(const Position& pos);
    int evaluate_king_safety(const Position& pos);
    int evaluate_mobility(const Position& pos);
    int evaluate_piece_bonuses(const Position& pos);
    
    // Helper function for piece-square evaluation
    void evaluate_piece_type_squares(const Position& pos, Color color, PieceType type,
                                     const int table[64], int& score);
    
    // Mobility helper functions
    int count_knight_moves(const Position& pos, Square sq);
    int count_bishop_moves(const Position& pos, Square sq);
    int count_rook_moves(const Position& pos, Square sq);
    
    // Check if only kings remain (draw)
    bool is_only_kings(const Position& pos);
    
    // Piece-square tables
    static const int pawn_table[64];
    static const int knight_table[64];
    static const int bishop_table[64];
    static const int rook_table[64];
    static const int queen_table[64];
    static const int king_middlegame_table[64];
    static const int king_endgame_table[64];
};

} // namespace luna

#endif // CHESS_ENGINE_EVALUATOR_H
