/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/audio_engine.hpp"
#include "system/config_manager.hpp"
#include <iostream>

namespace cge
{

AudioEngine* AudioEngine::get_instance()
{ 
    static AudioEngine instance;
    return &instance;
}

AudioEngine::AudioEngine() = default;
AudioEngine::~AudioEngine() { this->shutdown(); }

// Initialize the FMOD system. Returns true if successful, false otherwise. 
bool AudioEngine::init(int max_channels, bool use_3d)
{
    // Create FMOD system
    FMOD_RESULT result = FMOD::System_Create(&fmod_system_);
    if (result != FMOD_OK) 
    { 
        std::cerr << "Failed to create FMOD system...\n";
        return false;
    }

    // Get version
    unsigned int version;
    fmod_system_->getVersion(&version);

    // Setup
    num_channels_ = max_channels;
    
    // Set the maximum number of software channels
    result = fmod_system_->setSoftwareChannels(num_channels_);
    if(result != FMOD_OK)
    {
        std::cerr << "Failed to set software channels: " << result << std::endl;
    }
    
    channels_.resize(num_channels_, nullptr);

    // Initialize with flags
    FMOD_INITFLAGS flags = FMOD_INIT_NORMAL;
    if (use_3d) flags |= FMOD_INIT_3D_RIGHTHANDED;

    // Initialize system
    result = fmod_system_->init(num_channels_, flags, nullptr);
    if(result != FMOD_OK)
    {
        std::cerr << "Failed to initialize FMOD system: " << result << std::endl;
        return false;
    }

    return true;
}

// Shutdown the FMOD system. 
void AudioEngine::shutdown()
{
    // Release all sounds
    for(auto &[key, sound] : sound_map_)
    {
        if(sound) sound->release();
    }

    sound_map_.clear();

    // Release FMOD system
    if(fmod_system_)
    {
        fmod_system_->close();
        fmod_system_->release();
        fmod_system_ = nullptr;
    }

    channels_.clear();
}

// Load a sound into the FMOD system.  
bool AudioEngine::load_sound(const std::string &path,
                             const std::string &key,
                             bool               is_3d,
                             bool               looping)
{
    // Skip if already loaded
    if(sound_map_.find(key) != sound_map_.end()) return true;

    // Configure mode
    FMOD_MODE mode = is_3d ? FMOD_3D : FMOD_2D;
    if(looping) { mode |= FMOD_LOOP_NORMAL; }
    else { mode |= FMOD_LOOP_OFF; }

    // Load sound
    FMOD::Sound *sound = nullptr;
    FMOD_RESULT  result = fmod_system_->createSound(path.c_str(), mode, nullptr, &sound);

    if(result != FMOD_OK)
    {
        std::cerr << "Failed to load sound: " << path << " Error: " << result << std::endl;
        return false;
    }

    // Store sound in map
    sound_map_[key] = sound;
    return true;
}

// Unload a sound from the sound map by key. 
void AudioEngine::unload_sound(const std::string &key)
{
    auto it = sound_map_.find(key);
    if(it != sound_map_.end())
    {
        if(it->second) it->second->release();
        sound_map_.erase(it);
    }
    else
    {
        std::cerr << "Key not found when unloading sound: " << key << "\n";
    }
}

// Get sound object by key. 
FMOD::Sound* AudioEngine::get_sound(const std::string &key)
{
    auto it = sound_map_.find(key);
    if(it != sound_map_.end()) { return it->second; }
    return nullptr;
}

/*
* Play sound by key and return the channel ID. Takes a Boolean argument representing whether the sound 
* should be played immediately or paused. This allows the caller to determine when the audio is played
* using the returned channel ID. This is useful when callers intend to configure DSP effects before playing.
*/
int AudioEngine::play_sound(const std::string &key, float volume, bool pause)
{
    FMOD::Sound *sound = this->get_sound(key);
    if(!sound) 
    {
        std::cerr << "Sound not found: " << key << std::endl;
        return -1;
    }

    // Let FMOD allocate a channel automatically
    FMOD::Channel* channel = nullptr;
    
    // Check if this is a 3D sound
    FMOD_MODE mode;
    sound->getMode(&mode);
    bool is_3d = (mode & FMOD_3D) != 0;

    // Play the sound
    FMOD_RESULT result = fmod_system_->playSound(sound, nullptr, pause, &channel);
    if(result != FMOD_OK) 
    {
        std::cerr << "Failed to play sound: " << key << " Error: " << result << std::endl;
        
        // If we're out of channels, force update and try again
        if(result == FMOD_ERR_CHANNEL_ALLOC)
        {
            fmod_system_->update();
            result = fmod_system_->playSound(sound, nullptr, pause, &channel);
            if(result != FMOD_OK)
            {
                std::cerr << "Still failed after update. Error: " << result << std::endl;
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }
    
    if(!channel)
    {
        std::cerr << "Channel is null for sound: " << key << std::endl;
        return -1;
    }

    // Set volume
    result = channel->setVolume(volume);
    if(result != FMOD_OK)
    {
        std::cerr << "Failed to set volume for sound: " << key << std::endl;
    }

    // For 3D sounds, we ensure that 3D attributes are configured.
    if (is_3d) 
    {
        // Set default 3D attributes
        FMOD_VECTOR pos = {0.0f, 0.0f, 0.0f};
        FMOD_VECTOR vel = {0.0f, 0.0f, 0.0f};
        channel->set3DAttributes(&pos, &vel);
    }

    // For effects support, we need to track the channel
    // Find an available slot
    for(int i = 0; i < num_channels_; i++)
    {
        // Don't check if old channels are playing - just overwrite
        // This avoids issues with invalid channel pointers
        if(channels_[i] == nullptr || i == next_channel_slot_)
        {
            channels_[i] = channel;
            next_channel_slot_ = (i + 1) % num_channels_;
            return i;
        }
    }
    
    // Fallback: use round-robin
    int slot = next_channel_slot_;
    channels_[slot] = channel;
    next_channel_slot_ = (next_channel_slot_ + 1) % num_channels_;
    return slot;
}

// Creates echo effect and adds it to the passed channel. 
void AudioEngine::add_echo(int channel_id, float delay_ms, float feedback)
{
    if(channel_id < 0 || channel_id >= num_channels_ || !channels_[channel_id]) return;

    // Create effect
    FMOD::DSP *dsp = nullptr;
    fmod_system_->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp);

    // Set parameters
    dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY, delay_ms);
    dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, feedback);

