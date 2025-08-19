/*
    Variant-aware position wrapper that integrates the rule engine
    with the standard Position class for chess variants.
    
    Author: Nicolas Miller
    Date: 07/23/2025
*/

#ifndef VARIANT_POSITION_H
#define VARIANT_POSITION_H

#include "position.h"
#include "rule_interface.h"
#include <memory>

namespace luna 
{

enum class GameResult 
{
    None,
    WhiteWins,
    BlackWins,
    Draw
};

class VariantPosition 
{
public:
    VariantPosition(Position& pos, RuleEngine* rule_engine = nullptr);
    
    // Set the rule engine for variant rules
    void set_rule_engine(RuleEngine* engine) { rule_engine_ = engine; }
    
    // Get underlying position
    Position& position() { return position_; }
    const Position& position() const { return position_; }
    
    // Variant-aware move generation
    std::vector<Move> generate_legal_moves() const;
    
    // Make a move with variant rules applied
    void make_move(const Move& move);
    
    // Check if game is over according to variant rules
    bool is_game_over() const;
    
    // Get game result
    GameResult get_game_result() const { return game_result_; }
    
    // Check if a specific player has won according to variant rules
    bool has_player_won(Color color) const;
    
    // Reset to starting position
    void reset();
    
private:
    Position& position_;
    RuleEngine* rule_engine_;
    GameResult game_result_;
};

} // namespace luna

#endif // VARIANT_POSITION_H
