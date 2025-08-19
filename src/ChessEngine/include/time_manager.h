/*
    Time management for chess engine.
    Checks for timeout.

    Author: Nicolas Miller
    Date: 07/09/2025
*/

#ifndef CHESS_ENGINE_TIME_MANAGER_H
#define CHESS_ENGINE_TIME_MANAGER_H

#include <chrono>

namespace luna {

class TimeManager {
public:
    TimeManager();
    
    void start_search(int time_ms);
    bool should_stop() const;
    int elapsed_ms() const;

private:
    std::chrono::steady_clock::time_point start_time_;
    int allocated_time_ms_;
};

} // namespace luna

#endif // CHESS_ENGINE_TIME_MANAGER_H