    // Add to channel
    channels_[channel_id]->addDSP(0, dsp);
}

// Update the FMOD system.
void AudioEngine::update()
{
    if(fmod_system_) 
    { 
        FMOD_RESULT result = fmod_system_->update();
        if(result != FMOD_OK)
        {
            std::cerr << "FMOD update failed with error: " << result << std::endl;
        }
    }
}

// Returns the channel object associated with a channel id.
FMOD::Channel* AudioEngine::get_channel(int channel_id)
{
    if(channel_id >= 0 && channel_id < channels_.size()) 
    {
        // Don't validate the channel - just return it
        // This avoids issues with checking invalid pointers
        return channels_[channel_id];
    }
    return nullptr;
}

/*
* Set the position of the 3D listener. Takes a Vector2 position and 
* converts it to an FMOD_VECTOR for configuration.
*/
void AudioEngine::set_3d_listener_position(const Vector2& position)
{
    if(!fmod_system_) return;

    // Convert to FMOD vector
    FMOD_VECTOR pos = {position.x, position.y, 0.0f};
    
    // Set up direction vector
    FMOD_VECTOR forward = {0.0f, 1.0f, 0.0f};
    FMOD_VECTOR up = {0.0f, 0.0f, 1.0f};

    // Velocity = zero (not necessary for our engine)
    FMOD_VECTOR velocity = {0.0f, 0.0f, 0.0f};

    // Update listener position
    FMOD_RESULT result = fmod_system_->set3DListenerAttributes(0, &pos, &velocity, &forward, &up);
    if (result != FMOD_OK) 
    {
        std::cerr << "Failed to set 3D listener attributes. Error: " << result << std::endl;
    }
    
    // Call update immediately after changing listener attributes
    fmod_system_->update();
}

// For user configuration. This probably shouldn't be in the engine itself because it's 
// specific to theme music, but it works fine.
void AudioEngine::toggle_music()
{
    bool music_enabled = ConfigManager::get_instance().get_music_enabled();

    // Toggle the music enabled flag in config
    ConfigManager::get_instance().set_music_enabled(!music_enabled);

    // Note: This implementation assumes theme music is managed elsewhere
    // The channel-based approach is fragile and should be refactored
}

// Debug helper to check channel usage
void AudioEngine::debug_print_channel_info()
{
    if(!fmod_system_) return;
    
    int channels_playing = 0;
    fmod_system_->getChannelsPlaying(&channels_playing);
    
    std::cout << "FMOD Debug Info:" << std::endl;
    std::cout << "  Channels playing: " << channels_playing << "/" << num_channels_ << std::endl;
    
    // Check current sound map
    std::cout << "  Loaded sounds: " << sound_map_.size() << std::endl;
    for(const auto& [key, sound] : sound_map_)
    {
        if(sound)
        {
            std::cout << "    - " << key << std::endl;
        }
    }
}

} // namespace cge