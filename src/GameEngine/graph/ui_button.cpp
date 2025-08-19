/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "graph/ui_button.hpp"
#include "platform/math.hpp"
#include "platform/config.hpp"
#include "graph/camera_node.hpp"
#include "system/config_manager.hpp"

namespace cge
{

UIButton::UIButton() {}

void UIButton::init(SceneState &scene_state)
{
    // Init Base Texture
    if (!normal_texture_.get_filepath().empty()) normal_texture_.init(scene_state);
    else display_init_error();
    
    // Init Hover Texture
    if (!hover_texture_.get_filepath().empty()) hover_texture_.init(scene_state);
    else display_init_error();

    // Init Pressed Texture
    if (!pressed_texture_.get_filepath().empty()) pressed_texture_.init(scene_state);
    else display_init_error();

    // Set current texture
    current_texture_ = &normal_texture_;

    // Assign to sprite texture if pointer exists
    if (sprite_node_ != nullptr) sprite_node_->set_texture(current_texture_);
    else std::cerr << "UI Button has undefined sprite pointer!\n";

    // Update transform if set
    if (transform_node_ != nullptr)
    {
        transform_node_->set_position(x_, y_);
        transform_node_->right_scale(width_, height_);
    }
    else
    {
        std::cerr << "UI Button has undefined transform node pointer!\n";
    }

    // Throw error if camera pointer isn't stored
    if (camera_node_ == nullptr) std::cerr << "UI Button has undefined camera node pointer!\n";

    // Initialize children
    init_children(scene_state);
}

void UIButton::destroy()
{
    // Destroy textures
    normal_texture_.destroy();
    hover_texture_.destroy();
    pressed_texture_.destroy();

    // Destroy children
    destroy_children();
    clear_children();
}

void UIButton::draw(SceneState &scene_state)
{
    // Nothing to draw for the buttons; sprites handle drawing
    draw_children(scene_state);
}

void UIButton::update(SceneState &scene_state)
{
    // Skip if we don't have the required nodes
    if (!transform_node_ || !sprite_node_ || !camera_node_) 
    {
        return;
    }
    
    // Get mouse position in screen coordinates
    float mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    Vector2 screen_position(mouse_x, mouse_y);
    
    // Convert to world coordinates using the camera
    Vector2 world_pos = camera_node_->get_camera().screen_to_world(
        screen_position, 
        cge::ConfigManager::get_instance().get_screen_width(), 
        cge::ConfigManager::get_instance().get_screen_height());
    
    // Check if mouse is over the button
    bool is_hovering = contains_point(world_pos.x, world_pos.y);
    
    // Check for mouse button press
    const GameActionList &actions = scene_state.io_handler->get_game_actions();
    bool is_mouse_pressed = false;
    
    for (uint8_t i = 0; i < actions.num_actions; i++) 
    {
        if (actions.actions[i] == GameAction::MOUSE_BUTTON_LEFT) 
        {
            is_mouse_pressed = true;
            break;
        }
    }
    
    // Update button state
    ButtonState previous_state = state_;
    
    if (is_hovering) 
    {
        if (is_mouse_pressed) 
        {
            state_ = ButtonState::PRESSED;
        } 
        else 
        {
            state_ = ButtonState::HOVER;
        }
    } 
    else 
    {
        state_ = ButtonState::NORMAL;
    }
    
    // Update texture based on state
    TextureNode* texture_to_use = &normal_texture_;
    
    switch (state_) 
    {
        case ButtonState::NORMAL:
            texture_to_use = &normal_texture_;
            break;
        case ButtonState::HOVER:
            texture_to_use = &hover_texture_;
            break;
        case ButtonState::PRESSED:
            texture_to_use = &pressed_texture_;
            break;
    }
    
    // Only update if the texture changed
    if (current_texture_ != texture_to_use) 
    {
        current_texture_ = texture_to_use;
        sprite_node_->set_texture(current_texture_);
    }
    
    // Call callback if button was just pressed
    if (previous_state == ButtonState::HOVER && state_ == ButtonState::PRESSED && callback_) 
    {
        callback_();
    }
}

void UIButton::set_position(float x, float y)
{
    x_ = x;
    y_ = y;
}

void UIButton::set_size(float width, float height)
{
    width_ = width;
    height_ = height;
}

void UIButton::set_transform_node(TransformNode* transform)
{
    transform_node_ = transform;
}

void UIButton::set_sprite_node(SpriteNode* sprite)
{
    sprite_node_ = sprite;
}

void UIButton::set_camera_node(CameraNode* camera)
{
    camera_node_ = camera;
}

void UIButton::set_normal_sprite(const std::string& filepath)
{
    normal_texture_.set_filepath(filepath);
    normal_texture_.set_blend(true);
    normal_texture_.set_blend_alpha(200);
}

void UIButton::set_hover_sprite(const std::string& filepath)
{
    hover_texture_.set_filepath(filepath);
    hover_texture_.set_blend(true);
    hover_texture_.set_blend_alpha(200);
}

void UIButton::set_pressed_sprite(const std::string& filepath)
{
    pressed_texture_.set_filepath(filepath);
    pressed_texture_.set_blend(true);
    pressed_texture_.set_blend_alpha(200);
}

void UIButton::set_callback(std::function<void()> callback)
{
    callback_ = callback;
}

bool UIButton::contains_point(float x, float y) const
{
    // Use the full button size for hit detection
    float half_width = width_ * 0.5f;
    float half_height = height_ * 0.5f;
    
    return (x >= x_ - half_width && 
            x <= x_ + half_width && 
            y >= y_ - half_height && 
            y <= y_ + half_height);
}

void UIButton::display_init_error()
{
    std::cerr << "Normal Button texture uninitailized. Check that buttons are initialized in your scene!\n";
}

} // namespace cge
