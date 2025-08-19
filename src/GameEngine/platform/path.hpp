/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef PLATFORM_PATH_HPP
#define PLATFORM_PATH_HPP

#include <vector>
#include "platform/math.hpp"

namespace cge
{

// Defines a Point on a path. Contains 
// both x, y coordinates and a pause time, 
// which defines how long to remain at
// the point.
struct PathPoint
{
    float x, y;
    float pause_time;

    PathPoint(float x, float y, float pause_time = 0.0f) 
        : x(x), y(y), pause_time(pause_time) {}
};
 
// Utility Path class to configure automated movement for NPCs
class Path
{
public:
    Path() = default;

    // Add a point to the path
    void add_point(float x, float y, float pause_time = 0.0f)
    {
        points_.emplace_back(x, y, pause_time);
    }

    // Get point at index
    const PathPoint &get_point(size_t index) const { return points_[index % points_.size()]; }

    // Get total number of points
    size_t size() const { return points_.size(); };

    // Looping management
    bool is_looping() const { return looping_; };
    void set_looping(bool looping) { looping_ = looping;  }

private:
    std::vector<PathPoint> points_;
    bool looping_{true};
};

} // namespace cge

#endif // PLATFORM_PATH_HPP
