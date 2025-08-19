/*
    Unified UCI/UCI+ Interface implementation for Luna chess engine.
    Combines standard UCI and a custom UCI extension (UCI+).
    
    Author: Nicolas Miller
    Date: 07/24/2025
*/

#ifndef CHESS_ENGINE_UNIFIED_UCI_INTERFACE_H
#define CHESS_ENGINE_UNIFIED_UCI_INTERFACE_H

#include "engine.h"
#include "position.h"
#include "rule_interface.h"
#include "types.h"
#include <string>
#include <sstream>
#include <atomic>
#include <thread>
#include <memory>
#include <map>

namespace luna 
{

class UnifiedUCIInterface 
{
public:
    UnifiedUCIInterface();
    ~UnifiedUCIInterface();
    
    // Main UCI loop - reads commands from stdin and processes them
    void run();
    
private:
    // Mode management
    bool uci_plus_mode_;
    void enable_uci_plus_mode();
    
    // Command handlers - both UCI and UCI+
    void handle_uci();
    void handle_uciplus();
    void handle_isready();
    void handle_ucinewgame();
    void handle_position(const std::string& command);
    void handle_go(const std::string& command);
    void handle_stop();
    void handle_setoption(const std::string& command);
    void handle_quit();
    
    // UCI+ specific command handlers
    void handle_variant(const std::string& command);
    void handle_listvariants();
    void handle_setrule(const std::string& command);
    void handle_listrules();
    void handle_go_extended(const std::string& command);
    
    // Output functions
    void send_id();
    void send_options();
    void send_bestmove(const Move& move);
    void send_info(int depth, int score, int nodes, int time_ms, const std::vector<Move>& pv);
    void send_info_string(const std::string& info);
    
    // UCI+ specific output functions
    void send_info_variant(const std::string& variant_name);
    void send_info_rule(const std::string& rule_name, const std::string& status);
    void send_info_eval(int static_eval, int dynamic_eval);
    void send_info_explain(const Move& move, const std::string& explanation);
    
    // Helper functions
    Move parse_move(const std::string& move_str, const Position& pos);
    std::vector<std::string> split_string(const std::string& str);
    bool is_uci_plus_command(const std::string& command) const;
    
    // Engine and position state
    std::unique_ptr<Engine> engine_;
    Position current_position_;
    
    // Search control
    std::atomic<bool> searching_;
    std::atomic<bool> stop_search_;
    std::thread search_thread_;
    
    // UCI+ specific state
    std::string current_variant_;
    std::unique_ptr<RuleEngine> rule_engine_;
    
    // Constants
    static constexpr const char* ENGINE_NAME = "Luna";
    static constexpr const char* ENGINE_AUTHOR = "Nicolas Miller";
    static constexpr const char* ENGINE_VERSION = "1.0";
    static constexpr const char* UCIPLUS_VERSION = "1.0";
};

} // namespace luna

#endif // CHESS_ENGINE_UNIFIED_UCI_INTERFACE_H
