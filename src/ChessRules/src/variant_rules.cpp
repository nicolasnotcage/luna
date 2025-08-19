/*
    Rule variant implementations.
    
    Author: Nicolas Miller
    Date: 07/24/2025
*/

#include "variant_rules.h"

namespace luna 
{

// King of the Hill implementation
const std::set<Square> KingOfTheHillRule::hill_squares_ = 
{
    Square::D4, Square::D5, Square::E4, Square::E5
};

bool KingOfTheHillRule::is_king_on_hill(const Position& pos, Color color) 
{
    Square king_square = pos.king_square(color);
    return hill_squares_.find(king_square) != hill_squares_.end();
}

} // namespace luna
