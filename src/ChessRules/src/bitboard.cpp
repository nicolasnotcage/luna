/*
    Implementation of bitboard representation for an
    independent study of chess engines.

    Author: Nicolas Miller
    Date: 06/11/2025
*/

#include "bitboard.h"
#include "types.h"

#include <iostream>

// Default constructor - creates an empty bitboard
Bitboard::Bitboard() : bitboard(0ULL) {}

// Constructor that creates a bitboard with all squares in a file set
Bitboard::Bitboard(File file) : bitboard(0ULL)
{
    for (int rank = 0; rank < static_cast<int>(Rank::NB); ++rank)
    {
        Square sq = make_square(file, static_cast<Rank>(rank));
        set_bit(sq);
    }
}

// Constructor that creates a bitboard with all squares in a rank set
Bitboard::Bitboard(Rank rank) : bitboard(0ULL)
{
    for (int file = 0; file < static_cast<int>(File::NB); ++file)
    {
        Square sq = make_square(static_cast<File>(file), rank);
        set_bit(sq);
    }
}

// Constructor that creates a bitboard with a single square set
Bitboard::Bitboard(Square square) : bitboard(0ULL)
{
    set_bit(square);
}

// Set the bit at the index of the Square. 
void Bitboard::set_bit(Square square)
{
    uint8_t bit_index = static_cast<uint8_t>(square);
    bitboard |= (1ULL << bit_index);
}

// Clear the bit at the index of the Square. 
void Bitboard::clear_bit(Square square)
{
    uint8_t bit_index = static_cast<uint8_t>(square);
    bitboard &= ~(1ULL << bit_index);
}

// Return whether the bit at the square is set.
bool Bitboard::is_bit_set(Square square) const
{
    uint8_t bit_index = static_cast<uint8_t>(square);
    return (bitboard & (1ULL << bit_index)) != 0;
}

// Pop and return the index of the LSB
uint8_t Bitboard::pop_lsb()
{
    if (bitboard == 0) return static_cast<uint8_t>(Square::None);

    uint64_t lsb_mask = bitboard & (~bitboard + 1);     // Get LSB mask
    uint8_t index = index_of_mask(lsb_mask);            // Get index of LSB
    bitboard &= ~lsb_mask;                              // Clear LSB
    return index;                                       // Return index
}

// Return the index of the LSB
uint8_t Bitboard::get_lsb_index() const   
{
    if (bitboard == 0) return static_cast<uint8_t>(Square::None);
    return index_of_mask(bitboard & (~bitboard + 1));
}

// Return the population count of the bitboard. 
// TODO: Speed this up?
uint8_t Bitboard::count_bits() const
{
    uint64_t bb_copy = bitboard;    // Create copy of bitboard
    uint8_t count = 0;
    while (bb_copy)
    {
        count += bb_copy & 1;       // Add 1 if LSB is set
        bb_copy >>= 1;              // Shift to check next bit
    }

    return count;
}

// Return index of MSB
uint8_t Bitboard::get_msb_index() const
{
    if (bitboard == 0) return static_cast<uint8_t>(Square::None);

    // Start at the highest bit and work backwards
    uint8_t index = 63; 
    while (((bitboard >> index) & 1) == 0) 
    {
        --index;
    }
    return index;
}

// Print the bitboard as a standard game board. 
void Bitboard::print_bitboard() const 
{
    std::cout << "  +---+---+---+---+---+---+---+---+" << std::endl;
    
    for (int rank = 7; rank >= 0; rank--) 
    {
        std::cout << (rank + 1) << " |";
        
        for (int file = 0; file < 8; file++) 
        {
            int square = rank * 8 + file;
            char piece = ((bitboard >> square) & 1) ? 'X' : '.';
            std::cout << " " << piece << " |";
        }
        
        std::cout << std::endl;
        std::cout << "  +---+---+---+---+---+---+---+---+" << std::endl;
    }
    
    std::cout << "    a   b   c   d   e   f   g   h  " << std::endl;
    
    // Print bitboard value in hexadecimal
    std::cout << "Bitboard value: 0x" << std::hex << std::uppercase << bitboard << std::dec << std::endl;
}

// Return a bitboard for a specific File.
Bitboard Bitboard::file_bitboard(File file)
{
    return Bitboard(file);
}

// Return a bitboard for a specific Rank.
Bitboard Bitboard::rank_bitboard(Rank rank)
{
    return Bitboard(rank);
}

