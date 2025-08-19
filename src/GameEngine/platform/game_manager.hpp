#ifndef PLATFORM_GAME_MANAGER
#define PLATFORM_GAME_MANAGER

#include "platform/time_manager.hpp"
#include "platform/io_handler.hpp"
#include "platform/scene_manager.hpp"

namespace cge
{
// A singleton game manager class
class GameManager
{
public:
    // Compile-time constants
    static constexpr double NUM_UPDATES_PER_SECOND = 60.0;
    static constexpr double NUM_DRAWS_PER_SECOND = 60.0;
    static constexpr double UPDATE_INTERVAL = 1.0 / NUM_UPDATES_PER_SECOND;
    static constexpr double DRAW_INTERVAL = 1.0 / NUM_DRAWS_PER_SECOND;

    // Static function to get an instance of the game manager object
    static GameManager* get_instance()
    { 
        static GameManager instance;
        return &instance;
    }

    // Delete copy and move constructor/assignment operators
    GameManager(const GameManager &) = delete;
    GameManager &operator=(const GameManager &) = delete;
    GameManager(GameManager &&) = delete;
    GameManager &operator=(GameManager &&) = delete;

    // Run game loop using a SceneManager
    void run_game_loop(SceneManager& scene_manager, IoHandler& io_handler)
    {
        double current_time = time_manager_->get_current_time();
        double delta_time = current_time - last_time;

        // Force 60fps when delta time is too small
        if(delta_time < 0.001)
        {
            delta_time = 1.0 / 60.0; // 60fps equivalent
        }

        int times_updated = 0;
        while(current_time - last_update_time_ > UPDATE_INTERVAL && times_updated < 3)
        {
            // Update io_handler with each update loop
            io_handler.update();

            scene_manager.update(delta_time);
            last_update_time_ += UPDATE_INTERVAL;
            times_updated++;
        }

        if(times_updated == 3) last_update_time_ = current_time;

        if(current_time - last_draw_time_ > DRAW_INTERVAL)
        {
            scene_manager.render();
            last_draw_time_ = current_time;
        }

        last_time = current_time;
    }

private:
    GameManager()
    {
        time_manager_ = TimeManager::get_instance();
        last_update_time_ = time_manager_->get_current_time();
        last_draw_time_ = last_update_time_;
        last_time = last_update_time_;
    }

    ~GameManager() = default;

    TimeManager        *time_manager_;
    double              last_time{0.0};
    double              last_update_time_{0.0};
    double              last_draw_time_{0.0};

    void sleep(int milliseconds);
};

} // namespace cge

#endif // PLATFORM_GAME_MANAGER
