/*
    Unified UCI/UCI+ Interface implementation for Luna chess engine.
    
    Author: Nicolas Miller
    Date: 07/24/2025
*/

#include "unified_uci_interface.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <cctype>

namespace luna 
{

UnifiedUCIInterface::UnifiedUCIInterface() 
    : engine_(std::make_unique<Engine>()),
      searching_(false),
      stop_search_(false),
      uci_plus_mode_(false),
      current_variant_("standard")
{
}

UnifiedUCIInterface::~UnifiedUCIInterface() 
{
    if (searching_) 
    {
        stop_search_ = true;
        engine_->stop_search();
    }
    if (search_thread_.joinable()) 
    {
        search_thread_.join();
    }
}

void UnifiedUCIInterface::run() 
{
    std::string line;
    
    while (std::getline(std::cin, line)) 
    {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) continue;
        
        // Parse command
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        
        // Handle commands
        if (command == "uci") 
        {
            handle_uci();
        } 
        else if (command == "uciplus") 
        {
            handle_uciplus();
        } 
        else if (command == "isready") 
        {
            handle_isready();
        } 
        else if (command == "ucinewgame") 
        {
            handle_ucinewgame();
        } 
        else if (command == "position") 
        {
            handle_position(line);
        } 
        else if (command == "go") 
        {
            if (uci_plus_mode_) 
            {
                handle_go_extended(line);
            } 
            else 
            {
                handle_go(line);
            }
        } 
        else if (command == "stop") 
        {
            handle_stop();
        } 
        else if (command == "setoption") 
        {
            handle_setoption(line);
        } 
        else if (command == "quit") 
        {
            handle_quit();
            break;
        } 
        else if (uci_plus_mode_) 
        {
            // UCI+ specific commands only available in UCI+ mode
            if (command == "variant") 
            {
                handle_variant(line);
            } 
            else if (command == "listvariants") 
            {
                handle_listvariants();
            } 
            else if (command == "setrule") 
            {
                handle_setrule(line);
            } 
            else if (command == "listrules") 
            {
                handle_listrules();
            }
        }
        // Ignore unknown commands per UCI protocol
    }
}

void UnifiedUCIInterface::handle_uci() 
{
    // Standard UCI mode
    uci_plus_mode_ = false;
    send_id();
    send_options();
    std::cout << "uciok" << std::endl;
}

void UnifiedUCIInterface::handle_uciplus() 
{
    // Enable UCI+ mode
    enable_uci_plus_mode();
    
    std::cout << "id name " << ENGINE_NAME << " " << ENGINE_VERSION 
              << " UCI+ " << UCIPLUS_VERSION << std::endl;
    std::cout << "id author " << ENGINE_AUTHOR << std::endl;
    
    // Send UCI+ specific options
    send_options();
    std::cout << "option name Variant type combo default standard var standard";
    for (const auto& variant : rule_engine_->get_available_variants()) 
    {
        if (variant != "standard") 
        {
            std::cout << " var " << variant;
        }
    }
    std::cout << std::endl;
    
    std::cout << "uciplusok" << std::endl;
}

void UnifiedUCIInterface::enable_uci_plus_mode() 
{
    uci_plus_mode_ = true;
    if (!rule_engine_) rule_engine_ = std::make_unique<RuleEngine>();
}

void UnifiedUCIInterface::handle_isready() 
{
    // Wait for any ongoing search to complete
    if (search_thread_.joinable()) search_thread_.join();
    std::cout << "readyok" << std::endl;
}

void UnifiedUCIInterface::handle_ucinewgame() 
{
    // Stop any ongoing search
    if (searching_) 
    {
        stop_search_ = true;
        if (search_thread_.joinable()) search_thread_.join();
    }
    
    // Reset position to starting position
    current_position_ = Position();
}

