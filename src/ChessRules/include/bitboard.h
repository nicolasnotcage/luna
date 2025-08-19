/*
    A bitboard class representation for an independent study
    in chess engine design and implementation. Provides:
        - Unsigned 64-bit bitboard representation
        - Functions for bit manipulation
        - Operator overloads
        -  Static factory functions to generate pre-defined bitboards

    Author: Nicolas Miller
    Date: 06/11/2025
*/

#ifndef BITBOARD_H
#define BITBOARD_H

#include "types.h"

class Bitboard
{
public:
    // Constructors
    Bitboard();
    Bitboard(File file);
    Bitboard(Rank rank);
    Bitboard(Square square);

    // Set a bit at a specific square
    void set_bit(Square square);

    // Clear a bit at a specific square
    void clear_bit(Square square);

    // Check if a bit is set at a specific square
    bool is_bit_set(Square square) const;

    // Remove and return the least significant set bit
    uint8_t pop_lsb();

    // Count the number of set bits
    uint8_t count_bits() const;

    // Get the index of the least significant bit
    uint8_t get_lsb_index() const;

    // Get the index of the most significant bit
    uint8_t get_msb_index() const;

    // Print bitboard
    void print_bitboard() const;

    // Operator overloads
    Bitboard& operator|=(const Bitboard& other);
    Bitboard& operator&=(const Bitboard& other);
    Bitboard& operator^=(const Bitboard& other);
    Bitboard operator|(const Bitboard& other) const;
    Bitboard operator&(const Bitboard& other) const;
    Bitboard operator^(const Bitboard& other) const;
    Bitboard operator~() const;

    // Static methods to get predefined bitboards
    static Bitboard file_bitboard(File file);
    static Bitboard rank_bitboard(Rank rank);
    static Bitboard square_bitboard(Square square);

    // Non-sliding piece attack lookup functions
    static Bitboard knight_attacks(Square square);
    static Bitboard king_attacks(Square square);
    static Bitboard pawn_attacks(Square square, Color color);
    
    // Sliding piece attack lookup functions
    static Bitboard bishop_attacks(Square square, Bitboard occupied);
    static Bitboard rook_attacks(Square square, Bitboard occupied);
    static Bitboard queen_attacks(Square square, Bitboard occupied);

    // Initialize non-sliding attack lookup tables
    static void init_attack_tables();

private:
    uint64_t bitboard;

    // Lookup tables
    static Bitboard knight_attacks_table[static_cast<int>(Square::NB)];
    static Bitboard king_attacks_table[static_cast<int>(Square::NB)];
    static Bitboard pawn_attacks_table[2][static_cast<int>(Square::NB)]; // [color][square]

    // Ray tables
    static Bitboard ray_table[8][static_cast<int>(Square::NB)]; // [direction][square]

    // Helper functions for ray generation
    static void init_ray_table();
    static Bitboard generate_ray(Square square, Direction direction);

    // Helper functions for lookup table generation
    static void init_knight_attacks();
    static void init_king_attacks();
    static void init_pawn_attacks();

    // Return the index of LSB from a given mask
    static uint8_t index_of_mask(uint64_t lsb_mask) 
    {
        uint8_t index = 0;
        while ((lsb_mask >> index) != 1) 
        {
            ++index;
        }
        return index;
    }
};

#endif // BITBOARD_H
