#include "platform/input_interpreter.hpp"
#include "platform/event.hpp"
#include "platform/game_action.hpp"

namespace cge
{

// Translates raw SDL events into a list of game-specifc actions. 
GameActionList InputInterpreter::interpret_events(const SDLEventInfo &events)
{
    GameActionList action_list;
    action_list.num_actions = 0;

    for (uint8_t i = 0; i < events.num_events; i++)
    {
        if(action_list.num_actions >= GameActionList::MAX_ACTIONS) break;

        // Initialize action to null
        GameAction action = GameAction::NONE;

        switch (events.events[i])
        {
            // Givens
            case EventType::QUIT: action = GameAction::QUIT; break;
            case EventType::PLAY_SOUND: action = GameAction::PLAY_SOUND; break;
            case EventType::TOGGLE_MUSIC: action = GameAction::TOGGLE_MUSIC; break;
            case EventType::KEY_DOWN_ESCAPE: action = GameAction::TOGGLE_PAUSE; break;

            // WASD movement
            case EventType::KEY_HELD_W: action = GameAction::PLAYER_MOVE_UP; break;
            case EventType::KEY_HELD_S: action = GameAction::PLAYER_MOVE_DOWN; break;
            case EventType::KEY_HELD_A: action = GameAction::PLAYER_MOVE_LEFT; break;
            case EventType::KEY_HELD_D: action = GameAction::PLAYER_MOVE_RIGHT; break;

            // Zoom behavior
            case EventType::KEY_DOWN_MINUS: 
            case EventType::MOUSE_WHEEL_DOWN: action = GameAction::CAMERA_ZOOM_OUT; break;
            case EventType::KEY_DOWN_PLUS:
            case EventType::MOUSE_WHEEL_UP: action = GameAction::CAMERA_ZOOM_IN; break;

            // For clicking to get world coordinates
            case EventType::MOUSE_BUTTON_LEFT: action = GameAction::MOUSE_BUTTON_LEFT; break;

            // For player whistling
            case EventType::KEY_DOWN_F: action = GameAction::PLAYER_WHISTLE; break;

            // For player investigating
            case EventType::KEY_DOWN_E: action = GameAction::INVESTIGATE; break;

            // For toggling audio
            case EventType::KEY_DOWN_T: action = GameAction::TOGGLE_MUSIC; break;

            // For saving the game
            case EventType::KEY_DOWN_O: action = GameAction::SAVE_GAME; break;

            // For advancing text
            case EventType::KEY_DOWN_SPACE: action = GameAction::ADVANCE_TEXT; break;

            // Chess promotion actions
            case EventType::KEY_DOWN_Q: action = GameAction::PROMOTE_TO_QUEEN; break;
            case EventType::KEY_DOWN_R: action = GameAction::PROMOTE_TO_ROOK; break;
            case EventType::KEY_DOWN_B: action = GameAction::PROMOTE_TO_BISHOP; break;
            case EventType::KEY_DOWN_N: action = GameAction::PROMOTE_TO_KNIGHT; break;

            // Skip over unbound events
            default: continue;
        }

        if (action != GameAction::NONE) action_list.actions[action_list.num_actions++] = action;
    }

    return action_list;
}

} // namespace cge