/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/scene_manager.hpp"

namespace cge
{

SceneManager* SceneManager::get_instance()
{
    static SceneManager instance;
    return &instance;
}

SceneManager::~SceneManager()
{
    clear_all_scenes();
}

Scene* SceneManager::create_scene_by_key(const std::string& key)
{
    auto it = scene_factories_.find(key);
    if (it != scene_factories_.end())
    {
        return it->second();
    }
    return nullptr;
}

bool SceneManager::push_scene_by_key(const std::string& key)
{
    Scene* scene = create_scene_by_key(key);
    if (scene)
    {
        push_scene(scene);
        return true;
    }

    return false;
}

void SceneManager::get_all_scenes(std::vector<Scene*>& scenes)
{
    scenes = scene_stack_;
}

void SceneManager::init(SDLInfo* sdl_info, IoHandler* io_handler)
{
    sdl_info_ = sdl_info;
    io_handler_ = io_handler;
}

void SceneManager::push_scene(Scene* scene)
{
    // Pause the current scene if there is one
    if (!scene_stack_.empty())
    {
        scene_stack_.back()->on_pause();
    }

    // Initialize and push the new scene
    scene->init(sdl_info_, io_handler_);
    scene->on_enter();
    scene_stack_.push_back(scene);
}

bool SceneManager::pop_scene()
{
    if (scene_stack_.empty())
    {
        return false;
    }

    // Clean up the current scene
    scene_stack_.back()->on_exit();
    scene_stack_.pop_back();

    // Resume the new top scene if there is one
    if (!scene_stack_.empty())
    {
        scene_stack_.back()->on_resume();
    }

    return true;
}

bool SceneManager::replace_scene(Scene* scene)
{
    if (scene_stack_.empty())
    {
        push_scene(scene);
        return false;
    }

    // Clean up the current scene
    scene_stack_.back()->on_exit();
    scene_stack_.pop_back();

    // Initialize and push the new scene
    scene->init(sdl_info_, io_handler_);
    scene->on_enter();
    scene_stack_.push_back(scene);

    return true;
}

Scene* SceneManager::get_current_scene()
{
    if (scene_stack_.empty())
    {
        return nullptr;
    }
    return scene_stack_.back();
}

void SceneManager::update(double delta)
{
    if (!scene_stack_.empty())
    {
        scene_stack_.back()->update(delta);
    }
}

void SceneManager::render()
{
    if (!scene_stack_.empty())
    {
        scene_stack_.back()->render();
    }
}

void SceneManager::clear_all_scenes()
{
    while (!scene_stack_.empty())
    {
        scene_stack_.back()->on_exit();
        scene_stack_.back()->destroy();
        scene_stack_.pop_back();
    }
}

} // namespace cge