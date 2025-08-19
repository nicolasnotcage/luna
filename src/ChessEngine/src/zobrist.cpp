/*
    Implementation of Zobrist hashing for chess positions.
    Provides fast incremental hash updates for transposition tables.

    Author: Nicolas Miller
    Date: 08/12/2025
*/

#include "zobrist.h"
#include "position.h"
#include <random>
#include <cassert>
#include <iostream>

namespace luna 
{

// Static member definitions
uint64_t ZobristHash::piece_keys_[64][12];
uint64_t ZobristHash::castling_keys_[16];
uint64_t ZobristHash::en_passant_keys_[8];
uint64_t ZobristHash::side_to_move_key_;
bool ZobristHash::initialized_ = false;

void ZobristHash::initialize() 
{
    if (initialized_) return;
    
    // Use a fixed seed for reproducible hashes across runs
    std::mt19937_64 rng(0x1234567890ABCDEFULL);
    
    // Initialize piece keys
    for (int square = 0; square < 64; ++square) 
    {
        for (int piece = 0; piece < 12; ++piece) 
        {
            piece_keys_[square][piece] = rng();
        }
    }
    
    // Initialize castling keys
    for (int rights = 0; rights < 16; ++rights) 
    {
        castling_keys_[rights] = rng();
    }
    
    // Initialize en passant keys (one for each file)
    for (int file = 0; file < 8; ++file) 
    {
        en_passant_keys_[file] = rng();
    }
    
    // Initialize side to move key
    side_to_move_key_ = rng();
    
    initialized_ = true;
}

uint64_t ZobristHash::hash_position(const Position& pos) 
{
    assert(initialized_);

    uint64_t hash = 0;
    
    // Hash all pieces on the board
    for (int square = 0; square < 64; ++square) 
    {
        Piece piece = pos.piece_on(static_cast<Square>(square));
        if (piece != Piece::None) 
        {
            hash ^= piece_keys_[square][piece_index(piece)];
        }
    }
    
    // Hash castling rights
    hash ^= castling_keys_[pos.castling_rights()];
    
    // Hash en passant square
    if (pos.en_passant_square() != Square::None) 
    {
        int file = static_cast<int>(file_of(pos.en_passant_square()));
        hash ^= en_passant_keys_[file];
    }
    
    // Hash side to move
    if (pos.side_to_move() == Color::Black) 
    {
        hash ^= side_to_move_key_;
    }
    
    return hash;
}

uint64_t ZobristHash::update_hash_make_move(uint64_t current_hash, const Position& pos, const Move& move) 
{
    assert(initialized_);
    
    uint64_t hash = current_hash;
    
    // Always flip side to move
    hash ^= side_to_move_key_;
    
    // Get the moving piece
    Piece moving_piece = pos.piece_on(move.from_square);
    assert(moving_piece != Piece::None);
    
    // Remove piece from source square
    hash ^= piece_keys_[static_cast<int>(move.from_square)][piece_index(moving_piece)];
    
    // Handle different move types
    switch (move.move_type) 
    {
        case MoveType::Normal:
            // Add piece to destination square
            hash ^= piece_keys_[static_cast<int>(move.to_square)][piece_index(moving_piece)];
            break;
            
        case MoveType::Capture:
            // Remove captured piece
            if (move.captured_piece != Piece::None) 
            {
                hash ^= piece_keys_[static_cast<int>(move.to_square)][piece_index(move.captured_piece)];
            }
            // Add moving piece to destination
            hash ^= piece_keys_[static_cast<int>(move.to_square)][piece_index(moving_piece)];
            break;
            
        case MoveType::Castle:
            // Add king to destination
            hash ^= piece_keys_[static_cast<int>(move.to_square)][piece_index(moving_piece)];
            
            // Handle rook movement for castling
            if (move.to_square == Square::G1) 
            {   // White kingside
                hash ^= piece_keys_[static_cast<int>(Square::H1)][piece_index(Piece::WhiteRook)];
                hash ^= piece_keys_[static_cast<int>(Square::F1)][piece_index(Piece::WhiteRook)];
            } 
            else if (move.to_square == Square::C1) 
            {   // White queenside
                hash ^= piece_keys_[static_cast<int>(Square::A1)][piece_index(Piece::WhiteRook)];
                hash ^= piece_keys_[static_cast<int>(Square::D1)][piece_index(Piece::WhiteRook)];
            } 
            else if (move.to_square == Square::G8) 
            {   // Black kingside
                hash ^= piece_keys_[static_cast<int>(Square::H8)][piece_index(Piece::BlackRook)];
                hash ^= piece_keys_[static_cast<int>(Square::F8)][piece_index(Piece::BlackRook)];
            } 
            else if (move.to_square == Square::C8) 
            {   // Black queenside
                hash ^= piece_keys_[static_cast<int>(Square::A8)][piece_index(Piece::BlackRook)];
                hash ^= piece_keys_[static_cast<int>(Square::D8)][piece_index(Piece::BlackRook)];
            }
            break;
            
        case MoveType::EnPassant:
        {
            // Add pawn to destination
            hash ^= piece_keys_[static_cast<int>(move.to_square)][piece_index(moving_piece)];
            
            // Remove captured pawn (not on destination square)
            Square captured_square;
            if (pos.side_to_move() == Color::White) 
            {
                captured_square = static_cast<Square>(static_cast<int>(move.to_square) - 8);
            } 
            else 
            {
                captured_square = static_cast<Square>(static_cast<int>(move.to_square) + 8);
            }
            Piece captured_pawn = (pos.side_to_move() == Color::White) ? Piece::BlackPawn : Piece::WhitePawn;
            hash ^= piece_keys_[static_cast<int>(captured_square)][piece_index(captured_pawn)];
            break;
        }
            
        case MoveType::Promotion:
        {
            // Add promoted piece to destination
            hash ^= piece_keys_[static_cast<int>(move.to_square)][piece_index(move.promotion_piece)];
            
            // Remove captured piece if it's a capture promotion
            if (move.captured_piece != Piece::None) 
            {
                hash ^= piece_keys_[static_cast<int>(move.to_square)][piece_index(move.captured_piece)];
            }
            break;
        }
            
        default:
            break;
    }
    
    // Update castling rights hash
    hash ^= castling_keys_[pos.castling_rights()]; // Remove old
    
    // Update en passant hash
    if (pos.en_passant_square() != Square::None) 
    {
        int file = static_cast<int>(file_of(pos.en_passant_square()));
        hash ^= en_passant_keys_[file]; // Remove old
    }
    
    return hash;
}

uint64_t ZobristHash::piece_hash(Piece piece, Square square)
{
    assert(initialized_);
    if (piece == Piece::None) return 0;
    return piece_keys_[static_cast<int>(square)][piece_index(piece)];
}

uint64_t ZobristHash::castling_hash(uint8_t castling_rights) 
{
    assert(initialized_);
    return castling_keys_[castling_rights & 0xF];
}

uint64_t ZobristHash::en_passant_hash(Square en_passant_square) 
{
    assert(initialized_);
    if (en_passant_square == Square::None) return 0;
    int file = static_cast<int>(file_of(en_passant_square));
    return en_passant_keys_[file];
}

uint64_t ZobristHash::side_to_move_hash() 
{
    assert(initialized_);
    return side_to_move_key_;
}

int ZobristHash::piece_index(Piece piece) 
{
    // Convert Piece enum to array index (0-11)
    return static_cast<int>(piece);
}

} // namespace luna
