#include <iostream>
#include <random>
#include "utils.h"

std::uniform_real_distribution<float> dis_min1_1;
std::uniform_real_distribution<float> dis_uniform;
std::mt19937 rnd_generator;
void init_utils(std::mt19937& gen)
{
	rnd_generator = gen;
	dis_min1_1 = std::uniform_real_distribution<float>{ -1.0f, 1.0f };
	dis_uniform = std::uniform_real_distribution<float>{ 0.0f, 1.0f };
}

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	vec3 origin_minus_center = r.origin - center;

	float a = dot(r.direction, r.direction);
	float b = dot(r.direction, origin_minus_center);
	float c = dot(origin_minus_center, origin_minus_center) - radius * radius;
	float delta = b * b - a * c;

	if (delta < 0)
		return false;

	float t = (-b - sqrt(delta)) / a;
	if (t >= t_min && t <= t_max)
	{
		rec.t = t;
		rec.hit_pos = r.point_at_parameter(t);
		rec.normal = normalize(rec.hit_pos - center);
		return true;
	}
	t = (-b + sqrt(delta)) / a;
	if (t >= t_min && t <= t_max)
	{
		rec.t = t;
		rec.hit_pos = r.point_at_parameter(t);
		rec.normal = normalize(rec.hit_pos - center);
		return true;
	}
	return false;
}

bool cube::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	throw std::runtime_error("Not implemented");
}

bool lambertian::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
{
	vec3 target = rec.hit_pos + rec.normal + get_random_vector();
	scattered = ray{ rec.hit_pos, normalize(target - rec.hit_pos) };
	attenuation = albedo;
	return true;
}

bool metal::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
{
	vec3 reflected = reflect(r_in.direction, rec.normal);
	scattered = ray{ rec.hit_pos, normalize(reflected + get_random_vector() * fuzz) };
	attenuation = albedo;
	return (dot(scattered.direction, rec.normal) > 0.0f);
}

bool dielectric::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
{
	vec3 outward_normal;
	vec3 reflected = reflect(r_in.direction, rec.normal);
	float ni_over_nt;
	attenuation = vec3{ 1.0f, 1.0f, 1.0f };
	vec3 refracted;
	float reflect_prob;
	float cosine;
	if (dot(r_in.direction, rec.normal) > 0.0f)
	{
		outward_normal = rec.normal * -1;
		ni_over_nt = ref_idx;
		cosine = ref_idx * dot(r_in.direction, rec.normal);

	}
	else
	{
		outward_normal = rec.normal;
		ni_over_nt = 1.0f / ref_idx;
		cosine = -dot(r_in.direction, rec.normal);
	}
	if (refract(r_in.direction, outward_normal, ni_over_nt, refracted))
		reflect_prob = schlick(cosine, ref_idx);
	else
		reflect_prob = 1.0f;

	if (dis_uniform(rnd_generator) < reflect_prob)
		scattered = ray{ rec.hit_pos, normalize(reflected) };
	else
		scattered = ray{ rec.hit_pos, normalize(refracted) };
	return true;
}

ray camera::get_ray(float u, float v) const
{
	return ray{ origin, normalize(lower_left_corner + horizontal * u + vertical * v) };
}

// functions: -------------------------------------------------------------
float norm2(const vec2& v)
{
	return dot(v, v);
}
float norm2(const vec3& v)
{
	return dot(v, v);
}

float norm(const vec2& v)
{
	return sqrt(norm2(v));
}
float norm(const vec3& v)
{
	return sqrt(norm2(v));
}

vec3 normalize(const vec3& v)
{
	return v / norm(v);
}
vec2 normalize(const vec2& v)
{
	return v / norm(v);
}

vec3 cross(const vec3& v1, const vec3& v2)
{
	return vec3(v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x);
}

float dot(const vec2& v1, const vec2& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}
float dot(const vec3& v1, const vec3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float triangle_area(const vec2& v1, const vec2& v2, const vec2& v3)
{
	vec2 u1 = v2 - v1;
	vec2 u2 = v3 - v1;

	return abs(0.5f * (u1.x * u2.y - u1.y * u2.x));
}

vec3 get_barycentric(const vec2& pos, const vec2& v1, const vec2& v2, const vec2& v3)
{
	float area = triangle_area(v1, v2, v3);
	return vec3(triangle_area(pos, v2, v3) / area,
		triangle_area(pos, v1, v3) / area,
		triangle_area(pos, v1, v2) / area);
}

vec3 get_random_vector()
{
	vec3 vec;
	while (true)
	{
		vec = vec3(dis_min1_1(rnd_generator), dis_min1_1(rnd_generator), dis_min1_1(rnd_generator));
		if (norm(vec) <= 1.0f)
			break;
	}
	vec = normalize(vec);
	return vec;
}

vec3 reflect(const vec3& v, const vec3& normal)
{
	return v - normal * 2 * dot(v, normal);
}

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted)
{
	float dt = dot(v, n);
	float discriminant = 1 - ni_over_nt * ni_over_nt * (1 - dt * dt);
	if (discriminant <= 0.0f)
		return false;
	refracted = (v - n * dt) * ni_over_nt - n * sqrt(discriminant);
	return true;
}

float schlick(float cosine, float ref_idx)
{
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 *= r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5.0f);
}
