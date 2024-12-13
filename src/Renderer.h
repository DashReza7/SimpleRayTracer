#pragma once

#include <random>
#include "Scene.h"
#include "Film.h"
#include <numbers>

class Renderer
{
private:
    Scene *scene;
    Film *film;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> uniform_ivl_dis{0.0f, 1.0f};
    uint32_t debug_row = 0;
    uint32_t debug_col = 0;
    
    bool hit_brute_force(const Ray &r, float t_min, float t_max, HitRecord &rec)
    {
        rec.t = t_max;
        HitRecord tmp_rec;
        bool hit_flag = false;
        for (Shape* s : scene->shapes)
        {
            if (s->hit(r, t_min, t_max, tmp_rec) && tmp_rec.t < rec.t)
            {
                hit_flag = true;
                rec = tmp_rec;
                rec.hit_obj = s;
            }
        }
        
        if (hit_flag)
            return true;
        return false;
    }
    
    vec3 get_pixel_color(const Ray &r, uint32_t depth)
    {
        if (depth == 0)
            return vec3{0.0f, 0.0f, 0.0f};
        
        HitRecord rec;
        bool is_hit = scene->use_BVH ? scene->bvh->hit(r, T_MIN, T_MAX, rec) : hit_brute_force(r, T_MIN, T_MAX, rec);
        if (!is_hit)
        {
            // didn't hit any object. hit sky
//            vec3 color_foo{2 * (r.direction.y + 0.5f), 0.9f, 85.0f};
            vec3 color_foo{0.77f, 0.89f, 0.91f};
            color_foo.clamp(0.0f, 1.0f);
            return color_foo;
        }
        else
        {
            // hit an object
            auto hit_shape = (Shape *) rec.hit_obj;
            float random_uniform = uniform_ivl_dis(gen);
                vec3 albedo = hit_shape->mat.get_albedo(random_uniform, rec);
            Ray scattered_ray = hit_shape->mat.get_scattered_ray(r, rec, random_uniform, uniform_ivl_dis(gen), get_random_unit_vector(gen, uniform_ivl_dis), get_random_vec_around_normal(gen, uniform_ivl_dis, rec.normal));
            
            vec3 final_color = albedo * get_pixel_color(scattered_ray, depth - 1);
            final_color.clamp(0.0f, 1.0f);
            return final_color;
        }
    }
    
public:
    Renderer(Scene *s, Film *f)
    {
        scene = s;
        film = f;
        gen = std::mt19937(rd());
    }
    
    void render()
    {
        for (uint32_t i = 0; i < scene->height; ++i)
        {
            debug_row = i;
            for (uint32_t j = 0; j < scene->width; ++j)
            {
                debug_col = j;
                if (scene->anti_aliasing)
                {
                    // TODO: implement blue noise instead of uniform noise
                    vec3 pixel_color = vec3{0.0f, 0.0f, 0.0f};
                    for (int k = 0; k < scene->samples_per_pixel; ++k)
                    {
                        float u = (static_cast<float>(j) + uniform_ivl_dis(gen)) / static_cast<float>(scene->width);
                        float v = (static_cast<float>(i) + uniform_ivl_dis(gen)) / static_cast<float>(scene->height);
                        pixel_color += get_pixel_color(scene->main_camera->get_ray(u, v, uniform_ivl_dis(gen)), scene->maxdepth);
                    }
                    pixel_color = pixel_color / static_cast<float>(scene->samples_per_pixel);
                    if (scene->gamma_correction)
                        pixel_color = vec3{pow(pixel_color.x, 0.5f), pow(pixel_color.y, 0.5f),
                                           pow(pixel_color.z, 0.5f)};
                    film->commit(j, i, pixel_color);
                }
                else
                {
                    auto u = (static_cast<float>(j) + 0.5f) / static_cast<float>(scene->width);
                    auto v = (static_cast<float>(i) + 0.5f) / static_cast<float>(scene->height);
                    vec3 pixel_color = get_pixel_color(scene->main_camera->get_ray(u, v, uniform_ivl_dis(gen)), scene->maxdepth);
                    if (scene->gamma_correction)
                        pixel_color = vec3{pow(pixel_color.x, 0.5f), pow(pixel_color.y, 0.5f),
                                           pow(pixel_color.z, 0.5f)};
                    film->commit(j, i, pixel_color);
                }
            }
            std::cout << std::format("Progress: {:.2f}%\r", (float) i / (float) scene->height * 100.0f);
        }
        std::cout << "Progress: 100.0%" << std::endl;
    }
};
