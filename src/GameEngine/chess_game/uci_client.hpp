/*
    UCI client for chess game using asynchronous Windows IPC.
    Updated from library-based implementation to now use Windows
    processes and anonymous pipes.
    
    Author: Nicolas Miller
    Date: 08/01/2025
*/

#ifndef CHESS_GAME_UCI_CLIENT_HPP
#define CHESS_GAME_UCI_CLIENT_HPP

#include "position.h"
#include "types.h"
#include "system/process_manager.h"
#include <string>
#include <memory>
#include <thread>
#include <atomic>

namespace cge
{

class UCIClient
{
public:
    UCIClient();
    ~UCIClient();
    
    // Non-copyable
    UCIClient(const UCIClient&) = delete;
    UCIClient& operator=(const UCIClient&) = delete;
    
    // Engine control
    bool start_engine(const std::string& engine_path, bool use_uci_plus = false);
    void stop_engine();
    bool is_engine_running() const { return engine_running_; }
    void stop_search();
    
    // Get best move from engine (non-blocking)
    Move get_best_move(const Position& position, int time_ms = 1000);
    
    // UCI+ variant support
    bool set_variant(const std::string& variant_name);
    bool is_uci_plus_capable() const { return supports_uci_plus_; }
    
private:
    // Process management
    std::unique_ptr<ProcessManager> process_manager_;
    std::atomic<bool>               engine_running_{false};
    bool                            supports_uci_plus_{false};
    
    // Thread for engine communication loop
    std::unique_ptr<std::thread>    engine_thread_;
    std::atomic<bool>               stop_thread_{false};
    
    // Engine state
    std::string         engine_name_;
    std::string         engine_author_;
    std::atomic<bool>   search_in_progress_{false};
    std::atomic<bool>   waiting_for_ready_{false};  
    std::atomic<bool>   ready_received_{false};     
    std::string         last_best_move_;
    
    // Simplified internal methods
    bool initialize_engine();
    void engine_communication_loop();  // Simplified main loop
    bool send_command(const std::string& command);
    std::string read_until_timeout(int timeout_ms);
    bool wait_for_ready();
    
    // UCI protocol helpers
    std::string position_to_uci(const Position& position);
    Move parse_move_string(const std::string& move_str, const Position& position);
    std::string parse_best_move(const std::string& response);
};

} // namespace cge

#endif // CHESS_GAME_UCI_CLIENT_HPP