// Return a bitboard for a specific Square.
Bitboard Bitboard::square_bitboard(Square square)
{
    return Bitboard(square);
}

// Define lookup tables. We store one bitboard for the attacks that each piece can 
// make from each square on the board. Requires more memory, but this prevents us 
// from having to calculate attacks on the fly.
Bitboard Bitboard::knight_attacks_table[static_cast<int>(Square::NB)];
Bitboard Bitboard::king_attacks_table[static_cast<int>(Square::NB)];
Bitboard Bitboard::pawn_attacks_table[2][static_cast<int>(Square::NB)];

// Initialize knight attack table
void Bitboard::init_knight_attacks() 
{
    for (int sq = 0; sq < static_cast<int>(Square::NB); sq++) 
    {
        // Get square and initialize bitboard
        Square square = static_cast<Square>(sq);
        uint64_t bb = 0ULL;
        
        // Get the file and rank of the square
        File f = file_of(square);
        Rank r = rank_of(square);
        
        // Calculate all possible knight moves from this square and set the bits for the attacked squares
        
        // North-North-East: 2 north, 1 east
        int NNE = static_cast<int>(Direction::North) + static_cast<int>(Direction::North) + static_cast<int>(Direction::East); 
        if (static_cast<int>(r) + 2 < static_cast<int>(Rank::NB) && static_cast<int>(f) + 1 < static_cast<int>(File::NB))
            bb |= 1ULL << (sq + NNE);
            
        // North-East-East: 1 north, 2 east
        int NEE = static_cast<int>(Direction::North) + static_cast<int>(Direction::East) + static_cast<int>(Direction::East);
        if (static_cast<int>(r) + 1 < static_cast<int>(Rank::NB) && static_cast<int>(f) + 2 < static_cast<int>(File::NB))
            bb |= 1ULL << (sq + NEE);
            
        // South-East-East: 1 south, 2 east
        int SEE = static_cast<int>(Direction::South) + static_cast<int>(Direction::East) + static_cast<int>(Direction::East);
        if (static_cast<int>(r) - 1 >= 0 && static_cast<int>(f) + 2 < static_cast<int>(File::NB))
            bb |= 1ULL << (sq + SEE);
            
        // South-South-East: 2 south, 1 east
        int SSE = static_cast<int>(Direction::South) + static_cast<int>(Direction::South) + static_cast<int>(Direction::East);
        if (static_cast<int>(r) - 2 >= 0 && static_cast<int>(f) + 1 < static_cast<int>(File::NB))
            bb |= 1ULL << (sq + SSE);
            
        // South-South-West: 2 south, 1 west
        int SSW = static_cast<int>(Direction::South) + static_cast<int>(Direction::South) + static_cast<int>(Direction::West);
        if (static_cast<int>(r) - 2 >= 0 && static_cast<int>(f) - 1 >= 0)
            bb |= 1ULL << (sq + SSW);
            
        // South-West-West: 1 south, 2 west
        int SWW = static_cast<int>(Direction::South) + static_cast<int>(Direction::West) + static_cast<int>(Direction::West);
        if (static_cast<int>(r) - 1 >= 0 && static_cast<int>(f) - 2 >= 0)
            bb |= 1ULL << (sq + SWW);
            
        // North-West-West: 1 north, 2 west
        int NWW = static_cast<int>(Direction::North) + static_cast<int>(Direction::West) + static_cast<int>(Direction::West);
        if (static_cast<int>(r) + 1 < static_cast<int>(Rank::NB) && static_cast<int>(f) - 2 >= 0)
            bb |= 1ULL << (sq + NWW);
            
        // North-North-West: 2 north, 1 west
        int NNW = static_cast<int>(Direction::North) + static_cast<int>(Direction::North) + static_cast<int>(Direction::West);
        if (static_cast<int>(r) + 2 < static_cast<int>(Rank::NB) && static_cast<int>(f) - 1 >= 0)
            bb |= 1ULL << (sq + NNW);
        
        // Store the result in the lookup table
        Bitboard::knight_attacks_table[sq] = Bitboard();
        Bitboard::knight_attacks_table[sq].bitboard = bb;
    }
}

