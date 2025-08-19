/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef PLATFORM_CORE_HPP
#define PLATFORM_CORE_HPP

#include "platform/types.hpp"

#include <string>

namespace cge
{

void init_sdl();

void create_sdl_components(SDLInfo           &sdl_info,
                           int                window_width,
                           int                window_height,
                           const std::string &window_name);

void destroy_sdl_components(SDLInfo &sdl_info);

} // namespace cge

#endif // PLATFORM_CORE_HPP
