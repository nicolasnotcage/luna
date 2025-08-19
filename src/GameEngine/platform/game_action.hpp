/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef GAME_ACTION_HPP
#define GAME_ACTION_HPP

#include <vector>

namespace cge
{

// Actions specific to the game; will be used when mapping 
// raw SDL input to game-specific behavior
enum class GameAction
{
    NONE, 

	// System actions
	QUIT,
    TOGGLE_PAUSE,
    SAVE_GAME,

	// Audio actions
	PLAY_SOUND,
	TOGGLE_MUSIC,

	// Camera actions
    CAMERA_MOVE_UP,
    CAMERA_MOVE_DOWN,
    CAMERA_MOVE_LEFT,
    CAMERA_MOVE_RIGHT,
    CAMERA_ZOOM_IN,
    CAMERA_ZOOM_OUT,

    // Player actions
    PLAYER_MOVE_UP, 
    PLAYER_MOVE_DOWN,
    PLAYER_MOVE_LEFT,
    PLAYER_MOVE_RIGHT,
    PLAYER_WHISTLE,
    ADVANCE_TEXT,
    INVESTIGATE,

    // Mouse actions
    MOUSE_BUTTON_LEFT,

    // Chess promotion actions
    PROMOTE_TO_QUEEN,
    PROMOTE_TO_ROOK,
    PROMOTE_TO_BISHOP,
    PROMOTE_TO_KNIGHT
};

// A collection of game actions
struct GameActionList
{
    static constexpr size_t MAX_ACTIONS = 20;
    uint8_t                 num_actions;
    GameAction              actions[MAX_ACTIONS];
};

} // namespace cge

#endif // GAME_ACTION_HPP