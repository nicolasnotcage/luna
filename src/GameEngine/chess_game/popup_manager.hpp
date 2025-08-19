/*
    A utility class to manage pop-up menus within the chess game. Abstracted out of the main
    scene to keep it focused on game flow. This class provides representations for:
    
    1) Promotion Piece Selection Menu
    2) Game Over State Pop-Ups (win, loss, stalemate)

    Author: Nicolas Miller
    Date: 06/25/2025
*/

#ifndef CHESS_GAME_POPUP_MANAGER_HPP
#define CHESS_GAME_POPUP_MANAGER_HPP

#include "graph/camera_node.hpp"
#include "graph/geometry_node.hpp"
#include "graph/scene_state.hpp"
#include "graph/texture_node.hpp"
#include "graph/transform_node.hpp"
#include "graph/node.hpp"

#include "chess_game/constants.h"
#include "position.h"
#include "types.h"
#include "ChessRules/include/variant_position.h"

#include <memory>
#include <string>

namespace cge
{

// Forward declarations
class AudioEngine;
class MoveHandler;

// Popup node types; used in main scene
using PromotionPrompt = TransformNodeT<TextureNodeT<GeometryNodeT<>>>;
using PlayerWon = TransformNodeT<TextureNodeT<GeometryNodeT<>>>;
using PlayerLost = TransformNodeT<TextureNodeT<GeometryNodeT<>>>;
using GameTied = TransformNodeT<TextureNodeT<GeometryNodeT<>>>;

class PopupManager
{
public:
    PopupManager();
    ~PopupManager() = default;

    // Initialize the popup manager
    void init(SceneState& scene_state);
    
    // Store scene state reference
    void set_scene_state(SceneState& scene_state) { scene_state_ = &scene_state; }
    
    // Destroy resources
    void destroy();
    
    // Promotion UI handling 
    void show_promotion_prompt();
    void hide_promotion_prompt();
    
    // Game over popup handling
    void show_game_over_popup(const Position& chess_position, Color player_color);
    void show_game_over_popup_with_result(luna::GameResult game_result, Color player_color);
    void hide_all_popups();
    
    // Getters
    bool is_game_over_popup_shown() const { return show_game_over_popup_; }
    double get_popup_display_timer() const { return popup_display_timer_; }
    
    // Update popup timer
    void update_popup_timer(double delta);

    // Setup nodes
    void setup_popup_nodes(
        PromotionPrompt* promotion_prompt,
        PlayerWon* player_won,
        PlayerLost* player_lost,
        GameTied* game_tied,
        float camera_width,
        float camera_height);

private:
    // Popup textures
    std::unique_ptr<TextureNode> promotion_texture_;
    std::unique_ptr<TextureNode> you_win_texture_;
    std::unique_ptr<TextureNode> you_lose_texture_;
    std::unique_ptr<TextureNode> stalemate_texture_;
    
    // Popup state
    bool show_game_over_popup_{false};
    double popup_display_timer_{0.0};
    
    // Node references
    PromotionPrompt* promotion_prompt_{nullptr};
    PlayerWon* player_won_{nullptr};
    PlayerLost* player_lost_{nullptr};
    GameTied* game_tied_{nullptr};
    
    // Scene state reference
    SceneState* scene_state_{nullptr};
    
    // Helper methods
    void load_textures(SceneState& scene_state);
    void setup_promotion_popup(float camera_width, float camera_height);
    void setup_game_over_popups(float camera_width, float camera_height);
};

} // namespace cge

#endif // CHESS_GAME_POPUP_MANAGER_HPP
