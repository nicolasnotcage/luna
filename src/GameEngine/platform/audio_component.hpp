#ifndef PLATFORM_AUDIO_COMPONENT_HPP
#define PLATFORM_AUDIO_COMPONENT_HPP

#include "platform/math.hpp"
#include <string>

namespace cge
{

class TransformNode;

// A component class representing a singular instance of audio
// that is attached to a game object. 
class AudioComponent
{
public:
    AudioComponent(TransformNode *owner);
    ~AudioComponent();

    // Sets the associated sound and whether that sound is 3D. 
    void set_sound(const std::string &sound_key);

    // Play the audio clip. Returns the channel ID of the channel on 
    // which the clip plays.
    int  play(float volume = 1.0f);

    // Stop the audio clip.
    void stop();

    // Pause the audio clip.
    void pause();

    // Resume the audio clip.
    void resume();

    // Set the volume of the audio clip.
    void set_volume(float volume);

    // Set the pitch of the audio clip.
    void set_pitch(float pitch);

    // Set whether the audio clip should loop.
    void set_loop(bool loop);

    // Sets the minimum distance at which the sound is heard at full volume.
    void set_min_distance(float min_distance);

    // Sets the maximum distance beyond which the sound is no longer audible.
    void set_max_distance(float max_distance);

    // Update the 3D attributes of the current channel to track position. 
    void update_position();

    // DSP
    void set_echo(bool enabled, float delay_ms = 300.0f, float feedback = 30.0f);

    // State
    bool is_playing() const;
    TransformNode *get_owner() const { return owner_; }

private:
    TransformNode *owner_;                  // The owner TransformNode
    std::string    sound_key_;              // Key associated with the sound
    int            channel_id_{-1};         // Channel assigned to the sound
    float          volume_{1.0f};           // Sound playback volume    
    float          pitch_{1.0f};            // Pitch of the sound
    bool           loop_{false};            // Whether or not the sound should loop
    float          min_distance_{1.0f};     // Minimum distance at which the sound is heard at full volume.
    float          max_distance_{10.0f};    // Maximum distance beyond which the sound is no longer audible.
    bool           is_3d_{false};           // Whether or not the sound is 3D
    bool           has_echo_{false};        // Whether or not the sound should echo
    float          echo_delay_{0.0f};       // Echo delay time
    float          echo_feedback_{0.0f};    // Echo feedback amount
};

} // namespace cge

#endif // PLATFORM_AUDIO_COMPONENT_HPP
