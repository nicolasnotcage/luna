/*
    Board coordinate system utility class implementation.

    Author: Nicolas Miller
    Date: 06/25/2025
*/

#include "chess_game/board_coordinate_system.hpp"
#include "chess_game/constants.h"

namespace cge
{

void BoardCoordinateSystem::init(float board_side, float square_size, Color player_color)
{
    board_side_ = board_side;
    square_size_ = square_size;
    player_color_ = player_color;
}

Vector2 BoardCoordinateSystem::chess_to_screen(Square square) const
{
    int file = static_cast<int>(square) % 8;
    int rank = static_cast<int>(square) / 8;

    // Rotate board for black player
    if (player_color_ == Color::Black)
    {
        file = 7 - file;
        rank = 7 - rank;
    }

    float half = board_side_ * 0.5f;
    float x = -half + square_size_*0.5f + file * square_size_;
    float y =  half - square_size_*0.5f - rank * square_size_;
    return Vector2(x,y);
}

Square BoardCoordinateSystem::screen_to_chess(float wx, float wy) const
{
    float half = board_side_ * 0.5f;

    // Convert to board-local coordinates (0..board_side_)
    float local_x = wx + half;
    float local_y = half - wy;

    if (local_x < 0.0f || local_x >= board_side_ ||
        local_y < 0.0f || local_y >= board_side_)
        return Square::None;

    int file = static_cast<int>(local_x / square_size_);
    int rank = static_cast<int>(local_y / square_size_);

    // Rotate board for black player
    if (player_color_ == Color::Black)
    {
        file = 7 - file;
        rank = 7 - rank;
    }

    return static_cast<Square>(rank * 8 + file);
}

Vector2 BoardCoordinateSystem::mouse_to_world(float mx, float my, const Camera& camera) const
{
    int screen_width = ConfigManager::get_instance().get_screen_width();
    int screen_height = ConfigManager::get_instance().get_screen_height();

    return camera.screen_to_world(Vector2(mx,my), screen_width, screen_height);
}

} // namespace cge
