/*
    Implementation of the main chess game scene. 

    Author: Nicolas Miller
    Date: 06/25/2025
*/

#include "chess_game/main_scene.hpp"
#include "chess_game/constants.h"
#include "platform/scene_manager.hpp"
#include "system/file_locator.hpp"
#include "system/serializer.hpp"
#include "system/config_manager.hpp"

#include <iostream>
#include <cstdlib>
#include <ctime>

namespace cge
{

void MainScene::init(SDLInfo *sdl_info, IoHandler *io_handler)
{
    sdl_info_   = sdl_info;
    io_handler_ = io_handler;

    SDL_SetRenderDrawColor(sdl_info->renderer,0,0,0,0);
    SDL_SetRenderDrawBlendMode(sdl_info->renderer,SDL_BLENDMODE_BLEND);

    scene_state_.reset();
    scene_state_.sdl_info    = sdl_info_;
    scene_state_.io_handler  = io_handler_;

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Initialize popup manager
    popup_manager_.init(scene_state_);
    
    // Initialize audio manager
    audio_manager_.init(scene_state_);
    
    // Initialize move handler with position, popup manager, and audio manager
    // TODO: Set whether or not you want to use UCI+ variants here for now; will add config options later.
    // If set to true, you'll be playing King of the Hill. Set to False for standard game.
    bool enable_variants = false;
    move_handler_.init(chess_position_, popup_manager_, audio_manager_, enable_variants);

    // Configure variant for testing
    if (enable_variants) 
    {
        bool variant_set = move_handler_.set_variant("king_of_the_hill");
        if (variant_set) 
        {
            std::cout << "Successfully configured King of the Hill variant" << std::endl;
        } 
        else 
        {
            std::cout << "Failed to set variant - using standard chess" << std::endl;
        }
    }

    // Load textures
    load_textures();

    // Initialize root
    root_.init(scene_state_);

    // Setup camera and board
    setup_camera();   
    setup_board();
    
    // Initialize coordinate system
    coord_system_.init(board_side_, square_size_, player_color_);
    
    // Configure squares and setup pieces
    initialize_square_centers();
    setup_pieces();
    
    // Setup popups using the popup manager
    auto& camera_node = root_.get_child<0>();
    float camera_width = camera_node.get_camera().get_width();
    float camera_height = camera_node.get_camera().get_height();
    
    // Get popup nodes
    auto* promotion_prompt = &camera_node.get_child<2>();
    auto* player_won = &camera_node.get_child<3>();
    auto* player_lost = &camera_node.get_child<4>();
    auto* game_tied = &camera_node.get_child<5>();
    
    // Setup popup nodes
    popup_manager_.setup_popup_nodes(
        promotion_prompt,
        player_won,
        player_lost,
        game_tied,
        camera_width,
        camera_height
    );

    // Load chess-specific sounds using the audio manager
    audio_manager_.load_chess_sounds();
    
    // If player plays black, computer moves first
    if (player_color_ == Color::Black) 
    {
        is_computer_turn_ = true;
        computer_move_timer_ = COMPUTER_MOVE_DELAY;  // Delay before first move
    }
}

void MainScene::load_textures()
{
    // Board
    board_texture_ = std::make_unique<TextureNode>();
    board_texture_->set_filepath("images/chess/board.png");
    board_texture_->init(scene_state_);

    // Pieces
    const std::vector<std::pair<std::string,std::string>> files = {
        {"white_pawn"  ,"images/chess/white_pawn.png"},
        {"white_knight","images/chess/white_knight.png"},
        {"white_bishop","images/chess/white_bishop.png"},
        {"white_rook"  ,"images/chess/white_rook.png"},
        {"white_queen" ,"images/chess/white_queen.png"},
        {"white_king"  ,"images/chess/white_king.png"},
        {"black_pawn"  ,"images/chess/black_pawn.png"},
        {"black_knight","images/chess/black_knight.png"},
        {"black_bishop","images/chess/black_bishop.png"},
        {"black_rook"  ,"images/chess/black_rook.png"},
        {"black_queen" ,"images/chess/black_queen.png"},
        {"black_king"  ,"images/chess/black_king.png"}};

    for (auto &[key,path] : files)
    {
        auto tex = std::make_unique<TextureNode>();
        tex->set_filepath(path);
        tex->init(scene_state_);
        piece_textures_[key] = std::move(tex);
    }
}

// Setup camera relative to screen size
void MainScene::setup_camera()
{
    auto &camera_node = root_.get_child<0>();

    float screen_width = static_cast<float>(ConfigManager::get_instance().get_screen_width());
    float screen_height = static_cast<float>(ConfigManager::get_instance().get_screen_height());
    float aspect = screen_width / screen_height;

    float camera_width = CAMERA_HEIGHT * aspect;

    camera_node.get_camera().set_dimensions(camera_width,CAMERA_HEIGHT);
    camera_node.get_camera().set_position(0.0f,0.0f);
}

void MainScene::setup_board()
{
    auto &camera_node      = root_.get_child<0>();
    auto &board_transform  = camera_node.get_child<0>();
    auto &board_sprite     = board_transform.get_child<0>();

    board_sprite.set_filepath(board_texture_->get_filepath());
    board_sprite.init(scene_state_);

    // Compute sizes
    float camera_width = camera_node.get_camera().get_width();
    float camera_height = camera_node.get_camera().get_height();
    board_side_ = BOARD_SCALE_FACTOR * std::min(camera_width,camera_height);
    square_size_ = board_side_ / 8.0f;

    // Scale & Center
    board_transform.right_scale(board_side_,board_side_);
    board_transform.set_position(0.0f,0.0f);
}

void MainScene::initialize_square_centers()
{
    for (int sq = 0; sq < 64; ++sq)
        square_centers_[sq] = coord_system_.chess_to_screen(static_cast<Square>(sq));
}

void MainScene::setup_pieces()
{
    auto &piece_container = root_.get_child<0>().get_child<1>();

    // Retrieve all 32 chess piece nodes for setup
    auto pieces = get_all_piece_nodes(std::make_index_sequence<32>{});
    for (ChessPiece* p : pieces)
    {
        p->init(scene_state_);
        p->right_scale(square_size_ * PIECE_SCALE_FACTOR, square_size_ * PIECE_SCALE_FACTOR);
    }
    update_piece_visuals();
}

void MainScene::update_piece_visuals()
{
    piece_map_.clear();
    auto pieces = get_all_piece_nodes(std::make_index_sequence<32>{});
    int idx = 0;

    for (int sq = 0; sq < 64; ++sq)
    {
        Piece p = chess_position_.piece_on(static_cast<Square>(sq));
        if (p == Piece::None) continue;

        ChessPiece* node = pieces[idx++];

        auto key = get_piece_texture_key(p);
        if (piece_textures_.count(key))
            node->get_child<0>().set_texture(piece_textures_[key].get());

        Vector2 pos = square_centers_[sq];
        node->set_position(pos.x,pos.y);

        piece_map_[static_cast<Square>(sq)] = node;
    }

    // Hide unused
    for (int i = idx; i < 32; ++i)
        pieces[i]->right_scale(0.0f,0.0f);
}

bool MainScene::is_player_turn() const
{
    return chess_position_.side_to_move() == player_color_;
}


void MainScene::snap_piece_to_square(ChessPiece* piece, Square square)
{
    if (piece && square != Square::None) 
    {
        Vector2 pos = square_centers_[static_cast<int>(square)];
        piece->set_position(pos.x, pos.y);
    }
}

void MainScene::handle_mouse_down(float world_x, float world_y)
{
    // Don't handle mouse input during promotion or game over
    if (move_handler_.is_waiting_for_promotion() || game_over_) return;
    
    if (!is_player_turn()) return;

    Square clicked_square = coord_system_.screen_to_chess(world_x, world_y);
    
    if (clicked_square != Square::None) 
    {
        Piece piece = chess_position_.piece_on(clicked_square);
        
        // Check if we clicked on our piece (player's color)
        if (piece != Piece::None && color_of(piece) == player_color_) 
        {
            selected_square_ = clicked_square;
            drag_square_ = clicked_square;
            is_dragging_ = true;
            
            // Store the original position for snap-back
            drag_original_position_ = square_centers_[static_cast<int>(clicked_square)];
            
            // Calculate offset from piece center
            drag_offset_.x = world_x - drag_original_position_.x;
            drag_offset_.y = world_y - drag_original_position_.y;
        }
    }
}

void MainScene::handle_mouse_up(float world_x, float world_y)
{
    // Don't handle mouse input during promotion or game over
    if (move_handler_.is_waiting_for_promotion() || game_over_) return;
    
    if (!is_dragging_ || !is_player_turn()) return;

    Square release_square = coord_system_.screen_to_chess(world_x, world_y);
    ChessPiece* dragged_piece = piece_map_[drag_square_];
    
    bool move_made = false;
    
    if (release_square != Square::None && release_square != selected_square_) 
    {
        // Try to make the move
        if (move_handler_.is_legal_move(selected_square_, release_square)) 
        {
            // Execute the move (returns true if completed, false if waiting for promotion)
            bool move_completed = move_handler_.execute_move(selected_square_, release_square, player_color_);
            
            // Update visuals
            update_piece_visuals();
            
            if (move_completed) 
            {
                // Switch turns only if move was completed
                is_computer_turn_ = true;
                computer_move_timer_ = COMPUTER_MOVE_DELAY;
            }
            
            // If not completed, we're waiting for promotion input
            move_made = true;
        } 
        else 
        {
            // Play illegal move sound
            audio_manager_.play_illegal_sound(1.0f);
        }
    }
    
    // If move was not made, snap the piece back to its original square
    if (!move_made && dragged_piece) 
    {
        snap_piece_to_square(dragged_piece, selected_square_);
    }
    
    // Reset drag state
    is_dragging_ = false;
    selected_square_ = Square::None;
    drag_square_ = Square::None;
}

void MainScene::handle_mouse_motion(float world_x, float world_y)
{
    if (!is_dragging_) return;

    // Update the position of the dragged piece
    if (piece_map_.find(drag_square_) != piece_map_.end()) 
    {
        ChessPiece* piece = piece_map_[drag_square_];
        if (piece) 
        {
            piece->set_position(world_x - drag_offset_.x, world_y - drag_offset_.y);
        }
    }
}

void MainScene::handle_keyboard_input()
{
    if (!move_handler_.is_waiting_for_promotion()) return;
    
    const GameActionList& actions = io_handler_->get_game_actions();
    
    for (uint8_t i = 0; i < actions.num_actions; ++i)
    {
        PieceType promotion_piece_type = PieceType::Pawn;  // Default invalid
        
        switch (actions.actions[i])
        {
            case GameAction::PROMOTE_TO_QUEEN:
                promotion_piece_type = PieceType::Queen;
                break;
            case GameAction::PROMOTE_TO_ROOK:
                promotion_piece_type = PieceType::Rook;
                break;
            case GameAction::PROMOTE_TO_BISHOP:
                promotion_piece_type = PieceType::Bishop;
                break;
            case GameAction::PROMOTE_TO_KNIGHT:
                promotion_piece_type = PieceType::Knight;
                break;
            default:
                // Not a promotion action, skip
                continue;
        }
        
        if (promotion_piece_type != PieceType::Pawn)  // Valid promotion piece
        {
            move_handler_.complete_promotion(promotion_piece_type);
            
            // Update visuals
            update_piece_visuals();
            
            // Switch turns
            is_computer_turn_ = true;
            computer_move_timer_ = COMPUTER_MOVE_DELAY;
            break;
        }
    }
}

void MainScene::make_computer_move()
{
    move_handler_.make_computer_move(player_color_);
    
    // Update board
    update_piece_visuals();
    
    // Check if this move ended the game
    bool game_just_ended = move_handler_.is_game_over();
    
    // Switch turns
    is_computer_turn_ = false;
    
    // If the game just ended, prevent immediate game over detection
    if (game_just_ended && !game_over_) 
    {
        // Use elapsed_time_ to delay game over detection; probably a better 
        // solve, but this works...
        elapsed_time_ = -2; 
    }
}

std::string MainScene::get_piece_texture_key(Piece piece)
{
    switch (piece) 
    {
        case Piece::WhitePawn:  return "white_pawn";
        case Piece::WhiteKnight:return "white_knight";
        case Piece::WhiteBishop:return "white_bishop";
        case Piece::WhiteRook:  return "white_rook";
        case Piece::WhiteQueen: return "white_queen";
        case Piece::WhiteKing:  return "white_king";
        case Piece::BlackPawn:  return "black_pawn";
        case Piece::BlackKnight:return "black_knight";
        case Piece::BlackBishop:return "black_bishop";
        case Piece::BlackRook:  return "black_rook";
        case Piece::BlackQueen: return "black_queen";
        case Piece::BlackKing:  return "black_king";
        default: return "";
    }
}

void MainScene::check_game_over()
{
    bool was_game_over = game_over_;
    bool move_handler_game_over = move_handler_.is_game_over();
    
    // Only set game over if we're not in a delay period (elapsed_time_ >= 0)
    if (elapsed_time_ >= 0.0) {
        game_over_ = move_handler_game_over;
    }
    
    // Reset elapsed time when game becomes over
    if (!was_game_over && game_over_) elapsed_time_ = 0.0;
}

void MainScene::update(double delta)
{
    scene_state_.io_handler = io_handler_;
    scene_state_.delta = delta;

    // Update audio engine
    audio_manager_.update();
    
    // Game ongoing
    if (!game_over_)
    {
        // Handle delay period after computer winning move
        if (elapsed_time_ < 0.0) {
            elapsed_time_ += delta;
            // Don't process input during delay, just update visuals
            root_.update(scene_state_);
            return;
        }
        
        // Pause Game
        const GameActionList& actions = io_handler_->get_game_actions();
        for (uint8_t i = 0; i < actions.num_actions; ++i)
        {
            if (actions.actions[i] == GameAction::TOGGLE_PAUSE)
            {
                SceneManager::get_instance()->push_scene_by_key("pause_menu");
                return;
            }
        }

        // Handle promotion input
        if (move_handler_.is_waiting_for_promotion())
        {
            handle_keyboard_input();
            
            // Don't process other inputs during promotion
            root_.update(scene_state_);
            return;
        }

        // Handle computer move timer
        if (is_computer_turn_ && computer_move_timer_ > 0) 
        {
            computer_move_timer_ -= delta;
            if (computer_move_timer_ <= 0) 
            {
                make_computer_move();
            }
        }
        
        // Get mouse state
        float mouse_x, mouse_y;
        uint32_t mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);
        auto &cam = root_.get_child<0>().get_camera();
        Vector2 world_pos = coord_system_.mouse_to_world(mouse_x, mouse_y, cam);
        
        // Check for mouse button events
        for (uint8_t i = 0; i < actions.num_actions; i++) 
        {
            if (actions.actions[i] == GameAction::MOUSE_BUTTON_LEFT) 
            {
                // This fires on click - we need to check actual state
                if (mouse_state & SDL_BUTTON_LMASK) 
                {
                    if (!is_dragging_) 
                    {
                        handle_mouse_down(world_pos.x, world_pos.y);
                    }
                }
            }
        }
        
        // Handle mouse release
        if (is_dragging_ && !(mouse_state & SDL_BUTTON_LMASK)) 
        {
            handle_mouse_up(world_pos.x, world_pos.y);
        }
        
        // Handle mouse motion while dragging
        if (is_dragging_) 
        {
            handle_mouse_motion(world_pos.x, world_pos.y);
        }

    }

