#include "platform/animation.hpp"

#include <cstdint>

namespace cge
{

 /**********************************************************************
 * Animation Class Implementation
 **********************************************************************/
Animation::Animation() : name_(""), looping_(false) {}

Animation::Animation(const std::string &name, bool looping) 
	: name_(name), looping_(looping) 
	{}

void Animation::add_frame(uint32_t frame_id, uint32_t duration) 
{
    frames_.push_back({frame_id, duration});
}

const std::string &Animation::get_name() const { return name_; }

void Animation::set_looping(bool looping) { looping_ = looping; }

bool Animation::is_looping() const { return looping_; }

const std::vector<cge::Animation::AnimationFrame> &Animation::get_frames() const { return frames_; }

/**********************************************************************
* Animator Class Implementation
**********************************************************************/
Animator::Animator() = default;

// Update Animator state for the current Animation.
void Animator::update(float delta)
{
    if(!is_playing_ || current_animation_.empty()) return;

    // Get current animation if it exists
    auto it = animations.find(current_animation_);
    if(it == animations.end()) return;

    // Get current animation frames
    const Animation &current_anim = it->second;
    const std::vector<Animation::AnimationFrame> &frames = current_anim.get_frames();
    if(frames.empty()) return;

    // Accumulate time
    time_accumulator_ += delta * playback_speed_;

    // Get duration of current frame
    uint32_t current_duration = frames[current_frame_index_].duration;

    // Convert from seconds to animation ticks
    float time_per_tick = 1.0f / ticks_per_second_;

    while(time_accumulator_ >= current_duration * time_per_tick)
    {
        // Subtract the time for this frame
        time_accumulator_ -= current_duration * time_per_tick;

        // Move to next frame
        current_frame_index_++;

        // If animation is complete, loop or stop depending on looping state.
        if(current_frame_index_ >= frames.size())
        {
            if(current_anim.is_looping())
            {
                current_frame_index_ = 0;
            }
            else
            {
                current_frame_index_ = frames.size() - 1;
                is_playing_ = false;
                break;
            }
        }

        // Update duration for the new frame
        current_duration = frames[current_frame_index_].duration;
    }
}

// Play animation.
void Animator::play(const std::string &animation_name)
{
    // If already playing this animation, do nothing
    if(is_playing_ && current_animation_ == animation_name) return; 

    // Make sure animation exists
    auto it = animations.find(animation_name);
    if(it == animations.end()) return;

    // Initilize animation
    current_animation_ = animation_name;
    current_frame_index_ = 0;
    time_accumulator_ = 0.0f;
    is_playing_ = true;
}

// Pause animation.
void Animator::pause() { is_playing_ = false; }

// Resume animation.
void Animator::resume() { is_playing_ = true; }

// Reset animation.
void Animator::reset()
{
    current_frame_index_ = 0;
    time_accumulator_ = 0.0f;
}

// Add animation.
void Animator::add_animation(const Animation &animation)
{
    animations[animation.get_name()] = animation;
}

// Get current Frame ID.
uint32_t Animator::get_current_frame_id() const
{
    if(current_animation_.empty()) return 0;

    auto it = animations.find(current_animation_);
    if(it == animations.end() || it->second.get_frames().empty()) return 0;

    const std::vector<Animation::AnimationFrame> &frames = it->second.get_frames();

    if(current_frame_index_ >= frames.size()) return 0;

    return frames[current_frame_index_].frame_id;
}

// Set animation playback speed.
void Animator::set_playback_speed(float speed) { playback_speed_ = speed; }

// Set animation looping status.
void Animator::set_looping(bool looping)
{
    if(current_animation_.empty()) return;

    auto it = animations.find(current_animation_);
    if(it != animations.end()) it->second.set_looping(looping);
}

bool Animator::is_playing() const { return is_playing_; }

const std::string &Animator::get_current_animation_name() const { return current_animation_; }

void Animator::set_ticks_per_second(float ticks_per_second) { ticks_per_second_ = ticks_per_second; }


} // namespace cge

