#ifndef SERIALIZATION_SAVE_MANAGER_HPP
#define SERIALIZATION_SAVE_MANAGER_HPP

#include "system/serializer.hpp"

#include <string>
#include <vector>

namespace cge
{

class Scene;

class SaveManager
{
public:
    static SaveManager& get_instance();

    // Initialize with path to save file
    bool init(const std::string& save_filepath);

    // Save game state for a single scene
    bool save_game(const Scene* scene);

    // Load game state into a single scene
    bool load_game(Scene* scene);

    // Save game state for multiple scenes
    bool save_game_state(const std::vector<Scene*>& scenes);

    // Load game state into multiple scenes
    bool load_game_state(std::vector<Scene*>& scenes);

    // Check if save file exists
    bool save_exists() const;

private:
    SaveManager() = default;
    ~SaveManager();

    BinarySerializer serializer_;
    std::string save_filepath_;
    bool is_loaded_ = false;
};
} // namespace cge

#endif // SERIALIZATION_SAVE_MANAGER_HPP
