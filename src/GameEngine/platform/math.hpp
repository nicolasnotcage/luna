/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef GRAPH_MATH_HPP
#define GRAPH_MATH_HPP

#include <array>
#include <list>

namespace cge
{

float degrees_to_radians(float deg);

// forward declare
struct HVector2;

struct Vector2
{
    float x;
    float y;

    // default constructor
    Vector2();

    Vector2(float x_in, float y_in);

    // copy constructor
    Vector2(const Vector2 &in);

    // returns the norm (length)
    float norm() const;

    // returns the norm-squared
    float norm_squared() const;

    // make this a unit vector
    void normalize();

    // returns a copy of this vector as a unit vector
    Vector2 normalized() const;

    // returns the dot product of this vector and 'v'
    float dot(const Vector2 &v) const;

    // assignment operator
    void operator=(const Vector2 &v);

    // negates this vector
    void operator-();

    // adds vector 'v' to this vector
    void operator+=(const Vector2 &v);

    // returns the sum of this and 'v'
    Vector2 operator+(const Vector2 &v) const;

    // subtracts vector 'v' from this vector
    void operator-=(const Vector2 &v);

    // returns the difference: this minus 'v'
    Vector2 operator-(const Vector2 &v) const;

    // scales this vector by 's'
    void operator*=(float s);

    // returns a copy of this vector scaled by 's'
    Vector2 operator*(float s) const;

    HVector2 as_hvector() const;
};

// enables the operation: scalar * vector
Vector2 operator*(float s, const Vector2 &v);

// Homogenous 2D Vector
struct HVector2
{
    float x;
    float y;
    float w;

    // default constructor
    HVector2();

    HVector2(float x_in, float y_in);

    // copy constructor
    HVector2(const HVector2 &in);

    Vector2 as_vector() const;

    // assignment operator
    void operator=(const HVector2 &v);
};

struct Matrix3
{
    // Primary matrix data, contiguous in memery
    std::array<float, 9> a;

    // default constructor
    Matrix3();

    // copy constructor
    Matrix3(const Matrix3 &in);

    // returns a scale matrix
    static Matrix3 scale_matrix(float x, float y);

    // returns a rotation matrix
    static Matrix3 rotation_matrix(float angle_rad);

    // returns a translation matrix
    static Matrix3 translation_matrix(float x, float y);

    // assignment operator
    void operator=(const Matrix3 &in);

    // sets this matrix as an identity matrix
    void set_identity();

    // left multiplies this matrix by scale matrix
    void left_scale(float x, float y);

    // right multiplies this matrix by scale matrix
    void right_scale(float x, float y);

    // left multiplies this matrix by rotation matrix
    void left_rotate(float angle_rad);

    // right multiplies this matrix by rotation matrix
    void right_rotate(float angle_rad);

    // left multiplies this matrix by scale matrix
    void left_translate(float x, float y);

    // right multiplies this matrix by scale matrix
    void right_translate(float x, float y);

    // right multiplies this matrix by m
    void operator*=(const Matrix3 &m);

    // returns this matrix right multiplied by m
    Matrix3 operator*(const Matrix3 &m) const;

    // return this right-multiplied by 'v'
    Vector2 operator*(const Vector2 &v) const;

    // return this right-multiplied by 'v'
    HVector2 operator*(const HVector2 &v) const;
};

class MatrixStack
{
  public:
    MatrixStack();
    ~MatrixStack();

    void reset();
    void push();
    void pop();

    Matrix3 &top();

  private:
    std::list<Matrix3> stack_;
};

// For collision detection
// Circle Bounding Volume
struct Circle
{
    Vector2 center;
    float   radius;
    Circle(const Vector2 &center_in, float radius_in);
    bool intersects(const Circle &other) const;
};

// Axis-Aligned Box Bounding Volume
struct AABB2
{
    Vector2 min;
    Vector2 max;
    AABB2(const Vector2 &min_in, const Vector2 &max_in);
    bool intersects(const AABB2 &other) const;
};


} // namespace cge

#endif // GRAPH_MATH_HPP
