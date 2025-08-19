/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef PLATFORM_TYPES_HPP
#define PLATFORM_TYPES_HPP

#include "platform/sdl.h"

namespace cge
{

// Custom struct containing pointers to SDL window and SDL renderer objects
struct SDLInfo
{
    SDL_Window   *window;
    SDL_Renderer *renderer;
};

} // namespace cge

#endif // PLATFORM_TYPES_HPP
