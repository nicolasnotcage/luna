/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/math.hpp"

#include <cmath>
#include <cstring>

namespace cge
{

namespace
{
constexpr float EPSILON = 0.00001f;
}

float degrees_to_radians(float deg)
{
    constexpr float PI_RAD_FACTOR = 0.0174532924f;
    return deg * PI_RAD_FACTOR;
}

Vector2::Vector2() : x{0.0f}, y{0.0f} {}

Vector2::Vector2(float x_in, float y_in) : x{x_in}, y{y_in} {}

Vector2::Vector2(const Vector2 &in) { *this = in; }

float Vector2::norm() const { return std::sqrt(norm_squared()); }

float Vector2::norm_squared() const { return dot(*this); }

void Vector2::normalize()
{
    auto n = norm();
    if(n > EPSILON)
    {
        x /= n;
        y /= n;
    }
}

Vector2 Vector2::normalized() const
{
    Vector2 result(*this);
    result.normalize();
    return result;
}

float Vector2::dot(const Vector2 &v) const { return x * v.x + y * v.y; }

void Vector2::operator=(const Vector2 &v)
{
    x = v.x;
    y = v.y;
}

void Vector2::operator-()
{
    x = -x;
    y = -y;
}

void Vector2::operator+=(const Vector2 &v)
{
    x += v.x;
    y += v.y;
}

Vector2 Vector2::operator+(const Vector2 &v) const { return Vector2(x + v.x, y + v.y); }

void Vector2::operator-=(const Vector2 &v)
{
    x -= v.x;
    y -= v.y;
}

Vector2 Vector2::operator-(const Vector2 &v) const { return Vector2(x - v.x, y - v.y); }

void Vector2::operator*=(float s)
{
    x *= s;
    y *= s;
}

Vector2 Vector2::operator*(float s) const { return Vector2(x * s, y * s); }

HVector2 Vector2::as_hvector() const { return HVector2(x, y); }

Vector2 operator*(float s, const Vector2 &v) { return Vector2(v.x * s, v.y * s); }

HVector2::HVector2() : x{0.0f}, y{0.0f}, w{1.0f} {}

HVector2::HVector2(float x_in, float y_in) : x{x_in}, y{y_in}, w{1.0f} {}

HVector2::HVector2(const HVector2 &in) { *this = in; }

void HVector2::operator=(const HVector2 &v)
{
    x = v.x;
    y = v.y;
    w = v.w;
}

Vector2 HVector2::as_vector() const { return Vector2(x, y); }

/*
 * Matrix3
 */

Matrix3::Matrix3() { set_identity(); }

Matrix3::Matrix3(const Matrix3 &in) { *this = in; }

Matrix3 Matrix3::scale_matrix(float x, float y)
{
    Matrix3 m; // set as identity
    m.a[0] = x;
    m.a[4] = y;
    return m;
}

Matrix3 Matrix3::rotation_matrix(float angle_rad)
{
    const float c = std::cos(angle_rad);
    const float s = std::sin(angle_rad);
    Matrix3     m; // set as identity
    m.a[0] = c;
    m.a[1] = s;
    m.a[3] = -s;
    m.a[4] = c;
    return m;
}

Matrix3 Matrix3::translation_matrix(float x, float y)
{
    Matrix3 m; // set as identity
    m.a[6] = x;
    m.a[7] = y;
    return m;
}

void Matrix3::operator=(const Matrix3 &in)
{
    std::memcpy(a.data(), in.a.data(), sizeof(float) * 9);
}

void Matrix3::set_identity()
{
    a[0] = 1.0f;
    a[1] = 0.0f;
    a[2] = 0.0f;

    a[3] = 0.0f;
    a[4] = 1.0f;
    a[5] = 0.0f;

    a[6] = 0.0f;
    a[7] = 0.0f;
    a[8] = 1.0f;
}

void Matrix3::left_scale(float x, float y) { *this = scale_matrix(x, y) * (*this); }

void Matrix3::right_scale(float x, float y) { *this = (*this) * scale_matrix(x, y); }

void Matrix3::left_rotate(float angle_rad) { *this = rotation_matrix(angle_rad) * (*this); }

void Matrix3::right_rotate(float angle_rad) { *this = (*this) * rotation_matrix(angle_rad); }

void Matrix3::left_translate(float x, float y) { *this = translation_matrix(x, y) * (*this); }

void Matrix3::right_translate(float x, float y) { *this = (*this) * translation_matrix(x, y); }

void Matrix3::operator*=(const Matrix3 &m)
{
    float t_1, t_2, t_3;
    t_1 = a[0] * m.a[0] + a[3] * m.a[1] + a[6] * m.a[2];
    t_2 = a[0] * m.a[3] + a[3] * m.a[4] + a[6] * m.a[5];
    t_3 = a[0] * m.a[6] + a[3] * m.a[7] + a[6] * m.a[8];

    a[0] = t_1;
    a[3] = t_2;
    a[6] = t_3;

    t_1 = a[1] * m.a[0] + a[4] * m.a[1] + a[7] * m.a[2];
    t_2 = a[1] * m.a[3] + a[4] * m.a[4] + a[7] * m.a[5];
    t_3 = a[1] * m.a[6] + a[4] * m.a[7] + a[7] * m.a[8];

    a[1] = t_1;
    a[4] = t_2;
    a[7] = t_3;

    t_1 = a[2] * m.a[0] + a[5] * m.a[1] + a[8] * m.a[2];
    t_2 = a[2] * m.a[3] + a[5] * m.a[4] + a[8] * m.a[5];
    t_3 = a[2] * m.a[6] + a[5] * m.a[7] + a[8] * m.a[8];

    a[2] = t_1;
    a[5] = t_2;
    a[8] = t_3;
}

Matrix3 Matrix3::operator*(const Matrix3 &m) const
{
    Matrix3 result(*this);
    result *= m;
    return result;
}

Vector2 Matrix3::operator*(const Vector2 &v) const
{
    auto result = *this * v.as_hvector();
    return result.as_vector();
}

HVector2 Matrix3::operator*(const HVector2 &v) const
{
    HVector2 result;
    result.x = a[0] * v.x + a[3] * v.y + a[6] * v.w;
    result.y = a[1] * v.x + a[4] * v.y + a[7] * v.w;
    result.w = a[2] * v.x + a[5] * v.y + a[8] * v.w;
    return result;
}

MatrixStack::MatrixStack() { reset(); }

MatrixStack::~MatrixStack() { stack_.clear(); }

void MatrixStack::reset()
{
    stack_.clear();
    stack_.emplace_back();
}

void MatrixStack::push()
{
    const auto &back_matrix = stack_.back();
    stack_.emplace_back(back_matrix);
}

void MatrixStack::pop()
{
    switch(stack_.size())
    {
        case 0: stack_.emplace_back(); break;
        case 1: stack_.back().set_identity(); break;
        default: stack_.pop_back(); break;
    }
}

Matrix3 &MatrixStack::top() { return stack_.back(); }

// Bounding volume functions
Circle::Circle(const Vector2 &center_in, float radius_in) 
    : center(center_in), radius(radius_in) 
    {}

bool Circle::intersects(const Circle &other) const 
{
    // Calculate distance between centers
    Vector2 diff = center - other.center;
    float   distanceSquared = diff.dot(diff);

    // Check if the distance is less than or equal the sum of the radii
    float radiusSum = radius + other.radius;
    return distanceSquared <= radiusSum * radiusSum;
}

// Axis-aligned box
AABB2::AABB2(const Vector2 &min_in, const Vector2 &max_in) 
    : min(min_in), max(max_in)
    {}

// Note: This algorithm taken from Ericson, adjusted for 2D.
bool AABB2::intersects(const AABB2 &other) const
{
    // Not intersecting if separated along an axis
    if(this->max.x < other.min.x || this->min.x > other.max.x) return false;
    if(this->max.y < other.min.y || this->min.y > other.max.y) return false;

    // Overlapping on all axes means AABBs are intersecting
    return true;
}

} // namespace cge
