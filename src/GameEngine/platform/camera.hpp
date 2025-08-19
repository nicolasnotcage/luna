/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef GRAPH_CAMERA_HPP
#define GRAPH_CAMERA_HPP

#include <array>
#include <list>
#include "platform/math.hpp"

namespace cge
{

class Camera
{
public:
	Camera(float center_x = 0.0f, float center_y = 0.0f, 
           float height = 10.0f, float width = 10.0f);

    // Setters and getters
	void        set_position(float x, float y);
    void        set_dimensions(float width, float height);
    Vector2     get_position() const;
    float       get_width() const;
    float       get_height() const;

    // Camera movement and resizing
    void move(float dx, float dy);
    void zoom(float factor);

    // Get world-to-screen matrix
    Matrix3 get_world_to_screen_matrix(int screen_width, int screen_height) const;

    // Get screen-to-world position
    Vector2 screen_to_world(const Vector2 &screen_position, int screen_width, int screen_height) const;


  private:
    float center_x;
    float center_y;
    float width;
    float height;
};

} // namespace cge

#endif // GRAPH_CAMERA_HPP
