/*
    Implementation of chess search algorithms.
    Includes negamax with alpha-beta pruning, iterative deepening,
    move ordering, and quiescence search.

    UPDATE 08/13/2025: Now uses transposition tables.

    Author: Nicolas Miller
    Date: 08/13/2025
*/

#include "ChessEngine/include/search.h"
#include "ChessEngine/include/constants.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <limits>
#include <numeric>
#include <cstring>
#include <random>

namespace luna {

Search::Search(Evaluator* eval)
    : evaluator_(eval), stop_search_(false), nodes_since_time_check_(0), tt_(DEFAULT_HASH_SIZE_MB),
      tt_hits_(0), tt_cutoffs_(0)
    {
        // Initialize killer moves
        std::memset(killer_moves_, 0, sizeof(killer_moves_));
    }

Move Search::search_position(Position& position, int max_depth, TimeManager* tm) 
{
    time_manager_ = tm;
    stop_search_ = false;
    Move best_move;
    
    // Reset search info and node counter
    info_.nodes_searched = 0;
    info_.depth_reached = 0;
    info_.score = 0;
    info_.pv.clear();
    nodes_since_time_check_ = 0;
    
    // Reset TT statistics
    tt_hits_ = 0;
    tt_cutoffs_ = 0;
    
    // Simple opening book for when engine plays as white from starting position
    // Check if this is the actual starting position (not just move count 0)
    std::string starting_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    if (position.to_fen() == starting_fen) 
    {
        std::vector<Move> legal_moves = position.generate_legal_moves();
        std::vector<Move> opening_moves;
        
        // Select from standard opening moves: e4, d4, Nf3, c4
        // TODO: Add a more traditional "opening book" for the engine
        for (const Move& move : legal_moves) 
        {
            // e2-e4 (King's pawn)
            if (move.from_square == Square::E2 && move.to_square == Square::E4) 
            {
                opening_moves.push_back(move);
            }
            // d2-d4 (Queen's pawn)
            else if (move.from_square == Square::D2 && move.to_square == Square::D4) 
            {
                opening_moves.push_back(move);
            }
            // Ng1-f3 (King's knight)
            else if (move.from_square == Square::G1 && move.to_square == Square::F3) 
            {
                opening_moves.push_back(move);
            }
            // c2-c4 (English opening)
            else if (move.from_square == Square::C2 && move.to_square == Square::C4) 
            {
                opening_moves.push_back(move);
            }
        }
        
        // Pick a random opening move
        if (!opening_moves.empty()) 
        {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, static_cast<int>(opening_moves.size()) - 1);
            Move selected_move = opening_moves[dis(gen)];
            
            // Still need to set search info for UCI output
            info_.depth_reached = 1;
            info_.score = 0;
            info_.nodes_searched = 1;
            info_.pv.push_back(selected_move);
            
            // Print search info
            print_search_info(1, 0, time_manager_->elapsed_ms());
            
            return selected_move;
        }
    }
    
    // Iterative deepening
    for (int depth = 1; depth <= max_depth && !stop_search_; depth++) 
    {
        Move iteration_best_move;
        
        // Start new search iteration
        tt_.new_search();
        
        // Search with the current depth
        int score = negamax_root(position, depth, -INFINITY_SCORE, INFINITY_SCORE, iteration_best_move, 0);
        
        if (!stop_search_) 
        {
            info_.depth_reached = depth;
            info_.score = score;
            
            // Update best move from this iteration
            if (iteration_best_move.from_square != Square::None) 
            {
                best_move = iteration_best_move;
                // Update PV with best move
                if (info_.pv.empty()) {
                    info_.pv.push_back(best_move);
                }
            }
            
            // Print search info for debugging
            print_search_info(depth, score, time_manager_->elapsed_ms());
            
            // Print TT statistics
            std::cout << "[TT STATS] Depth " << depth << ": " << tt_hits_ << " hits, " 
                      << tt_cutoffs_ << " cutoffs";
            if (info_.nodes_searched > 0) {
                int hit_rate = (tt_hits_ * 100) / info_.nodes_searched;
                int cutoff_rate = (tt_cutoffs_ * 100) / info_.nodes_searched;
                std::cout << " (hit rate: " << hit_rate << "%, cutoff rate: " << cutoff_rate << "%)";
            }
            std::cout << std::endl;
        }
    }
    