// Initialize king attack table
void Bitboard::init_king_attacks() 
{
    for (int sq = 0; sq < static_cast<int>(Square::NB); sq++) 
    {
        Square square = static_cast<Square>(sq);
        uint64_t bb = 0ULL;
        
        // Get the file and rank of the square
        File f = file_of(square);
        Rank r = rank_of(square);
        
        // Calculate all possible king moves from this square and set the corresponding bits
        
        // North
        int N = static_cast<int>(Direction::North);
        if (static_cast<int>(r) + 1 < static_cast<int>(Rank::NB))
            bb |= 1ULL << (sq + N);
            
        // North-East
        int NE = static_cast<int>(Direction::NorthEast);
        if (static_cast<int>(r) + 1 < static_cast<int>(Rank::NB) && static_cast<int>(f) + 1 < static_cast<int>(File::NB))
            bb |= 1ULL << (sq + NE);
            
        // East
        int E = static_cast<int>(Direction::East);
        if (static_cast<int>(f) + 1 < static_cast<int>(File::NB))
            bb |= 1ULL << (sq + E);
            
        // South-East
        int SE = static_cast<int>(Direction::SouthEast);
        if (static_cast<int>(r) - 1 >= 0 && static_cast<int>(f) + 1 < static_cast<int>(File::NB))
            bb |= 1ULL << (sq + SE);
            
        // South
        int S = static_cast<int>(Direction::South);
        if (static_cast<int>(r) - 1 >= 0)
            bb |= 1ULL << (sq + S);
            
        // South-West
        int SW = static_cast<int>(Direction::SouthWest);
        if (static_cast<int>(r) - 1 >= 0 && static_cast<int>(f) - 1 >= 0)
            bb |= 1ULL << (sq + SW);
            
        // West
        int W = static_cast<int>(Direction::West);
        if (static_cast<int>(f) - 1 >= 0)
            bb |= 1ULL << (sq + W);
            
        // North-West
        int NW = static_cast<int>(Direction::NorthWest);
        if (static_cast<int>(r) + 1 < static_cast<int>(Rank::NB) && static_cast<int>(f) - 1 >= 0)
            bb |= 1ULL << (sq + NW);
        
        // Store the result in the lookup table
        Bitboard::king_attacks_table[sq] = Bitboard();
        Bitboard::king_attacks_table[sq].bitboard = bb;
    }
}

// Initialize pawn attack table
void Bitboard::init_pawn_attacks() 
{
    for (int sq = 0; sq < static_cast<int>(Square::NB); sq++) 
    {
        Square square = static_cast<Square>(sq);
        uint64_t bb_white = 0ULL;
        uint64_t bb_black = 0ULL;
        
        // Get the file and rank of the square
        File f = file_of(square);
        Rank r = rank_of(square);
        
        // White pawn attacks (moving up the board)
        if (static_cast<int>(r) + 1 < static_cast<int>(Rank::NB)) 
        {
            // North-East attack
            int NE = static_cast<int>(Direction::NorthEast);
            if (static_cast<int>(f) + 1 < static_cast<int>(File::NB))
                bb_white |= 1ULL << (sq + NE);
                
            // North-West attack
            int NW = static_cast<int>(Direction::NorthWest);
            if (static_cast<int>(f) - 1 >= 0)
                bb_white |= 1ULL << (sq + NW);
        }
        
        // Black pawn attacks (moving down the board)
        if (static_cast<int>(r) - 1 >= 0) 
        {
            // South-East attack
            int SE = static_cast<int>(Direction::SouthEast);
            if (static_cast<int>(f) + 1 < static_cast<int>(File::NB))
                bb_black |= 1ULL << (sq + SE);
                
            // South-West attack
            int SW = static_cast<int>(Direction::SouthWest);
            if (static_cast<int>(f) - 1 >= 0)
                bb_black |= 1ULL << (sq + SW);
        }
        
        // Store the results in the lookup table
        Bitboard::pawn_attacks_table[static_cast<int>(Color::White)][sq] = Bitboard();
        Bitboard::pawn_attacks_table[static_cast<int>(Color::White)][sq].bitboard = bb_white;
        
        Bitboard::pawn_attacks_table[static_cast<int>(Color::Black)][sq] = Bitboard();
        Bitboard::pawn_attacks_table[static_cast<int>(Color::Black)][sq].bitboard = bb_black;
    }
}

// Knight attack lookup function
Bitboard Bitboard::knight_attacks(Square square) 
{
    if (square == Square::NB || square == Square::None)
        return Bitboard();
        
    return knight_attacks_table[static_cast<int>(square)];
}

