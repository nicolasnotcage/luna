/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef EXAMPLES_GAME_OVER_MENU_HPP
#define EXAMPLES_GAME_OVER_MENU_HPP

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

// Game over menu scene for the game
class GameOverMenuScene : public Scene
{
public:
    GameOverMenuScene() = default;
    virtual ~GameOverMenuScene() = default;

    // Scene interface implementation
    virtual void init(SDLInfo* sdl_info, IoHandler* io_handler) override;
    virtual void destroy() override;
    virtual void render() override;
    virtual void update(double delta) override;

    // Serializable overrides
    virtual void serialize(Serializer& serializer) const override;
    virtual void deserialize(Serializer& serializer) override;

    // Called when scene becomes active (top of stack)
    virtual void on_enter() override;

private:
    // Configure graph
    using MenuBackground = TransformNodeT<SpriteNodeT<GeometryNodeT<>>>;
    using MenuButton = UIButtonT<TransformNodeT<SpriteNodeT<GeometryNodeT<>>>>;
    using MenuTitle = TransformNodeT<SpriteNodeT<GeometryNodeT<>>>;
    using MenuGraph = CameraNodeT<MenuBackground, MenuTitle, MenuButton, MenuButton, MenuButton>;
    using GameOverMenuRoot = RootNodeT<MenuGraph>;

    // Set root
    GameOverMenuRoot root_;

    // Initialize textures stored within the game over menu.
    void initialize_textures();

    // Textures owned by the game over menu
    TextureNode background_texture_;
    TextureNode title_texture_;
    
    // Scene state and SDL info
    SDLInfo* sdl_info_;
    SceneState scene_state_;
    IoHandler* io_handler_;
};

} // namespace cge

#endif // EXAMPLES_GAME_OVER_MENU_HPP
