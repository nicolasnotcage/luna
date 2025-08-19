/**
* This file contains definitions for both Animation and Animator classes, which 
* define the animation behavior used within the engine.
**/

#ifndef PLATFORM_ANIMATION
#define PLATFORM_ANIMATION

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

namespace cge
{

struct Frame
{
    int x, y;           // Position in sprite sheet
    int width, height;  // Dimensions
};

// Contains data specific to an animation (i.e., frames with timing information).
class Animation
{
public:
    struct AnimationFrame
    {
        uint32_t frame_id; // position in sprite sheet
        uint32_t duration; // in ticks
    };

    Animation();
    Animation(const std::string &name, bool looping);

    // Add animation frame
    void add_frame(uint32_t frame_id, uint32_t duration);

    // Get animation name
    const std::string &get_name() const;

    // Set and get looping state
    void set_looping(bool looping);
    bool is_looping() const;

    // Get animation frames
    const std::vector<AnimationFrame> &get_frames() const;
    

private:
    std::string name_;
    std::vector<AnimationFrame> frames_;
    bool looping_;
};

// Animator class controls playback of animations
class Animator
{
public:
    Animator();

    void update(float delta);
    void play(const std::string &animation_name);
    void pause();
    void resume();
    void reset();

    // Add animation to animator
    void add_animation(const Animation &animation);

    // Get the current frame id
    uint32_t get_current_frame_id() const;

    // Get the current animation name
    const std::string &get_current_animation_name() const;

    // Check animator play state
    bool is_playing() const;

    // Timeline control
    void set_playback_speed(float speed); 
    void set_looping(bool looping);
    void set_ticks_per_second(float ticks_per_second);

private:
    std::unordered_map<std::string, Animation> animations;
    std::string current_animation_{""};
    uint32_t current_frame_index_{0};
    float time_accumulator_{0.0f};
    bool is_playing_{false};
    float                                      playback_speed_{1.0f};
    float                                      ticks_per_second_{60.0f};
};

} // namespace cge

#endif // PLATFORM_ANIMATION