// King attack lookup function
Bitboard Bitboard::king_attacks(Square square) 
{
    if (square == Square::NB || square == Square::None)
        return Bitboard();
        
    return king_attacks_table[static_cast<int>(square)];
}

// Pawn attack lookup function
Bitboard Bitboard::pawn_attacks(Square square, Color color) 
{
    if (square == Square::NB || square == Square::None || color == Color::NB)
        return Bitboard();
        
    return pawn_attacks_table[static_cast<int>(color)][static_cast<int>(square)];
}

// Initialize all non-sliding attack tables
void Bitboard::init_attack_tables() 
{
    Bitboard::init_knight_attacks();
    Bitboard::init_king_attacks();
    Bitboard::init_pawn_attacks();
    Bitboard::init_ray_table();
}

// Define ray table; one entry for each direction for each square
Bitboard Bitboard::ray_table[8][static_cast<int>(Square::NB)];

// Array of directions for iteration
const Direction all_directions[8] = 
    {
        Direction::North,
        Direction::NorthEast,
        Direction::East,
        Direction::SouthEast,
        Direction::South,
        Direction::SouthWest,
        Direction::West,
        Direction::NorthWest
    };

// Generate a ray bitboard from a square in a given direction
Bitboard Bitboard::generate_ray(Square square, Direction direction) 
{
    Bitboard ray;
    int direction_offset = static_cast<int>(direction);
    int sq = static_cast<int>(square);

    // Get rank and file of square
    Rank rank = rank_of(square);
    File file = file_of(square);

    // Move in specified direction until we hit the edge of the board
    switch (direction)
    {
        case Direction::North:
            for (int i = 1; i < 8 - static_cast<int>(rank); i++)
            {
                ray.set_bit(static_cast<Square>(sq + i * direction_offset));
            }
            break;
        case Direction::South:
            for (int i = 1; i <= static_cast<int>(rank); i++) 
            {
                ray.set_bit(static_cast<Square>(sq + i * direction_offset));
            }
            break;
            
        case Direction::East:
            for (int i = 1; i < 8 - static_cast<int>(file); i++) 
            {
                ray.set_bit(static_cast<Square>(sq + i * direction_offset));
            }
            break;
            
        case Direction::West:
            for (int i = 1; i <= static_cast<int>(file); i++) 
            {
                ray.set_bit(static_cast<Square>(sq + i * direction_offset));
            }
            break;
            
        case Direction::NorthEast:
            for (int i = 1; i < std::min(8 - static_cast<int>(rank), 8 - static_cast<int>(file)); i++) 
            {
                ray.set_bit(static_cast<Square>(sq + i * direction_offset));
            }
            break;
            
        case Direction::NorthWest:
            for (int i = 1; i < std::min(8 - static_cast<int>(rank), static_cast<int>(file) + 1); i++) 
            {
                ray.set_bit(static_cast<Square>(sq + i * direction_offset));
            }
            break;
            
        case Direction::SouthEast:
            for (int i = 1; i < std::min(static_cast<int>(rank) + 1, 8 - static_cast<int>(file)); i++) 
            {
                ray.set_bit(static_cast<Square>(sq + i * direction_offset));
            }
            break;
            
        case Direction::SouthWest:
            for (int i = 1; i < std::min(static_cast<int>(rank) + 1, static_cast<int>(file) + 1); i++) 
            {
                ray.set_bit(static_cast<Square>(sq + i * direction_offset));
            }
            break;
            
        default:
            break;  // Invalid direction
    }
    
    return ray;
}

// Initialize ray tables
void Bitboard::init_ray_table()
{
    for (int dir = 0; dir < 8; dir++) 
    {
        Direction direction = all_directions[dir];
        for (int sq = 0; sq < static_cast<int>(Square::NB); sq++) 
        {
            ray_table[dir][sq] = generate_ray(static_cast<Square>(sq), direction);
        }
    }
}

