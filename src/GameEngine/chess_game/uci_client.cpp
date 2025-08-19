/*
    UCI Client implementation with asynchronous Windows IPC.
    
    Author: Nicolas Miller
    Date: 08/01/2025
*/

#include "chess_game/uci_client.hpp"
#include <iostream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <cctype>

namespace cge
{

UCIClient::UCIClient() = default;

UCIClient::~UCIClient()
{
    stop_engine();
}

bool UCIClient::start_engine(const std::string& engine_path, bool use_uci_plus)
{
    // If engine is already running, stop it first
    if (engine_running_.load()) 
    {
        stop_engine();
    }
    
    try 
    {
        // Create process manager and start the engine process
        process_manager_ = std::make_unique<ProcessManager>();
        
        if (!process_manager_->start_process(engine_path)) 
        {
            std::cerr << "Failed to start engine process: " << engine_path << std::endl;
            return false;
        }
        
        // Initialize UCI protocol (synchronous)
        if (!initialize_engine()) 
        {
            std::cerr << "Failed to initialize UCI protocol" << std::endl;
            stop_engine();
            return false;
        }
        
        // Check for UCI+ support if requested
        supports_uci_plus_ = false;
        if (use_uci_plus) 
        {
            send_command("uciplus");
            std::string response = read_until_timeout(1000);
            if (response.find("uciplusok") != std::string::npos) 
            {
                std::cout << "UCI+ mode enabled" << std::endl;
                supports_uci_plus_ = true;
            }
        }
        
        // Start the background communication thread
        stop_thread_ = false;
        engine_thread_ = std::make_unique<std::thread>([this]() 
        {
            engine_communication_loop();
        });
        
        engine_running_ = true;
        
        std::cout << "Engine started: " << engine_name_ 
                  << " by " << engine_author_ << std::endl;
        return true;
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Exception starting engine: " << e.what() << std::endl;
        stop_engine();
        return false;
    }
}

void UCIClient::stop_engine()
{
    if (!engine_running_.load()) return;
    
    engine_running_ = false;
    
    // Stop the background thread
    if (engine_thread_ && engine_thread_->joinable()) 
    {
        stop_thread_ = true;
        
        // Try to send quit command
        try 
        {
            send_command("quit");
        } 
        catch (...) 
        {
            // Ignore errors during shutdown
        }
        
        engine_thread_->join();
        engine_thread_.reset();
    }
    
    // Stop the process
    if (process_manager_) 
    {
        process_manager_->stop_process();
        process_manager_.reset();
    }
    
    supports_uci_plus_ = false;
    std::cout << "Engine stopped" << std::endl;
}

Move UCIClient::get_best_move(const Position& position, int time_ms)
{
    if (!engine_running_.load()) 
    {
        return Move();
    }
    
    // Clear previous best move
    last_best_move_.clear();
    
    // Send position to engine
    std::string position_cmd = position_to_uci(position);
    if (!send_command(position_cmd))
    {
        std::cerr << "Failed to send position" << std::endl;
        return Move();
    }
    
    // Wait for engine to be ready
    if (!wait_for_ready()) 
    {
        std::cerr << "Engine not ready after position" << std::endl;
        return Move();
    }
    
    // Start search
    search_in_progress_ = true;
    std::string go_cmd = "go movetime " + std::to_string(time_ms);
    if (!send_command(go_cmd)) 
    {
        search_in_progress_ = false;
        std::cerr << "Failed to send go command" << std::endl;
        return Move();
    }
    
    // Wait for the search to complete (with timeout)
    auto start_time = std::chrono::steady_clock::now();
    auto timeout = std::chrono::milliseconds(time_ms + 2000); // Add 2 second buffer; TODO: Make buffer configurable
    
    while (search_in_progress_.load() && 
           std::chrono::steady_clock::now() - start_time < timeout) 
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    search_in_progress_ = false;
    
    // Check if we got a best move
    if (last_best_move_.empty()) 
    {
        std::cerr << "No best move received from engine" << std::endl;
        return Move();
    }
    
    // Parse the move
    return parse_move_string(last_best_move_, position);
}

bool UCIClient::set_variant(const std::string& variant_name)
{
    if (!supports_uci_plus_ || !engine_running_.load()) 
    {
        return false;
    }
    
    std::string variant_cmd = "variant " + variant_name;
    if (!send_command(variant_cmd)) 
    {
        return false;
    }
    
    // Wait a bit and see if we get a positive response
    std::string response = read_until_timeout(1000);
    if (response.find("info string variant " + variant_name) != std::string::npos) 
    {
        std::cout << "Variant " << variant_name << " set successfully" << std::endl;
        return true;
    }
    
    return false;
}

void UCIClient::stop_search()
{
    if (engine_running_.load() && search_in_progress_.load()) 
    {
        send_command("stop");
        search_in_progress_ = false;
    }
}

bool UCIClient::initialize_engine()
{
    // Send UCI command
    if (!send_command("uci")) 
    {
        return false;
    }
    
    // Read response until we see "uciok"
    auto start_time = std::chrono::steady_clock::now();
    auto timeout = std::chrono::milliseconds(5000);     // TODO: Make timeout configurable 
    
    while (std::chrono::steady_clock::now() - start_time < timeout) 
    {
        std::string line;
        if (process_manager_->read_line_from_child(line)) 
        {
            std::cout << "[UCI Init] " << line << std::endl;
            
            // Parse engine info
            if (line.find("id name ") == 0) 
            {
                engine_name_ = line.substr(8);
            } 
            else if (line.find("id author ") == 0) 
            {
                engine_author_ = line.substr(10);
            } 
            else if (line == "uciok") 
            {
                return true;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    return false;
}

void UCIClient::engine_communication_loop()
{
    // Background thread that reads engine output
    while (!stop_thread_.load()) 
    {
        std::string line;
        if (process_manager_ && process_manager_->read_line_from_child(line)) 
        {
            // Debug output
            std::cout << "[Engine] " << line << std::endl;
            
            // Check for readyok when we're waiting for it
            if (waiting_for_ready_.load() && line == "readyok") 
            {
                ready_received_ = true;
            }
            // Check for bestmove
            else if (line.find("bestmove ") == 0) 
            {
                last_best_move_ = parse_best_move(line);
                search_in_progress_ = false;
            }
        }
        else 
        {
            // No data available, sleep a bit
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

bool UCIClient::send_command(const std::string& command)
{
    if (!process_manager_ || !process_manager_->is_running()) 
    {
        return false;
    }
    
    std::cout << "[UCI Send] " << command << std::endl;
    return process_manager_->write_to_child(command + "\n");
}

std::string UCIClient::read_until_timeout(int timeout_ms)
{
    std::string result;
    auto start_time = std::chrono::steady_clock::now();
    auto timeout = std::chrono::milliseconds(timeout_ms);
    
    while (std::chrono::steady_clock::now() - start_time < timeout) 
    {
        std::string line;
        if (process_manager_->read_line_from_child(line)) 
        {
            result += line + "\n";
        }
        else 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    return result;
}

bool UCIClient::wait_for_ready()
{
    // Set flags to indicate we're waiting for ready
    ready_received_ = false;
    waiting_for_ready_ = true;
    
    if (!send_command("isready")) 
    {
        waiting_for_ready_ = false;
        return false;
    }
    
    // Wait for "readyok" response via the background thread
    auto start_time = std::chrono::steady_clock::now();
    auto timeout = std::chrono::milliseconds(2000);
    
    while (!ready_received_.load() && 
           std::chrono::steady_clock::now() - start_time < timeout) 
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    waiting_for_ready_ = false;
    return ready_received_.load();
}

std::string UCIClient::position_to_uci(const Position& position)
{
    return "position fen " + position.to_fen();
}

std::string UCIClient::parse_best_move(const std::string& response)
{
    // Extract move from "bestmove e2e4 ponder e7e5" format
    if (response.find("bestmove ") != 0) 
    {
        return "";
    }
    
    std::string move_str = response.substr(9); // Remove "bestmove "
    
    // Find the end of the move (before "ponder" if present)
    size_t space_pos = move_str.find(' ');
    if (space_pos != std::string::npos) 
    {
        move_str = move_str.substr(0, space_pos);
    }
    
    return move_str;
}

Move UCIClient::parse_move_string(const std::string& move_str, const Position& position)
{
    if (move_str.length() < 4) 
    {
        return Move();
    }
    
    // Parse from and to squares (e.g., "e2e4")
    Square from = string_to_square(move_str.substr(0, 2));
    Square to = string_to_square(move_str.substr(2, 2));
    
    if (from == Square::None || to == Square::None) 
    {
        return Move();
    }
    
    // Parse promotion if present (e.g., "e7e8q")
    Piece promotion_piece = Piece::None;
    if (move_str.length() == 5) 
    {
        char promo_char = static_cast<char>(std::tolower(move_str[4]));
        Color color = position.side_to_move();
        
        switch (promo_char) 
        {
            case 'q': promotion_piece = make_piece(color, PieceType::Queen); break;
            case 'r': promotion_piece = make_piece(color, PieceType::Rook); break;
            case 'b': promotion_piece = make_piece(color, PieceType::Bishop); break;
            case 'n': promotion_piece = make_piece(color, PieceType::Knight); break;
        }
    }
    
    // Find matching move in legal moves
    std::vector<Move> legal_moves = position.generate_legal_moves();
    
    for (const Move& move : legal_moves) 
    {
        if (move.from_square == from && move.to_square == to) 
        {
            // Check promotion piece if it's a promotion move
            if (move.move_type == MoveType::Promotion) 
            {
                if (move.promotion_piece == promotion_piece) 
                {
                    return move;
                }
            } 
            else if (promotion_piece == Piece::None) 
            {
                // Not a promotion move and no promotion specified
                return move;
            }
        }
    }
    
    return Move(); // Move not found
}

} // namespace cge