    return best_move;
}

// Root-level negamax search with alpha-beta pruning.
// Used for the first search level (tracks best move for iterative deepening)
// Called only from search_position; handles move tracking and root-specific logic.
int Search::negamax_root(Position& pos, int depth, int alpha, int beta, Move& best_move, int ply) 
{
    // Check for timeout
    if (should_check_time()) 
    {
        stop_search_ = true;
        return 0;
    }
    
    info_.nodes_searched++;
    
    // Generate and order moves
    std::vector<Move> legal_moves = pos.generate_legal_moves();
    if (legal_moves.empty()) 
    {
        if (pos.is_in_check()) 
        {
            // Checkmate - return negative mate score
            return -MATE_SCORE + static_cast<int>(pos.move_count());
        }
        else 
        {
            // Stalemate
            return 0;
        }
    }
    
    // Order moves for better pruning (no TT move at root)
    Move empty_move;
    order_moves(legal_moves, pos, empty_move);
    
    int best_score = -INFINITY_SCORE;
    
    // Search all moves
    for (const Move& move : legal_moves) 
    {
        if (stop_search_) break;
        
        // Make move
        pos.make_move(move);
        
        // Recursive search with negamax
        int score = -negamax(pos, depth - 1, -beta, -alpha, ply + 1);
        
        // Undo move
        pos.undo_move();
        
        if (stop_search_) break;
        
        // Update best move
        if (score > best_score) 
        {
            best_score = score;
            best_move = move;
            
            if (score > alpha) 
            {
                alpha = score;
            }
        }
        
        // Beta cutoff
        if (score >= beta) 
        {
            // Update killer moves for non-captures
            if (move.move_type != MoveType::Capture) 
            {
                killer_moves_[0][1] = killer_moves_[0][0];
                killer_moves_[0][0] = move;
            }
            
            return beta;
        }
    }
    
    return best_score;
}

