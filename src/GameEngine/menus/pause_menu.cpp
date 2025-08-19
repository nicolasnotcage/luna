/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "menus/pause_menu.hpp"
#include "platform/math.hpp"
#include "platform/scene_manager.hpp"
#include "platform/audio_engine.hpp"
#include "system/config_manager.hpp"
#include "system/save_manager.hpp"
#include "system/file_locator.hpp"

namespace cge
{

void PauseMenuScene::init(SDLInfo* sdl_info, IoHandler* io_handler)
{
    sdl_info_ = sdl_info;
    io_handler_ = io_handler;

    SDL_SetRenderDrawColor(sdl_info->renderer, 0, 0, 0, 0);
    SDL_SetRenderDrawBlendMode(sdl_info->renderer, SDL_BLENDMODE_BLEND);

    // Reset scene state
    scene_state_.reset();
    scene_state_.sdl_info = sdl_info_;
    scene_state_.io_handler = io_handler_;

    // Initialize textures
    initialize_textures();

    // Configure camera with dimensions adjusted for screen aspect ratio
    auto &camera = root_.get_child<0>();
    float aspect_ratio = static_cast<float>(cge::ConfigManager::get_instance().get_screen_width()) / 
                         static_cast<float>(cge::ConfigManager::get_instance().get_screen_height());
    float camera_height = 15.0f;
    float camera_width = camera_height * aspect_ratio;
    camera.get_camera().set_dimensions(camera_width, camera_height);
    camera.get_camera().set_position(0.0f, 0.0f);
    
    // Disable zooming in pause menu
    camera.set_zoom_enabled(false);

    // Get references to components
    auto &menu_background_transform = camera.get_child<0>();
    auto &menu_background_sprite = menu_background_transform.get_child<0>();
    
    // Get button references
    auto &resume_button = camera.get_child<1>();
    auto &main_menu_button = camera.get_child<2>();
    auto &exit_button = camera.get_child<3>();
    
    // Configure background
    menu_background_sprite.set_texture(&background_texture_);

    // Scale pause menu to camera size
    menu_background_transform.right_scale(camera_width, camera_height); 
    menu_background_transform.right_translate(0.0f, 0.0f); 
    
    // Configure Resume button
    auto &resume_transform = resume_button.get_child<0>();
    auto &resume_sprite = resume_transform.get_child<0>();
    
    // Set up the button with references to nodes in the graph
    resume_button.set_transform_node(&resume_transform);
    resume_button.set_sprite_node(&resume_sprite);
    resume_button.set_camera_node(&camera);
    
    // Configure the button
    resume_button.set_position(0.0f, -3.0f);
    resume_button.set_size(6.0f, 2.0f);
    resume_button.set_normal_sprite("images/ui/buttons/resume/resume_base_button.png");
    resume_button.set_hover_sprite("images/ui/buttons/resume/resume_button_on_hover.png");
    resume_button.set_pressed_sprite("images/ui/buttons/resume/resume_button_clicked.png");
    resume_button.set_callback([this]() 
        {
        // Pop the pause menu to return to the main scene
        SceneManager::get_instance()->pop_scene();
        });
    
    // Configure Main Menu button
    auto &main_menu_transform = main_menu_button.get_child<0>();
    auto &main_menu_sprite = main_menu_transform.get_child<0>();
    
    // Set up the button with references to nodes in the graph
    main_menu_button.set_transform_node(&main_menu_transform);
    main_menu_button.set_sprite_node(&main_menu_sprite);
    main_menu_button.set_camera_node(&camera);
    
    // Configure the button
    main_menu_button.set_position(0.0f, 0.0f);
    main_menu_button.set_size(6.0f, 2.0f);
    main_menu_button.set_normal_sprite("images/ui/buttons/main_menu/main_menu_base_button.png");
    main_menu_button.set_hover_sprite("images/ui/buttons/main_menu/main_menu_button_on_hover.png");
    main_menu_button.set_pressed_sprite("images/ui/buttons/main_menu/main_menu_button_clicked.png");
    main_menu_button.set_callback([this]() 
        {
        // Get the main scene (one below the pause menu in the stack)
        std::vector<Scene*> scenes;
        SceneManager::get_instance()->get_all_scenes(scenes);
        
        if (scenes.size() >= 2) 
        {
            // The main scene should be the second-to-last scene in the stack
            Scene* main_scene = scenes[scenes.size() - 2];
            
            // Save the main scene state
            SaveManager::get_instance().save_game(main_scene);
            
            // Pop the pause menu
            SceneManager::get_instance()->pop_scene();
            
            // Pop the main scene to return to the main menu
            SceneManager::get_instance()->pop_scene();
        }
        });
    
    // Configure Exit button
    auto &exit_transform = exit_button.get_child<0>();
    auto &exit_sprite = exit_transform.get_child<0>();
    
    // Set up the button with references to nodes in the graph
    exit_button.set_transform_node(&exit_transform);
    exit_button.set_sprite_node(&exit_sprite);
    exit_button.set_camera_node(&camera);
    
    // Configure the button
    exit_button.set_position(0.0f, 3.0f);
    exit_button.set_size(6.0f, 2.0f);
    exit_button.set_normal_sprite("images/ui/buttons/quit_game/quit_game_base_button.png");
    exit_button.set_hover_sprite("images/ui/buttons/quit_game/quit_game_button_on_hover.png");
    exit_button.set_pressed_sprite("images/ui/buttons/quit_game/quit_game_button_clicked.png");
    exit_button.set_callback([this]() 
        {
        // Get the main scene (one below the pause menu in the stack)
        std::vector<Scene*> scenes;
        SceneManager::get_instance()->get_all_scenes(scenes);
        
        if (scenes.size() >= 2) {
            // The main scene should be the second-to-last scene in the stack
            Scene* main_scene = scenes[scenes.size() - 2];
            
            // Save the main scene state
            SaveManager::get_instance().save_game(main_scene);
            
            // Request quit
            SDL_Event quit_event;
            quit_event.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&quit_event);
        }
        });
    
