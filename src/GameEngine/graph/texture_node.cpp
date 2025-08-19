/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "graph/texture_node.hpp"

#include "image/image.hpp"
#include "system/file_locator.hpp"

namespace cge
{

TextureNode::TextureNode() :
    texture_{nullptr},
    width_{0},
    height_{0},
    filepath_{""},
    apply_color_mod_{false},
    color_mods_{0},
    apply_blend_{false},
    blend_alpha_{0}
{
}

void TextureNode::init(SceneState &scene_state)
{
    auto file_info = locate_path_for_filename(filepath_);
    auto result = image::create_texture(*scene_state.sdl_info, file_info.path);

    texture_ = result.texture;
    width_ = result.width;
    height_ = result.height;

    // Prevents distortion when scaling up pixel art
    SDL_SetTextureScaleMode(texture_, SDL_ScaleMode::SDL_SCALEMODE_NEAREST);

    init_children(scene_state);
}

void TextureNode::destroy()
{
    destroy_children();
    clear_children();

    SDL_DestroyTexture(texture_);
}

void TextureNode::draw(SceneState &scene_state)
{
    if (is_rendered_)
    {
        // Store old values
        TextureNode *prex_texture_node = scene_state.texture_node;
        bool         old_using_sprite_sheet = scene_state.using_sprite_sheet;
        SDL_Rect     old_rect = scene_state.current_frame_rect;

        scene_state.texture_node = this;

        if(apply_blend_)
        {
            SDL_SetTextureBlendMode(texture_, SDL_BLENDMODE_BLEND);
            SDL_SetTextureAlphaMod(texture_, blend_alpha_);
        }
        else { SDL_SetTextureBlendMode(texture_, SDL_BLENDMODE_NONE); }

        if(apply_color_mod_)
        {
            SDL_SetTextureBlendMode(texture_, SDL_BLENDMODE_BLEND);
            SDL_SetTextureColorMod(texture_, color_mods_[0], color_mods_[1], color_mods_[2]);
        }     

        // Handle sprite sheets
        if(is_sprite_sheet_ && frames_.find(current_frame_id_) != frames_.end())
        {
            scene_state.using_sprite_sheet = true;

            Frame frame = frames_[current_frame_id_];
            scene_state.current_frame_rect.x = frame.x;
            scene_state.current_frame_rect.y = frame.y;
            scene_state.current_frame_rect.w = frame.width;
            scene_state.current_frame_rect.h = frame.height;
        }
        else
        { 
            scene_state.using_sprite_sheet = false;
        }

        draw_children(scene_state);
        
        // Restore old values
        scene_state.texture_node = prex_texture_node;
        scene_state.using_sprite_sheet = old_using_sprite_sheet;
        scene_state.current_frame_rect = old_rect;
    }
}

void TextureNode::update(SceneState &scene_state)
{ 
    update_children(scene_state); 
}

SDL_Texture *TextureNode::sdl_texture() { return texture_; }

int TextureNode::width() const { return width_; }

int TextureNode::height() const { return height_; }

void TextureNode::set_filepath(const std::string &filepath) { filepath_ = filepath; }

void TextureNode::set_color_mods(const uint8_t mods[3])
{
    apply_color_mod_ = true;
    color_mods_[0] = mods[0];
    color_mods_[1] = mods[1];
    color_mods_[2] = mods[2];
}

void TextureNode::set_blend(bool blend) { apply_blend_ = blend; }

void TextureNode::set_blend_alpha(uint8_t alpha) { blend_alpha_ = alpha; }

void TextureNode::define_frame(uint32_t frame_id, int x, int y, int width, int height)
{
    // Define and store sprite sheet frame
    Frame frame{x, y, width, height};
    frames_[frame_id] = frame;

    // When initialized, set sprite sheet flag and update current frame ID
    if(frames_.size() == 1) 
    { 
        is_sprite_sheet_ = true;
        current_frame_id_ = frame_id;
    }
}

void TextureNode::define_grid(int cols, int rows, int width, int height)
{
    uint32_t frame_id = 0;

    // Iteratively define sprite sheet frames on 
    // grid-based sheets
    for(int y = 0; y < rows; y++)
    {
        for(int x = 0; x < cols; x++)
        {
            define_frame(frame_id++,
                         x * width,  // Column offset
                         y * height, // Row offset
                         width,
                         height);
        }
    }

    // Set first frame as current
    if(!frames_.empty()) { current_frame_id_ = 0; }
}

void TextureNode::set_current_frame(uint32_t frame_id)
{
    if (frames_.find(frame_id) != frames_.end()) current_frame_id_ = frame_id;
}

const std::unordered_map<uint32_t, Frame> &TextureNode::get_frames() const { return frames_; }

uint32_t TextureNode::get_current_frame_id() const { return current_frame_id_; }

} // namespace cge
