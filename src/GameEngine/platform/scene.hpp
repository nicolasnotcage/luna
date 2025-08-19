/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef PLATFORM_SCENE_HPP
#define PLATFORM_SCENE_HPP

#include "platform/io_handler.hpp"
#include "platform/core.hpp"
#include "system/serializable.hpp"

namespace cge
{

// Base class for all scenes. Scenes are serializable. 
class Scene : public Serializable
{
public:
    Scene() = default;
    virtual ~Scene() = default;

    virtual void init(SDLInfo* sdl_info, IoHandler* io_handler) = 0;
    virtual void destroy() = 0;
    virtual void render() = 0;
    virtual void update(double delta) = 0;

    // Called when scene becomes active (top of stack)
    virtual void on_enter() {}

    // Called when scene no longer active (not on top of stack)
    virtual void on_exit() {}

    // Called when scene is paused (covered by another scene)
    virtual void on_pause() {}

    // Called when scene is resumed (uncovered)
    virtual void on_resume() {}

    // Serializable overrides
    virtual void serialize(Serializer& serializer) const override = 0;
    virtual void deserialize(Serializer& serializer) override = 0;
};

} // namespace cge

#endif // PLATFORM_SCENE_HPP
