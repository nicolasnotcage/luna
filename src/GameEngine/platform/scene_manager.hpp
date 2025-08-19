/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef PLATFORM_SCENE_MANAGER_HPP
#define PLATFORM_SCENE_MANAGER_HPP

#include "platform/scene.hpp"
#include "platform/io_handler.hpp"
#include "platform/core.hpp"

#include <vector>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <functional>

namespace cge
{

/**
* Singleton class to manage a stack of Scene objects. The SceneManager 
* maintains a stack of scenes and handles transitions between them. Only 
* the top scene is active at any given time.
*/
class SceneManager
{
public:
    // Get the singleton instance of SceneManager. Returns a pointer
    // to the singleton instance.
    static SceneManager* get_instance();

    // Delete copy and move constructor/assignment operators
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager(SceneManager&&) = delete;
    SceneManager& operator=(SceneManager&&) = delete;

    // Register a scene factory function with a key.
    template<typename T>
    void register_scene(const std::string& key)
    {
        scene_factories_[key] = []() { return new T(); };
    }

    // Create a scene by key. Takes a key identifier for 
    // the scene and returns a pointer to the created scene
    // or nullptr if key not found.
    Scene* create_scene_by_key(const std::string& key);

    // Create and push a scene by key. Takes a key 
    // identifier for the scene. Returns true if the 
    // scene is pushed successfully, false otherwise.
    bool push_scene_by_key(const std::string& key);

    // Get all scenes in the stack. 
    void get_all_scenes(std::vector<Scene*>& scenes);

    // Initialize scene manager with SDL info and IO handler.
    void init(SDLInfo* sdl_info, IoHandler* io_handler);

    // Push scene to the stack and make it active. 
    void push_scene(Scene* scene);

    // Pop top scene from the stack. Returns true 
    // if a scene was popped, false if stack was 
    // empty.
    bool pop_scene();

    // Replace top scene with a new scene. Returns 
    // true if a scene was replaced, false if the 
    // stack was empty and scene was pushed instead.
    bool replace_scene(Scene* scene);

    // Get a pointer to the current active scene, or 
    // nullptr if stack is empty.
    Scene* get_current_scene();

    // Update the current scene.
    void update(double delta);

    // Render the current scene.
    void render();

    // Clean up all scenes and clear the stack.
    void clear_all_scenes();

private:
    SceneManager() = default;
    ~SceneManager();

    std::vector<Scene*> scene_stack_;              // Stack of scene pointers with the active scene at the top
    SDLInfo* sdl_info_ = nullptr;                  // Pointer to SDL info
    IoHandler* io_handler_ = nullptr;              // Pointer to IO handler for input events
    std::unordered_map<std::string, std::function<Scene* ()>> scene_factories_;  // Map of scene keys to factory functions
};

} // namespace cge

#endif // PLATFORM_SCENE_MANAGER_HPP