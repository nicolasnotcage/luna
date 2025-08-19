/*
    Chess search algorithms interface.
    Includes negamax with alpha-beta pruning, iterative deepening,
    move ordering, and quiescence search.

    UPDATE 08/13/2025: Added transposition tables to search. 

    Author: Nicolas Miller
    Date: 08/13/2025
*/

#ifndef CHESS_ENGINE_SEARCH_H
#define CHESS_ENGINE_SEARCH_H

#include "constants.h"
#include "evaluator.h"
#include "time_manager.h"
#include "transposition_table.h"
#include "position.h"
#include "types.h"

#include <vector>

namespace luna 
{

class Search 
{
public:
    struct SearchInfo 
    {
        int nodes_searched;
        int depth_reached;
        int score;
        std::vector<Move> pv;  // Principal variation
    };
    
    Search(Evaluator* eval);
    
    // Main search function
    Move search_position(Position& position, int max_depth, TimeManager* tm);
    
    // Get search information
    const SearchInfo& get_search_info() const;
    
    // Set stop flag (for UCI stop command)
    void stop() { stop_search_ = true; }
    
private:
    // Core negamax with alpha-beta (now includes ply for TT)
    int negamax(Position& pos, int depth, int alpha, int beta, int ply);
    
    // Root negamax that tracks the best move (now includes ply for TT)
    int negamax_root(Position& pos, int depth, int alpha, int beta, Move& best_move, int ply);
    
    // Move ordering for better pruning (now includes TT move)
    void order_moves(std::vector<Move>& moves, const Position& pos, const Move& tt_move);
    
    // Quiescence search (now includes ply for TT)
    int quiescence(Position& pos, int alpha, int beta, int ply);
    
    // Print search information for debugging
    void print_search_info(int depth, int score, int time_ms) const;
    
    // Helper to get piece value
    int get_piece_value(PieceType pt) const;
    
    Evaluator* evaluator_;
    TimeManager* time_manager_;
    SearchInfo info_;
    bool stop_search_;  // Flag to indicate when to stop the search
    
    // Transposition table
    TranspositionTable tt_;
    
    // Killer moves for move ordering
    Move killer_moves_[MAX_PLY][2];
    
    // Time checking optimization
    int nodes_since_time_check_;
    
    // TT statistics for debugging
    mutable int tt_hits_;
    mutable int tt_cutoffs_;
    
    // Helper to check if we should stop (with frequency optimization)
    bool should_check_time();
};

} // namespace luna

#endif // CHESS_ENGINE_SEARCH_H
