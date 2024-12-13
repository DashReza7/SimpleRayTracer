#pragma once

#include <iostream>
#include <format>
#include <random>

const float EPS = 1e-4f;

class vec2
{
public:
    float x;
    float y;
    
    vec2() : x(0.0f), y(0.0f) {}
    vec2(float x, float y) : x(x), y(y) {}
    vec2(const vec2& v) = default;
    
    friend std::ostream& operator<<(std::ostream& os, const vec2& v)
    {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }
    friend std::istream& operator>>(std::istream& is, vec2& v)
    {
        is >> v.x >> v.y;
        return is;
    }
    
    vec2 operator+(const vec2& other) const
    {
        return vec2{x + other.x, y + other.y};
    }
    vec2 operator-(const vec2& other) const
    {
        return vec2{x - other.x, y - other.y};
    }
    vec2 operator*(const vec2& other) const
    {
        return vec2{x * other.x, y * other.y};
    }
    vec2 operator*(float other) const
    {
        return vec2{x * other, y * other};
    }
    vec2 operator/(float other) const
    {
        if (other == 0)
            throw std::runtime_error("other is zero. division by zero!");
        return vec2{x / other, y / other};
    }
    vec2& operator=(const vec2& other) = default;
    vec2& operator+=(const vec2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }
    vec2& operator-=(const vec2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    vec2& operator*=(const vec2& other)
    {
        x *= other.x;
        y *= other.y;
        return *this;
    }
    bool operator==(const vec2& other) const
    {
        return (x == other.x && y == other.y);
    }
    bool operator!=(const vec2& other) const
    {
        return !(*this == other);
    }
    float operator[](int idx) const
    {
        if (idx < 0 || idx > 1)
            throw std::runtime_error("index out of range!");
        return *(&this->x + idx);
    }
};

class vec4;
class vec3
{
public:
    float x;
    float y;
    float z;
    
    vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    vec3(const vec3& v) = default;
    explicit vec3(const vec4& v);
    
