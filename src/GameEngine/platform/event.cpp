/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/event.hpp"
#include "platform/sdl.h"

#include <iostream>

namespace cge
{

SDLEventInfo get_current_events()
{
    SDLEventInfo event_info;
    uint8_t      num_events = 0;

    SDL_Event e;
    while(SDL_PollEvent(&e) && num_events < SDLEventInfo::MAX_EVENTS)
    {
        switch(e.type)
        {
            // Mouse button actions
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if(e.button.button == SDL_BUTTON_LEFT)
                    event_info.events[num_events++] = EventType::MOUSE_BUTTON_LEFT;
                else if(e.button.button == SDL_BUTTON_RIGHT)
                    event_info.events[num_events++] = EventType::MOUSE_BUTTON_RIGHT;
                break;

            // Mouse wheel actions
            case SDL_EVENT_MOUSE_WHEEL:
                if(e.wheel.y > 0) 
                    event_info.events[num_events++] = EventType::MOUSE_WHEEL_UP;
                else if(e.wheel.y < 0)
                    event_info.events[num_events++] = EventType::MOUSE_WHEEL_DOWN;
                break;

            // Quit action
            case SDL_EVENT_QUIT: event_info.events[num_events++] = EventType::QUIT; break;

            // Key press actions
            case SDL_EVENT_KEY_DOWN:
                switch(e.key.key)
                {
                    // Given actions
                    case SDLK_ESCAPE: event_info.events[num_events++] = EventType::KEY_DOWN_ESCAPE; break;
                    case SDLK_SPACE: event_info.events[num_events++] = EventType::KEY_DOWN_SPACE; break;

                    // Other keys
                    case SDLK_W: event_info.events[num_events++] = EventType::KEY_DOWN_W; break;
                    case SDLK_A: event_info.events[num_events++] = EventType::KEY_DOWN_A; break;
                    case SDLK_S: event_info.events[num_events++] = EventType::KEY_DOWN_S; break;
                    case SDLK_D: event_info.events[num_events++] = EventType::KEY_DOWN_D; break;
                    case SDLK_T: event_info.events[num_events++] = EventType::KEY_DOWN_T; break;
                    case SDLK_F: event_info.events[num_events++] = EventType::KEY_DOWN_F; break;
                    case SDLK_E: event_info.events[num_events++] = EventType::KEY_DOWN_E; break;
                    case SDLK_O: event_info.events[num_events++] = EventType::KEY_DOWN_O; break;
                    
                    // Chess promotion keys
                    case SDLK_Q: event_info.events[num_events++] = EventType::KEY_DOWN_Q; break;
                    case SDLK_R: event_info.events[num_events++] = EventType::KEY_DOWN_R; break;
                    case SDLK_B: event_info.events[num_events++] = EventType::KEY_DOWN_B; break;
                    case SDLK_N: event_info.events[num_events++] = EventType::KEY_DOWN_N; break;
                    
                    case SDLK_PLUS:
                    case SDLK_EQUALS:
                        event_info.events[num_events++] = EventType::KEY_DOWN_PLUS;
                        break;
                    case SDLK_MINUS:
                        event_info.events[num_events++] = EventType::KEY_DOWN_MINUS;
                        break;

                    default: break;
                }

                break;

            default: break;
        }
    }

    // Check for held keys
    int numkeys;
    const bool *keystate = SDL_GetKeyboardState(&numkeys);

    // Add key held states to the event list
    if(keystate[SDL_SCANCODE_W] && num_events < SDLEventInfo::MAX_EVENTS)
        event_info.events[num_events++] = EventType::KEY_HELD_W;
    if(keystate[SDL_SCANCODE_A] && num_events < SDLEventInfo::MAX_EVENTS)
        event_info.events[num_events++] = EventType::KEY_HELD_A;
    if(keystate[SDL_SCANCODE_S] && num_events < SDLEventInfo::MAX_EVENTS)
        event_info.events[num_events++] = EventType::KEY_HELD_S;
    if(keystate[SDL_SCANCODE_D] && num_events < SDLEventInfo::MAX_EVENTS)
        event_info.events[num_events++] = EventType::KEY_HELD_D;

    event_info.num_events = num_events;

    return event_info;
}

} // namespace cge