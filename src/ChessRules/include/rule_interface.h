/*
    Custom rule interface. Currently only supports King of 
    the Hill variant. Will extend with more variants once 
    I figure out how modular rule system will work with the 
    engine.     
    
    Author: Nicolas Miller
    Date: 07/24/2025
*/

#ifndef CHESS_RULES_RULE_INTERFACE_H
#define CHESS_RULES_RULE_INTERFACE_H

#include "position.h"
#include "types.h"
#include <string>
#include <vector>
#include <map>

namespace luna 
{

class RuleEngine 
{
public:
    RuleEngine() = default;
    ~RuleEngine() = default;
    
    // Load variant (currently supports "standard" and "king_of_the_hill")
    void load_variant(const std::string& variant_name);
    
    // Check if a rule exists
    bool has_rule(const std::string& rule_name) const;
    
    // Get available variants
    std::vector<std::string> get_available_variants() const;
    
private:
    bool koth_active_ = false;
};

} // namespace luna

#endif // CHESS_RULES_RULE_INTERFACE_H
