/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef GRAPH_SCENE_STATE_HPP
#define GRAPH_SCENE_STATE_HPP

#include "platform/types.hpp"
#include "platform/math.hpp"

namespace cge
{

// Forward declaration
class TextureNode;
class IoHandler;

// Utility struct containing objects relevant to managing scene state. 
struct SceneState
{
    SDLInfo     *sdl_info;
    TextureNode *texture_node;
    float        delta;
    MatrixStack  matrix_stack;
    IoHandler   *io_handler;

    // For sprite sheet support
    bool         using_sprite_sheet{false};
    SDL_Rect     current_frame_rect = {0, 0, 0, 0};

    // For sprite handling
    bool sprite_flipped{false};
    bool in_sprite_context{false};

    void reset();
};

} // namespace cge

#endif // GRAPH_SCENE_STATE_HPP