    // Game over
    else 
    {
        elapsed_time_ += delta;
        
        // Show game over popup after a short delay
        if (!popup_manager_.is_game_over_popup_shown() && elapsed_time_ >= GAME_OVER_DELAY) 
        {
            // Use the variant-aware popup method that gets the result from move handler
            luna::GameResult game_result = move_handler_.get_game_result();
            popup_manager_.show_game_over_popup_with_result(game_result, player_color_);
        }
        
        // Update popup display timer
        popup_manager_.update_popup_timer(delta);

        // Push game over scene after popup has been shown for a while
        if (popup_manager_.is_game_over_popup_shown() && popup_manager_.get_popup_display_timer() >= POPUP_DISPLAY_DURATION) 
        {
            SceneManager::get_instance()->push_scene_by_key("game_over_menu");
        }
    }

    // Update scene graph
    root_.update(scene_state_);
    
    // Check for game over AFTER all visual updates are complete
    check_game_over();
}

void MainScene::render()
{
    scene_state_.reset();
    scene_state_.sdl_info = sdl_info_;
    scene_state_.io_handler = io_handler_;

    root_.draw(scene_state_);
}

void MainScene::destroy()
{
    // Clear piece map
    piece_map_.clear();

    // Destroy textures
    if (board_texture_) board_texture_->destroy();
    
    for (auto& [key, texture] : piece_textures_) 
    {
        texture->destroy();
    }
    
    // Destroy popup manager
    popup_manager_.destroy();
    
    // Destroy root
    root_.destroy();
}