void UnifiedUCIInterface::handle_position(const std::string& command) 
{
    std::vector<std::string> tokens = split_string(command);
    
    if (tokens.size() < 2) return;
    
    size_t idx = 1;
    
    // Parse position
    if (tokens[1] == "startpos") 
    {
        current_position_ = Position();
        idx = 2;
        
        // Check for variant specification in UCI+ mode
        if (uci_plus_mode_ && idx < tokens.size() && tokens[idx] == "variant") 
        {
            if (idx + 1 < tokens.size()) 
            {
                handle_variant("variant " + tokens[idx + 1]);
                idx += 2;
            }
        }
    } 
    else if (tokens[1] == "fen") 
    {
        if (tokens.size() < 8) return; // Need at least 6 FEN fields
        
        // Reconstruct FEN string
        std::string fen;
        idx = 2;
        while (idx < tokens.size() && tokens[idx] != "moves" && tokens[idx] != "rules") 
        {
            if (!fen.empty()) fen += " ";
            fen += tokens[idx];
            idx++;
        }
        
        if (!current_position_.load_fen(fen)) 
        {
            send_info_string("Invalid FEN string");
            return;
        }
    } 
    else 
    {
        return; // Invalid position command
    }
    
    // Parse moves if present
    if (idx < tokens.size() && tokens[idx] == "moves") 
    {
        idx++;
        while (idx < tokens.size()) 
        {
            Move move = parse_move(tokens[idx], current_position_);
            if (move.from_square == Square::None) 
            {
                send_info_string("Invalid move: " + tokens[idx]);
                break;
            }
                        
            current_position_.make_move(move);
            
            idx++;
        }
    }
}

void UnifiedUCIInterface::handle_go(const std::string& command) 
{
    // Stop any ongoing search
    if (searching_) 
    {
        stop_search_ = true;
        engine_->stop_search();  // Tell the engine to stop
        if (search_thread_.joinable()) search_thread_.join();
        searching_ = false;  // Reset the flag
    }
    
    std::vector<std::string> tokens = split_string(command);
    
    // Parse go parameters
    int wtime = 0, btime = 0;
    int depth = 0, movetime = 0;
    bool infinite = false;
    
    for (size_t i = 1; i < tokens.size(); i++) {
        if (tokens[i] == "infinite") 
        {
            infinite = true;
        } 
        else if (tokens[i] == "wtime" && i + 1 < tokens.size()) 
        {
            wtime = std::stoi(tokens[++i]);
        } 
        else if (tokens[i] == "btime" && i + 1 < tokens.size()) 
        {
            btime = std::stoi(tokens[++i]);
        } 
        else if (tokens[i] == "depth" && i + 1 < tokens.size()) 
        {
            depth = std::stoi(tokens[++i]);
        } 
        else if (tokens[i] == "movetime" && i + 1 < tokens.size()) 
        {
            movetime = std::stoi(tokens[++i]);
        }
    }
    
    // Calculate time for this move
    int search_time = 0;
    if (movetime > 0) 
    {
        search_time = movetime;
    } 
    else if (!infinite) 
    {
        // Use 1/20 of remaining time by default
        int time_left = (current_position_.side_to_move() == Color::White) ? wtime : btime;
        if (time_left > 0) 
        {
            search_time = time_left / 20;
            search_time = std::min(search_time, 5000); // Don't use more than 5 seconds per move
        }
    }
    
    // Set depth if specified
    if (depth > 0) 
    {
        engine_->set_max_depth(depth);
        // If no time was specified but depth was, give enough time to complete the search
        if (search_time == 0 && !infinite) 
        {
            search_time = 60000; // 60 seconds should be enough for most depths
        }
    }
    
    // Wait for any previous thread to finish
    if (search_thread_.joinable()) 
    {
        search_thread_.join();
    }
    
    // Start search in separate thread
    stop_search_ = false;
    searching_ = true;
    
    search_thread_ = std::thread([this, search_time, infinite]() {
        try 
        {
            auto start_time = std::chrono::steady_clock::now();
            
            // Create position with rule engine modifications if in UCI+ mode
            Position search_position = current_position_;
            
            // Call find_best_move with time limit
            Move best_move = engine_->find_best_move(search_position, 
                                                     infinite ? 0 : search_time);
            
            // Get search information
            const Search::SearchInfo& info = engine_->get_search_info();
            
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>
                (std::chrono::steady_clock::now() - start_time).count();
            
            // Send info line with actual search data
            std::vector<Move> pv = info.pv;
            if (pv.empty() && best_move.from_square != Square::None) 
            {
                pv.push_back(best_move);
            }
            
            send_info(info.depth_reached, info.score, info.nodes_searched, static_cast<int>(elapsed), pv);
            
            // Send best move (even if stopped, send the best move found so far)
            send_bestmove(best_move);
            
            searching_ = false;
        } 
        catch (const std::exception& e) 
        {
            std::cerr << "Error in search thread: " << e.what() << std::endl;
            searching_ = false;
        }
    });
}

