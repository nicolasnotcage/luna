/*
    Implementation of chess game move handler.
    Updated to use UCI/UCI+ interface for chess engine communication.

    Author: Nicolas Miller
    Date: 07/23/2025
*/

#include "chess_game/move_handler.hpp"
#include "chess_game/constants.h"
#include "ChessRules/include/variant_rules.h"
#include "system/config_manager.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>

namespace cge
{

// Constants
static constexpr int ENGINE_THINK_TIME_MS = 2000;

void MoveHandler::init(Position& chess_position, PopupManager& popup_manager, 
                       AudioManager& audio_manager, bool enable_variants)
{
    popup_manager_ = &popup_manager;
    audio_manager_ = &audio_manager;
    game_over_ = false;
    waiting_for_promotion_ = false;
    
    // Initialize UCI client
    uci_client_ = std::make_unique<UCIClient>();
    use_uci_plus_ = enable_variants;
    
    // Get engine path from config
    std::string engine_path = ConfigManager::get_instance().get_engine_path();
    uci_client_->start_engine(engine_path, use_uci_plus_);

    if (enable_variants) 
    {
        // Create and configure rule engine
        rule_engine_ = std::make_unique<luna::RuleEngine>();
        
        // Create a VariantPosition wrapper that references the original position
        variant_position_wrapper_ = std::make_unique<luna::VariantPosition>(chess_position, rule_engine_.get());
        
        // Set pointers
        variant_position_ = variant_position_wrapper_.get();
        chess_position_ = &chess_position;
    } 
    else 
    {
        chess_position_ = &chess_position;
        variant_position_ = nullptr;
    }
    
    // Generate initial legal moves
    generate_and_store_legal_moves();
}

void MoveHandler::init_variant(luna::VariantPosition& variant_position, 
                               PopupManager& popup_manager, AudioManager& audio_manager)
{
    variant_position_ = &variant_position;
    chess_position_ = &variant_position.position();
    popup_manager_ = &popup_manager;
    audio_manager_ = &audio_manager;
    game_over_ = false;
    waiting_for_promotion_ = false;
    
    // Initialize UCI client in UCI+ mode
    uci_client_ = std::make_unique<UCIClient>();
    use_uci_plus_ = true;
    
    // Get engine path from config
    std::string engine_path = ConfigManager::get_instance().get_engine_path();
    uci_client_->start_engine(engine_path, use_uci_plus_);
    
    // Generate initial legal moves
    generate_and_store_legal_moves();
}

bool MoveHandler::is_legal_move(Square from, Square to) const
{
    // Check if the move is in our pre-generated list
    for (const auto& move : legal_moves_) 
    {
        if (move.from_square == from && move.to_square == to) 
        {
            return true;
        }
    }

    return false;
}

bool MoveHandler::execute_move(Square from, Square to, Color player_color)
{
    // Find the move in our stored legal moves
    Move* found_move = nullptr;
    for (auto& move : legal_moves_) 
    {
        if (move.from_square == from && move.to_square == to) 
        {
            found_move = &move;
            break;
        }
    }

    if (!found_move) 
    {
        return false;
    }

    // Check if this is a promotion move
    if (is_promotion_move(from, to))
    {
        // Store the move and show promotion UI
        pending_promotion_move_ = *found_move;
        waiting_for_promotion_ = true;
        popup_manager_->show_promotion_prompt();
        return false; // Move not completed yet
    }
    
    // Check if this is a capture move before making it
    bool is_capture = chess_position_->piece_on(to) != Piece::None;
    
    // Make the move
    if (variant_position_) 
    {
        variant_position_->make_move(*found_move);
    } 
    else 
    {
        chess_position_->make_move(*found_move);
    }
    
    // Regenerate legal moves for the next player
    generate_and_store_legal_moves();
    
    // Play appropriate sound
    bool is_check = chess_position_->is_in_check();
    play_move_sound(is_capture, is_check);
    
    return true; // Move completed
}

bool MoveHandler::is_promotion_move(Square from, Square to) const
{
    Piece piece = chess_position_->piece_on(from);
    if (piece == Piece::None) 
    {
        return false;
    }
    
    // Check if it's a pawn
    PieceType piece_type = type_of(piece);
    if (piece_type != PieceType::Pawn) 
    {
        return false;
    }
    
    // Check if it's moving to the promotion rank
    int to_rank = static_cast<int>(to) / 8;
    Color piece_color = color_of(piece);
    
    return (piece_color == Color::White && to_rank == 7) ||
           (piece_color == Color::Black && to_rank == 0);
}

void MoveHandler::complete_promotion(PieceType promotion_piece_type)
{
    if (!waiting_for_promotion_) 
    {
        return;
    }
    
    // Get the player color from the pending move
    Piece moving_piece = chess_position_->piece_on(pending_promotion_move_.from_square);
    Color player_color = color_of(moving_piece);
    
    // Set the promotion piece
    pending_promotion_move_.promotion_piece = make_piece(player_color, promotion_piece_type);
    
    // Check if this is a capture move before making it
    bool is_capture = chess_position_->piece_on(pending_promotion_move_.to_square) != Piece::None;
    
    // Execute the promotion move
    if (variant_position_) 
    {
        variant_position_->make_move(pending_promotion_move_);
    } 
    else 
    {
        chess_position_->make_move(pending_promotion_move_);
    }
    
    // Regenerate legal moves for the next player
    generate_and_store_legal_moves();
    
    // Play appropriate sound
    bool is_check = chess_position_->is_in_check();
    play_move_sound(is_capture, is_check);
    
    // Hide promotion UI and reset state
    popup_manager_->hide_promotion_prompt();
    waiting_for_promotion_ = false;
    pending_promotion_move_ = Move{};
}

void MoveHandler::generate_and_store_legal_moves()
{
    // Update legal moves
    if (variant_position_) 
    {
        legal_moves_ = variant_position_->generate_legal_moves();
    } 
    else 
    {
        legal_moves_ = chess_position_->generate_legal_moves();
    }

    // Check if game is over
    check_game_over();  
}

void MoveHandler::check_game_over()
{
    // First check variant-specific win conditions
    if (variant_position_) 
    {
        if (variant_position_->is_game_over()) 
        {
            game_over_ = true;
            
            // Determine who won and play appropriate sound
            luna::GameResult result = variant_position_->get_game_result();
            if (result == luna::GameResult::WhiteWins) 
            {
                if (audio_manager_) 
                {
                    audio_manager_->play_win_sound(1.0f);
                }
            } 
            else if (result == luna::GameResult::BlackWins) 
            {
                if (audio_manager_) 
                {
                    audio_manager_->play_loss_sound(1.0f);
                }
            } 
            else if (result == luna::GameResult::Draw) 
            {
                if (audio_manager_) 
                {
                    audio_manager_->play_draw_sound(1.0f);
                }
            }
            return;
        }
    }
    
    // Standard game over conditions
    if (legal_moves_.empty())
    {
        // Game over; determine outcome
        if (chess_position_->is_in_check())
        {
            // Checkmate
            if (chess_position_->side_to_move() == Color::White)
            {
                // White is checkmated
                if (audio_manager_) 
                {
                    audio_manager_->play_loss_sound(1.0f);
                }
            }
            else
            {
                // Black is checkmated
                if (audio_manager_) 
                {
                    audio_manager_->play_win_sound(1.0f);
                }
            }
        }
        else
        {
            // Stalemate or draw
            if (audio_manager_) 
            {
                audio_manager_->play_draw_sound(1.0f);
            }
        }

        // Set game over flag 
        game_over_ = true;
    }
    else
    {
        game_over_ = false;
    }
}

void MoveHandler::play_move_sound(bool is_capture, bool is_check)
{
    if (!audio_manager_) 
    {
        return;
    }
    
    if (is_check)
    {
        audio_manager_->play_check_sound(1.0f);
    }
    else if (is_capture)
    {
        audio_manager_->play_take_sound(1.0f);
    }
    else
    {
        audio_manager_->play_move_sound(1.0f);
    }
}

void MoveHandler::make_computer_move(Color player_color)
{
    if (legal_moves_.empty()) 
    {
        return;
    }
    
    // Use UCI client to find best move
    std::cout << "Computer thinking..." << std::endl;
    Move best_move = uci_client_->get_best_move(*chess_position_, ENGINE_THINK_TIME_MS);
    
    // Verify the move is legal (should always be true)
    bool move_found = false;
    for (const auto& legal_move : legal_moves_)
    {
        if (legal_move == best_move)
        {
            move_found = true;
            break;
        }
    }
    
    if (!move_found)
    {
        std::cerr << "Warning: Engine returned illegal move, falling back to random" << std::endl;
        // Fall back to random move
        int index = std::rand() % legal_moves_.size();
        best_move = legal_moves_[index];
    }
    
    // For computer promotion, promote to queen by default
    if (is_promotion_move(best_move.from_square, best_move.to_square))
    {
        Color computer_color = (player_color == Color::White) ? Color::Black : Color::White;
        best_move.promotion_piece = make_piece(computer_color, PieceType::Queen);
    }
    
    // Check if this is a capture move before making it
    bool is_capture = chess_position_->piece_on(best_move.to_square) != Piece::None;
    
    // Make the move
    if (variant_position_) 
    {
        variant_position_->make_move(best_move);
    } 
    else 
    {
        chess_position_->make_move(best_move);
    }
    
    // Regenerate moves for the player
    generate_and_store_legal_moves();
    
    // Play appropriate sound
    bool is_check = chess_position_->is_in_check();
    play_move_sound(is_capture, is_check);
}

bool MoveHandler::set_variant(const std::string& variant_name)
{
    if (!uci_client_ || !rule_engine_) 
    {
        return false;
    }

    // Load variant in rule engine
    rule_engine_->load_variant(variant_name);

    // Also set in UCI client
    bool success = uci_client_->set_variant(variant_name);
    
    if (success) 
    {
        std::cout << "Game variant changed to: " << variant_name << std::endl;
        
        // Reset position to starting position for the variant
        if (variant_position_) 
        {
            variant_position_->reset();
        } 
        else 
        {
            *chess_position_ = Position();
        }
        
        // Regenerate legal moves with new variant rules
        generate_and_store_legal_moves();
    }
    
    return success;
}

luna::GameResult MoveHandler::get_game_result() const
{
    // If we have a variant position, get the result from it
    if (variant_position_) 
    {
        return variant_position_->get_game_result();
    }
    
    // Otherwise, determine result from standard chess position
    if (!game_over_) 
    {
        return luna::GameResult::None;
    }
    
    // Check if it's checkmate or stalemate
    if (legal_moves_.empty()) 
    {
        if (chess_position_->is_in_check()) 
        {
            // Checkmate - whoever's turn it is loses
            if (chess_position_->side_to_move() == Color::White) 
            {
                return luna::GameResult::BlackWins;
            } 
            else 
            {
                return luna::GameResult::WhiteWins;
            }
        } 
        else 
        {
            // Stalemate
            return luna::GameResult::Draw;
        }
    }
    
    // Game over but not due to checkmate/stalemate (shouldn't happen in standard chess)
    return luna::GameResult::Draw;
}

} // namespace cge
