/*
    A utility class to handle chess move logic in the chess game. 
    
    Author: Nicolas Miller
    Date: 06/25/2025
*/

#ifndef CHESS_GAME_MOVE_HANDLER_HPP
#define CHESS_GAME_MOVE_HANDLER_HPP

#include "position.h"
#include "types.h"
#include "ChessRules/include/variant_position.h"
#include "ChessRules/include/rule_interface.h"
#include "chess_game/constants.h"
#include "chess_game/popup_manager.hpp"
#include "chess_game/uci_client.hpp"
#include "platform/audio_manager.hpp"

#include <vector>
#include <memory>

namespace cge
{

class MoveHandler
{
public:
    MoveHandler() = default;
    ~MoveHandler() = default;

    // Initialize with position, popup manager, and audio manager
    void init(Position& chess_position, PopupManager& popup_manager, AudioManager& audio_manager, bool enable_variants);
    
    // Initialize with variant position for UCI+ mode
    void init_variant(luna::VariantPosition& variant_position, PopupManager& popup_manager, AudioManager& audio_manager);

    // Move validation and execution
    bool is_legal_move(Square from, Square to) const;
    bool execute_move(Square from, Square to, Color player_color); // Returns true if move completed
    bool is_promotion_move(Square from, Square to) const;
    
    // Promotion handling
    void complete_promotion(PieceType promotion_piece_type);
    bool is_waiting_for_promotion() const { return waiting_for_promotion_; }

    // Generate and store legal moves
    void generate_and_store_legal_moves();

    // Getters
    const std::vector<Move>& get_legal_moves() const { return legal_moves_; }
    bool is_game_over() const { return game_over_; }
    luna::GameResult get_game_result() const;

    // Make computer move
    void make_computer_move(Color player_color);
    
    // Set game variant (UCI+ mode)
    bool set_variant(const std::string& variant_name);

private:
    // References to external game objects
    Position* chess_position_{nullptr};
    luna::VariantPosition* variant_position_{nullptr};
    PopupManager* popup_manager_{nullptr};
    AudioManager* audio_manager_{nullptr};

    // Internal state
    std::vector<Move> legal_moves_;
    bool game_over_{false};
    bool waiting_for_promotion_{false};
    Move pending_promotion_move_;

    // Chess engine via UCI
    std::unique_ptr<UCIClient> uci_client_;
    bool use_uci_plus_{false};

    // For variant support
    std::unique_ptr<luna::VariantPosition> variant_position_wrapper_;
    std::unique_ptr<luna::RuleEngine> rule_engine_;

    // Helper methods
    void check_game_over();
    void play_move_sound(bool is_capture, bool is_check);
};

} // namespace cge

#endif // CHESS_GAME_MOVE_HANDLER_HPP
