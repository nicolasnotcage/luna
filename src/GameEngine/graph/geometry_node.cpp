/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "graph/geometry_node.hpp"

#include "graph/texture_node.hpp"
#include "graph/sprite_node.hpp"
#include "platform/math.hpp"
#include "platform/config.hpp"
#include "graph/camera_node.hpp"

namespace cge
{

void GeometryNode::init(SceneState &scene_state) {}

void GeometryNode::destroy() { clear_children(); }

void GeometryNode::draw(SceneState &scene_state)
{
    // Get vertices in screen space from local space
    auto tl = scene_state.matrix_stack.top() * Vector2(-0.5f, -0.5f);
    auto tr = scene_state.matrix_stack.top() * Vector2(0.5f, -0.5f);
    auto bl = scene_state.matrix_stack.top() * Vector2(-0.5f, 0.5f);

    // Flip sprite horizontally if needed
    bool should_flip = scene_state.sprite_flipped && scene_state.in_sprite_context;
    if(should_flip)
    { 
        std::swap(tl, tr);
        bl = scene_state.matrix_stack.top() * Vector2(0.5f, 0.5f);
    }

    // Use the screen space coordinates for rendering
    SDL_FPoint top_left{tl.x, tl.y};
    SDL_FPoint top_right{tr.x, tr.y};
    SDL_FPoint bottom_left{bl.x, bl.y};

    if(scene_state.using_sprite_sheet)
    {
        // Convert SDL_Rect to SDL_FRect for the source rectangle
        SDL_FRect src_frect;
        src_frect.x = static_cast<float>(scene_state.current_frame_rect.x);
        src_frect.y = static_cast<float>(scene_state.current_frame_rect.y);
        src_frect.w = static_cast<float>(scene_state.current_frame_rect.w);
        src_frect.h = static_cast<float>(scene_state.current_frame_rect.h);

        // Render just the current frame
        SDL_RenderTextureAffine(scene_state.sdl_info->renderer,
                                scene_state.texture_node->sdl_texture(),
                                &src_frect, // Now using SDL_FRect*
                                &top_left,
                                &top_right,
                                &bottom_left);
    }
    else
    {
        // Render the entire texture as before
        SDL_RenderTextureAffine(scene_state.sdl_info->renderer,
                                scene_state.texture_node->sdl_texture(),
                                nullptr,
                                &top_left,
                                &top_right,
                                &bottom_left);
    }
}


void GeometryNode::update(SceneState& scene_state)
{
    // Geometry Node update behavior (if needed)
}

} // namespace cge
