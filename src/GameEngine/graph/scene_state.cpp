/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "graph/scene_state.hpp"
#include "graph/texture_node.hpp"

namespace cge
{

// Sets SDLInfo and texture node data members to nullptr.n
// TODO: Reset delta and matrix stack as well?
void SceneState::reset()
{
    sdl_info = nullptr;
    texture_node = nullptr;
    io_handler = nullptr;
}

} // namespace cge
