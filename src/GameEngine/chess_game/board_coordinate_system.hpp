/*
    A utility class representing the board coordinate system used 
    in the chess game.
    
    Author: Nicolas Miller
    Date: 06/25/2025
*/

#ifndef CHESS_GAME_BOARD_COORDINATE_SYSTEM_HPP
#define CHESS_GAME_BOARD_COORDINATE_SYSTEM_HPP

#include "graph/camera_node.hpp"
#include "system/config_manager.hpp"
#include "chess_game/constants.h"
#include "types.h"
#include <cstdint>

namespace cge
{

class BoardCoordinateSystem
{
public:
    BoardCoordinateSystem() = default;
    ~BoardCoordinateSystem() = default;

    // Initialize with board metrics
    void init(float board_side, float square_size, Color player_color);

    // Set player color (for board rotation)
    void set_player_color(Color color) { player_color_ = color; }

    // Coordinate conversion functions
    Vector2 chess_to_screen(Square sq) const;
    Square screen_to_chess(float world_x, float world_y) const;
    Vector2 mouse_to_world(float mouse_x, float mouse_y, const Camera& camera) const;

private:
    float board_side_{0.0f};   // World-unit length of one edge of the board
    float square_size_{0.0f};  // World-unit length of one square
    Color player_color_{Color::White};  // Which color the player controls
};

} // namespace cge

#endif // CHESS_GAME_BOARD_COORDINATE_SYSTEM_HPP
