/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "menus/game_over_menu.hpp"
#include "platform/math.hpp"
#include "platform/scene_manager.hpp"
#include "platform/audio_engine.hpp"
#include "system/config_manager.hpp"
#include "system/save_manager.hpp"
#include "system/file_locator.hpp"

namespace cge
{

void GameOverMenuScene::init(SDLInfo* sdl_info, IoHandler* io_handler)
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
    
    // Disable zooming in game over menu
    camera.set_zoom_enabled(false);

    // Get references to components
    auto &menu_background_transform = camera.get_child<0>();
    auto &menu_background_sprite = menu_background_transform.get_child<0>();
    
    // Get title reference
    auto &title_transform = camera.get_child<1>();
    auto &title_sprite = title_transform.get_child<0>();
    
    // Get button references
    auto &play_again_button = camera.get_child<2>();
    auto &main_menu_button = camera.get_child<3>();
    auto &exit_button = camera.get_child<4>();
    
    // Configure background and title
    menu_background_sprite.set_texture(&background_texture_);
    title_sprite.set_texture(&title_texture_);

    // Scale and position the background to fill the screen
    menu_background_transform.right_scale(camera_width, camera_height); // Match camera dimensions
    menu_background_transform.right_translate(0.0f, 0.0f); // Center in the camera view
    
    // Scale and position the title
    title_transform.right_scale(camera_width - 5.0f, 3.0f);
    title_transform.right_translate(0.0f, -1.2f);

    // Configure Play Again button
    auto &play_again_transform = play_again_button.get_child<0>();
    auto &play_again_sprite = play_again_transform.get_child<0>();
    
    // Set up the button with references to nodes in the graph
    play_again_button.set_transform_node(&play_again_transform);
    play_again_button.set_sprite_node(&play_again_sprite);
    play_again_button.set_camera_node(&camera);
    
    // Configure the play again button
    play_again_button.set_position(0.0f, -0.5f);
    play_again_button.set_size(6.0f, 2.0f);
    play_again_button.set_normal_sprite("images/ui/buttons/play_again/play_again_base_button.png");
    play_again_button.set_hover_sprite("images/ui/buttons/play_again/play_again_button_on_hover.png");
    play_again_button.set_pressed_sprite("images/ui/buttons/play_again/play_again_button_clicked.png");
    play_again_button.set_callback([this]() 
        {
        // Get all scenes
        std::vector<Scene*> scenes;
        SceneManager::get_instance()->get_all_scenes(scenes);
        
        // Pop the game over menu
        SceneManager::get_instance()->pop_scene();
        
        // Pop the main scene (if it exists)
        if (scenes.size() >= 2) 
        {
            SceneManager::get_instance()->pop_scene();
        }
        
        // Create and push a new main scene
        Scene* main_scene = SceneManager::get_instance()->create_scene_by_key("main_scene");
        SceneManager::get_instance()->push_scene(main_scene);
    });
    
    // Configure Main Menu button
    auto &main_menu_transform = main_menu_button.get_child<0>();
    auto &main_menu_sprite = main_menu_transform.get_child<0>();
    
    // Set up the button with references to nodes in the graph
    main_menu_button.set_transform_node(&main_menu_transform);
    main_menu_button.set_sprite_node(&main_menu_sprite);
    main_menu_button.set_camera_node(&camera);
    
    // Configure the button
    main_menu_button.set_position(0.0f, 2.0f);
    main_menu_button.set_size(6.0f, 2.0f);
    main_menu_button.set_normal_sprite("images/ui/buttons/main_menu/main_menu_base_button.png");
    main_menu_button.set_hover_sprite("images/ui/buttons/main_menu/main_menu_button_on_hover.png");
    main_menu_button.set_pressed_sprite("images/ui/buttons/main_menu/main_menu_button_clicked.png");
    main_menu_button.set_callback([this]() 
        {
        // Get all scenes
        std::vector<Scene*> scenes;
        SceneManager::get_instance()->get_all_scenes(scenes);
        
        // Save the main scene state if it exists
        if (scenes.size() >= 2) 
        {
            // The main scene should be the second-to-last scene in the stack
            Scene* main_scene = scenes[scenes.size() - 2];
            
            // Save the main scene state
            SaveManager::get_instance().save_game(main_scene);
        }
        
        // Pop the game over menu
        SceneManager::get_instance()->pop_scene();
        
        // Pop the main scene (if it exists)
        if (scenes.size() >= 2) 
        {
            SceneManager::get_instance()->pop_scene();
        }
        
        // Push a new main menu scene
        SceneManager::get_instance()->push_scene_by_key("main_menu");
    });
    
    // Configure Exit button
    auto &exit_transform = exit_button.get_child<0>();
    auto &exit_sprite = exit_transform.get_child<0>();
    
    // Set up the button with references to nodes in the graph
    exit_button.set_transform_node(&exit_transform);
    exit_button.set_sprite_node(&exit_sprite);
    exit_button.set_camera_node(&camera);
    
    // Configure the button
    exit_button.set_position(0.0f, 4.5f);
    exit_button.set_size(6.0f, 2.0f);
    exit_button.set_normal_sprite("images/ui/buttons/exit/exit_base_button.png");
    exit_button.set_hover_sprite("images/ui/buttons/exit/exit_button_on_hover.png");
    exit_button.set_pressed_sprite("images/ui/buttons/exit/exit_button_clicked.png");
    exit_button.set_callback([this]() 
        {
        // Get all scenes
        std::vector<Scene*> scenes;
        SceneManager::get_instance()->get_all_scenes(scenes);
        
        // Save the main scene state if it exists
        if (scenes.size() >= 2) 
        {
            // The main scene should be the second-to-last scene in the stack
            Scene* main_scene = scenes[scenes.size() - 2];
            
            // Save the main scene state
            SaveManager::get_instance().save_game(main_scene);
        }
        
        // Request quit
        SDL_Event quit_event;
        quit_event.type = SDL_EVENT_QUIT;
        SDL_PushEvent(&quit_event);
    });
   
    // Initialize root node
    root_.init(scene_state_);
}

void GameOverMenuScene::initialize_textures()
{
    // Background
    background_texture_.set_filepath("images/ui/ui_background.png");
    background_texture_.set_blend(true);
    background_texture_.set_blend_alpha(200);
    background_texture_.init(scene_state_);
    
    // Title
    title_texture_.set_filepath("images/ui/game_over_text.png");
    title_texture_.set_blend(true);
    title_texture_.set_blend_alpha(255); 
    title_texture_.init(scene_state_);
}

void GameOverMenuScene::update(double delta)
{
    scene_state_.io_handler = io_handler_;
    scene_state_.delta = delta;

    // Update scene graph - UIButton nodes will handle their own state
    root_.update(scene_state_);
}

void GameOverMenuScene::destroy()
{
    root_.destroy();
    
    // Destroy textures
    background_texture_.destroy();
    title_texture_.destroy();
}

void GameOverMenuScene::render()
{
    scene_state_.reset();
    scene_state_.sdl_info = sdl_info_;
    scene_state_.io_handler = io_handler_;

    root_.draw(scene_state_);
}

void GameOverMenuScene::serialize(Serializer& serializer) const
{
    // No need to serialize the game over menu state
}

void GameOverMenuScene::deserialize(Serializer& serializer)
{
    // No need to deserialize the game over menu state
}

// Called when the game over menu becomes active
void GameOverMenuScene::on_enter()
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
