#pragma once

#include "util/VecMath.h"

class Light
{
public:
    vec3 color;
    
    virtual float calc_attenuation(vec3 pos) const = 0;
};

class DirnLight : public Light
{
public:
    vec3 dirn_to_light;
    
    DirnLight() : dirn_to_light(vec3{0.0f, 0.0f, 0.0f})
    {}
    
    DirnLight(vec3 color, vec3 direction_to_light) : dirn_to_light(direction_to_light)
    {
        this->color = color;
    }
    
    float calc_attenuation(vec3 pos) const override
    {
        return 1.0f;
    }
};

class PointLight : public Light
{
public:
    vec3 position;
    float atten[3]{};
    
    PointLight() : position(vec3{})
    {
        atten[0] = 1.0f;
        atten[1] = 0.0f;
        atten[2] = 0.0f;
    }
    
    PointLight(vec3 color, vec3 position, float constant, float linear, float quadratic) : position(position)
    {
        this->color = color;
        atten[0] = constant;
        atten[1] = linear;
        atten[2] = quadratic;
    }
    
    float calc_attenuation(vec3 obj_posn) const override
    {
        float distance = norm(obj_posn - position);
        return 1.0f / (atten[0] + atten[1] * distance + atten[2] * distance * distance);
    }
};
