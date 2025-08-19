/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef PLATFORM_INPUT_HPP
#define PLATFORM_INPUT_HPP

#include <cstddef>
#include <cstdint>

namespace cge
{

enum class EventType
{
    PLAY_SOUND,
    TOGGLE_MUSIC,
    QUIT,
    TOGGLE_PAUSE,

    // New raw input events
    KEY_DOWN_W, 
    KEY_DOWN_A, 
    KEY_DOWN_S, 
    KEY_DOWN_D,
    KEY_DOWN_PLUS,  // For keyboard zoom in
    KEY_DOWN_MINUS, // For keyboard zoom out
    MOUSE_WHEEL_UP,
    MOUSE_WHEEL_DOWN,
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_RIGHT,
    MOUSE_MOTION,
    KEY_DOWN_SPACE,
    KEY_DOWN_T,
    KEY_DOWN_F,
    KEY_DOWN_E,
    KEY_DOWN_O,
    KEY_DOWN_ESCAPE,

    // Chess promotion keys
    KEY_DOWN_Q,
    KEY_DOWN_R,
    KEY_DOWN_B,
    KEY_DOWN_N,

    // Key held states
    KEY_HELD_W,
    KEY_HELD_A,
    KEY_HELD_S,
    KEY_HELD_D,
};


struct SDLEventInfo
{
    static constexpr size_t MAX_EVENTS = 20;
    uint8_t                 num_events;
    EventType               events[MAX_EVENTS];
};

SDLEventInfo get_current_events();

} // namespace cge

#endif // PLATFORM_INPUT_HPP