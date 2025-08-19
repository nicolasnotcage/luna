/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/core.hpp"
#include "platform/event.hpp"
#include "system/file_locator.hpp"
#include "system/preprocessor.h"

#include "chess_game/main_scene.hpp"
#include "menus/main_menu.hpp"
#include "menus/pause_menu.hpp"
#include "menus/game_over_menu.hpp"

#include "platform/game_manager.hpp"
#include "platform/game_action.hpp"
#include "platform/io_handler.hpp"
#include "platform/time_manager.hpp"
#include "platform/scene_manager.hpp"

#include "system/config_manager.hpp"
#include "system/save_manager.hpp"

#include "fmod/fmod.hpp"
#include "platform/audio_engine.hpp"
#include "bitboard.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <string>

int main(int argc, char *argv[])
{
    // Configure system paths and initialize SDL
    auto source_path = STD_STRING(SRC_DIR);
    auto resource_path = STD_STRING(RESOURCE_DIR);
    
    // Set system paths
    cge::set_system_paths(argv[0], resource_path, source_path);
    
    cge::init_sdl();

    // Initialize config manager first to get screen size and other settings
    cge::ConfigManager& config_manager = cge::ConfigManager::get_instance();
    if (config_manager.init("config.txt")) 
    {
        // std::cout << "Config loaded successfully\n";    
    }

    // Parse command line arguments for engine path
    std::string engine_path_override;
    for (int i = 1; i < argc; i++) 
    {
        std::string arg = argv[i];
        if (arg == "--engine" && i + 1 < argc) 
        {
            engine_path_override = argv[i + 1];
            i++; // Skip the next argument since we used it as the engine path
        }
        else if (arg.find("--engine=") == 0) 
        {
            engine_path_override = arg.substr(9); // Remove "--engine=" prefix
        }
        else if (arg == "--help" || arg == "-h") 
        {
            std::cout << "Chess Lab - Usage:\n";
            std::cout << "  " << argv[0] << " [options]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --engine <path>    Specify path to chess engine executable\n";
            std::cout << "  --engine=<path>    Alternative syntax for engine path\n";
            std::cout << "  --help, -h         Show this help message\n\n";
            std::cout << "Example:\n";
            std::cout << "  " << argv[0] << " --engine ./engines/stockfish.exe\n";
            return 0;
        }
    }

    // If engine path was provided via command line, update the config
    if (!engine_path_override.empty()) 
    {
        config_manager.set_engine_path(engine_path_override);
        std::cout << "Using engine: " << engine_path_override << std::endl;
    }

    // Generate default SDLInfo struct (contains pointers to an SDL renderer and SDL window) 
    cge::SDLInfo sdl_info;

    // Create and configure components of SDL instance using config values
    cge::create_sdl_components(sdl_info, 
                               config_manager.get_screen_width(),
                               config_manager.get_screen_height(),
                               "Chess Lab");

    // Create io and time handler instances
    cge::IoHandler    io_handler = cge::IoHandler();
    cge::TimeManager *time_manager = cge::TimeManager::get_instance();

    // Initialize the scene manager
    cge::SceneManager *scene_manager = cge::SceneManager::get_instance();
    scene_manager->init(&sdl_info, &io_handler);

    // Register available scenes
    scene_manager->register_scene<cge::MainMenuScene>("main_menu");
    scene_manager->register_scene<cge::MainScene>("main_scene");
    scene_manager->register_scene<cge::PauseMenuScene>("pause_menu");
    scene_manager->register_scene<cge::GameOverMenuScene>("game_over_menu");

    // Initialize save manager
    cge::SaveManager &save_manager = cge::SaveManager::get_instance();
    if (save_manager.init("save.dat")) 
    {
        // std::cout << "Save manager initialized\n";
    }
    
    // Initialize audio engine
    cge::AudioEngine *audio_engine = cge::AudioEngine::get_instance();
    if (!audio_engine->init(32, true))
    {
        std::cerr << "Failed to initialize AudioEngine...\n";
        return 1;
    }
    
    // Push the initial scene (main menu)
    scene_manager->push_scene_by_key("main_menu");

    // Get instance of game manager class
    auto game_manager = cge::GameManager::get_instance();

    // Main game loop
    bool run_game = true;
    while (run_game)
    {
        // Run game loop with scene manager
        game_manager->run_game_loop(*scene_manager, io_handler);

        // Check for game actions
        const cge::GameActionList &actions = io_handler.get_game_actions();
        for (uint8_t i = 0; i < actions.num_actions; i++)
        {
            // Only quit if the QUIT action is detected
            if (actions.actions[i] == cge::GameAction::QUIT)
            {
                run_game = false;
                break;
            }
        }
    }

    // Get all scenes from the stack
    std::vector<cge::Scene*> scenes;
    scene_manager->get_all_scenes(scenes);

    // Save all scenes at once
    if (!scenes.empty()) 
    {
        save_manager.save_game_state(scenes);
    }
    
    // Cleanup after game loop
    scene_manager->clear_all_scenes();
    
    // Shutdown audio engine
    audio_engine->shutdown();
    
    cge::destroy_sdl_components(sdl_info);
    return 0;
}
