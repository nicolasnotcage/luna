/*
    Main entry point for Luna chess engine with unified UCI/UCI+ interface.
    
    Author: Nicolas Miller
    Date: 07/24/2025
*/

#include "unified_uci_interface.h"
#include "bitboard.h"
#include "tests.h"
#include <iostream>
#include <string>

void print_usage(const char* program_name) 
{
    std::cout << "Luna Chess Engine v1.0" << std::endl;
    std::cout << "Usage: " << program_name << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  (no args)    Start in UCI mode" << std::endl;
    std::cout << "  --test       Run engine tests" << std::endl;
    std::cout << "  --help       Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Note: The engine automatically detects UCI vs UCI+ mode based on" << std::endl;
    std::cout << "      the 'uci' or 'uciplus' command sent by the GUI." << std::endl;
}

int main(int argc, char* argv[]) 
{
    // Initialize attack tables (required for move generation)
    Bitboard::init_attack_tables();
    
    // Parse command line arguments
    if (argc > 1) {
        std::string arg = argv[1];
        
        if (arg == "--test") {
            // Run tests if requested
            luna::ChessTests tests;
            tests.set_visualization(true);
            tests.set_perft_depth(3);
            tests.run_all_tests();
            return tests.all_tests_passed() ? 0 : 1;
        } else if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return 0;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            print_usage(argv[0]);
            return 1;
        }
    }
    
    // Start unified interface
    luna::UnifiedUCIInterface uci;
    uci.run();
    
    return 0;
}
