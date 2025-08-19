#include "tests.h"

#include <iostream>
#include <cassert>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <chrono>
#include <map>
#include <algorithm>
#include <functional>
#include "movegen.h"

// ANSI color codes for better output
const std::string GREEN = "\033[32m";
const std::string RED = "\033[31m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string RESET = "\033[0m";

// Helper function to convert various types to string for display in test assertions
template<typename T>
std::string to_debug_string(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

// Specializations for common types
inline std::string to_debug_string(const std::string& value) {
    return "\"" + value + "\"";
}

inline std::string to_debug_string(bool value) {
    return value ? "true" : "false";
}

// Specializations for chess-specific enum types
inline std::string to_debug_string(const Color& color) {
    switch (color) {
        case Color::White: return "White";
        case Color::Black: return "Black";
        case Color::NB: return "NB";
        default: return "Unknown Color";
    }
}

inline std::string to_debug_string(const PieceType& pt) {
    switch (pt) {
        case PieceType::Pawn: return "Pawn";
        case PieceType::Knight: return "Knight";
        case PieceType::Bishop: return "Bishop";
        case PieceType::Rook: return "Rook";
        case PieceType::Queen: return "Queen";
        case PieceType::King: return "King";
        case PieceType::NB: return "NB";
        default: return "Unknown PieceType";
    }
}

inline std::string to_debug_string(const Piece& piece) {
    switch (piece) {
        case Piece::WhitePawn: return "WhitePawn";
        case Piece::WhiteKnight: return "WhiteKnight";
        case Piece::WhiteBishop: return "WhiteBishop";
        case Piece::WhiteRook: return "WhiteRook";
        case Piece::WhiteQueen: return "WhiteQueen";
        case Piece::WhiteKing: return "WhiteKing";
        case Piece::BlackPawn: return "BlackPawn";
        case Piece::BlackKnight: return "BlackKnight";
        case Piece::BlackBishop: return "BlackBishop";
        case Piece::BlackRook: return "BlackRook";
        case Piece::BlackQueen: return "BlackQueen";
        case Piece::BlackKing: return "BlackKing";
        case Piece::NB: return "NB";
        case Piece::None: return "None";
        default: return "Unknown Piece";
    }
}

inline std::string to_debug_string(const Square& square) {
    if (square == Square::NB) return "NB";
    if (square == Square::None) return "None";
    return square_to_string(square);
}

inline std::string to_debug_string(const File& file) {
    switch (file) {
        case File::A: return "A";
        case File::B: return "B";
        case File::C: return "C";
        case File::D: return "D";
        case File::E: return "E";
        case File::F: return "F";
        case File::G: return "G";
        case File::H: return "H";
        case File::NB: return "NB";
        default: return "Unknown File";
    }
}

inline std::string to_debug_string(const Rank& rank) {
    switch (rank) {
        case Rank::One: return "One";
        case Rank::Two: return "Two";
        case Rank::Three: return "Three";
        case Rank::Four: return "Four";
        case Rank::Five: return "Five";
        case Rank::Six: return "Six";
        case Rank::Seven: return "Seven";
        case Rank::Eight: return "Eight";
        case Rank::NB: return "NB";
        default: return "Unknown Rank";
    }
}

inline std::string to_debug_string(const MoveType& mt) {
    switch (mt) {
        case MoveType::Normal: return "Normal";
        case MoveType::Capture: return "Capture";
        case MoveType::Castle: return "Castle";
        case MoveType::EnPassant: return "EnPassant";
        case MoveType::Promotion: return "Promotion";
        case MoveType::NB: return "NB";
        default: return "Unknown MoveType";
    }
}

// Implementation of ChessTests methods

namespace luna {

void ChessTests::TestResult::print_summary() const {
    std::cout << "\n" << BLUE << "Test Summary: " << RESET;
    std::cout << GREEN << passed << " passed" << RESET << ", ";
    if (failed > 0) {
        std::cout << RED << failed << " failed" << RESET << std::endl;
    } else {
        std::cout << "0 failed" << std::endl;
    }
}

void ChessTests::print_test_header(const std::string& test_name) {
    std::cout << "\n" << YELLOW << "=== " << test_name << " ===" << RESET << std::endl;
}

void ChessTests::print_subtest(const std::string& subtest_name) {
    std::cout << "\n" << BLUE << ">> " << subtest_name << RESET << std::endl;
}

void ChessTests::set_visualization(bool enable) {
    visualize_output = enable;
    std::cout << (enable ? "Visualization enabled" : "Visualization disabled") << std::endl;
}

bool ChessTests::is_visualization_enabled() const {
    return visualize_output;
}

void ChessTests::set_perft_depth(int depth) {
    perft_depth = depth;
    std::cout << "Perft depth set to " << depth << std::endl;
}

void ChessTests::visualize_board(const Position& pos) {
    if (!visualize_output) return;
    
    std::cout << "\n  Board State:\n";
    std::cout << "  +---+---+---+---+---+---+---+---+\n";
    
    for (int r = 7; r >= 0; r--) {
        std::cout << (r + 1) << " |";
        for (int f = 0; f < 8; f++) {
            Square sq = make_square(static_cast<File>(f), static_cast<Rank>(r));
            Piece piece = pos.piece_on(sq);
            
            std::string piece_str = " ";
            if (piece != Piece::None) {
                piece_str = piece_to_string(piece);
                if (color_of(piece) == Color::White) {
                    piece_str = GREEN + piece_str + RESET;
                } else {
                    piece_str = RED + piece_str + RESET;
                }
            }
            
            std::cout << " " << piece_str << " |";
        }
        std::cout << "\n  +---+---+---+---+---+---+---+---+\n";
    }
    
    std::cout << "    a   b   c   d   e   f   g   h\n\n";
    
    // Print additional position info
    std::cout << "  Side to move: " << (pos.side_to_move() == Color::White ? "White" : "Black") << "\n";
    std::cout << "  FEN: " << pos.to_fen() << "\n";
    std::cout << std::endl;
}

void ChessTests::visualize_bitboard(const Bitboard& bb) {
    if (!visualize_output) return;
    
    std::cout << "\n  Bitboard Visualization:\n";
    std::cout << "  +---+---+---+---+---+---+---+---+\n";
    
    for (int r = 7; r >= 0; r--) {
        std::cout << (r + 1) << " |";
        for (int f = 0; f < 8; f++) {
            Square sq = make_square(static_cast<File>(f), static_cast<Rank>(r));
            bool is_set = bb.is_bit_set(sq);
            
            if (is_set) {
                std::cout << " " << GREEN << "X" << RESET << " |";
            } else {
                std::cout << "   |";
            }
        }
        std::cout << "\n  +---+---+---+---+---+---+---+---+\n";
    }
    
    std::cout << "    a   b   c   d   e   f   g   h\n\n";
    std::cout << "  Bits set: " << bb.count_bits() << std::endl;
}

int ChessTests::get_passed_count() const {
    return global_results.passed;
}

int ChessTests::get_failed_count() const {
    return global_results.failed;
}

bool ChessTests::all_tests_passed() const {
    return global_results.failed == 0;
}

// Square function tests
void ChessTests::test_square_functions() 
{
    print_test_header("Testing Square Functions");
    
    // Test square_to_string
    print_subtest("square_to_string");
    TEST_ASSERT_EQ(square_to_string(Square::A1), "a1", "Square::A1 to string");
    TEST_ASSERT_EQ(square_to_string(Square::E5), "e5", "Square::E5 to string");
    TEST_ASSERT_EQ(square_to_string(Square::H8), "h8", "Square::H8 to string");
    TEST_ASSERT_EQ(square_to_string(Square::D4), "d4", "Square::D4 to string");
    
    // Test string_to_square
    print_subtest("string_to_square");
    TEST_ASSERT_EQ(string_to_square("e4"), Square::E4, "String 'e4' to Square");
    TEST_ASSERT_EQ(string_to_square("h1"), Square::H1, "String 'h1' to Square");
    TEST_ASSERT_EQ(string_to_square("a8"), Square::A8, "String 'a8' to Square");
    TEST_ASSERT_EQ(string_to_square("invalid"), Square::None, "Invalid string returns Square::None");
    TEST_ASSERT_EQ(string_to_square("i9"), Square::None, "Out of bounds string returns Square::None");
    
    // Test make_square
    print_subtest("make_square");
    Square e4 = make_square(File::E, Rank::Four);
    TEST_ASSERT_EQ(e4, Square::E4, "make_square(File::E, Rank::Four) creates E4");
    TEST_ASSERT_EQ(make_square(File::A, Rank::One), Square::A1, "make_square creates A1");
    TEST_ASSERT_EQ(make_square(File::H, Rank::Eight), Square::H8, "make_square creates H8");
    
    // Test file_of and rank_of
    print_subtest("file_of and rank_of");
    Square d7 = string_to_square("d7");
    TEST_ASSERT_EQ(static_cast<int>(file_of(d7)), 3, "File of d7 is 3 (D)");
    TEST_ASSERT_EQ(static_cast<int>(rank_of(d7)), 6, "Rank of d7 is 6 (7th rank)");
    TEST_ASSERT_EQ(static_cast<int>(file_of(Square::A1)), 0, "File of a1 is 0");
    TEST_ASSERT_EQ(static_cast<int>(rank_of(Square::H8)), 7, "Rank of h8 is 7");
    
    std::cout << GREEN << "All square function tests passed" << RESET << std::endl;
}

// Piece function tests
void ChessTests::test_piece_functions() 
{
    print_test_header("Testing Piece Functions");
    
    // Test make_piece
    print_subtest("make_piece");
    Piece white_knight = make_piece(Color::White, PieceType::Knight);
    Piece black_queen = make_piece(Color::Black, PieceType::Queen);
    
    TEST_ASSERT_EQ(white_knight, Piece::WhiteKnight, "make_piece creates white knight");
    TEST_ASSERT_EQ(black_queen, Piece::BlackQueen, "make_piece creates black queen");
    
    // Test piece_to_string
    print_subtest("piece_to_string");
    TEST_ASSERT_EQ(piece_to_string(white_knight), "N", "White knight string is 'N'");
    TEST_ASSERT_EQ(piece_to_string(black_queen), "q", "Black queen string is 'q'");
    TEST_ASSERT_EQ(piece_to_string(Piece::WhitePawn), "P", "White pawn string is 'P'");
    TEST_ASSERT_EQ(piece_to_string(Piece::BlackKing), "k", "Black king string is 'k'");
    
    // Test type_of and color_of
    print_subtest("type_of and color_of");
    TEST_ASSERT_EQ(static_cast<int>(type_of(white_knight)), 1, "Type of white knight is 1 (Knight)");
    TEST_ASSERT_EQ(static_cast<int>(color_of(black_queen)), 1, "Color of black queen is 1 (Black)");
    TEST_ASSERT_EQ(type_of(Piece::WhiteKing), PieceType::King, "Type of white king is King");
    TEST_ASSERT_EQ(color_of(Piece::BlackPawn), Color::Black, "Color of black pawn is Black");
    
    std::cout << GREEN << "All piece function tests passed" << RESET << std::endl;
}

// Distance function tests
void ChessTests::test_distance_functions() 
{
    print_test_header("Testing Distance Functions");
    
    Square a1 = Square::A1;
    Square h8 = Square::H8;
    Square e4 = string_to_square("e4");
    Square e5 = string_to_square("e5");
    Square f4 = string_to_square("f4");
    
    print_subtest("file_distance");
    TEST_ASSERT_EQ(file_distance(a1, h8), 7, "File distance from a1 to h8");
    TEST_ASSERT_EQ(file_distance(e4, a1), 4, "File distance from e4 to a1");
    TEST_ASSERT_EQ(file_distance(e4, e5), 0, "File distance from e4 to e5");
    
    print_subtest("rank_distance");
    TEST_ASSERT_EQ(rank_distance(a1, h8), 7, "Rank distance from a1 to h8");
    TEST_ASSERT_EQ(rank_distance(e4, a1), 3, "Rank distance from e4 to a1");
    TEST_ASSERT_EQ(rank_distance(e4, f4), 0, "Rank distance from e4 to f4");
    
    print_subtest("distance (Chebyshev)");
    TEST_ASSERT_EQ(distance(a1, h8), 7, "Chebyshev distance from a1 to h8");
    TEST_ASSERT_EQ(distance(e4, a1), 4, "Chebyshev distance from e4 to a1");
    TEST_ASSERT_EQ(distance(e4, e5), 1, "Chebyshev distance from e4 to e5");
    
    std::cout << GREEN << "All distance function tests passed" << RESET << std::endl;
}

// Direction tests
void ChessTests::test_direction_values() 
{
    print_test_header("Testing Direction Values");
    
    // Test direction enum values
    print_subtest("Direction enum values");
    TEST_ASSERT_EQ(static_cast<int>(Direction::North), 8, "North = 8");
    TEST_ASSERT_EQ(static_cast<int>(Direction::East), 1, "East = 1");
    TEST_ASSERT_EQ(static_cast<int>(Direction::South), -8, "South = -8");
    TEST_ASSERT_EQ(static_cast<int>(Direction::West), -1, "West = -1");
    TEST_ASSERT_EQ(static_cast<int>(Direction::NorthEast), 9, "NorthEast = 9");
    TEST_ASSERT_EQ(static_cast<int>(Direction::SouthEast), -7, "SouthEast = -7");
    TEST_ASSERT_EQ(static_cast<int>(Direction::SouthWest), -9, "SouthWest = -9");
    TEST_ASSERT_EQ(static_cast<int>(Direction::NorthWest), 7, "NorthWest = 7");
    
    // Test direction navigation
    print_subtest("Direction navigation from e4");
    Square e4 = string_to_square("e4");
    int e4_idx = static_cast<int>(e4);
    
    Square north = static_cast<Square>(e4_idx + static_cast<int>(Direction::North));
    Square east = static_cast<Square>(e4_idx + static_cast<int>(Direction::East));
    Square ne = static_cast<Square>(e4_idx + static_cast<int>(Direction::NorthEast));
    
    TEST_ASSERT_EQ(square_to_string(north), "e5", "One step North from e4 is e5");
    TEST_ASSERT_EQ(square_to_string(east), "f4", "One step East from e4 is f4");
    TEST_ASSERT_EQ(square_to_string(ne), "f5", "One step NorthEast from e4 is f5");
    
    std::cout << GREEN << "All direction tests passed" << RESET << std::endl;
}

// Bitboard function tests
void ChessTests::test_bitboard_functions()
{
    print_test_header("Testing Bitboard Functions");
    
    // Test empty bitboard
    print_subtest("Empty bitboard");
    Bitboard empty_bb;
    TEST_ASSERT_EQ(empty_bb.count_bits(), 0, "Empty bitboard has 0 bits set");
    TEST_ASSERT_EQ(empty_bb.get_lsb_index(), static_cast<uint8_t>(Square::None), "Empty bitboard LSB is None");
    
    // Test file constructor
    print_subtest("File constructor");
    Bitboard file_e_bb(File::E);
    TEST_ASSERT_EQ(file_e_bb.count_bits(), 8, "File E bitboard has 8 bits set");
    TEST_ASSERT(file_e_bb.is_bit_set(Square::E1), "E1 is set in File E bitboard");
    TEST_ASSERT(file_e_bb.is_bit_set(Square::E8), "E8 is set in File E bitboard");
    TEST_ASSERT(!file_e_bb.is_bit_set(Square::A1), "A1 is not set in File E bitboard");
    
    if (visualize_output) {
        visualize_bitboard(file_e_bb);
    }
    
    // Test rank constructor
    print_subtest("Rank constructor");
    Bitboard rank_4_bb(Rank::Four);
    TEST_ASSERT_EQ(rank_4_bb.count_bits(), 8, "Rank 4 bitboard has 8 bits set");
    TEST_ASSERT(rank_4_bb.is_bit_set(Square::A4), "A4 is set in Rank 4 bitboard");
    TEST_ASSERT(rank_4_bb.is_bit_set(Square::H4), "H4 is set in Rank 4 bitboard");
    TEST_ASSERT(!rank_4_bb.is_bit_set(Square::E1), "E1 is not set in Rank 4 bitboard");
    
    if (visualize_output) {
        visualize_bitboard(rank_4_bb);
    }
    
    // Test square constructor
    print_subtest("Square constructor");
    Bitboard d5_bb(Square::D5);
    TEST_ASSERT_EQ(d5_bb.count_bits(), 1, "Square D5 bitboard has 1 bit set");
    TEST_ASSERT(d5_bb.is_bit_set(Square::D5), "D5 is set in Square D5 bitboard");
    TEST_ASSERT(!d5_bb.is_bit_set(Square::D4), "D4 is not set in Square D5 bitboard");
    
    if (visualize_output) {
        visualize_bitboard(d5_bb);
    }
    
    // Test bit manipulation
    print_subtest("Bit manipulation");
    Bitboard test_bb;
    test_bb.set_bit(Square::E4);
    test_bb.set_bit(Square::D5);
    test_bb.set_bit(Square::H8);
    
    if (visualize_output) {
        visualize_bitboard(test_bb);
    }
    
    TEST_ASSERT_EQ(test_bb.count_bits(), 3, "Bitboard has 3 bits set after setting");
    TEST_ASSERT(test_bb.is_bit_set(Square::E4), "E4 is set");
    
    test_bb.clear_bit(Square::D5);
    
    if (visualize_output) {
        visualize_bitboard(test_bb);
    }
    
    TEST_ASSERT_EQ(test_bb.count_bits(), 2, "Bitboard has 2 bits after clearing D5");
    TEST_ASSERT(!test_bb.is_bit_set(Square::D5), "D5 is not set after clearing");
    
    // Test LSB/MSB
    print_subtest("LSB/MSB operations");
    TEST_ASSERT_EQ(test_bb.get_lsb_index(), static_cast<uint8_t>(Square::E4), "LSB is E4");
    TEST_ASSERT_EQ(test_bb.get_msb_index(), static_cast<uint8_t>(Square::H8), "MSB is H8");
    
    uint8_t popped = test_bb.pop_lsb();
    TEST_ASSERT_EQ(popped, static_cast<uint8_t>(Square::E4), "Popped LSB is E4");
    TEST_ASSERT_EQ(test_bb.count_bits(), 1, "Bitboard has 1 bit after popping");
    
    std::cout << GREEN << "All bitboard function tests passed" << RESET << std::endl;
}

// Attack table tests
void ChessTests::test_attack_tables()
{
    print_test_header("Testing Attack Tables");
    
    // Test knight attacks
    print_subtest("Knight attacks");
    Bitboard knight_d4 = Bitboard::knight_attacks(Square::D4);
    TEST_ASSERT_EQ(knight_d4.count_bits(), 8, "Knight on D4 attacks 8 squares");
    TEST_ASSERT(knight_d4.is_bit_set(Square::C2), "Knight on D4 attacks C2");
    TEST_ASSERT(knight_d4.is_bit_set(Square::E6), "Knight on D4 attacks E6");
    TEST_ASSERT(!knight_d4.is_bit_set(Square::D5), "Knight on D4 doesn't attack D5");
    
    if (visualize_output) {
        visualize_bitboard(knight_d4);
    }
    
    Bitboard knight_h1 = Bitboard::knight_attacks(Square::H1);
    TEST_ASSERT_EQ(knight_h1.count_bits(), 2, "Knight on H1 attacks 2 squares");
    TEST_ASSERT(knight_h1.is_bit_set(Square::F2), "Knight on H1 attacks F2");
    TEST_ASSERT(knight_h1.is_bit_set(Square::G3), "Knight on H1 attacks G3");
    
    if (visualize_output) {
        visualize_bitboard(knight_h1);
    }
    
    // Test king attacks
    print_subtest("King attacks");
    Bitboard king_e4 = Bitboard::king_attacks(Square::E4);
    TEST_ASSERT_EQ(king_e4.count_bits(), 8, "King on E4 attacks 8 squares");
    TEST_ASSERT(king_e4.is_bit_set(Square::D3), "King on E4 attacks D3");
    TEST_ASSERT(king_e4.is_bit_set(Square::F5), "King on E4 attacks F5");
    TEST_ASSERT(!king_e4.is_bit_set(Square::C2), "King on E4 doesn't attack C2");
    
    if (visualize_output) {
        visualize_bitboard(king_e4);
    }
    
    Bitboard king_a8 = Bitboard::king_attacks(Square::A8);
    TEST_ASSERT_EQ(king_a8.count_bits(), 3, "King on A8 attacks 3 squares");
    TEST_ASSERT(king_a8.is_bit_set(Square::A7), "King on A8 attacks A7");
    TEST_ASSERT(king_a8.is_bit_set(Square::B8), "King on A8 attacks B8");
    TEST_ASSERT(king_a8.is_bit_set(Square::B7), "King on A8 attacks B7");
    
    if (visualize_output) {
        visualize_bitboard(king_a8);
    }
    
    // Test pawn attacks
    print_subtest("Pawn attacks");
    Bitboard white_pawn_e4 = Bitboard::pawn_attacks(Square::E4, Color::White);
    TEST_ASSERT_EQ(white_pawn_e4.count_bits(), 2, "White pawn on E4 attacks 2 squares");
    TEST_ASSERT(white_pawn_e4.is_bit_set(Square::D5), "White pawn on E4 attacks D5");
    TEST_ASSERT(white_pawn_e4.is_bit_set(Square::F5), "White pawn on E4 attacks F5");
    
    if (visualize_output) {
        visualize_bitboard(white_pawn_e4);
    }
    
    Bitboard black_pawn_e5 = Bitboard::pawn_attacks(Square::E5, Color::Black);
    TEST_ASSERT_EQ(black_pawn_e5.count_bits(), 2, "Black pawn on E5 attacks 2 squares");
    TEST_ASSERT(black_pawn_e5.is_bit_set(Square::D4), "Black pawn on E5 attacks D4");
    TEST_ASSERT(black_pawn_e5.is_bit_set(Square::F4), "Black pawn on E5 attacks F4");
    
    if (visualize_output) {
        visualize_bitboard(black_pawn_e5);
    }
    
    Bitboard white_pawn_a2 = Bitboard::pawn_attacks(Square::A2, Color::White);
    TEST_ASSERT_EQ(white_pawn_a2.count_bits(), 1, "White pawn on A2 attacks 1 square");
    TEST_ASSERT(white_pawn_a2.is_bit_set(Square::B3), "White pawn on A2 attacks B3");
    
    if (visualize_output) {
        visualize_bitboard(white_pawn_a2);
    }
    
    std::cout << GREEN << "All attack table tests passed" << RESET << std::endl;
}

// Sliding piece attack tests
void ChessTests::test_sliding_piece_attacks()
{
    print_test_header("Testing Sliding Piece Attacks");
    
    Bitboard empty;
    Bitboard occupied;
    occupied.set_bit(Square::D4);
    occupied.set_bit(Square::F6);
    occupied.set_bit(Square::B2);
    occupied.set_bit(Square::D6);
    
    if (visualize_output) {
        std::cout << "Occupied squares:" << std::endl;
        visualize_bitboard(occupied);
    }
    
    // Test bishop attacks
    print_subtest("Bishop attacks");
    Bitboard bishop_empty = Bitboard::bishop_attacks(Square::E5, empty);
    Bitboard bishop_occupied = Bitboard::bishop_attacks(Square::E5, occupied);
    
    if (visualize_output) {
        std::cout << "Bishop attacks on empty board:" << std::endl;
        visualize_bitboard(bishop_empty);
        
        std::cout << "Bishop attacks with occupied squares:" << std::endl;
        visualize_bitboard(bishop_occupied);
    }
    
    // On empty board, bishop on e5 should attack 13 squares
    TEST_ASSERT_EQ(bishop_empty.count_bits(), 13, "Bishop on E5 attacks 13 squares on empty board");
    TEST_ASSERT(bishop_empty.is_bit_set(Square::A1), "Bishop on E5 attacks A1 on empty board");
    TEST_ASSERT(bishop_empty.is_bit_set(Square::H8), "Bishop on E5 attacks H8 on empty board");
    
    // With blockers, fewer squares
    TEST_ASSERT(bishop_occupied.count_bits() < 13, "Bishop on E5 attacks fewer squares with blockers");
    TEST_ASSERT(bishop_occupied.is_bit_set(Square::F6), "Bishop on E5 attacks blocker at F6");
    TEST_ASSERT(!bishop_occupied.is_bit_set(Square::G7), "Bishop on E5 doesn't attack G7 (blocked by F6)");
    
    // Test rook attacks
    print_subtest("Rook attacks");
    Bitboard rook_empty = Bitboard::rook_attacks(Square::E5, empty);
    Bitboard rook_occupied = Bitboard::rook_attacks(Square::E5, occupied);
    
    if (visualize_output) {
        std::cout << "Rook attacks on empty board:" << std::endl;
        visualize_bitboard(rook_empty);
        
        std::cout << "Rook attacks with occupied squares:" << std::endl;
        visualize_bitboard(rook_occupied);
    }
    
    TEST_ASSERT_EQ(rook_empty.count_bits(), 14, "Rook on E5 attacks 14 squares on empty board");
    TEST_ASSERT(rook_empty.is_bit_set(Square::E1), "Rook on E5 attacks E1 on empty board");
    TEST_ASSERT(rook_empty.is_bit_set(Square::A5), "Rook on E5 attacks A5 on empty board");
    
    // Test queen attacks
    print_subtest("Queen attacks");
    Bitboard queen_empty = Bitboard::queen_attacks(Square::E5, empty);
    TEST_ASSERT_EQ(queen_empty.count_bits(), 27, "Queen on E5 attacks 27 squares on empty board");
    
    if (visualize_output) {
        std::cout << "Queen attacks on empty board:" << std::endl;
        visualize_bitboard(queen_empty);
    }
    
    // Edge cases
    print_subtest("Edge cases");
    Bitboard bishop_a1 = Bitboard::bishop_attacks(Square::A1, empty);
    TEST_ASSERT_EQ(bishop_a1.count_bits(), 7, "Bishop on A1 attacks 7 squares");
    TEST_ASSERT(bishop_a1.is_bit_set(Square::H8), "Bishop on A1 attacks H8");
    
    Bitboard rook_h8 = Bitboard::rook_attacks(Square::H8, empty);
    TEST_ASSERT_EQ(rook_h8.count_bits(), 14, "Rook on H8 attacks 14 squares");
    TEST_ASSERT(rook_h8.is_bit_set(Square::A8), "Rook on H8 attacks A8");
    TEST_ASSERT(rook_h8.is_bit_set(Square::H1), "Rook on H8 attacks H1");
    
    std::cout << GREEN << "All sliding piece attack tests passed" << RESET << std::endl;
}

// Position class tests
void ChessTests::test_position_fen_loading()
{
    print_test_header("Testing FEN Loading");
    
    Position pos;
    
    // Test starting position
    print_subtest("Starting position");
    bool loaded = pos.load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    TEST_ASSERT(loaded, "FEN loading succeeds");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    TEST_ASSERT_EQ(pos.piece_on(Square::E1), Piece::WhiteKing, "White king on E1");
    TEST_ASSERT_EQ(pos.piece_on(Square::E8), Piece::BlackKing, "Black king on E8");
    TEST_ASSERT_EQ(pos.piece_on(Square::A1), Piece::WhiteRook, "White rook on A1");
    TEST_ASSERT_EQ(pos.piece_on(Square::H8), Piece::BlackRook, "Black rook on H8");
    TEST_ASSERT_EQ(pos.piece_on(Square::E2), Piece::WhitePawn, "White pawn on E2");
    TEST_ASSERT_EQ(pos.piece_on(Square::E7), Piece::BlackPawn, "Black pawn on E7");
    TEST_ASSERT_EQ(pos.piece_on(Square::E4), Piece::None, "E4 is empty");
    TEST_ASSERT_EQ(pos.side_to_move(), Color::White, "White to move");
    
    // Test FEN export
    print_subtest("FEN export");
    std::string exported_fen = pos.to_fen();
    TEST_ASSERT_EQ(exported_fen, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "FEN export matches original");
    
    // Test position after 1.e4
    print_subtest("Position after 1.e4");
    loaded = pos.load_fen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    TEST_ASSERT(loaded, "FEN loading succeeds for 1.e4 position");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    TEST_ASSERT_EQ(pos.piece_on(Square::E4), Piece::WhitePawn, "White pawn on E4");
    TEST_ASSERT_EQ(pos.piece_on(Square::E2), Piece::None, "E2 is empty");
    TEST_ASSERT_EQ(pos.side_to_move(), Color::Black, "Black to move");
    
    // Test invalid FEN
    print_subtest("Invalid FEN handling");
    loaded = pos.load_fen("invalid fen string");
    TEST_ASSERT(!loaded, "Invalid FEN returns false");
    
    std::cout << GREEN << "All FEN loading tests passed" << RESET << std::endl;
}

void ChessTests::test_position_attack_detection()
{
    print_test_header("Testing Attack Detection");
    
    Position pos;
    
    // Test knight attacks
    print_subtest("Knight attacks");
    pos.load_fen("8/8/8/3n4/8/8/8/8 w - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    TEST_ASSERT(pos.is_square_attacked(Square::C3, Color::Black), "Knight on D5 attacks C3");
    TEST_ASSERT(pos.is_square_attacked(Square::E3, Color::Black), "Knight on D5 attacks E3");
    TEST_ASSERT(pos.is_square_attacked(Square::F4, Color::Black), "Knight on D5 attacks F4");
    TEST_ASSERT(!pos.is_square_attacked(Square::D4, Color::Black), "Knight on D5 doesn't attack D4");
    
    // Test bishop attacks with obstruction
    print_subtest("Bishop attacks with obstruction");
    pos.load_fen("8/8/2p5/3b4/8/5P2/8/8 w - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    TEST_ASSERT(pos.is_square_attacked(Square::A2, Color::Black), "Bishop attacks A2");
    TEST_ASSERT(pos.is_square_attacked(Square::E6, Color::Black), "Bishop attacks E6");
    TEST_ASSERT(pos.is_square_attacked(Square::F3, Color::Black), "Bishop attacks F3");
    TEST_ASSERT(!pos.is_square_attacked(Square::G2, Color::Black), "Bishop doesn't attack G2 (blocked)");
    
    // Test pawn attacks
    print_subtest("Pawn attacks");
    pos.load_fen("8/8/8/3P4/8/8/8/8 w - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    TEST_ASSERT(pos.is_square_attacked(Square::C6, Color::White), "White pawn on D5 attacks C6");
    TEST_ASSERT(pos.is_square_attacked(Square::E6, Color::White), "White pawn on D5 attacks E6");
    TEST_ASSERT(!pos.is_square_attacked(Square::D6, Color::White), "White pawn doesn't attack D6");
    
    std::cout << GREEN << "All attack detection tests passed" << RESET << std::endl;
}

void ChessTests::test_position_check_detection()
{
    print_test_header("Testing Check Detection");
    
    Position pos;
    
    // Test not in check
    print_subtest("Not in check");
    pos.load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    TEST_ASSERT(!pos.is_in_check(), "Starting position: White not in check");
    
    pos.load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1");
    TEST_ASSERT(!pos.is_in_check(), "Starting position: Black not in check");
    
    // Test check from queen
    print_subtest("Check from queen");
    pos.load_fen("3q4/8/8/8/8/8/8/3K4 w - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    TEST_ASSERT(pos.is_in_check(), "White king in check from queen");
    
    // Test check from rook
    print_subtest("Check from rook");
    pos.load_fen("3k4/8/8/8/8/8/8/3R4 b - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    TEST_ASSERT(pos.is_in_check(), "Black king in check from rook");
    
    // Test check from knight
    print_subtest("Check from knight");
    pos.load_fen("8/8/5n2/3K4/8/8/8/8 w - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    TEST_ASSERT(pos.is_in_check(), "White king in check from knight");
    
    // Test blocked check
    print_subtest("Blocked check");
    pos.load_fen("3k4/8/3n4/8/8/8/8/3R4 b - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    TEST_ASSERT(!pos.is_in_check(), "Black king not in check (knight blocks rook)");
    
    std::cout << GREEN << "All check detection tests passed" << RESET << std::endl;
}

// Helper function to count moves by piece type
std::map<PieceType, int> count_moves_by_piece(const Position& pos, const std::vector<Move>& moves)
{
    std::map<PieceType, int> counts;
    for (PieceType pt = PieceType::Pawn; pt < PieceType::NB; pt = static_cast<PieceType>(static_cast<int>(pt) + 1))
    {
        counts[pt] = 0;
    }
    
    for (const Move& move : moves)
    {
        Piece piece = pos.piece_on(move.from_square);
        PieceType type = type_of(piece);
        counts[type]++;
    }
    
    return counts;
}

// Helper function to count moves by type
std::map<MoveType, int> count_moves_by_type(const std::vector<Move>& moves)
{
    std::map<MoveType, int> counts;
    for (MoveType mt = MoveType::Normal; mt < MoveType::NB; mt = static_cast<MoveType>(static_cast<int>(mt) + 1))
    {
        counts[mt] = 0;
    }
    
    for (const Move& move : moves)
    {
        counts[move.move_type]++;
    }
    
    return counts;
}

void ChessTests::test_position_move_generation()
{
    print_test_header("Testing Complete Move Generation");
    
    Position pos;
    std::vector<Move> moves;
    
    // Test starting position - now should have 20 moves
    print_subtest("Starting position (all pieces)");
    pos.load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    moves = pos.generate_legal_moves();
    TEST_ASSERT_EQ(moves.size(), 20, "Starting position has 20 legal moves");
    
    // Count moves by piece type
    auto piece_counts = count_moves_by_piece(pos, moves);
    TEST_ASSERT_EQ(piece_counts[PieceType::Pawn], 16, "16 pawn moves");
    TEST_ASSERT_EQ(piece_counts[PieceType::Knight], 4, "4 knight moves");
    TEST_ASSERT_EQ(piece_counts[PieceType::Bishop], 0, "0 bishop moves");
    TEST_ASSERT_EQ(piece_counts[PieceType::Rook], 0, "0 rook moves");
    TEST_ASSERT_EQ(piece_counts[PieceType::Queen], 0, "0 queen moves");
    TEST_ASSERT_EQ(piece_counts[PieceType::King], 0, "0 king moves");
    
    // Test pawn moves
    print_subtest("Pawn move generation");
    pos.load_fen("8/8/8/8/3P4/8/P1P1P3/8 w - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    moves = pos.generate_legal_moves();
    // A2 can move 2, C2 can move 2, E2 can move 2, D4 can move 1 = 7 total
    TEST_ASSERT_EQ(moves.size(), 7, "Pawns have 7 moves total");
    
    // Test pawn captures
    print_subtest("Pawn captures");
    pos.load_fen("8/8/3p1p2/4P3/8/8/8/8 w - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    moves = pos.generate_legal_moves();
    auto type_counts = count_moves_by_type(moves);
    TEST_ASSERT_EQ(moves.size(), 3, "Pawn has 3 moves (1 push, 2 captures)");
    TEST_ASSERT_EQ(type_counts[MoveType::Normal], 1, "1 normal move");
    TEST_ASSERT_EQ(type_counts[MoveType::Capture], 2, "2 captures");
    
    // Test en passant
    print_subtest("En passant");
    pos.load_fen("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    moves = pos.generate_legal_moves();
    
    // Check for en passant move
    bool has_en_passant = false;
    for (const Move& move : moves) {
        if (move.move_type == MoveType::EnPassant) {
            has_en_passant = true;
            TEST_ASSERT_EQ(move.from_square, Square::E5, "En passant from E5");
            TEST_ASSERT_EQ(move.to_square, Square::F6, "En passant to F6");
        }
    }
    TEST_ASSERT(has_en_passant, "En passant move is available");
    
    // Test promotion
    print_subtest("Pawn promotion");
    pos.load_fen("8/P7/8/8/8/8/7p/8 w - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    moves = pos.generate_legal_moves();
    type_counts = count_moves_by_type(moves);
    TEST_ASSERT_EQ(type_counts[MoveType::Promotion], 4, "4 promotion moves");
    
    // Test bishop moves
    print_subtest("Bishop moves");
    pos.load_fen("8/8/3p4/4B3/3P4/8/8/8 w - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    moves = pos.generate_legal_moves();
    piece_counts = count_moves_by_piece(pos, moves);
    TEST_ASSERT(piece_counts[PieceType::Bishop] > 0, "Bishop has moves");
    
    // Test rook moves
    print_subtest("Rook moves");
    pos.load_fen("8/8/8/3pRp2/8/8/8/8 w - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    moves = pos.generate_legal_moves();
    piece_counts = count_moves_by_piece(pos, moves);
    TEST_ASSERT_EQ(piece_counts[PieceType::Rook], 9, "Rook has 9 moves");
    
    // Test queen moves
    print_subtest("Queen moves");
    pos.load_fen("8/8/3p4/4Q3/3P4/8/8/8 w - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    moves = pos.generate_legal_moves();
    piece_counts = count_moves_by_piece(pos, moves);
    TEST_ASSERT(piece_counts[PieceType::Queen] > 15, "Queen has many moves");
    
    // Test castling
    print_subtest("Castling moves");
    pos.load_fen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    moves = pos.generate_legal_moves();
    type_counts = count_moves_by_type(moves);
    TEST_ASSERT_EQ(type_counts[MoveType::Castle], 2, "2 castling moves available");
    
    // Test castling blocked
    print_subtest("Castling blocked");
    pos.load_fen("r3k2r/8/8/8/8/8/8/RN2K1NR w KQkq - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    moves = pos.generate_legal_moves();
    type_counts = count_moves_by_type(moves);
    TEST_ASSERT_EQ(type_counts[MoveType::Castle], 0, "No castling moves (pieces in the way)");
    
    // Test castling through check
    print_subtest("Can't castle through check");
    pos.load_fen("r3k1r1/8/8/8/8/8/3r4/R3K2R w KQ - 1 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    moves = pos.generate_legal_moves();
    type_counts = count_moves_by_type(moves);
    TEST_ASSERT_EQ(type_counts[MoveType::Castle], 0, "No castling (would move through check)");
    
    // Test legal move filtering (pinned piece)
    print_subtest("Pinned piece");
    pos.load_fen("8/8/8/r3N2K/8/8/8/4k3 w - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    moves = pos.generate_legal_moves();
    
    // Knight is pinned and can't move
    bool knight_can_move = false;
    for (const Move& move : moves) {
        if (move.from_square == Square::E5) {
            knight_can_move = true;
        }
    }
    TEST_ASSERT(!knight_can_move, "Pinned knight cannot move");
    
    std::cout << GREEN << "All move generation tests passed" << RESET << std::endl;
}

void ChessTests::test_position_make_move()
{
    print_test_header("Testing Make Move (All Types)");
    
    Position pos;
    
    // Test simple moves
    print_subtest("Simple moves");
    pos.load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    Move pawn_move(Square::E2, Square::E4, MoveType::Normal);
    pos.make_move(pawn_move);
    
    if (visualize_output) {
        std::cout << "After e2-e4:" << std::endl;
        visualize_board(pos);
    }
    
    TEST_ASSERT_EQ(pos.piece_on(Square::E2), Piece::None, "E2 is empty after move");
    TEST_ASSERT_EQ(pos.piece_on(Square::E4), Piece::WhitePawn, "Pawn on E4 after move");
    TEST_ASSERT_EQ(pos.side_to_move(), Color::Black, "Black to move");
    
    // Check en passant square was set
    std::string fen = pos.to_fen();
    TEST_ASSERT(fen.find("e3") != std::string::npos, "En passant square set to e3");
    
    // Test capture
    print_subtest("Capture move");
    pos.load_fen("rnbqkbnr/ppp1pppp/8/3p4/8/2N5/PPPPPPPP/R1BQKBNR w KQkq - 0 2");
    
    Move capture_move(Square::C3, Square::D5, MoveType::Capture);
    pos.make_move(capture_move);
    
    if (visualize_output) {
        std::cout << "After Nc3xd5:" << std::endl;
        visualize_board(pos);
    }
    
    TEST_ASSERT_EQ(pos.piece_on(Square::C3), Piece::None, "C3 is empty after capture");
    TEST_ASSERT_EQ(pos.piece_on(Square::D5), Piece::WhiteKnight, "Knight on D5 after capture");
    
    // Test castling kingside
    print_subtest("Kingside castling");
    pos.load_fen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    
    Move castle_k(Square::E1, Square::G1, MoveType::Castle);
    pos.make_move(castle_k);
    
    if (visualize_output) {
        std::cout << "After O-O:" << std::endl;
        visualize_board(pos);
    }
    
    TEST_ASSERT_EQ(pos.piece_on(Square::E1), Piece::None, "E1 empty after castling");
    TEST_ASSERT_EQ(pos.piece_on(Square::G1), Piece::WhiteKing, "King on G1");
    TEST_ASSERT_EQ(pos.piece_on(Square::H1), Piece::None, "H1 empty after castling");
    TEST_ASSERT_EQ(pos.piece_on(Square::F1), Piece::WhiteRook, "Rook on F1");
    
    // Test castling queenside
    print_subtest("Queenside castling");
    pos.load_fen("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");
    
    Move castle_q(Square::E8, Square::C8, MoveType::Castle);
    pos.make_move(castle_q);
    
    if (visualize_output) {
        std::cout << "After ...O-O-O:" << std::endl;
        visualize_board(pos);
    }
    
    TEST_ASSERT_EQ(pos.piece_on(Square::E8), Piece::None, "E8 empty after castling");
    TEST_ASSERT_EQ(pos.piece_on(Square::C8), Piece::BlackKing, "King on C8");
    TEST_ASSERT_EQ(pos.piece_on(Square::A8), Piece::None, "A8 empty after castling");
    TEST_ASSERT_EQ(pos.piece_on(Square::D8), Piece::BlackRook, "Rook on D8");
    
    // Test en passant capture
    print_subtest("En passant capture");
    pos.load_fen("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
    
    Move ep_move(Square::E5, Square::F6, MoveType::EnPassant);
    pos.make_move(ep_move);
    
    if (visualize_output) {
        std::cout << "After exf6 e.p.:" << std::endl;
        visualize_board(pos);
    }
    
    TEST_ASSERT_EQ(pos.piece_on(Square::E5), Piece::None, "E5 empty after en passant");
    TEST_ASSERT_EQ(pos.piece_on(Square::F6), Piece::WhitePawn, "White pawn on F6");
    TEST_ASSERT_EQ(pos.piece_on(Square::F5), Piece::None, "F5 empty (captured pawn removed)");
    
    // Test promotion
    print_subtest("Promotion");
    pos.load_fen("8/P7/8/8/8/8/8/8 w - - 0 1");
    
    Move promo_move(Square::A7, Square::A8, MoveType::Promotion, Piece::WhiteQueen);
    pos.make_move(promo_move);
    
    if (visualize_output) {
        std::cout << "After a8=Q:" << std::endl;
        visualize_board(pos);
    }
    
    TEST_ASSERT_EQ(pos.piece_on(Square::A7), Piece::None, "A7 empty after promotion");
    TEST_ASSERT_EQ(pos.piece_on(Square::A8), Piece::WhiteQueen, "Queen on A8");
    
    // Test castling rights update
    print_subtest("Castling rights updates");
    pos.load_fen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    
    // Move king
    Move king_move(Square::E1, Square::E2, MoveType::Normal);
    pos.make_move(king_move);
    
    fen = pos.to_fen();
    TEST_ASSERT(fen.find("kq") != std::string::npos && fen.find("KQ") == std::string::npos, 
                "White castling rights removed after king move");
    
    std::cout << GREEN << "All make move tests passed" << RESET << std::endl;
}

// Comprehensive test for complete game scenarios
void ChessTests::test_game_scenarios()
{
    print_test_header("Testing Game Scenarios");
    
    Position pos;
    
    // Test Italian Opening sequence
    print_subtest("Italian Opening sequence");
    pos.load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    // 1. e4
    pos.load_fen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    
    if (visualize_output) {
        std::cout << "After 1.e4:" << std::endl;
        visualize_board(pos);
    }
    
    TEST_ASSERT_EQ(pos.piece_on(Square::E4), Piece::WhitePawn, "1.e4 - pawn on e4");
    
    // 1... e5
    pos.load_fen("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2");
    
    if (visualize_output) {
        std::cout << "After 1...e5:" << std::endl;
        visualize_board(pos);
    }
    
    TEST_ASSERT_EQ(pos.piece_on(Square::E5), Piece::BlackPawn, "1...e5 - pawn on e5");
    
    // Test endgame position
    print_subtest("Endgame position");
    pos.load_fen("8/8/4k3/8/8/4K3/4P3/8 w - - 0 1");
    
    if (visualize_output) {
        std::cout << "Endgame position:" << std::endl;
        visualize_board(pos);
    }
    
    TEST_ASSERT_EQ(pos.piece_on(Square::E3), Piece::WhiteKing, "White king on e3");
    TEST_ASSERT_EQ(pos.piece_on(Square::E6), Piece::BlackKing, "Black king on e6");
    TEST_ASSERT_EQ(pos.piece_on(Square::E2), Piece::WhitePawn, "White pawn on e2");
    
    // Test complex middlegame position
    print_subtest("Complex middlegame position");
    pos.load_fen("r1bqk2r/pp1nbppp/2p1pn2/3p4/2PP4/2N1PN2/PP2BPPP/R1BQKR2 w Qkq - 0 8");
    
    if (visualize_output) {
        std::cout << "Complex middlegame position:" << std::endl;
        visualize_board(pos);
    }
    
    std::vector<Move> moves = pos.generate_legal_moves();
    TEST_ASSERT(moves.size() > 20, "Complex position has many moves");
    
    // Perft test for move generation correctness
    print_subtest("Standard Perft Tests");
    
    // Define a perft function
    std::function<uint64_t(Position&, int)> perft = [&](Position& position, int depth) -> uint64_t 
    {
        if (depth == 0) return 1;
        
        uint64_t nodes = 0;
        std::vector<Move> moves = position.generate_legal_moves();
        
        for (const Move& move : moves) {
            Position copy = position;
            copy.make_move(move);
            
            uint64_t curr_nodes = perft(copy, depth - 1);
            nodes += curr_nodes;
        }
        
        return nodes;
    };
    
    std::cout << "Using perft depth: " << perft_depth << std::endl;
    
    // Test 1: Starting position
    print_subtest("Position 1: Starting position");
    pos.load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    // Expected node counts for different depths
    uint64_t expected_counts[] = {1, 20, 400, 8902, 197281, 4865609};
    uint64_t expected = (perft_depth <= 5) ? expected_counts[perft_depth] : 4865609;
    uint64_t nodes = perft(pos, perft_depth);
    std::cout << "Perft(" << perft_depth << ") from starting position:" << std::endl;
    std::cout << "  Expected: " << expected << std::endl;
    std::cout << "  Actual:   " << nodes << std::endl;
    if (nodes == expected) {
        std::cout << GREEN << "  PASS: Starting position perft test passed!" << RESET << std::endl;
    } else {
        std::cout << RED << "  FAIL: Starting position perft test failed!" << RESET << std::endl;
    }
    TEST_ASSERT_EQ(nodes, expected, "Perft(5) from starting position = 4865609");
    
    // Test 2: Kiwipete position (complex middlegame)
    print_subtest("Position 2: Kiwipete position");
    pos.load_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    // Expected node counts for different depths for Kiwipete position
    uint64_t kiwipete_counts[] = {1, 48, 2039, 97862, 4085603, 193690690};
    expected = (perft_depth <= 5) ? kiwipete_counts[perft_depth] : 193690690;
    nodes = perft(pos, perft_depth);
    std::cout << "Perft(" << perft_depth << ") from Kiwipete position:" << std::endl;
    std::cout << "  Expected: " << expected << std::endl;
    std::cout << "  Actual:   " << nodes << std::endl;
    if (nodes == expected) {
        std::cout << GREEN << "  PASS: Kiwipete position perft test passed!" << RESET << std::endl;
    } else {
        std::cout << RED << "  FAIL: Kiwipete position perft test failed!" << RESET << std::endl;
    }
    TEST_ASSERT_EQ(nodes, expected, "Perft(5) from Kiwipete position = 193690690");
    
    // Test 3: Position 3 (another complex position)
    print_subtest("Position 3: Endgame position");
    pos.load_fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    // Expected node counts for different depths for Position 3
    uint64_t pos3_counts[] = {1, 14, 191, 2812, 43238, 674624};
    expected = (perft_depth <= 5) ? pos3_counts[perft_depth] : 674624;
    nodes = perft(pos, perft_depth);
    std::cout << "Perft(" << perft_depth << ") from Position 3:" << std::endl;
    std::cout << "  Expected: " << expected << std::endl;
    std::cout << "  Actual:   " << nodes << std::endl;
    if (nodes == expected) {
        std::cout << GREEN << "  PASS: Position 3 perft test passed!" << RESET << std::endl;
    } else {
        std::cout << RED << "  FAIL: Position 3 perft test failed!" << RESET << std::endl;
    }
    TEST_ASSERT_EQ(nodes, expected, "Perft(5) from Position 3 = 674624");
    
    // Test 4: Position 4 (position with en passant captures)
    print_subtest("Position 4: En passant position");
    pos.load_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    // Expected node counts for different depths for Position 4
    uint64_t pos4_counts[] = {1, 6, 264, 9467, 422333, 15833292};
    expected = (perft_depth <= 5) ? pos4_counts[perft_depth] : 15833292;
    nodes = perft(pos, perft_depth);
    std::cout << "Perft(" << perft_depth << ") from Position 4:" << std::endl;
    std::cout << "  Expected: " << expected << std::endl;
    std::cout << "  Actual:   " << nodes << std::endl;
    if (nodes == expected) {
        std::cout << GREEN << "  PASS: Position 4 perft test passed!" << RESET << std::endl;
    } else {
        std::cout << RED << "  FAIL: Position 4 perft test failed!" << RESET << std::endl;
    }
    TEST_ASSERT_EQ(nodes, expected, "Perft(5) from Position 4 = 422333");
    
    // Test 5: Position 5 (position with castling)
    print_subtest("Position 5: Castling position");
    pos.load_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    // Expected node counts for different depths for Position 5
    uint64_t pos5_counts[] = {1, 44, 1486, 62379, 2103487, 89941194};
    expected = (perft_depth <= 5) ? pos5_counts[perft_depth] : 89941194;
    nodes = perft(pos, perft_depth);
    std::cout << "Perft(" << perft_depth << ") from Position 5:" << std::endl;
    std::cout << "  Expected: " << expected << std::endl;
    std::cout << "  Actual:   " << nodes << std::endl;
    if (nodes == expected) {
        std::cout << GREEN << "  PASS: Position 5 perft test passed!" << RESET << std::endl;
    } else {
        std::cout << RED << "  FAIL: Position 5 perft test failed!" << RESET << std::endl;
    }
    TEST_ASSERT_EQ(nodes, expected, "Perft(5) from Position 5 = 89941194");
    
    // Test 6: Position 6 (position with promotions)
    print_subtest("Position 6: Symmetrical position");
    pos.load_fen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    // Expected node counts for different depths for Position 6
    uint64_t pos6_counts[] = {1, 46, 2079, 89890, 3894594, 164075551};
    expected = (perft_depth <= 5) ? pos6_counts[perft_depth] : 164075551;
    nodes = perft(pos, perft_depth);
    std::cout << "Perft(" << perft_depth << ") from Position 6:" << std::endl;
    std::cout << "  Expected: " << expected << std::endl;
    std::cout << "  Actual:   " << nodes << std::endl;
    if (nodes == expected) {
        std::cout << GREEN << "  PASS: Position 6 perft test passed!" << RESET << std::endl;
    } else {
        std::cout << RED << "  FAIL: Position 6 perft test failed!" << RESET << std::endl;
    }
    TEST_ASSERT_EQ(nodes, expected, "Perft(5) from Position 6 = 164075551");
    
    std::cout << GREEN << "All game scenario tests passed" << RESET << std::endl;
}

// Performance tests
void ChessTests::test_performance()
{
    print_test_header("Performance Tests");
    
    print_subtest("Bitboard operations");
    Bitboard bb;
    const int iterations = 1000000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        bb.set_bit(static_cast<Square>(i % 64));
        bb.count_bits();
        bb.clear_bit(static_cast<Square>(i % 64));
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "  " << iterations << " bitboard operations: " 
              << duration.count() << " microseconds" << std::endl;
    
    print_subtest("Attack generation");
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations / 100; i++) {
        Square sq = static_cast<Square>(i % 64);
        Bitboard::knight_attacks(sq);
        Bitboard::king_attacks(sq);
        Bitboard::pawn_attacks(sq, Color::White);
    }
    end = std::chrono::high_resolution_clock::now();
    
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "  " << iterations / 100 << " attack generations: " 
              << duration.count() << " microseconds" << std::endl;
    
    print_subtest("Move generation");
    Position pos;
    pos.load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; i++) {
        std::vector<Move> moves = pos.generate_legal_moves();
    }
    end = std::chrono::high_resolution_clock::now();
    
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "  10000 move generations from starting position: " 
              << duration.count() << " microseconds" << std::endl;
    
    std::cout << GREEN << "Performance tests completed" << RESET << std::endl;
}

// Edge case tests
void ChessTests::test_edge_cases()
{
    print_test_header("Testing Edge Cases");
    
    Position pos;
    
    // Test empty board
    print_subtest("Empty board");
    pos.load_fen("8/8/8/8/8/8/8/8 w - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    std::vector<Move> moves = pos.generate_legal_moves();
    TEST_ASSERT_EQ(moves.size(), 0, "Empty board has no moves");
    
    
    // Test maximum pieces (illegal but tests robustness)
    print_subtest("Maximum pieces position");
    pos.load_fen("rnbqkbnr/pppppppp/8/8/PPPPPPPP/PPPPPPPP/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    // Should handle without crashing
    moves = pos.generate_legal_moves();
    std::cout << "  Generated " << moves.size() << " moves (position is illegal but handled)" << std::endl;
    
    // Test promotion edge case
    print_subtest("Multiple promotions available");
    pos.load_fen("1n2k3/P7/8/8/8/8/7p/3K4 w - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    moves = pos.generate_legal_moves();
    auto type_counts = count_moves_by_type(moves);
    TEST_ASSERT_EQ(type_counts[MoveType::Promotion], 8, "8 promotion moves (4 quiet + 4 captures)");
    
    // Test stalemate position
    print_subtest("Stalemate position");
    pos.load_fen("k7/8/1K6/8/8/8/8/8 b - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    // Debug stalemate position
    std::cout << "Debugging stalemate position:" << std::endl;
    std::vector<Move> pseudo_legal;
    MoveGenerator::generate_all_moves(pos, pseudo_legal, pos.side_to_move());
    std::cout << "Pseudo-legal moves: " << pseudo_legal.size() << std::endl;
    
    for (const Move& move : pseudo_legal) {
        std::cout << "Move: " << square_to_string(move.from_square) << " to " << square_to_string(move.to_square) << std::endl;
        
        // Make move on a copy
        Position test_pos = pos;
        Square king_before = test_pos.king_square(pos.side_to_move());
        std::cout << "  King before: " << square_to_string(king_before) << std::endl;
        
        test_pos.make_move(move);
        
        // After move, we need to check if our king (the one that just moved) is in check
        Color our_color = pos.side_to_move();
        Color enemy_color = (our_color == Color::White) ? Color::Black : Color::White;
        
        Square king_after = test_pos.king_square(our_color);
        std::cout << "  King after: " << square_to_string(king_after) << std::endl;
        
        bool is_attacked = test_pos.is_square_attacked(king_after, enemy_color);
        std::cout << "  Is king attacked? " << (is_attacked ? "Yes" : "No") << std::endl;
    }
    
    moves = pos.generate_legal_moves();
    std::cout << "Legal moves: " << moves.size() << std::endl;
    TEST_ASSERT_EQ(moves.size(), 0, "Stalemate - no legal moves");
    TEST_ASSERT(!pos.is_in_check(), "Not in check (stalemate, not checkmate)");
    
    // Test checkmate position
    print_subtest("Checkmate position");
    pos.load_fen("R6k/8/7K/8/8/8/8/8 b - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    moves = pos.generate_legal_moves();
    TEST_ASSERT_EQ(moves.size(), 0, "Checkmate - no legal moves");
    TEST_ASSERT(pos.is_in_check(), "In check (checkmate)");
    
    std::cout << GREEN << "All edge case tests passed" << RESET << std::endl;
}

// Regression tests for specific bugs
void ChessTests::test_regression_bugs()
{
    print_test_header("Regression Tests");
    
    Position pos;
    
    // Test for en passant edge case
    print_subtest("En passant after double push");
    pos.load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    // Make double pawn push
    Move e2e4(Square::E2, Square::E4, MoveType::Normal);
    pos.make_move(e2e4);
    
    // Verify en passant square is set
    std::string fen = pos.to_fen();
    TEST_ASSERT(fen.find("e3") != std::string::npos, "En passant square set after double push");
    
    // Test castling rights removed correctly
    print_subtest("Castling rights after rook capture");
    pos.load_fen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    
    // Capture black's a-rook
    Move rxa8(Square::A1, Square::A8, MoveType::Capture);
    pos.make_move(rxa8);
    
    fen = pos.to_fen();
    TEST_ASSERT(fen.find("k") != std::string::npos && fen.find("q") == std::string::npos,
                "Black queenside castling rights removed after a8 capture");
    
    // Test boundary conditions for move generation
    print_subtest("Pieces on edges");
    pos.load_fen("N6N/8/8/8/8/8/8/N6N w - - 0 1");
    
    if (visualize_output) {
        visualize_board(pos);
    }
    
    std::vector<Move> moves = pos.generate_legal_moves();
    TEST_ASSERT_EQ(moves.size(), 8, "4 knights in corners have 8 moves total");
    
    std::cout << GREEN << "All regression tests passed" << RESET << std::endl;
}

void ChessTests::test_position_unmake_move()
{
    print_test_header("Testing Unmake Move Functionality");
    
    Position pos;
    
    // Helper lambda to verify position states are identical
    auto positions_equal = [](const Position& p1, const Position& p2) -> bool {
        // Compare FEN strings as a comprehensive check
        if (p1.to_fen() != p2.to_fen()) return false;
        
        // Additional checks for internal state
        if (p1.castling_rights() != p2.castling_rights()) return false;
        if (p1.en_passant_square() != p2.en_passant_square()) return false;
        if (p1.halfmove_clock() != p2.halfmove_clock()) return false;
        if (p1.fullmove_number() != p2.fullmove_number()) return false;
        if (p1.side_to_move() != p2.side_to_move()) return false;
        
        // Check all squares
        for (int sq = 0; sq < 64; ++sq) {
            if (p1.piece_on(static_cast<Square>(sq)) != p2.piece_on(static_cast<Square>(sq))) {
                return false;
            }
        }
        
        return true;
    };
    
    // Test 1: Simple pawn move
    print_subtest("Simple pawn move make/unmake");
    pos.load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Position original = pos;
    
    Move e2e4(Square::E2, Square::E4, MoveType::Normal);
    pos.make_move(e2e4);
    
    if (visualize_output) {
        std::cout << "After e2-e4:" << std::endl;
        visualize_board(pos);
    }
    
    pos.undo_move();
    
    if (visualize_output) {
        std::cout << "After undo:" << std::endl;
        visualize_board(pos);
    }
    
    TEST_ASSERT(positions_equal(pos, original), "Position restored after simple move unmake");
    
    // Test 2: Capture move
    print_subtest("Capture move make/unmake");
    pos.load_fen("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2");
    original = pos;
    
    Move exd5(Square::E4, Square::D5, MoveType::Capture);
    pos.make_move(exd5);
    TEST_ASSERT_EQ(pos.piece_on(Square::D5), Piece::WhitePawn, "White pawn on d5 after capture");
    
    pos.undo_move();
    TEST_ASSERT(positions_equal(pos, original), "Position restored after capture unmake");
    TEST_ASSERT_EQ(pos.piece_on(Square::D5), Piece::BlackPawn, "Black pawn restored on d5");
    
    // Test 3: Castling
    print_subtest("Castling make/unmake");
    pos.load_fen("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    original = pos;
    
    Move castle_k(Square::E1, Square::G1, MoveType::Castle);
    pos.make_move(castle_k);
    TEST_ASSERT_EQ(pos.piece_on(Square::G1), Piece::WhiteKing, "King on g1 after castling");
    TEST_ASSERT_EQ(pos.piece_on(Square::F1), Piece::WhiteRook, "Rook on f1 after castling");
    
    pos.undo_move();
    TEST_ASSERT(positions_equal(pos, original), "Position restored after castling unmake");
    TEST_ASSERT_EQ(pos.piece_on(Square::E1), Piece::WhiteKing, "King back on e1");
    TEST_ASSERT_EQ(pos.piece_on(Square::H1), Piece::WhiteRook, "Rook back on h1");
    TEST_ASSERT_EQ(pos.castling_rights() & 0x3, 0x3, "White castling rights restored");
    
    // Test 4: En passant
    print_subtest("En passant make/unmake");
    pos.load_fen("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
    original = pos;
    
    Move ep(Square::E5, Square::F6, MoveType::EnPassant);
    pos.make_move(ep);
    TEST_ASSERT_EQ(pos.piece_on(Square::F6), Piece::WhitePawn, "White pawn on f6 after en passant");
    TEST_ASSERT_EQ(pos.piece_on(Square::F5), Piece::None, "Black pawn removed from f5");
    
    pos.undo_move();
    TEST_ASSERT(positions_equal(pos, original), "Position restored after en passant unmake");
    TEST_ASSERT_EQ(pos.piece_on(Square::F5), Piece::BlackPawn, "Black pawn restored on f5");
    TEST_ASSERT_EQ(pos.en_passant_square(), Square::F6, "En passant square restored");
    
    // Test 5: Promotion
    print_subtest("Promotion make/unmake");
    pos.load_fen("rnbqkbnr/pppppppP/8/8/8/8/PPPPPP1P/RNBQKBNR w KQq - 0 1");
    original = pos;

    Move promo(Square::H7, Square::H8, MoveType::Promotion, Piece::WhiteQueen);
    pos.make_move(promo);
    TEST_ASSERT_EQ(pos.piece_on(Square::H8), Piece::WhiteQueen, "Queen on h8 after promotion");

    pos.undo_move();
    TEST_ASSERT(positions_equal(pos, original), "Position restored after promotion unmake");
    TEST_ASSERT_EQ(pos.piece_on(Square::H7), Piece::WhitePawn, "Pawn restored on h7");
    TEST_ASSERT_EQ(pos.piece_on(Square::H8), Piece::BlackRook, "Black rook restored on h8");
    
    // Test 6: Multiple moves and unmakes
    print_subtest("Multiple moves make/unmake sequence");
    pos.load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    original = pos;
    
    // Make several moves
    Move m1(Square::E2, Square::E4, MoveType::Normal);
    Move m2(Square::E7, Square::E5, MoveType::Normal);
    Move m3(Square::G1, Square::F3, MoveType::Normal);
    Move m4(Square::B8, Square::C6, MoveType::Normal);
    
    pos.make_move(m1);
    pos.make_move(m2);
    pos.make_move(m3);
    pos.make_move(m4);
    
    if (visualize_output) {
        std::cout << "After 4 moves:" << std::endl;
        visualize_board(pos);
    }
    
    // Undo all moves
    pos.undo_moves(4);
    
    if (visualize_output) {
        std::cout << "After undoing 4 moves:" << std::endl;
        visualize_board(pos);
    }
    
    TEST_ASSERT(positions_equal(pos, original), "Position restored after multiple unmakes");
    
    // Test 7: Complex position with state changes
    print_subtest("Complex position with multiple state changes");
    pos.load_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    original = pos;
    
    std::vector<Move> moves = pos.generate_legal_moves();
    
    // Test make/unmake for each legal move
    int tested = 0;
    for (const Move& move : moves) {
        Position before = pos;
        pos.make_move(move);
        pos.undo_move();
        
        if (!positions_equal(pos, before)) {
            std::cout << RED << "Failed on move: " << square_to_string(move.from_square) 
                      << " to " << square_to_string(move.to_square) << RESET << std::endl;
            TEST_ASSERT(false, "Make/unmake failed to restore position");
        }
        tested++;
    }
    
    std::cout << GREEN << "  Successfully tested " << tested << " moves" << RESET << std::endl;
    
    // Test 8: Verify move history is properly maintained
    print_subtest("Move history consistency");
    pos.load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    TEST_ASSERT_EQ(pos.move_count(), 0, "No moves in history initially");
    
    Move e4(Square::E2, Square::E4, MoveType::Normal);
    pos.make_move(e4);
    TEST_ASSERT_EQ(pos.move_count(), 1, "One move in history after make_move");
    
    pos.undo_move();
    TEST_ASSERT_EQ(pos.move_count(), 0, "History cleared after undo");
    
    std::cout << GREEN << "All unmake move tests passed" << RESET << std::endl;
}

// Run all tests
void ChessTests::run_all_tests()
{
    std::cout << BLUE << "\n=== ChessRules Library Comprehensive Test Suite ===" << RESET << std::endl;
    std::cout << "Starting test run...\n" << std::endl;
    
    // Initialize attack tables
    Bitboard::init_attack_tables();
    
    try {
        // Core functionality tests
        test_square_functions();
        test_piece_functions();
        test_distance_functions();
        test_direction_values();
        
        // Bitboard tests
        test_bitboard_functions();
        test_attack_tables();
        test_sliding_piece_attacks();
        
        // Position tests
        test_position_fen_loading();
        test_position_attack_detection();
        test_position_check_detection();
        test_position_move_generation();
        test_position_make_move();
        test_position_unmake_move();
        
        // Advanced tests
        test_edge_cases();
        test_regression_bugs();
        test_game_scenarios();
        
        // Performance tests (optional)
        if (true) {  // Set to true to run performance tests
            test_performance();
        }
        
    } catch (const std::exception& e) {
        std::cerr << RED << "\nTest suite failed with exception: " << e.what() << RESET << std::endl;
        global_results.print_summary();
        return;
    }
    
    // Print final summary
    global_results.print_summary();
    
    if (global_results.failed == 0) {
        std::cout << "\n" << GREEN << "ALL TESTS PASSED!" << RESET << std::endl;
    } else {
        std::cout << "\n" << RED << "SOME TESTS FAILED" << RESET << std::endl;
    }
}

} // namespace luna