    vec3 operator+(const vec3& other) const
    {
        return vec3{x + other.x, y + other.y, z + other.z};
    }
    vec3 operator-(const vec3& other) const
    {
        return vec3{x - other.x, y - other.y, z - other.z};
    }
    vec3 operator*(const vec3& other) const
    {
        return vec3{x * other.x, y * other.y, z * other.z};
    }
    vec3 operator*(float other) const
    {
        return vec3{x * other, y * other, z * other};
    }
    vec3 operator/(float other) const
    {
        if (other == 0)
            throw std::runtime_error("other is zero. division by zero!");
        return vec3{x / other, y / other, z / other};
    }
    vec3& operator=(const vec3& other) = default;
    vec3& operator+=(const vec3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    vec3& operator-=(const vec3& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    vec3& operator*=(const vec3& other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }
    bool operator==(const vec3& other) const
    {
        return (x == other.x && y == other.y && z == other.z);
    }
    bool operator!=(const vec3& other) const
    {
        return !(*this == other);
    }
    float operator[](int idx) const
    {
        if (idx < 0 || idx > 2)
            throw std::runtime_error("index out of range!");
        return *(&this->x + idx);
    }
    
    std::string ToString()
    {
        return std::format("({}, {}, {})", x, y, z);
    }
    
    void clamp(float min, float max)
    {
        if (x < min)
            x = min;
        else if (x > max)
            x = max;
        if (y < min)
            y = min;
        else if (y > max)
            y = max;
        if (z < min)
            z = min;
        else if (z > max)
            z = max;
    }
};

class vec4
{
public:
    float x;
    float y;
    float z;
    float w;
    
    vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    vec4(const vec4& v) = default;
    vec4(const vec3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
    
    vec4 operator+(const vec4& other) const
    {
        return vec4{x + other.x, y + other.y, z + other.z, w + other.w};
    }
    vec4 operator-(const vec4& other) const
    {
        return vec4{x - other.x, y - other.y, z - other.z, w - other.w};
    }
    vec4 operator*(const vec4& other) const
    {
        return vec4{x * other.x, y * other.y, z * other.z, w * other.w};
    }
    vec4 operator*(float other) const
    {
        return vec4{x * other, y * other, z * other, w * other};
    }
    vec4 operator/(float other) const
    {
        if (other == 0)
            throw std::runtime_error("other is zero. division by zero!");
        return vec4{x / other, y / other, z / other, w / other};
    }
    vec4& operator=(const vec4& other) = default;
    vec4& operator+=(const vec4& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }
    vec4& operator-=(const vec4& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }
    vec4& operator*=(const vec4& other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        w *= other.w;
        return *this;
    }
    bool operator==(const vec4& other) const
    {
        return (x == other.x && y == other.y && z == other.z && w == other.w);
    }
    bool operator!=(const vec4& other) const
    {
        return !(*this == other);
    }
    float operator[](int idx) const
    {
        if (idx < 0 || idx > 3)
            throw std::runtime_error("index out of range!");
        return *(&this->x + idx);
    }
    
    std::string ToString()
    {
        return std::format("({}, {}, {}, {})", x, y, z, w);
    }
};

class mat4
{
public:
    vec4 c1;
    vec4 c2;
    vec4 c3;
    vec4 c4;
    
    mat4() : c1(vec4{}), c2(vec4{}), c3(vec4{}), c4(vec4{}) {}
    mat4(vec4 col1, vec4 col2, vec4 col3, vec4 col4) : c1(col1), c2(col2), c3(col3), c4(col4) {}
    explicit mat4(float diag)
    {
        c1 = vec4{diag, 0.0f, 0.0f, 0.0f};
        c2 = vec4{0.0f, diag, 0.0f, 0.0f};
        c3 = vec4{0.0f, 0.0f, diag, 0.0f};
        c4 = vec4{0.0f, 0.0f, 0.0f, diag};
    }
    explicit mat4(vec4 diag)
    {
        c1 = vec4{diag[0], 0.0f, 0.0f, 0.0f};
        c2 = vec4{0.0f, diag[1], 0.0f, 0.0f};
        c3 = vec4{0.0f, 0.0f, diag[2], 0.0f};
        c4 = vec4{0.0f, 0.0f, 0.0f, diag[3]};
    }
    
    mat4 operator+(const mat4& other) const
    {
        return mat4{ c1 + other.c1, c2 + other.c2, c3 + other.c3, c4 + other.c4 };
    }
    mat4 operator-(const mat4& other) const
    {
        return mat4{ c1 - other.c1, c2 - other.c2, c3 - other.c3, c4 - other.c4 };
    }
    mat4 operator*(const mat4& other) const
    {
        return mat4{
                *this * other.c1,
                *this * other.c2,
                *this * other.c3,
                *this * other.c4
        };
    }
    vec4 operator*(const vec4& other) const
    {
        return vec4{
                c1.x * other.x + c2.x * other.y + c3.x * other.z + c4.x * other.w,
                c1.y * other.x + c2.y * other.y + c3.y * other.z + c4.y * other.w,
                c1.z * other.x + c2.z * other.y + c3.z * other.z + c4.z * other.w,
                c1.w * other.x + c2.w * other.y + c3.w * other.z + c4.w * other.w
        };
    }
    vec3 operator*(const vec3& other) const
    {
        return vec3{
                c1.x * other.x + c2.x * other.y + c3.x * other.z,
                c1.y * other.x + c2.y * other.y + c3.y * other.z,
                c1.z * other.x + c2.z * other.y + c3.z * other.z
        };
    }
    mat4& operator=(const mat4& other) = default;
    mat4& operator+=(const mat4& other)
    {
        this->c1 += other.c1;
        this->c2 += other.c2;
        this->c3 += other.c3;
        this->c4 += other.c4;
        return *this;
    }
    mat4& operator-=(const mat4& other)
    {
        this->c1 -= other.c1;
        this->c2 -= other.c2;
        this->c3 -= other.c3;
        this->c4 -= other.c4;
        return *this;
    }
    bool operator==(const mat4& other) const
    {
        return this->c1 == other.c1 && this->c2 == other.c2 && this->c3 == other.c3 && this->c4 == other.c4;
    }
    bool operator!=(const mat4& other) const
    {
        return !(*this == other);
    }
    vec4 operator[](int idx) const
    {
        if (idx < 0 || idx > 3)
            throw std::runtime_error("index out of range!");
        return *(&this->c1 + idx);
    }
    
    std::string ToString()
    {
        return std::format("[{}, {}, {}, {}]\n[{}, {}, {}, {}]\n[{}, {}, {}, {}]\n[{}, {}, {}, {}]", c1.x, c2.x, c3.x, c4.x, c1.y, c2.y, c3.y, c4.y, c1.z, c2.z, c3.z, c4.z, c1.w, c2.w, c3.w, c4.w);
    }
};

class Ray
{
public:
    vec3 origin;
    vec3 direction;
    float time;
    
    Ray() : origin(vec3(0.0f, 0.0f, 0.0f)), direction(vec3(0.0f, 0.0f, 0.0f)), time(0.0f) {}
    
    Ray(const vec3 &origin, const vec3 &direction) : origin(origin), direction(direction), time(0.0f) {}
    Ray(const vec3 &origin, const vec3 &direction, float time) : origin(origin), direction(direction), time(time) {}
    
    vec3 operator()(float t) const
    {
        return origin + direction * t;
    }
};

float norm2(const vec2& v);
float norm2(const vec3& v);

float norm(const vec2& v);
float norm(const vec3& v);

vec3 normalize(const vec3& v);
vec2 normalize(const vec2& v);

vec3 cross(const vec3& v1, const vec3& v2);

float dot(const vec2& v1, const vec2& v2);
float dot(const vec3& v1, const vec3& v2);

float triangle_area(const vec2& v1, const vec2& v2, const vec2& v3);
float triangle_area(const vec3& v1, const vec3& v2, const vec3& v3);

mat4 transpose(const mat4& m);

float radians(float degree);

mat4 get_translation_matrix(const vec3& direction);

mat4 get_rotation_matrix(const vec3& axis, float angle);

vec3 reflect(const vec3& dir, const vec3& normal);

vec3 refract(const vec3& dir, const vec3& normal, float etai_over_etao);

float reflectance(float cosine, float etai_over_etao);

bool point_in_triangle(vec3 p, vec3 v1, vec3 v2, vec3 v3);

vec3 get_random_unit_vector(std::mt19937 &gen, std::uniform_real_distribution<float> &uniform_ivl_dis);

vec3 get_random_vec_around_normal(std::mt19937 &gen, std::uniform_real_distribution<float> &uniform_ivl_dis, vec3 normal);
