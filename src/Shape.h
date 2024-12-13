#pragma once

#include <algorithm>
#include "util/VecMath.h"
#include "SpatialDatastructures.h"
#include "Texture.h"

class Material
{
public:
    vec3 diffuse;
    vec3 specular;
    float shininess;
    vec3 ambient;
    vec3 emission;
    float fuzziness;
    float dielectric_idx;
    const Texture *texture;
    
    Material() = default;
    Material(const vec3 &diffuse, const vec3 &specular, float shininess, const vec3 &ambient, const vec3 &emission,
             float fuzziness, float dielectric_idx, const Texture *texture) : diffuse(diffuse), specular(specular), shininess(shininess),
                                                      ambient(ambient), emission(emission), fuzziness(fuzziness),
                                                      dielectric_idx(dielectric_idx), texture(texture) {}
    
    Ray get_scattered_ray(const Ray &r, const HitRecord &rec, float random_uniform, float random_uniform_2, vec3 random_unit_vector, vec3 random_vec_around_normal) const
    {
        Ray scattered_ray{};
        if (dielectric_idx != 0)
        {
            // the object is dielectric
            float mu;
            vec3 normal;
            if (dot(r.direction, rec.normal) <= 0)
            {
                mu = 1 / dielectric_idx;
                normal = rec.normal * (-1);
            }
            else
            {
                mu = dielectric_idx;
                normal = rec.normal;
            }
            
            float cos_theta = std::fmin(dot(r.direction, normal), 1.0f);
            float sin_theta = std::sqrt(1 - cos_theta * cos_theta);
            vec3 dirn;
            if (mu * sin_theta >= 1.0f || 0.5f * reflectance(cos_theta, mu) > random_uniform_2)
            {
                // reflect
                dirn = reflect(r.direction * (-1), normal * (-1));
            }
            else
            {
                // refract
                dirn = refract(r.direction, normal, mu);
            }
            scattered_ray = Ray{rec.hit_pos + normal * EPS, dirn, r.time};
        }
        else if (random_uniform <= shininess)
        {
            // trace the specular ray
            vec3 dirn = reflect(r.direction * (-1), rec.normal);
            dirn = normalize(
                    dirn + random_unit_vector * std::min(0.9f, fuzziness));
            scattered_ray = Ray{rec.hit_pos + rec.normal * EPS, dirn, r.time};
        }
        else
        {
            // trace the diffuse ray
            scattered_ray = Ray{rec.hit_pos + rec.normal * EPS,random_vec_around_normal, r.time};
        }
        return scattered_ray;
    }
    
    vec3 get_albedo(float random_uniform, const HitRecord &rec) const
    {
        if (texture != nullptr)
            return texture->get_value(rec.texture_u, rec.texture_v, rec.hit_pos);
        if (dielectric_idx != 0)
            return vec3{1.0f, 1.0f, 1.0f};
        if (random_uniform <= shininess)
            return specular;
        return diffuse;
    }
};

class Shape : public Hittable
{
public:
    Material mat;
};

class Sphere : public Shape
{
private:
    mat4 transform;
    mat4 inverse_transform;
    
    
public:
    Ray center;
    float radius;
    
    // Not-Moving sphere
    Sphere() : center(Ray{vec3{}, vec3{}}), radius(1.0f)
    {
        set_bbox();
    }
    Sphere(const vec3 &cen, float r, const Material &mat, const mat4 &transform, const mat4 &inverse_transform)
            : center(Ray{cen, vec3{}}), radius(r), transform(transform), inverse_transform(inverse_transform)
    {
        this->mat = mat;
        set_bbox();
    }
    // Moving sphere
    Sphere(const vec3 &center1, const vec3 &center2, float r, const Material &mat, const mat4 &transform, const mat4 &inverse_transform) :
            center(Ray{center1, center2 - center1}), radius(r), transform(transform), inverse_transform(inverse_transform)
    {
        this->mat = mat;
        // TODO:
        set_bbox();
    }
    
    bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) override;
    
    void set_bbox();
};

class Triangle : public Shape
{
public:
    vec3 v1;
    vec3 v2;
    vec3 v3;
    vec3 n;
    float area;
    
    Triangle() : v1(vec3{}), v2(vec3{}), v3(vec3{}), n(vec3{}), area(0.0f)
    {
        set_bbox();
    }
    Triangle(const vec3 &v1, const vec3 &v2, const vec3 &v3, const Material &mat) : v1(v1), v2(v2), v3(v3)
    {
        this->mat = mat;
        n = normalize(cross(v2 - v1, v3 - v1));
        area = triangle_area(v1, v2, v3);
        set_bbox();
    }
    
    bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) override;
    
    void set_bbox();
};

