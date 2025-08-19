/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/audio_manager.hpp"
#include "platform/audio_component.hpp"
#include "system/file_locator.hpp"
#include "system/config_manager.hpp"

namespace cge
{

AudioManager::AudioManager() {}

void AudioManager::init(SceneState& scene_state)
{
    scene_state_ = &scene_state;
    
    // Load sounds
    load_sounds();
}

// Load sounds specific to the main scene. Theme music is initialized and played at the menu
// so is not included here.
void AudioManager::load_sounds()
{
    AudioEngine* audio_engine = AudioEngine::get_instance();
    
    // Mute theme music if configuration set
    bool music_enabled = ConfigManager::get_instance().get_music_enabled();
    if (!music_enabled) 
    {
        // Note: This assumes theme music is playing on channel 3
        // This is fragile and should be refactored to track the music channel properly
        FMOD::Channel* music_channel = audio_engine->get_channel(3);
        if (music_channel) music_channel->setMute(true);
    }
}

// Load chess-specific sounds
void AudioManager::load_chess_sounds()
{
    AudioEngine* audio_engine = AudioEngine::get_instance();
    
    // Locate sound files
    auto move_sound_info = locate_path_for_filename("audio/move.wav");
    auto take_sound_info = locate_path_for_filename("audio/take.wav");
    auto illegal_sound_info = locate_path_for_filename("audio/illegal.wav");
    auto check_sound_info = locate_path_for_filename("audio/check.wav");
    auto win_sound_info = locate_path_for_filename("audio/win.wav");
    auto draw_sound_info = locate_path_for_filename("audio/draw.wav");
    auto loss_sound_info = locate_path_for_filename("audio/loss.wav");
    
    // Load sounds if not already loaded
    if (!audio_engine->get_sound("move_sound")) 
    {
        audio_engine->load_sound(move_sound_info.path, "move_sound", false, false);
    }
    
    if (!audio_engine->get_sound("take_sound")) 
    {
        audio_engine->load_sound(take_sound_info.path, "take_sound", false, false);
    }
    
    if (!audio_engine->get_sound("illegal_sound")) 
    {
        audio_engine->load_sound(illegal_sound_info.path, "illegal_sound", false, false);
    }
    
    if (!audio_engine->get_sound("check_sound")) 
    {
        audio_engine->load_sound(check_sound_info.path, "check_sound", false, false);
    }
    
    if (!audio_engine->get_sound("win_sound")) 
    {
        audio_engine->load_sound(win_sound_info.path, "win_sound", false, false);
    }
    
    if (!audio_engine->get_sound("draw_sound")) 
    {
        audio_engine->load_sound(draw_sound_info.path, "draw_sound", false, false);
    }
    
    if (!audio_engine->get_sound("loss_sound")) 
    {
        audio_engine->load_sound(loss_sound_info.path, "loss_sound", false, false);
    }
}

void AudioManager::play_sound(const std::string& sound_name, float volume)
{
    AudioEngine::get_instance()->play_sound(sound_name, volume);
}

// Chess-specific sound methods
void AudioManager::play_move_sound(float volume)
{
    play_sound("move_sound", volume);
}

void AudioManager::play_take_sound(float volume)
{
    play_sound("take_sound", volume);
}

void AudioManager::play_illegal_sound(float volume)
{
    play_sound("illegal_sound", volume);
}

void AudioManager::play_check_sound(float volume)
{
    play_sound("check_sound", volume);
}

void AudioManager::play_win_sound(float volume)
{
    play_sound("win_sound", volume);
}

void AudioManager::play_draw_sound(float volume)
{
    play_sound("draw_sound", volume);
}

void AudioManager::play_loss_sound(float volume)
{
    play_sound("loss_sound", volume);
}

void AudioManager::toggle_music()
{
    AudioEngine::get_instance()->toggle_music();
}

void AudioManager::update()
{
    AudioEngine::get_instance()->update();
}

std::string AudioManager::locate_audio_file(const std::string& filename)
{
    auto file_info = locate_path_for_filename("audio/" + filename);
    return file_info.path;
}

} // namespace cge
