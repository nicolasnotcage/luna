/*
    Implementation of time management for chess engine.
    Controls search time allocation and checks for timeout.

    Author: Nicolas Miller
    Date: 07/09/2025
*/

#include "ChessEngine/include/time_manager.h"
#include "ChessEngine/include/constants.h"

namespace luna {

TimeManager::TimeManager() 
    : allocated_time_ms_(DEFAULT_SEARCH_TIME_MS) {}

void TimeManager::start_search(int time_ms) {
    start_time_ = std::chrono::steady_clock::now();
    allocated_time_ms_ = time_ms;
}

bool TimeManager::should_stop() const {
    auto current_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>
        (current_time - start_time_).count();
    
    return elapsed >= allocated_time_ms_;
}

int TimeManager::elapsed_ms() const {
    auto current_time = std::chrono::steady_clock::now();
    return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>
        (current_time - start_time_).count());
}

} // namespace luna
