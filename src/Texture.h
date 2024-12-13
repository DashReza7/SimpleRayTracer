#pragma once

#include "util/VecMath.h"
#include "util/Utils.h"

class Texture
{
public:
    virtual ~Texture() = default;
    
    virtual vec3 get_value(float u, float v, const vec3 &hit_point) const = 0;
};

class SolidColor : public Texture
{
private:
    vec3 albedo;

public:
    SolidColor(const vec3 &albedo) : albedo(albedo) {}
    SolidColor(float r, float g, float b) : SolidColor(vec3{r, g, b}) {}
    
    vec3 get_value(float u, float v, const vec3 &hit_point) const override
    {
        return albedo;
    }
};

class CheckerTexture : public Texture
{
private:
    vec3 color0;
    vec3 color1;
    float inv_scale;
    
public:
    CheckerTexture(const vec3 &color0, const vec3 &color1, float scale) : color0(color0), color1(color1), inv_scale(1.0f / scale) {}
    
    vec3 get_value(float u, float v, const vec3 &hit_point) const override
    {
        int xInteger = int(std::floor(inv_scale * hit_point.x));
        int yInteger = int(std::floor(inv_scale * hit_point.y));
        int zInteger = int(std::floor(inv_scale * hit_point.z));
        
//        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;
        bool isEven = (xInteger + zInteger) % 2 == 0;
        
        return isEven ? color0 : color1;
    }
};

class ImageTexture : public Texture
{
private:
    std::vector<std::vector<std::array<uint8_t, 3>>> pixels;
    size_t width, height;
    
public:
    ImageTexture(const char *image_dir)
    {
        pixels = RTWImage::load(image_dir);
        height = pixels.size();
        width = pixels[0].size();
    }
    
    vec3 get_value(float u, float v, const vec3 &hit_point) const override
    {
        int i = int(v * (height - 1));
        int j = int(u * (width - 1));
        
        return vec3{static_cast<float>(pixels[i][j][0]) / 255.0f, static_cast<float>(pixels[i][j][1]) / 255.0f, static_cast<float>(pixels[i][j][2]) / 255.0f};
    }
};
