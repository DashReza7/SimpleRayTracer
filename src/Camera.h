#pragma once

#include "util/VecMath.h"

class Camera
{
private:
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 front;
    vec3 right;
    vec3 up;

public:
    vec3 position;
    
    Camera(float v_fov, float aspect_ratio, vec3 world_up, const vec3 &look_from, const vec3 &look_at)
    {
        float theta = radians(v_fov);
        float half_height = tan(theta / 2.0f);
        float half_width = aspect_ratio * half_height;
        position = look_from;
        front = normalize(look_at - look_from);
        right = normalize(cross(front, world_up));
        up = cross(right, front);
        lower_left_corner = front - right * half_width - up * half_height;
        horizontal = right * 2 * half_width;
        vertical = up * 2 * half_height;
    }
    
    Ray get_ray(float u, float v, float ray_time) const
    {
        return Ray{position, normalize(lower_left_corner + horizontal * u + vertical * v), ray_time };
    }
};
