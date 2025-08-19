#include "time_manager.hpp"

#include <chrono>

namespace cge
{

TimeManager::TimeManager() = default;
TimeManager::~TimeManager() = default;

// Return current time
double TimeManager::get_current_time() const
{
    return std::chrono::duration_cast<std::chrono::duration<double>>(my_clock.now().time_since_epoch()).count();
}

} // namespace cge

