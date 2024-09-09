#pragma once

#include <vector>
#include <iostream>
#include <random>
#include <cmath>
#include <corecrt_math_defines.h>


class vec3
{
public:
	float x;
	float y;
	float z;

	vec3() : x(0.0f), y(0.0f), z(0.0f) {}
	vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	vec3(const vec3& v) : x(v.x), y(v.y), z(v.z) {}

	friend std::ostream& operator<<(std::ostream& os, const vec3& v)
	{
		os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
		return os;
	}
	friend std::istream& operator>>(std::istream& is, vec3& v)
	{
		is >> v.x >> v.y >> v.z;
		return is;
	}

	vec3 operator+(const vec3& other) const
	{
		return vec3(x + other.x, y + other.y, z + other.z);
	}
	vec3 operator-(const vec3& other) const
	{
		return vec3(x - other.x, y - other.y, z - other.z);
	}
	vec3 operator*(const vec3& other) const
	{
		return vec3(x * other.x, y * other.y, z * other.z);
	}
	vec3 operator*(float other) const
	{
		return vec3(x * other, y * other, z * other);
	}
	vec3 operator/(float other) const
	{
		if (other == 0)
			throw std::runtime_error("other is zero. division by zero!");
		return vec3(x / other, y / other, z / other);
	}
	vec3& operator=(const vec3& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}
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
};
class vec2
{
public:
	float x;
	float y;

	vec2() : x(0.0f), y(0.0f) {}
	vec2(float x, float y) : x(x), y(y) {}
	vec2(const vec2& v) : x(v.x), y(v.y) {}

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
		return vec2(x + other.x, y + other.y);
	}
	vec2 operator-(const vec2& other) const
	{
		return vec2(x - other.x, y - other.y);
	}
	vec2 operator*(const vec2& other) const
	{
		return vec2(x * other.x, y * other.y);
	}
	vec2 operator*(float other) const
	{
		return vec2(x * other, y * other);
	}
	vec2 operator/(float other) const
	{
		if (other == 0)
			throw std::runtime_error("other is zero. division by zero!");
		return vec2(x / other, y / other);
	}
	vec2& operator=(const vec2& other)
	{
		x = other.x;
		y = other.y;
		return *this;
	}
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

class vec3uc
{
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;

	vec3uc() : r(0), g(0), b(0) {}
	vec3uc(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
};


void init_utils(std::mt19937& gen);

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

vec3 get_barycentric(const vec2& pos, const vec2& v1, const vec2& v2, const vec2& v3);

vec3 get_random_vector();

vec3 reflect(const vec3& v, const vec3& normal);

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted);

float schlick(float cosine, float ref_idx);


class ray
{
public:
	vec3 origin;
	vec3 direction;

	ray() : origin(vec3(0.0f, 0.0f, 0.0f)), direction(vec3(0.0f, 0.0f, 0.0f)) {}
	ray(const vec3& origin, const vec3& direction) : origin(origin), direction(direction) {}
	vec3 point_at_parameter(float t) const
	{
		return origin + (direction * t);
	}
};

class hit_record
{
public:
	float t;
	vec3 hit_pos;
	vec3 normal;

	hit_record() : t(0.0f), hit_pos(vec3(0.0f, 0.0f, 0.0f)), normal(vec3(0.0f, 0.0f, 0.0f)) {}

	hit_record& operator=(const hit_record& other)
	{
		t = other.t;
		hit_pos = other.hit_pos;
		normal = other.normal;
		return *this;
	}
};

class material
{
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};

class lambertian : public material
{
public:
	vec3 albedo;

	lambertian(const vec3& a) : albedo(a) {}
	bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override;
};

class metal : public material
{
public:
	vec3 albedo;
	float fuzz;

	metal(const vec3& a, float f) : albedo(a), fuzz(f) {}
	bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override;
};

class dielectric : public material
{
public:
	float ref_idx;

	dielectric(float ri) : ref_idx(ri) {}
	bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override;
};

class shape
{
public:
	material* mat = nullptr;

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};

class sphere : public shape
{
public:
	vec3 center;
	float radius;

	sphere() : center(vec3(0.0f, 0.0f, 0.0f)), radius(1.0f) {}
	sphere(vec3 cen, float r, material* mat) : center(cen), radius(r) { this->mat = mat; }
	bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
};

class cube : public shape
{
public:
	float width;
	float height;
	float depth;
	vec3 front;
	vec3 right;
	vec3 up;

	cube() : width(0.0f), height(0.0f), depth(0.0f), front(vec3{}), right(vec3{}), up(vec3{}) {}
	cube(float width, float height, float depth, vec3 front, vec3 right) : width(width), height(height), depth(depth), front(front), right(right), up(cross(right, front)) {}
	bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
};

class camera
{
public:
	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 world_up;
	vec3 front;
	vec3 right;
	vec3 up;

	camera(float v_fov, float aspect_ratio, vec3 world_up, const vec3& look_from, const vec3& look_at)
	{
		float theta = v_fov * M_PI / 180.0f;
		float half_height = tan(theta / 2.f);
		float half_width = aspect_ratio * half_height;
		this->world_up = world_up;
		origin = look_from;
		front = normalize(look_at - look_from);
		right = normalize(cross(front, world_up));
		up = cross(right, front);
		lower_left_corner = front - right * half_width - up * half_height;
		horizontal = right * 2 * half_width;
		vertical = up * 2 * half_height;
	}

	ray get_ray(float u, float v) const;
};