void MainScene::serialize(Serializer& serializer) const
{
    std::string fen = chess_position_.to_fen();
    serializer.write("chess_position", fen);
    serializer.write("is_computer_turn", is_computer_turn_);
    serializer.write("player_color", static_cast<int>(player_color_));
    serializer.write("game_over", game_over_);
    serializer.write("show_game_over_popup", popup_manager_.is_game_over_popup_shown());
}

void MainScene::deserialize(Serializer& serializer)
{
    std::string fen;
    if (serializer.read("chess_position", fen)) 
    {
        chess_position_.load_fen(fen);
    }
    
    serializer.read("is_computer_turn", is_computer_turn_);
    serializer.read("game_over", game_over_);
    bool show_game_over_popup = false;
    serializer.read("show_game_over_popup", show_game_over_popup);
    
    int color_int;
    if (serializer.read("player_color", color_int))
    {
        player_color_ = static_cast<Color>(color_int);
        coord_system_.init(board_side_, square_size_, player_color_);
    }
    
    // Update visuals and initialize move handler
    // TODO: Add UCI+ variant flag to serialization
    update_piece_visuals();
    move_handler_.init(chess_position_, popup_manager_, audio_manager_, false);
    
    // Show appropriate popups if needed
    bool show_popup = false;
    if (serializer.read("show_game_over_popup", show_popup) && show_popup)
    {
        popup_manager_.show_game_over_popup(chess_position_, player_color_);
    }
    else
    {
        popup_manager_.hide_all_popups();
    }
}

template<std::size_t... Is>
std::vector<ChessPiece*> MainScene::get_all_piece_nodes(std::index_sequence<Is...>)
{
    auto& piece_container = root_.get_child<0>().get_child<1>();
    return { &piece_container.get_child<Is>()... };
}

} // namespace cge
