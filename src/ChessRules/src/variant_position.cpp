/*
    Implementation of variant-aware position wrapper.
    
    Author: Nicolas Miller
    Date: 07/23/2025
*/

#include "variant_position.h"
#include "variant_rules.h"
#include <iostream>

namespace luna 
{

VariantPosition::VariantPosition(Position& pos, RuleEngine* rule_engine)
    : position_(pos), rule_engine_(rule_engine), game_result_(GameResult::None)
{
}

std::vector<Move> VariantPosition::generate_legal_moves() const
{
    // TODO: Still have to figure this one out. Might have to call a specific 
    // variant of this function for each variant...

    // For now, just returns normal legal moves.
    return position_.generate_legal_moves();
}

void VariantPosition::make_move(const Move& move)
{
    // Make the move on the position
    position_.make_move(move);
    
    // Handle variant-specific rules, including special win conditions
    if (rule_engine_) {
        // Check if variant rule set a win condition
        if (rule_engine_->has_rule("king_of_the_hill")) 
        {
            if (KingOfTheHillRule::is_king_on_hill(position_, Color::White)) 
            {
                game_result_ = GameResult::WhiteWins;
                std::cout << "White wins by King of the Hill!" << std::endl;
                return;
            }
            if (KingOfTheHillRule::is_king_on_hill(position_, Color::Black)) 
            {
                game_result_ = GameResult::BlackWins;
                std::cout << "Black wins by King of the Hill!" << std::endl;
                return;
            }
        }
    }
}

bool VariantPosition::is_game_over() const
{
    return game_result_ != GameResult::None;
}


bool VariantPosition::has_player_won(Color color) const
{
    if (color == Color::White && game_result_ == GameResult::WhiteWins) 
    {
        return true;
    }
    if (color == Color::Black && game_result_ == GameResult::BlackWins) 
    {
        return true;
    }
    return false;
}

void VariantPosition::reset()
{
    position_ = Position();
    game_result_ = GameResult::None;
}

} // namespace luna