void UnifiedUCIInterface::handle_go_extended(const std::string& command) 
{
    std::vector<std::string> tokens = split_string(command);
    
    // Check for UCI+ specific go commands
    bool has_perft = false;
    bool has_analyze = false;
    int perft_depth = 0;
    
    for (size_t i = 1; i < tokens.size(); i++) 
    {
        if (tokens[i] == "perft" && i + 1 < tokens.size()) 
        {
            has_perft = true;
            perft_depth = std::stoi(tokens[++i]);
        } 
        else if (tokens[i] == "analyze") 
        {
            has_analyze = true;
        }
    }
    
    if (has_perft) 
    {
        // Run perft test
        send_info_string("Running perft " + std::to_string(perft_depth));
        // TODO: Implement perft with rule engine
        return;
    }
    
    if (has_analyze) 
    {
        // Enter analysis mode
        send_info_string("Entering analysis mode");
        // TODO: Implement analysis mode
        return;
    }
    
    // Otherwise, use standard go handling
    handle_go(command);
}

void UnifiedUCIInterface::handle_stop() 
{
    if (searching_) 
    {
        stop_search_ = true;
        engine_->stop_search();
    }
}

void UnifiedUCIInterface::handle_setoption(const std::string& command) 
{
    std::vector<std::string> tokens = split_string(command);
    
    if (tokens.size() < 5) return; // Need at least: setoption name <name> value <value>
    
    if (tokens[1] != "name") return;
    
    // Find where "value" appears
    size_t value_idx = 0;
    for (size_t i = 2; i < tokens.size(); i++) 
    {
        if (tokens[i] == "value") 
        {
            value_idx = i;
            break;
        }
    }
    
    if (value_idx == 0 || value_idx + 1 >= tokens.size()) return;
    
    // Reconstruct option name
    std::string option_name;
    for (size_t i = 2; i < value_idx; i++) 
    {
        if (!option_name.empty()) option_name += " ";
        option_name += tokens[i];
    }
    
    // Get value
    std::string value = tokens[value_idx + 1];
    
    // Handle UCI+ specific options
    if (option_name == "Variant" && uci_plus_mode_) handle_variant("variant " + value);
}

void UnifiedUCIInterface::handle_quit() 
{
    if (searching_) 
    {
        stop_search_ = true;
        if (search_thread_.joinable()) search_thread_.join();
    }
}

// UCI+ specific command handlers
void UnifiedUCIInterface::handle_variant(const std::string& command) 
{
    if (!uci_plus_mode_ || !rule_engine_) return;
    
    std::vector<std::string> tokens = split_string(command);
    
    if (tokens.size() < 2) 
    {
        send_info_string("Error: variant command requires variant name");
        return;
    }
    
    std::string variant_name = tokens[1];
    
    // Check if variant is supported
    auto variants = rule_engine_->get_available_variants();
    if (std::find(variants.begin(), variants.end(), variant_name) == variants.end()) 
    {
        send_info_string("Error: unsupported variant: " + variant_name);
        return;
    }
    
    // Load variant
    current_variant_ = variant_name;
    rule_engine_->load_variant(variant_name);
    
    // Reset position to starting position for the variant
    current_position_ = Position();
    
    send_info_variant(variant_name);
    send_info_string("Variant " + variant_name + " loaded successfully");
}

void UnifiedUCIInterface::handle_listvariants() 
{
    if (!uci_plus_mode_ || !rule_engine_) return;
    
    std::cout << "info string Available variants:";
    for (const auto& variant : rule_engine_->get_available_variants()) 
    {
        std::cout << " " << variant;
    }
    std::cout << std::endl;
}

void UnifiedUCIInterface::handle_setrule(const std::string& command) 
{
    if (!uci_plus_mode_ || !rule_engine_) return;
    
    std::vector<std::string> tokens = split_string(command);
    
    if (tokens.size() < 3) 
    {
        send_info_string("Error: setrule requires rule name and parameters");
        return;
    }
    
    std::string rule_name = tokens[1];
    
    // Parse parameters (key=value pairs)
    std::map<std::string, std::string> params;
    for (size_t i = 2; i < tokens.size(); i++) 
    {
        size_t eq_pos = tokens[i].find('=');
        if (eq_pos != std::string::npos) 
        {
            std::string key = tokens[i].substr(0, eq_pos);
            std::string value = tokens[i].substr(eq_pos + 1);
            params[key] = value;
        }
    }
    
    // TODO: Dynamic rule checking when more rules are added
    if (rule_name != "king_of_the_hill") 
    {
        send_info_string("Error: only king_of_the_hill rule is supported in this version");
        return;
    }

    send_info_rule(rule_name, "configured");
}

