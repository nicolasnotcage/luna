#ifndef TESTS_H
#define TESTS_H

#include "types.h"
#include "bitboard.h"
#include "position.h"

#include <string>

namespace luna {

// Test class for ChessEngine
class ChessTests {
private:
    // Test result tracking
    struct TestResult {
        int passed = 0;
        int failed = 0;
        
        void pass() { passed++; }
        void fail() { failed++; }
        
        void print_summary() const;
    };

    TestResult global_results;
    bool visualize_output = false;
    int perft_depth = 3; // Default perft depth

    // Helper methods for test output
    void print_test_header(const std::string& test_name);
    void print_subtest(const std::string& subtest_name);
    
    // Visualization helper
    void visualize_board(const Position& pos);
    void visualize_bitboard(const Bitboard& bb);

public:
    ChessTests() = default;
    ~ChessTests() = default;

    // Toggle visualization of test output
    void set_visualization(bool enable);
    bool is_visualization_enabled() const;
    
    // Set perft depth for testing
    void set_perft_depth(int depth);

    // Test functions
    void test_square_functions();
    void test_piece_functions();
    void test_distance_functions();
    void test_direction_values();
    void test_bitboard_functions();
    void test_attack_tables();
    void test_sliding_piece_attacks();
    void test_position_fen_loading();
    void test_position_attack_detection();
    void test_position_check_detection();
    void test_position_move_generation();
    void test_position_make_move();
    void test_game_scenarios();
    void test_performance();
    void test_edge_cases();
    void test_regression_bugs();
    void test_position_unmake_move();

    // Run all tests
    void run_all_tests();

    // Get test results
    int get_passed_count() const;
    int get_failed_count() const;
    bool all_tests_passed() const;
};

} // namespace luna

// Helper macro for assertions with descriptive messages
#define TEST_ASSERT(condition, message) \
    do { \
        __pragma(warning(push)) \
        __pragma(warning(disable: 4127)) \
        if (!(condition)) { \
            std::cerr << RED << "ASSERTION FAILED: " << message << RESET << std::endl; \
            std::cerr << "  File: " << __FILE__ << ", Line: " << __LINE__ << std::endl; \
            global_results.fail(); \
            assert(false); \
        } else { \
            global_results.pass(); \
        } \
        __pragma(warning(pop)) \
    } while(0)

#define TEST_ASSERT_EQ(actual, expected, message) \
    TEST_ASSERT((actual) == (expected), \
                std::string(message) + " (expected: " + to_debug_string(expected) + ", actual: " + to_debug_string(actual) + ")")

#endif // TESTS_H
