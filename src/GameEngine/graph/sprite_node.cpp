#include "graph/sprite_node.hpp"
#include "graph/geometry_node.hpp"
#include "graph/scene_state.hpp"
#include "graph/texture_node.hpp"
#include "graph/transform_node.hpp"

#include "platform/movement_controller.hpp"

#include <memory>

namespace cge
{

SpriteNode::SpriteNode() : current_texture_(nullptr), current_frame_id_(0)
{
    // Create animator on construction
    animator_ = std::make_unique<Animator>();
}

void SpriteNode::init(SceneState &scene_state) { init_children(scene_state); }

void SpriteNode::destroy()
{
    destroy_children();
    clear_children();

    current_texture_ = nullptr;
    animation_textures_.clear();
}

void SpriteNode::draw(SceneState &scene_state)
{
    // If we don't have a texture, just draw children
    if(!current_texture_)
    {
        draw_children(scene_state);
        return;
    }

    // Store original state
    TextureNode *prev_texture_node = scene_state.texture_node;
    bool         prev_using_sprite_sheet = scene_state.using_sprite_sheet;
    SDL_Rect     prev_rect = scene_state.current_frame_rect;
    bool         prev_sprite_flipped = scene_state.sprite_flipped;
    bool         prev_in_sprite_context = scene_state.in_sprite_context;

    // Set current state for rendering
    scene_state.texture_node = current_texture_;
    scene_state.sprite_flipped = facing_left_;
    scene_state.in_sprite_context = true;

    // Set frame information
    bool using_sprite_sheet = false;
    if (current_texture_->is_spritesheet())
    {
        auto& frames = current_texture_->get_frames();
        auto frame_it = frames.find(current_frame_id_);
        if (frame_it != frames.end())
        {
            scene_state.using_sprite_sheet = true;
            const Frame& frame = frame_it->second;
            scene_state.current_frame_rect.x = frame.x;
            scene_state.current_frame_rect.y = frame.y;
            scene_state.current_frame_rect.w = frame.width;
            scene_state.current_frame_rect.h = frame.height;

            using_sprite_sheet = true;
        }
    }
    
    // Assign sprite sheet bool based on results of the above
    scene_state.using_sprite_sheet = using_sprite_sheet;


    // Draw children (which will use our updated scene state)
    draw_children(scene_state);

    // Restore original state
    scene_state.texture_node = prev_texture_node;
    scene_state.using_sprite_sheet = prev_using_sprite_sheet;
    scene_state.current_frame_rect = prev_rect;
    scene_state.sprite_flipped = prev_sprite_flipped;
    scene_state.in_sprite_context = prev_in_sprite_context;
}

void SpriteNode::update(SceneState &scene_state)
{
    // Set sprite flip state
    scene_state.sprite_flipped = facing_left_;

    // Update animation based on movement state
    update_animation_for_movement();

    // Update animation state if we have an animator
    if(animator_ && animator_->is_playing())
    {
        // Update the animator
        animator_->update(scene_state.delta);

        // Get the current frame ID
        current_frame_id_ = animator_->get_current_frame_id();

        // Update current texture based on the animation
        std::string current_anim = animator_->get_current_animation_name();
        auto        it = animation_textures_.find(current_anim);
        if(it != animation_textures_.end()) { current_texture_ = it->second; }
    }

    // Update children
    update_children(scene_state);
}

void SpriteNode::set_texture(TextureNode *texture)
{
    current_texture_ = texture;

    // If we have a texture and it's a sprite sheet, default to first frame
    if(texture && !texture->get_frames().empty())
    {
        current_frame_id_ = texture->get_current_frame_id();
    }
}

TextureNode *SpriteNode::get_texture() const { return current_texture_; }

// Animation delegation methods
void SpriteNode::add_animation(const Animation &animation) { animator_->add_animation(animation); }

void SpriteNode::add_animation_with_texture(const Animation &animation, TextureNode *texture)
{
    animator_->add_animation(animation);
    if(texture) { animation_textures_[animation.get_name()] = texture; }
}

void SpriteNode::play(const std::string &animation_name)
{
    // Update the texture first if we have a mapping
    auto it = animation_textures_.find(animation_name);
    if(it != animation_textures_.end()) { current_texture_ = it->second; }

    // Then play the animation
    animator_->play(animation_name);
}

void SpriteNode::pause() { animator_->pause(); }

void SpriteNode::resume() { animator_->resume(); }

void SpriteNode::reset() { animator_->reset(); }

void SpriteNode::set_playback_speed(float speed) { animator_->set_playback_speed(speed); }

void SpriteNode::set_looping(bool looping) { animator_->set_looping(looping); }

bool SpriteNode::is_playing() const { return animator_->is_playing(); }

const std::string &SpriteNode::get_current_animation_name() const
{
    return animator_->get_current_animation_name();
}

void SpriteNode::update_animation_for_movement()
{
    // Skip automatic animation switching if disabled
    if (!auto_animation_enabled_)
        return;
        
    if(is_moving_)
    {
        // If we have a walk animation, play it when moving
        if(animation_textures_.find("run") != animation_textures_.end())
        {
            // Only switch if not already playing walk animation
            if(get_current_animation_name() != "run") { play("run"); }
        }
    }
    else
    {
        // Play idle animation when not moving
        if(animation_textures_.find("idle") != animation_textures_.end())
        {
            // Only switch if not already playing idle animation
            if(get_current_animation_name() != "idle") { play("idle"); }
        }
    }
}

void SpriteNode::set_movement_state(bool is_moving, MoveDirection direction, bool facing_left)
{
    is_moving_ = is_moving;
    current_direction_ = direction;
    facing_left_ = facing_left;
}

} // namespace cge