void UnifiedUCIInterface::handle_listrules() 
{
    if (!uci_plus_mode_ || !rule_engine_) return;
    
    // For simplified rule engine, just show king_of_the_hill status
    if (rule_engine_->has_rule("king_of_the_hill")) 
    {
        send_info_string("Active rules: king_of_the_hill");
    } 
    else 
    {
        send_info_string("No custom rules active (standard chess rules apply)");
    }
    
    // List available rules (only lists KotH right now)
    send_info_string("Available rules: king_of_the_hill");
}

// Output functions
void UnifiedUCIInterface::send_id() 
{
    std::cout << "id name " << ENGINE_NAME << " " << ENGINE_VERSION << std::endl;
    std::cout << "id author " << ENGINE_AUTHOR << std::endl;
}

void UnifiedUCIInterface::send_options() 
{
    // Currently no standard UCI options
}

void UnifiedUCIInterface::send_bestmove(const Move& move) 
{
    std::cout << "bestmove " << move.to_string() << std::endl;
}

void UnifiedUCIInterface::send_info(int depth, int score, int nodes, int time_ms, 
                              const std::vector<Move>& pv) 
{
    std::cout << "info";
    std::cout << " depth " << depth;
    std::cout << " score cp " << score;
    std::cout << " nodes " << nodes;
    std::cout << " time " << time_ms;
    std::cout << " pv";
    for (const Move& move : pv) 
    {
        std::cout << " " << move.to_string();
    }
    std::cout << std::endl;
}

void UnifiedUCIInterface::send_info_string(const std::string& info) 
{
    std::cout << "info string " << info << std::endl;
}

// UCI+ specific output functions
void UnifiedUCIInterface::send_info_variant(const std::string& variant_name) 
{
    std::cout << "info string variant " << variant_name << std::endl;
}

void UnifiedUCIInterface::send_info_rule(const std::string& rule_name, const std::string& status) 
{
    std::cout << "info string rule " << rule_name << " " << status << std::endl;
}

void UnifiedUCIInterface::send_info_eval(int static_eval, int dynamic_eval) 
{
    std::cout << "info eval static " << static_eval << " dynamic " << dynamic_eval << std::endl;
}

void UnifiedUCIInterface::send_info_explain(const Move& move, const std::string& explanation) 
{
    std::cout << "info explain " << move.to_string() << " " << explanation << std::endl;
}

// Helper functions
Move UnifiedUCIInterface::parse_move(const std::string& move_str, const Position& pos) 
{
    if (move_str.length() < 4) return Move();
    
    // Parse from and to squares
    Square from = string_to_square(move_str.substr(0, 2));
    Square to = string_to_square(move_str.substr(2, 2));
    
    if (from == Square::None || to == Square::None) return Move();
    
    // Parse promotion if present
    Piece promotion_piece = Piece::None;
    if (move_str.length() == 5) 
    {
        char promo_char = static_cast<char>(std::tolower(move_str[4]));
        Color color = pos.side_to_move();
        switch (promo_char) 
        {
            case 'q': promotion_piece = make_piece(color, PieceType::Queen); break;
            case 'r': promotion_piece = make_piece(color, PieceType::Rook); break;
            case 'b': promotion_piece = make_piece(color, PieceType::Bishop); break;
            case 'n': promotion_piece = make_piece(color, PieceType::Knight); break;
            default: return Move(); // Invalid promotion
        }
    }
    
    // Generate legal moves and find matching move
    std::vector<Move> legal_moves = pos.generate_legal_moves();
        
    for (const Move& move : legal_moves) 
    {
        if (move.from_square == from && move.to_square == to) 
        {
            if (move.move_type == MoveType::Promotion) 
            {
                if (move.promotion_piece == promotion_piece) 
                {
                    return move;
                }
            } 
            else if (promotion_piece == Piece::None) 
            {
                return move;
            }
        }
    }
    
    return Move(); // Move not found in legal moves
}

std::vector<std::string> UnifiedUCIInterface::split_string(const std::string& str) 
{
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (iss >> token) 
    {
        tokens.push_back(token);
    }
    return tokens;
}

bool UnifiedUCIInterface::is_uci_plus_command(const std::string& command) const 
{
    static const std::vector<std::string> uci_plus_commands = 
    {
        "uciplus", "variant", "listvariants", "setrule", "listrules"
    };
    
    return std::find(uci_plus_commands.begin(), uci_plus_commands.end(), command) != uci_plus_commands.end();
}

} // namespace luna
