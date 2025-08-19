/*
    A pure static utility class dedicated to move generation in a chess engine.

    Author: Nicolas Miller
    Date: 06/11/2025
*/

#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "types.h"
#include "bitboard.h"
#include <vector>

class Position;

class MoveGenerator
{
public:
    // Main interface
    static void generate_all_moves(const Position& pos, std::vector<Move>& moves, Color color);
    static std::vector<Move> generate_legal_moves(const Position& pos);
    
    // Piece-specific move generation
    static void generate_pawn_moves(const Position& pos, std::vector<Move>& moves, Color color);
    static void generate_knight_moves(const Position& pos, std::vector<Move>& moves, Color color);
    static void generate_bishop_moves(const Position& pos, std::vector<Move>& moves, Color color);
    static void generate_rook_moves(const Position& pos, std::vector<Move>& moves, Color color);
    static void generate_queen_moves(const Position& pos, std::vector<Move>& moves, Color color);
    static void generate_king_moves(const Position& pos, std::vector<Move>& moves, Color color);
    static void generate_castling_moves(const Position& pos, std::vector<Move>& moves, Color color);
    
    // Attack detection utilities
    static bool is_square_attacked(const Position& pos, Square square, Color by_color);
    static Bitboard get_attackers_to(const Position& pos, Square square, Color by_color);

private:
    // Prevent instantiation
    MoveGenerator() = delete;
    MoveGenerator(const MoveGenerator&) = delete;
    MoveGenerator& operator=(const MoveGenerator&) = delete;
    MoveGenerator(MoveGenerator&&) = delete;
    MoveGenerator& operator=(MoveGenerator&&) = delete;
    ~MoveGenerator() = delete;
};

#endif // MOVEGEN_H