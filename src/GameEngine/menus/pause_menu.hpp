/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef EXAMPLES_PAUSE_MENU_HPP
#define EXAMPLES_PAUSE_MENU_HPP

#include "platform/scene.hpp"
#include "graph/root_node.hpp"
#include "graph/camera_node.hpp"
#include "graph/sprite_node.hpp"
#include "graph/transform_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/geometry_node.hpp"
#include "graph/ui_button.hpp"

namespace cge
{

// Pause menu scene for the game
class PauseMenuScene : public Scene
{
public:
    PauseMenuScene() = default;
    virtual ~PauseMenuScene() = default;

    // Scene interface implementation
    virtual void init(SDLInfo* sdl_info, IoHandler* io_handler) override;
    virtual void destroy() override;
    virtual void render() override;
    virtual void update(double delta) override;

    // Nothing to serialize for a menu.
    virtual void serialize(Serializer& serializer) const override;

    // Nothing to deserialize for a menu.
    virtual void deserialize(Serializer& serializer) override;

    // Called when scene becomes active (top of stack)
    virtual void on_enter() override;

private:
    // Configure graph
    using MenuBackground = TransformNodeT<SpriteNodeT<GeometryNodeT<>>>;
    using MenuButton = UIButtonT<TransformNodeT<SpriteNodeT<GeometryNodeT<>>>>;
    using MenuGraph = CameraNodeT<MenuBackground, MenuButton, MenuButton, MenuButton>;
    using PauseMenuRoot = RootNodeT<MenuGraph>;

    // Set root
    PauseMenuRoot root_;

    // Initialize menu textures.
    void initialize_textures();

    // Textures
    TextureNode background_texture_;
    
    // Scene state and SDL info
    SDLInfo* sdl_info_;
    SceneState scene_state_;
    IoHandler* io_handler_;
};

} // namespace cge

#endif // EXAMPLES_PAUSE_MENU_HPP