// Generate bishop attacks. Requires a bitboard representing the current state
// of the board (should contain all occupied squares). 
Bitboard Bitboard::bishop_attacks(Square square, Bitboard occupied)
{
    Bitboard attacks;

    // Bishop moves along diagonals
    const Direction bishop_directions[4] = 
        {
            Direction::NorthEast,
            Direction::NorthWest,
            Direction::SouthEast,
            Direction::SouthWest
        };
    
    // Update attacks bitboard with attacks for each bishop direction
    for (int i = 0; i < 4; i++)
    {
        Direction dir = bishop_directions[i];
        int dir_index = 0;

        // Find index in all-directions array
        for (int j = 0; j < 8; j++)
        {
            if (all_directions[j] == dir)
            {
                dir_index = j;
                break;
            }
        }

        Bitboard ray = ray_table[dir_index][static_cast<int>(square)];
        attacks.bitboard |= ray.bitboard;

        // Find blocking pieces
        Bitboard blockers;
        blockers.bitboard = ray.bitboard & occupied.bitboard;

        if (blockers.bitboard)
        {
            // Find closest blocker
            Square blocker_sq;

            if (dir == Direction::NorthEast || dir == Direction::NorthWest)
            {
                // Want southern-most blocker (LSB)
                blocker_sq = static_cast<Square>(blockers.get_lsb_index());
            }
            else
            {
                // Want northern-most blocker (MSB)
                blocker_sq = static_cast<Square>(blockers.get_msb_index());
            }

            // Remove squares beyond the blocker
            Bitboard beyond_blocker = generate_ray(blocker_sq, dir);
            attacks.bitboard &= ~beyond_blocker.bitboard;
        }
    }

    return attacks;
}

// Generate rook attacks.
// TODO: Nearly identical to bishop function. Could probably template it with T = Piece.
Bitboard Bitboard::rook_attacks(Square square, Bitboard occupied) 
{
    Bitboard attacks;
    
    // Rook moves along files and ranks
    const Direction rook_directions[4] = 
    {
        Direction::North,
        Direction::East,
        Direction::South,
        Direction::West
    };
    
    for (int i = 0; i < 4; i++) 
    {
        Direction dir = rook_directions[i];
        int dir_idx = 0;
        
        // Find the index in all_directions array
        for (int j = 0; j < 8; j++) 
        {
            if (all_directions[j] == dir) 
            {
                dir_idx = j;
                break;
            }
        }
        
        Bitboard ray = ray_table[dir_idx][static_cast<int>(square)];
        attacks.bitboard |= ray.bitboard;
        
        // Find blocking pieces
        Bitboard blockers;
        blockers.bitboard = ray.bitboard & occupied.bitboard;
        
        if (blockers.bitboard) 
        {
            // Find the closest blocker
            Square blocker_sq;
            
            if (dir == Direction::North || dir == Direction::East) 
            {
                // For northward/eastward rays, we want the southernmost/westernmost blocker (LSB)
                blocker_sq = static_cast<Square>(blockers.get_lsb_index());
            } 
            else 
            {
                // For southward/westward rays, we want the northernmost/easternmost blocker (MSB)
                blocker_sq = static_cast<Square>(blockers.get_msb_index());
            }
            
            // Remove squares beyond the blocker
            Bitboard beyond_blocker = generate_ray(blocker_sq, dir);
            attacks.bitboard &= ~beyond_blocker.bitboard;
        }
    }
    
    return attacks;
}

// Generate queen attacks (combination of bishop and rook attacks)
Bitboard Bitboard::queen_attacks(Square square, Bitboard occupied) 
{
    Bitboard bishop = bishop_attacks(square, occupied);
    Bitboard rook = rook_attacks(square, occupied);
    
    Bitboard queen;
    queen.bitboard = bishop.bitboard | rook.bitboard;
    
    return queen;
}

// Operator overloads
Bitboard& Bitboard::operator|=(const Bitboard& other)
{
    bitboard |= other.bitboard;
    return *this;
}

Bitboard& Bitboard::operator&=(const Bitboard& other)
{
    bitboard &= other.bitboard;
    return *this;
}

Bitboard& Bitboard::operator^=(const Bitboard& other)
{
    bitboard ^= other.bitboard;
    return *this;
}

Bitboard Bitboard::operator|(const Bitboard& other) const
{
    Bitboard result;
    result.bitboard = bitboard | other.bitboard;
    return result;
}

Bitboard Bitboard::operator&(const Bitboard& other) const
{
    Bitboard result;
    result.bitboard = bitboard & other.bitboard;
    return result;
}

Bitboard Bitboard::operator^(const Bitboard& other) const
{
    Bitboard result;
    result.bitboard = bitboard ^ other.bitboard;
    return result;
}

Bitboard Bitboard::operator~() const
{
    Bitboard result;
    result.bitboard = ~bitboard;
    return result;
}
