#ifndef SERIALIZATION_CONFIG_MANAGER_HPP
#define SERIALIZATION_CONFIG_MANAGER_HPP


#include "system/serializer.hpp"
#include <string>
#include <unordered_map>

namespace cge
{

class TextSerializer;

// A singleton class for managing game configuration. 
class ConfigManager
{
public:
    static ConfigManager& get_instance();

    // Initialize with path to config file
    bool init(const std::string& config_filepath);

    // Save current config
    bool save();

    // Load config from file
    bool load();

    // Create default config file it it doesn't exist
    bool create_default_config();
    
    // Screen size configuration
    int get_screen_width() const;
    void set_screen_width(int width);
    
    int get_screen_height() const;
    void set_screen_height(int height);

    // Game music configuration
    bool get_music_enabled() const;
    void set_music_enabled(bool enabled);

    // Chess engine configuration
    std::string get_engine_path() const;
    void set_engine_path(const std::string& path);

private:
    ConfigManager() = default;
    ~ConfigManager();

    TextSerializer serializer_;
    std::string config_filepath_;
    bool is_loaded_ = false;

    // Default config values
    int screen_width_{800};
    int screen_height_{600};
    bool music_enabled_{true};
    std::string engine_path_{"luna.exe"};
};
} // namespace cge

#endif // SERIALIZATION_CONFIG_MANAGER_HPP
