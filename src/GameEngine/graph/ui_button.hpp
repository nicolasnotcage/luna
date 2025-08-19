/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef GRAPH_UI_BUTTON_HPP
#define GRAPH_UI_BUTTON_HPP

#include "graph/node.hpp"
#include "graph/sprite_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/transform_node.hpp"
#include "platform/io_handler.hpp"

#include <functional>
#include <string>

namespace cge
{

// Forward declarations
class CameraNode;

/*
* A utility node designed to represent buttons for user interface components. 
* Buttons have three variations: Normal, Hover, and Pressed. Callback functions
* can be configured to execute when a button is pressed.
*/
class UIButton : public Node
{
public:
    UIButton();
    virtual ~UIButton() = default;

    // Node interface implementation
    virtual void init(SceneState &scene_state) override;
    virtual void destroy() override;
    virtual void draw(SceneState &scene_state) override;
    virtual void update(SceneState &scene_state) override;

    // Set the button's position in world space.
    void set_position(float x, float y);

    // Set the size of the button.
    void set_size(float width, float height);

    // Set the filepath associated with the image representing the Normal button state.
    void set_normal_sprite(const std::string& filepath);

    // Set the filepath associated with the image representing the Hover button state.
    void set_hover_sprite(const std::string& filepath);

    // Set the filepath associated with the image representing the Pressed button state.
    void set_pressed_sprite(const std::string& filepath);

    // Set the callback function associated with the button.
    void set_callback(std::function<void()> callback);
    
    // Set the TransformNode associated with the Button.
    void set_transform_node(TransformNode* transform);

    // Set the SpriteNode associated with the Button.
    void set_sprite_node(SpriteNode* sprite);

    // Set the CameraNode associated with the Button.
    void set_camera_node(CameraNode* camera);
    
    // Check if a point is inside the button
    bool contains_point(float x, float y) const;

private:
    enum class ButtonState
    {
        NORMAL,
        HOVER,
        PRESSED
    };

    ButtonState state_{ButtonState::NORMAL};    // The current state of the button (Normal, Hover, or Pressed)
    std::function<void()> callback_;            // Callback function to be executed on button press
    
    float x_{0.0f};                             // Button's x position
    float y_{0.0f};                             // Button's y position
    float width_{1.0f};                         // Button's width
    float height_{1.0f};                        // Button's height
    
    TextureNode normal_texture_;                // Button's normal texture
    TextureNode hover_texture_;                 // Button's hover texture
    TextureNode pressed_texture_;               // Button's pressed texture
    
    TextureNode* current_texture_{nullptr};     // A pointer to the current active texture
    
    // References to nodes in the graph
    TransformNode* transform_node_{nullptr};
    SpriteNode* sprite_node_{nullptr};
    CameraNode* camera_node_{nullptr};

    // Displays an initialization error
    void display_init_error();
};

template <typename... ChildrenTs>
using UIButtonT = NodeT<UIButton, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_UI_BUTTON_HPP