    // Initialize root node
    root_.init(scene_state_);
}

void PauseMenuScene::initialize_textures()
{
    // Background
    background_texture_.set_filepath("images/ui/ui_background.png");
    background_texture_.set_blend(true);
    background_texture_.set_blend_alpha(200); // Semi-transparent
    background_texture_.init(scene_state_);
}

void PauseMenuScene::update(double delta)
{
    scene_state_.io_handler = io_handler_;
    scene_state_.delta = delta;

    // Update scene graph - UIButton nodes will handle their own state
    root_.update(scene_state_);
    
    // Check for Escape key to close the pause menu
    const GameActionList &actions = io_handler_->get_game_actions();
    for (uint8_t i = 0; i < actions.num_actions; i++) 
    {
        if (actions.actions[i] == GameAction::TOGGLE_PAUSE) 
        {
            // Pop the pause menu to return to the main scene
            SceneManager::get_instance()->pop_scene();
            break;
        }
    }
}

void PauseMenuScene::destroy()
{
    root_.destroy();
    
    // Destroy background texture
    background_texture_.destroy();
}

void PauseMenuScene::render()
{
    scene_state_.reset();
    scene_state_.sdl_info = sdl_info_;
    scene_state_.io_handler = io_handler_;

    root_.draw(scene_state_);
}

void PauseMenuScene::serialize(Serializer& serializer) const
{
    // No need to serialize the pause menu state
}

void PauseMenuScene::deserialize(Serializer& serializer)
{
    // No need to deserialize the pause menu state
}

// Called when the pause menu becomes active
void PauseMenuScene::on_enter()
{
    // Make sure theme music continues playing
    bool music_enabled = ConfigManager::get_instance().get_music_enabled();
    if (!music_enabled) 
    {
        AudioEngine::get_instance()->get_channel(3)->setMute(true);
    } 
    else 
    {
        AudioEngine::get_instance()->get_channel(3)->setMute(false);
    }
}

} // namespace cge