// Recursive negamax search with alpha-beta pruning.
// Used for internal nodes (focuses purely on score calculation without move tracking)
// Called recursively from negamax_root and itself for deeper search levels.
int Search::negamax(Position& pos, int depth, int alpha, int beta, int ply) 
{
    // Check for timeout
    if (should_check_time()) 
    {
        stop_search_ = true;
        return 0;
    }
    
    info_.nodes_searched++;
    
    // Store original alpha for TT bound type determination
    int original_alpha = alpha;
    
    // Probe transposition table
    uint64_t hash_key = pos.hash_key();
    int tt_score, tt_depth;
    BoundType tt_bound;
    Move tt_move;
    
    if (tt_.probe(hash_key, tt_score, tt_depth, tt_bound, tt_move, ply)) 
    {
        // Use TT result if depth is sufficient
        if (tt_depth >= depth) 
        {
            switch (tt_bound) 
            {
                case BoundType::EXACT:
                    tt_hits_++;
                    tt_cutoffs_++;
                    return tt_score;
                case BoundType::LOWER_BOUND:
                    if (tt_score >= beta) 
                    {
                        tt_hits_++;
                        tt_cutoffs_++;
                        return tt_score;
                    }
                    tt_hits_++;
                    alpha = std::max(alpha, tt_score);
                    break;
                case BoundType::UPPER_BOUND:
                    if (tt_score <= alpha) 
                    {
                        tt_hits_++;
                        tt_cutoffs_++;
                        return tt_score;
                    }
                    tt_hits_++;
                    beta = std::min(beta, tt_score);
                    break;
                default:
                    break;
            }
            
            if (alpha >= beta) 
            {
                tt_cutoffs_++;
                return tt_score;
            }
        } 
        else 
        {
            tt_hits_++;
        }
    }
    
    // Check for game over
    std::vector<Move> legal_moves = pos.generate_legal_moves();
    if (legal_moves.empty())
    {
        if (pos.is_in_check()) 
        {
            // Checkmate - return negative mate score (bad for side to move)
            return -MATE_SCORE + static_cast<int>(pos.move_count());
        }
        else 
        {
            // Stalemate
            return 0;
        }
    }
    
    // Leaf node - evaluate position
    if (depth == 0) 
    {
        return quiescence(pos, alpha, beta, ply);
    }
    
    // Order moves for better pruning (use TT move if available)
    order_moves(legal_moves, pos, tt_move);
    
    int best_score = -INFINITY_SCORE;
    Move best_move;
    
    // Search all moves
    for (const Move& move : legal_moves) 
    {
        if (stop_search_) break;
        
        // Make move
        pos.make_move(move);
        
        // Recursive search with negamax
        int score = -negamax(pos, depth - 1, -beta, -alpha, ply + 1);
        
        // Undo move
        pos.undo_move();
        
        if (stop_search_) break;
        
        // Update best move
        if (score > best_score) 
        {
            best_score = score;
            best_move = move;
            
            if (score > alpha) 
            {
                alpha = score;
            }
        }
        
        // Beta cutoff
        if (score >= beta) 
        {
            // Update killer moves
            int killer_ply = std::min(static_cast<int>(pos.move_count()), MAX_PLY - 1);
            if (move.move_type != MoveType::Capture) 
            {
                killer_moves_[killer_ply][1] = killer_moves_[killer_ply][0];
                killer_moves_[killer_ply][0] = move;
            }
            
            best_score = beta;
            best_move = move;
            break;
        }
    }
    
    // Store in transposition table
    BoundType bound_type;
    if (best_score <= original_alpha) 
    {
        bound_type = BoundType::UPPER_BOUND;  // Fail-low
    } 
    else if (best_score >= beta) 
    {
        bound_type = BoundType::LOWER_BOUND;  // Fail-high
    } 
    else 
    {
        bound_type = BoundType::EXACT;        // Exact score
    }
    
    tt_.store(hash_key, best_score, depth, bound_type, best_move, ply);
    
    return best_score;
}

void Search::order_moves(std::vector<Move>& moves, const Position& pos, const Move& tt_move) 
{
    // Score each move for ordering
    std::vector<int> scores(moves.size());
    
    for (size_t i = 0; i < moves.size(); i++) 
    {
        const Move& move = moves[i];
        int score = 0;
        
        // TT move gets highest priority
        if (tt_move.from_square != Square::None && move == tt_move) 
        {
            score = TT_MOVE_SCORE;
        }
        // Captures - use MVV-LVA
        else if (move.move_type == MoveType::Capture || move.move_type == MoveType::EnPassant) 
        {
            // Get piece values for MVV-LVA
            Piece victim = pos.piece_on(move.to_square);
            if (move.move_type == MoveType::EnPassant) 
            {
                // En passant always captures a pawn
                victim = (pos.side_to_move() == Color::White) ? Piece::BlackPawn : Piece::WhitePawn;
            }
            
            Piece attacker = pos.piece_on(move.from_square);
            
            if (attacker != Piece::None && victim != Piece::None) 
            {
                // Use MVV-LVA offset table for precise capture scoring
                int victim_index = static_cast<int>(type_of(victim));
                int attacker_index = static_cast<int>(type_of(attacker));
                score = WINNING_CAPTURE_SCORE + MVV_LVA_OFFSET[attacker_index][victim_index];
            }
        }
        // Promotions
        else if (move.move_type == MoveType::Promotion) 
        {
            score = PROMOTION_SCORE + get_piece_value(type_of(move.promotion_piece));
        }
        // Killer moves
        else if (move == killer_moves_[std::min(static_cast<int>(pos.move_count()), MAX_PLY - 1)][0]) 
        {
            score = KILLER_MOVE_1_SCORE;
        }
        else if (move == killer_moves_[std::min(static_cast<int>(pos.move_count()), MAX_PLY - 1)][1]) 
        {
            score = KILLER_MOVE_2_SCORE;
        }
        // Other quiet moves get score of 0
        else 
        {
            score = 0;
        }
        
        scores[i] = score;
    }
    
    // Pair moves with their score
    std::vector<std::pair<Move, int>> move_score_pairs;
    move_score_pairs.reserve(moves.size());
    
    for (size_t i = 0; i < moves.size(); i++) 
    {
        move_score_pairs.emplace_back(moves[i], scores[i]);
    }
    
    // Sort by score (descending)
    std::sort(move_score_pairs.begin(), move_score_pairs.end(), 
        [](const auto& a, const auto& b) 
        {
            return a.second > b.second;  // Compare scores
        });
    
    // Extract sorted moves
    for (size_t i = 0; i < moves.size(); i++) 
    {
        moves[i] = move_score_pairs[i].first;
    }
}

