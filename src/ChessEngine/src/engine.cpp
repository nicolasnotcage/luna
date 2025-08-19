/*
    Implementation of main chess engine class.
    Coordinates search, evaluation, and time management.

    Author: Nicolas Miller
    Date: 07/09/2025
*/

#include "ChessEngine/include/engine.h"
#include "ChessEngine/include/constants.h"
#include <iostream>

namespace luna 
{

Engine::Engine() 
    : max_depth_(DEFAULT_SEARCH_DEPTH)
    {
        // Initialize components
        evaluator_ = std::make_unique<Evaluator>();
        search_ = std::make_unique<Search>(evaluator_.get());
        time_manager_ = std::make_unique<TimeManager>();
    }

Engine::~Engine() = default;

Move Engine::find_best_move(const Position& position, int time_ms) 
{
    // Make a copy of the position to search
    Position search_position = position;
    
    // Start time management
    time_manager_->start_search(time_ms);
    
    // Search for best move
    Move best_move = search_->search_position(search_position, max_depth_, time_manager_.get());
    
    // Get search statistics
    const Search::SearchInfo& info = search_->get_search_info();
    
    // Print final search summary
    std::cout << "Engine: Searched " << info.nodes_searched << " nodes"
              << " to depth " << info.depth_reached
              << " in " << time_manager_->elapsed_ms() << " ms"
              << " (score: " << info.score << ")" << std::endl;
    
    // If no move was found (shouldn't happen), return first legal move
    if (best_move.from_square == Square::None) 
    {
        std::cerr << "Warning: No best move found, returning first legal move" << std::endl;
        std::vector<Move> legal_moves = search_position.generate_legal_moves();
        if (!legal_moves.empty()) best_move = legal_moves[0];
    }
    
    return best_move;
}

void Engine::set_max_depth(int depth) 
{
    max_depth_ = std::max(1, std::min(depth, MAX_SEARCH_DEPTH));
}

const Search::SearchInfo& Engine::get_search_info() const
{
    return search_->get_search_info();
}

void Engine::stop_search()
{
    search_->stop();
}

} // namespace luna
