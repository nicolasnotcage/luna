/*
    Main chess engine interface.
    Coordinates search, evaluation, and time management.

    Author: Nicolas Miller
    Date: 07/09/2025
*/

#ifndef CHESS_ENGINE_ENGINE_H
#define CHESS_ENGINE_ENGINE_H

#include "search.h"
#include "evaluator.h"
#include "time_manager.h"
#include "types.h"
#include "position.h"

#include <memory>

namespace luna 
{

class Engine 
{
public:
    Engine();
    ~Engine();
    
    // Main interface - returns best move for position
    Move find_best_move(const Position& position, int time_ms = DEFAULT_SEARCH_TIME_MS);
    
    // Configure engine parameters
    void set_max_depth(int depth);
    
    // Get search information
    const Search::SearchInfo& get_search_info() const;
    
    // Stop the current search
    void stop_search();
    
private:
    std::unique_ptr<Search>                 search_;
    std::unique_ptr<Evaluator>              evaluator_;
    std::unique_ptr<TimeManager>            time_manager_;
    
    int max_depth_;
};

} // namespace luna

#endif // CHESS_ENGINE_ENGINE_H
