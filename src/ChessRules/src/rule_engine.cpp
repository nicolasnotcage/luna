/*
    Rule engine implementation. Currently only supports king of the hill
    variant.
    
    Author: Nicolas Miller
    Date: 07/24/2025
*/

#include "rule_interface.h"
#include "variant_rules.h"
#include <iostream>

namespace luna 
{

void RuleEngine::load_variant(const std::string& variant_name) 
{
    if (variant_name == "king_of_the_hill") 
    {
        koth_active_ = true;
        std::cout << "King of the Hill variant loaded" << std::endl;
    } 
    else 
    {
        koth_active_ = false;
        std::cout << "Standard chess variant loaded" << std::endl;
    }
}

bool RuleEngine::has_rule(const std::string& rule_name) const 
{
    // Currently only supporting king of the hill variant
    return (rule_name == "king_of_the_hill" && koth_active_);
}

std::vector<std::string> RuleEngine::get_available_variants() const 
{
    return {"standard", "king_of_the_hill"};
}

} // namespace luna
