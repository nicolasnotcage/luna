/*
    Variant implementations. Currently only supports
    King of the Hill.
    
    Author: Nicolas Miller
    Date: 07/24/2025
*/

#ifndef CHESS_RULES_VARIANT_RULES_H
#define CHESS_RULES_VARIANT_RULES_H

#include "position.h"
#include "types.h"
#include <set>

namespace luna 
{

// King of the Hill - Win by moving king to center squares
class KingOfTheHillRule 
{
public:
    // Check if a king is on the hill (center squares)
    static bool is_king_on_hill(const Position& pos, Color color);
    
private:
    static const std::set<Square> hill_squares_;
};

} // namespace luna

#endif // CHESS_RULES_VARIANT_RULES_H
