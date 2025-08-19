/*
    This file contains general type definitions to be used in the development
    of a chess game/engine, including various enumerations and utility functions
    for board and piece representation.  
    
    REVISED: Move struct no longer contains state information - that's handled
    by MoveWithState in position.h

    Author: Nicolas Miller
    Date: 06/11/2025
*/

#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <cctype>
#include <algorithm>
#include <cmath>

// Type Enumerations
enum class Color
{
    White,
    Black,
    NB,
    None
};

enum class PieceType
{
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King,
    NB
};

enum class Piece
{
    WhitePawn, WhiteKnight, WhiteBishop, WhiteRook, WhiteQueen, WhiteKing,
    BlackPawn, BlackKnight, BlackBishop, BlackRook, BlackQueen, BlackKing,
    NB, None
};

enum class Square
{
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    NB = 64, None = 65
};

enum class File
{
    A, B, C, D, E, F, G, H, NB
};

enum class Rank
{
    One, Two, Three, Four, Five, Six, Seven, Eight, NB
};

enum class CastlingRights
{
    White_OO, White_OOO,
    Black_OO, Black_OOO,
    NB
};

enum class MoveType
{
    Normal,
    Capture,
    Castle, 
    EnPassant,
    Promotion,
    NB
};

enum class Direction
{
    North = 8,
    East = 1,
    South = -8,
    West = -1,
    NorthEast = 9,
    SouthEast = -7,
    SouthWest = -9,
    NorthWest = 7
};

// Returns a square from a File and Rank.
inline Square make_square(File file, Rank rank) 
{
    return static_cast<Square>(static_cast<int>(rank) * static_cast<int>(File::NB) + static_cast<int>(file));
}

// Returns the File of a Square.
inline File file_of(Square square) 
{
    return static_cast<File>(static_cast<int>(square) % static_cast<int>(File::NB));
}

// Returns the Rank of a Square.
inline Rank rank_of(Square square) 
{
    return static_cast<Rank>(static_cast<int>(square) / static_cast<int>(File::NB));
}

// Returns a Piece made from a Color and PieceType.
inline Piece make_piece(Color color, PieceType piece_type)
{
    return static_cast<Piece>(static_cast<int>(color) * static_cast<int>(PieceType::NB) + static_cast<int>(piece_type));
}

// Returns the type of a Piece.
inline PieceType type_of(Piece piece)
{
    return static_cast<PieceType>(static_cast<int>(piece) % static_cast<int>(PieceType::NB));
}

// Returns the color of a Piece.
inline Color color_of(Piece piece)
{
    if (piece == Piece::None || piece == Piece::NB)
        return Color::None;
    
    return static_cast<Color>(static_cast<int>(piece) / static_cast<int>(PieceType::NB));
}

// Returns the distance between two files
inline int file_distance(Square square1, Square square2)
{
    int file1 = static_cast<int>(file_of(square1));
    int file2 = static_cast<int>(file_of(square2));
    
    return std::abs(file1 - file2);
}

// Returns the distance between two ranks
inline int rank_distance(Square square1, Square square2)
{
    int rank1 = static_cast<int>(rank_of(square1));
    int rank2 = static_cast<int>(rank_of(square2));
    
    return std::abs(rank1 - rank2);
}

// Returns the distance between two squares. Uses Chebyshev
// distance (maximum of file and rank distances). 
inline int distance(Square square1, Square square2)
{
    return std::max(file_distance(square1, square2), rank_distance(square1, square2));
}

// Returns a string representation of a square. 
inline std::string square_to_string(Square square) 
{
    if (square == Square::None || square == Square::NB) return "None";
    
    int square_int = static_cast<int>(square);
    char file = 'a' + (square_int % static_cast<int>(File::NB));
    char rank = static_cast<char>('1' + (square_int / static_cast<int>(File::NB)));
    
    return std::string(1, file) + std::string(1, rank);
}

// Returns a Square from a string. 
inline Square string_to_square(const std::string& s) 
{
    if (s.length() < 2) return Square::NB;
    
    char file = static_cast<char>(std::tolower(s[0]));
    char rank = s[1];
    
    if (file < 'a' || file > 'h' || rank < '1' || rank > '8')
        return Square::None;
    
    int file_idx = file - 'a';
    int rank_idx = rank - '1';
    
    return static_cast<Square>(rank_idx * static_cast<int>(File::NB) + file_idx);
}

// Returns a string representation of a Piece.
inline std::string piece_to_string(Piece piece)
{
    if (piece == Piece::None) return "None";
    if (piece == Piece::NB) return "NB";
    
    const char* piece_chars = "PNBRQK";
    char piece_char = piece_chars[static_cast<int>(type_of(piece))];
    
    return std::string(1, color_of(piece) == Color::White ? piece_char : static_cast<char>(std::tolower(piece_char)));
}

/*
    A struct representing a Move in a chess game. Contains both the move
    description and state information needed for undoing the move.
*/
struct Move
{
    // Move description (what the move is)
    Square from_square;                 // The square a piece is moving from
    Square to_square;                   // The square a piece is moving to
    MoveType move_type;                 // The type of move made (normal, capture, etc.)
    Piece promotion_piece;              // The piece promoted to, if applicable
    
    // State info for undo (captured by make_move)
    Piece captured_piece;               // The piece captured (if any)
    uint8_t previous_castling_rights;   // Castling rights before the move
    Square previous_en_passant_square;  // En passant square before the move
    int previous_halfmove_clock;        // Halfmove clock before the move
    
    // Constructors
    Move() : from_square(Square::None), to_square(Square::None), 
             move_type(MoveType::Normal), promotion_piece(Piece::None),
             captured_piece(Piece::None), previous_castling_rights(0),
             previous_en_passant_square(Square::None), previous_halfmove_clock(0) {}
    
    Move(Square from, Square to, MoveType type, Piece promotion = Piece::None) 
        : from_square(from), to_square(to), move_type(type), promotion_piece(promotion),
          captured_piece(Piece::None), previous_castling_rights(0),
          previous_en_passant_square(Square::None), previous_halfmove_clock(0) {}
    
    // Equality operator; compares move description
    bool operator==(const Move& other) const
    {
        return from_square == other.from_square &&
               to_square == other.to_square &&
               move_type == other.move_type &&
               promotion_piece == other.promotion_piece;
    }
    
    // Convert to string notation (e.g., "e2e4", "e7e8q")
    std::string to_string() const
    {
        std::string result = square_to_string(from_square) + square_to_string(to_square);
        
        // Add promotion piece if applicable
        if (move_type == MoveType::Promotion && promotion_piece != Piece::None)
        {
            std::string piece_str = piece_to_string(promotion_piece);
            result += static_cast<char>(std::tolower(piece_str[0]));
        }
        
        return result;
    }
};

#endif // TYPES_H