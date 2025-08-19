/*
    Zobrist hashing implementation for chess positions.
    Provides fast incremental hash updates for transposition tables.

    Author: Nicolas Miller
    Date: 08/12/2025
*/

#ifndef CHESS_ENGINE_ZOBRIST_H
#define CHESS_ENGINE_ZOBRIST_H

#include "types.h"
#include <cstdint>

class Position;

namespace luna 
{

class ZobristHash 
{
public:
    // Initialize the Zobrist keys (call once at startup)
    static void initialize();
    
    // Generate hash for a complete position
    static uint64_t hash_position(const Position& pos);
    
    // Update hash incrementally when making a move
    static uint64_t update_hash_make_move(uint64_t current_hash, const Position& pos, const Move& move);
    
    // Individual hash components for manual updates
    static uint64_t piece_hash(Piece piece, Square square);
    static uint64_t castling_hash(uint8_t castling_rights);
    static uint64_t en_passant_hash(Square en_passant_square);
    static uint64_t side_to_move_hash();
    
private:
    // Zobrist keys
    static uint64_t piece_keys_[64][12];    // [square][piece]
    static uint64_t castling_keys_[16];     // [castling_rights]
    static uint64_t en_passant_keys_[8];    // [file]
    static uint64_t side_to_move_key_;
    
    static bool initialized_;
    
    // Helper to get piece index for array lookup
    static int piece_index(Piece piece);
};

} // namespace luna

#endif // CHESS_ENGINE_ZOBRIST_H
