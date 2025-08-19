/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

/*
* This header defines an AudioManager class that abstracts audio handling.
* It manages sound loading, playback, and game-specific audio events.
*/

#ifndef PLATFORM_AUDIO_MANAGER_HPP
#define PLATFORM_AUDIO_MANAGER_HPP

#include "platform/audio_engine.hpp"
#include "graph/scene_state.hpp"
#include "system/file_locator.hpp"

#include <string>
#include <memory>
#include <vector>

namespace cge
{

class AudioManager
{
public:
    AudioManager();
    ~AudioManager() = default;
    
    void init(SceneState& scene_state);
    
    // Load and configure sounds
    void load_sounds();
    
    // Load chess-specific sounds
    void load_chess_sounds();
    
    // Generic sound playback
    void play_sound(const std::string& sound_name, float volume = 1.0f);
    
    // Chess-specific sound methods
    void play_move_sound(float volume = 1.0f);
    void play_take_sound(float volume = 1.0f);
    void play_illegal_sound(float volume = 1.0f);
    void play_check_sound(float volume = 1.0f);
    void play_win_sound(float volume = 1.0f);
    void play_draw_sound(float volume = 1.0f);
    void play_loss_sound(float volume = 1.0f);
    
    // Toggle music
    void toggle_music();
    
    // Update audio engine (should be called every frame)
    void update();
    
private:
    // Helper method to locate audio files
    std::string locate_audio_file(const std::string& filename);
    
    // Scene state reference
    SceneState* scene_state_{nullptr};
};

} // namespace cge

#endif // PLATFORM_AUDIO_MANAGER_HPP
