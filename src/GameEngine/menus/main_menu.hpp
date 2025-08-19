/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef EXAMPLES_MAIN_MENU_HPP
#define EXAMPLES_MAIN_MENU_HPP

#include "graph/root_node.hpp"
#include "graph/camera_node.hpp"
#include "graph/sprite_node.hpp"
#include "graph/transform_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/geometry_node.hpp"
#include "graph/ui_button.hpp"

#include "platform/audio_component.hpp"
#include "platform/io_handler.hpp"
#include "platform/path.hpp"
#include "platform/scene.hpp"

#include <array>


namespace cge
{

// Main menu scene for the game
class MainMenuScene : public Scene
{
public:
    MainMenuScene() = default;
    virtual ~MainMenuScene() = default;

    // Scene interface implementation
    virtual void init(SDLInfo* sdl_info, IoHandler* io_handler) override;
    virtual void destroy() override;
    virtual void render() override;
    virtual void update(double delta) override;

    // Serializable overrides
    virtual void serialize(Serializer& serializer) const override;
    virtual void deserialize(Serializer& serializer) override;

private:
    // Configure graph
    using MenuBackground = TransformNodeT<SpriteNodeT<GeometryNodeT<>>>;
    using MenuButton = UIButtonT<TransformNodeT<SpriteNodeT<GeometryNodeT<>>>>;
    using MenuTitle = TransformNodeT<SpriteNodeT<GeometryNodeT<>>>;
    using MenuGraph = CameraNodeT<MenuBackground, MenuTitle, MenuButton, MenuButton, MenuButton, MenuButton>;
    using UIRoot = RootNodeT<MenuGraph>;

    // Set root
    UIRoot root_;

    SDLInfo* sdl_info_;
    SceneState scene_state_;
    IoHandler* io_handler_;

    // Initialize textures owned by the main menu.
    void initialize_textures();
    
    // Configure audio used by the main menu.
    void setup_audio();

    // Textures
    TextureNode background_texture_;
    TextureNode title_texture_;
};

} // namespace cge

#endif // EXAMPLES_MAIN_MENU_HPP
