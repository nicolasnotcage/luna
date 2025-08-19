/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef GRAPH_TEXTURE_NODE_HPP
#define GRAPH_TEXTURE_NODE_HPP

#include "graph/node.hpp"
#include "graph/node_t.hpp"

#include "platform/sdl.h"
#include "platform/animation.hpp"

namespace cge
{
class TextureNode : public Node
{
  public:
    TextureNode();

    ~TextureNode() = default;

    // Overrides
    void init(SceneState &scene_state) override;
    void destroy() override;
    void draw(SceneState &scene_state) override;
    void update(SceneState &scene_state) override;

    SDL_Texture *sdl_texture();

    // Texture configuration
    int width() const;
    int height() const;
    void set_filepath(const std::string &filepath);
    void set_color_mods(const uint8_t mods[3]);
    void set_blend(bool blend);
    void set_blend_alpha(uint8_t alpha);

    //------------------------------------------
    // Sprite sheet functionality
    //------------------------------------------

    // Define a single frame of a sprite sheet
    void define_frame(uint32_t frame_id, int x, int y, int width, int height);

    // Define a regular grid-based sprite sheet
    void define_grid(int cols, int rows, int frame_width, int frame_height);

    // Set current sprite sheet frame
    void set_current_frame(uint32_t frame_id);

    // Map of sprite sheet frame IDs to Frames
    const std::unordered_map<uint32_t, Frame> &get_frames() const;

    // Return current sprite sheet frame ID
    uint32_t get_current_frame_id() const;

    // Return if texture node is a spritesheet
    bool is_spritesheet() const { return is_sprite_sheet_; }

    // Return filepath
    std::string get_filepath() const { return filepath_; }

    // Set whether or not the node should be rendered
    void set_should_render(bool should_render) { is_rendered_ = should_render; }

  protected:
    SDL_Texture *texture_;
    int          width_;
    int          height_;
    std::string  filepath_{};
    bool         is_rendered_{true};

    bool    apply_color_mod_;
    uint8_t color_mods_[3];

    bool    apply_blend_;
    uint8_t blend_alpha_;

    // Sprite sheet members
    std::unordered_map<uint32_t, Frame> frames_;
    uint32_t                            current_frame_id_;
    bool                                is_sprite_sheet_{false};
};

template <typename... ChildrenTs>
using TextureNodeT = NodeT<TextureNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_TEXTURE_NODE_HPP
