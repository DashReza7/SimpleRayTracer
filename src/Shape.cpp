#include "util/VecMath.h"
#include "Shape.h"
#include <numbers>

bool Sphere::hit (const Ray &r, float t_min, float t_max, HitRecord &rec)
{
    // inverse_transformed_ray
    vec4 temp_origin = inverse_transform * vec4{ r.origin, 1.0f };
    vec4 temp_direction = inverse_transform * vec4{ r.direction, 0.0f };
    Ray it_r{ vec3{temp_origin}, normalize(vec3{temp_direction}), r.time };

    vec3 current_center = this->center.origin +  this->center.direction * r.time;
    vec3 origin_minus_center = it_r.origin - current_center;
    
    float a = dot(it_r.direction, it_r.direction);
    float b = dot(it_r.direction, origin_minus_center);
    float c = dot(origin_minus_center, origin_minus_center) - radius * radius;
    float delta = b * b - a * c;
    
    if (delta < 0)
        return false;
    
    float t = (-b - sqrt(delta)) / a;
    if (t >= t_min)
    {
        vec3 original_hitpos = it_r(t);
        vec4 tmp_hit_pos = transform * vec4{ original_hitpos , 1.0f };
        rec.hit_pos = vec3{tmp_hit_pos};
        float hit_distance = norm(rec.hit_pos - r.origin);
        if (hit_distance <= t_max)
        {
            rec.t = hit_distance;
            rec.normal = normalize(transpose(inverse_transform) * normalize(original_hitpos - current_center));
            rec.hit_obj = this;
            
            vec3 normalized_hitpoint = (rec.hit_pos - current_center) / radius;
            float theta = std::acos(-normalized_hitpoint.y);
            float phi = std::atan2(-normalized_hitpoint.z, normalized_hitpoint.x) + std::numbers::pi;
            rec.texture_u = phi / (2*std::numbers::pi);
            rec.texture_v = theta / (std::numbers::pi);
            
            return true;
        }
    }
    t = (-b + sqrt(delta)) / a;
    if (t >= t_min)
    {
        vec3 original_hitpos = it_r(t);
        vec4 tmp_hit_pos = transform * vec4{original_hitpos , 1.0f };
        rec.hit_pos = vec3{tmp_hit_pos};
        float hit_distance = norm(rec.hit_pos - r.origin);
        if (hit_distance <= t_max)
        {
            rec.t = hit_distance;
            rec.normal = normalize(transpose(inverse_transform) * (original_hitpos - current_center));
            rec.hit_obj = this;
            
            vec3 normalized_hitpoint = (rec.hit_pos - current_center) / radius;
            float theta = std::acos(-normalized_hitpoint.y);
            float phi = std::atan2(-normalized_hitpoint.z, normalized_hitpoint.x) + std::numbers::pi;
            rec.texture_u = phi / (2*std::numbers::pi);
            rec.texture_v = theta / (std::numbers::pi);
            
            return true;
        }
    }
    return false;
}

void Sphere::set_bbox()
{
    std::vector<vec3> offsets = {
            vec3{+1, +1, +1},
            vec3{+1, +1, -1},
            vec3{+1, -1, +1},
            vec3{+1, -1, -1},
            vec3{-1, +1, +1},
            vec3{-1, +1, -1},
            vec3{-1, -1, +1},
            vec3{-1, -1, -1}
    };
    std::vector<vec4> pts_1;
    std::vector<vec4> pts_2;
    for (auto offset : offsets)
    {
        pts_1.emplace_back(offset * radius + center.origin, 1.0f);
        pts_2.emplace_back(offset * radius + center.origin + center.direction, 1.0f);
    }
    for (auto & pt : pts_1)
    {
        pt = transform * pt;
        pt = pt / pt.w;
    }
    for (auto & pt : pts_2)
    {
        pt = transform * pt;
        pt = pt / pt.w;
    }
    Interval x_ivl = Interval{pts_1[0].x, pts_1[0].x};
    Interval y_ivl = Interval{pts_1[0].y, pts_1[0].y};
    Interval z_ivl = Interval{pts_1[0].z, pts_1[0].z};
    for (const auto& pt : pts_1)
    {
        x_ivl.low = std::min(x_ivl.low, pt.x);
        x_ivl.high = std::max(x_ivl.high, pt.x);
        y_ivl.low = std::min(y_ivl.low, pt.y);
        y_ivl.high = std::max(y_ivl.high, pt.y);
        z_ivl.low = std::min(z_ivl.low, pt.z);
        z_ivl.high = std::max(z_ivl.high, pt.z);
    }
    for (const auto& pt : pts_2)
    {
        x_ivl.low = std::min(x_ivl.low, pt.x);
        x_ivl.high = std::max(x_ivl.high, pt.x);
        y_ivl.low = std::min(y_ivl.low, pt.y);
        y_ivl.high = std::max(y_ivl.high, pt.y);
        z_ivl.low = std::min(z_ivl.low, pt.z);
        z_ivl.high = std::max(z_ivl.high, pt.z);
    }
    
    AABB tmp_bbox = AABB{x_ivl, y_ivl, z_ivl};
    tmp_bbox.expand(0.01f);
    bbox = tmp_bbox;
}

bool Triangle::hit(const Ray& r, float t_min, float t_max, HitRecord& rec)
{
    float t = dot(v1 - r.origin, n) / dot(r.direction, n);
    if (t < 0 || t > t_max)
        return false;
    rec.hit_pos = r(t);
    if (!point_in_triangle(rec.hit_pos, v1, v2, v3))
        return false;
    rec.t = t;
    rec.normal = n;
    rec.hit_obj = this;
    return true;
}

void Triangle::set_bbox()
{
    vec3 p0 = vec3{std::min(v1.x, std::min(v2.x, v3.x)),
                   std::min(v1.y, std::min(v2.y, v3.y)),
                   std::min(v1.z, std::min(v2.z, v3.z))};
    vec3 p1 = vec3{std::max(v1.x, std::max(v2.x, v3.x)),
                   std::max(v1.y, std::max(v2.y, v3.y)),
                   std::max(v1.z, std::max(v2.z, v3.z))};
    AABB tmp_bbox = AABB{p0, p1};
    tmp_bbox.expand(0.01f);
    bbox = tmp_bbox;
}
