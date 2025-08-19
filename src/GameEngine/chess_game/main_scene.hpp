/*
    The main scene for our chess game, which demonstrates the capabilities 
    of the chess engine. Core game loop logic and user interaction is handled
    by this class. There are various utility classes that handle things related
    to audio, pop-up textures, and coordinate systems.

    Author: Nicolas Miller
    Date: 06/25/2025
*/

#ifndef CHESS_GAME_MAIN_SCENE_HPP
#define CHESS_GAME_MAIN_SCENE_HPP

#include "graph/camera_node.hpp"
#include "graph/geometry_node.hpp"
#include "graph/node.hpp"
#include "graph/root_node.hpp"
#include "graph/scene_state.hpp"
#include "graph/sprite_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/transform_node.hpp"

#include "chess_game/constants.h"

#include "platform/io_handler.hpp"
#include "platform/scene.hpp"
#include "platform/scene_manager.hpp"
#include "platform/audio_manager.hpp"

#include "chess_game/popup_manager.hpp"
#include "chess_game/board_coordinate_system.hpp"
#include "chess_game/move_handler.hpp"

#include "position.h"
#include "types.h"

#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

// Forward declarations for variant support
namespace luna {
    class RuleEngine;
    class VariantPosition;
}

namespace cge
{

// Graph aliases
using ChessPiece  = TransformNodeT<SpriteNodeT<GeometryNodeT<>>>;
using ChessBoard  = TransformNodeT<TextureNodeT<GeometryNodeT<>>>;

// 32 pre-allocated piece slots
using PieceContainer = TransformNodeT<
    ChessPiece, ChessPiece, ChessPiece, ChessPiece, ChessPiece, ChessPiece, ChessPiece, ChessPiece,
    ChessPiece, ChessPiece, ChessPiece, ChessPiece, ChessPiece, ChessPiece, ChessPiece, ChessPiece,
    ChessPiece, ChessPiece, ChessPiece, ChessPiece, ChessPiece, ChessPiece, ChessPiece, ChessPiece,
    ChessPiece, ChessPiece, ChessPiece, ChessPiece, ChessPiece, ChessPiece, ChessPiece, ChessPiece>;

using ChessScene = CameraNodeT<ChessBoard, PieceContainer, PromotionPrompt, PlayerWon, PlayerLost, GameTied>;

class MainScene : public Scene
{
public:
    virtual ~MainScene() = default;

    void init(SDLInfo *sdl_info, IoHandler *io_handler) override;
    void destroy() override;
    void render() override;
    void update(double delta) override;

    void serialize(Serializer& serializer) const override;
    void deserialize(Serializer& serializer) override;

    // Set the player's color (should be called before init)
    void set_player_color(Color color) 
    { 
        player_color_ = color; 
        coord_system_.set_player_color(color);
    }

private:
    SDLInfo  *sdl_info_{};
    IoHandler* io_handler_{};
    RootNodeT<ChessScene> root_;
    SceneState scene_state_;

    // Chess State
    Position                        chess_position_;
    std::array<Vector2,64>          square_centers_{};
    std::map<Square, ChessPiece*>   piece_map_;
    bool                            game_over_{false};
    float                           elapsed_time_{0.0};
    bool                            is_computer_turn_{false};
    double                          computer_move_timer_{0.0};
    Color                           player_color_{Color::White};
    
    // Variant support
    std::unique_ptr<luna::RuleEngine> rule_engine_;
    std::unique_ptr<luna::VariantPosition> variant_position_;
    
    // Interaction handling
    Square  selected_square_{Square::None};
    Square  drag_square_{Square::None};
    Vector2 drag_offset_{};
    Vector2 drag_original_position_{};
    bool    is_dragging_{false};
    
    // Composition instances
    PopupManager popup_manager_;
    BoardCoordinateSystem coord_system_;
    MoveHandler move_handler_;
    AudioManager audio_manager_;

    // Board metrics
    float board_side_{0.0f};   // World-unit length of one edge of the board
    float square_size_{0.0f};  // World-unit length of one square

    // Textures
    std::unique_ptr<TextureNode>                       board_texture_;
    std::map<std::string,std::unique_ptr<TextureNode>> piece_textures_;

    // Helper functions
    void load_textures();
    void setup_camera();
    void setup_board();
    void initialize_square_centers();
    void setup_pieces();
    void update_piece_visuals();
    void check_game_over();
    bool is_player_turn() const;
    static std::string get_piece_texture_key(Piece p);

    // Input and game flow
    void handle_mouse_down(float wx,float wy);
    void handle_mouse_up  (float wx,float wy);
    void handle_mouse_motion(float wx,float wy);
    void handle_keyboard_input();
    void snap_piece_to_square(ChessPiece* piece,Square sq);
    void make_computer_move();
    
    template<std::size_t...Is>
    std::vector<ChessPiece*> get_all_piece_nodes(std::index_sequence<Is...>);
};

} // namespace cge

#endif // CHESS_GAME_MAIN_SCENE_HPP