int Search::quiescence(Position& pos, int alpha, int beta, int ply) 
{
    if (should_check_time()) 
    {
        stop_search_ = true;
        return 0;
    }
    
    info_.nodes_searched++;
    
    // Stand pat - evaluate current position from side to move's perspective
    int stand_pat = evaluator_->evaluate(pos);
    
    if (stand_pat >= beta) 
    {
        return beta;
    }
    
    if (alpha < stand_pat) 
    {
        alpha = stand_pat;
    }
    
    // Generate only capture moves and promotions
    std::vector<Move> captures;
    std::vector<Move> all_moves = pos.generate_legal_moves();
    
    for (const Move& move : all_moves) 
    {
        if (move.move_type == MoveType::Capture || 
            move.move_type == MoveType::EnPassant ||
            move.move_type == MoveType::Promotion) 
            {
                captures.push_back(move);
            }
    }
    
    // Order captures (no TT move for quiescence)
    Move empty_move;
    order_moves(captures, pos, empty_move);
    
    // Search captures
    for (const Move& move : captures) 
    {
        if (stop_search_) break;
        
        pos.make_move(move);
        int score = -quiescence(pos, -beta, -alpha, ply + 1);
        pos.undo_move();
        
        if (score >= beta) 
        {
            return beta;
        }
        
        if (score > alpha) 
        {
            alpha = score;
        }
    }
    
    return alpha;
}

void Search::print_search_info(int depth, int score, int time_ms) const 
{
    std::cout << "info depth " << depth 
              << " score cp " << score
              << " nodes " << info_.nodes_searched
              << " time " << time_ms;
    
    if (time_ms > 0) 
    {
        std::cout << " nps " << (info_.nodes_searched * 1000) / time_ms;
    }
    
    // Print PV if available
    if (!info_.pv.empty()) 
    {
        std::cout << " pv";
        for (const Move& move : info_.pv) 
        {
            std::cout << " " << move.to_string();
        }
    }
    
    std::cout << std::endl;
}

const Search::SearchInfo& Search::get_search_info() const 
{
    return info_;
}

int Search::get_piece_value(PieceType pt) const 
{
    switch (pt) 
    {
        case PieceType::Pawn:   return PAWN_VALUE;
        case PieceType::Knight: return KNIGHT_VALUE;
        case PieceType::Bishop: return BISHOP_VALUE;
        case PieceType::Rook:   return ROOK_VALUE;
        case PieceType::Queen:  return QUEEN_VALUE;
        case PieceType::King:   return KING_VALUE;
        default: return 0;
    }
}

bool Search::should_check_time() 
{
    // Only check time every CHECK_FREQUENCY nodes to reduce overhead
    if (++nodes_since_time_check_ >= CHECK_FREQUENCY) 
    {
        nodes_since_time_check_ = 0;
        return time_manager_ && time_manager_->should_stop();
    }
    return false;
}

} // namespace luna
