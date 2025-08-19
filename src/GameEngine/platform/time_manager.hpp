#ifndef PLATFORM_TIME_MANAGER
#define PLATFORM_TIME_MANAGER

#include <chrono>

namespace cge
{

// A singleton game manager class
class TimeManager
{
public:
    static TimeManager* get_instance()
    { 
        static TimeManager instance;
        return &instance;
    }

    // Delete copy and move constructor/assignment operators
    TimeManager(const TimeManager &) = delete;
    TimeManager &operator=(const TimeManager &) = delete;
    TimeManager(TimeManager &&) = delete;
    TimeManager &operator=(TimeManager &&) = delete;

    double get_current_time() const;
    
    // Get time of the last frame
    double get_last_time() const { return last_time_; }
    
    // Set time of the last frame
    void set_last_time(double time) { last_time_ = time; }

private:
    TimeManager();
    ~TimeManager();

    std::chrono::high_resolution_clock  my_clock;
    double last_time_{0.0};
};

} // namespace cge

#endif // PLATFORM_TIME_MANAGER
