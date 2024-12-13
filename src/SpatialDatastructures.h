#pragma once

#include <format>
#include <algorithm>
#include <vector>
#include "util/VecMath.h"

class Hittable;

class HitRecord
{
public:
    float t;
    vec3 hit_pos;
    vec3 normal;
    Hittable* hit_obj;
    float texture_u;
    float texture_v;
    
    HitRecord &operator=(const HitRecord &other) = default;
};

class Interval
{
public:
    float low, high;
    
    Interval() : low(0.0f), high(0.0f) {}
    Interval(float l, float h) : low(l), high(h) {}
    
    std::string ToString()
    {
        return std::format("[{}, {}]", low, high);
    }
    
    inline void expand(float delta)
    {
        float padding = delta / 2.0f;
        low -= padding;
        high += padding;
    }
    
    inline float length() const
    {
        return high - low;
    }
};

class AABB
{
public:
    Interval x_ivl, y_ivl, z_ivl;
    
    AABB() : x_ivl(Interval{}), y_ivl(Interval{}), z_ivl(Interval{}) {}
    AABB(vec3 a, vec3 b)
    {
        x_ivl = (a[0] <= b[0]) ? Interval{a[0], b[0]} : Interval{b[0], a[0]};
        y_ivl = (a[1] <= b[1]) ? Interval{a[1], b[1]} : Interval{b[1], a[1]};
        z_ivl = (a[2] <= b[2]) ? Interval{a[2], b[2]} : Interval{b[2], a[2]};
    }
    AABB(const Interval& x_ivl, const Interval& y_ivl, const Interval& z_ivl) : x_ivl(x_ivl), y_ivl(y_ivl), z_ivl(z_ivl) {}
    
    std::string ToString()
    {
        return std::format("x_ivl: {}\ny_ivl: {}\nz_ivl: {}", x_ivl.ToString(), y_ivl.ToString(), z_ivl.ToString());
    }
    
    void expand(float delta)
    {
        x_ivl.expand(delta);
        y_ivl.expand(delta);
        z_ivl.expand(delta);
    }
    
    bool hit(const Ray &r, float t_min, float t_max) const
    {
        for (int i = 0; i < 3; ++i)
            {
                Interval axis_ivl;
                if (i == 0)
                    axis_ivl = x_ivl;
                else if (i == 1)
                    axis_ivl = y_ivl;
                else
                    axis_ivl = z_ivl;
                const float dirn_inv = 1.0f / r.direction[i];
                
                float t0 = (axis_ivl.low - r.origin[i]) * dirn_inv;
                float t1 = (axis_ivl.high - r.origin[i]) * dirn_inv;
                if (t0 > t1)
                    std::swap(t0, t1);
                
                if (t0 > t_min)
                    t_min = t0;
                if (t1 < t_max)
                    t_max = t1;
                
                if (t_max <= t_min)
                    return false;
            }

        return true;
    }
    
    uint32_t get_longest_axis() const
    {
        if (x_ivl.length() > y_ivl.length())
            return x_ivl.length() > z_ivl.length() ? 0 : 2;
        else
            return y_ivl.length() > z_ivl.length() ? 1 : 2;
    }
    
    static AABB union_aabbs(const std::vector<AABB> &bboxes)
    {
        Interval x_ivl = Interval{bboxes[0].x_ivl.low, bboxes[0].x_ivl.high};
        Interval y_ivl = Interval{bboxes[0].y_ivl.low, bboxes[0].y_ivl.high};
        Interval z_ivl = Interval{bboxes[0].z_ivl.low, bboxes[0].z_ivl.high};
        
        for (const auto &bbox: bboxes)
        {
            x_ivl.low = std::min(x_ivl.low, bbox.x_ivl.low);
            x_ivl.high = std::max(x_ivl.high, bbox.x_ivl.high);
            y_ivl.low = std::min(y_ivl.low, bbox.y_ivl.low);
            y_ivl.high = std::max(y_ivl.high, bbox.y_ivl.high);
            z_ivl.low = std::min(z_ivl.low, bbox.z_ivl.low);
            z_ivl.high = std::max(z_ivl.high, bbox.z_ivl.high);
        }
        
        return AABB{x_ivl, y_ivl, z_ivl};
    }
};

class Hittable
{
public:
    AABB bbox;
    
    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) = 0;
};

class BVHNode : public Hittable
{
private:
    static bool box_x_compare(const Hittable* a, const Hittable* b)
    {
        return a->bbox.x_ivl.low < b->bbox.x_ivl.low;
    }
    static bool box_y_compare(const Hittable* a, const Hittable* b)
    {
        return a->bbox.y_ivl.low < b->bbox.y_ivl.low;
    }
    static bool box_z_compare(const Hittable* a, const Hittable* b)
    {
        return a->bbox.z_ivl.low < b->bbox.z_ivl.low;
    }
    
public:
    Hittable *left, *right;

    BVHNode(Hittable* l, Hittable* r, const AABB& bbox) : left(l), right(r)
    {
        this->bbox = bbox;
    }
    ~BVHNode()
    {
        if (typeid(left) == typeid(BVHNode))
            delete left;
        if (typeid(right) == typeid(BVHNode))
            delete right;
    }
    
    bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) override
    {
        if (!bbox.hit(r, t_min, t_max))
            return false;
        
        HitRecord rec_l;
        bool hit_left = left->hit(r, t_min, t_max, rec_l);
        HitRecord rec_r;
        bool hit_right = right->hit(r, t_min, t_max, rec_r);
        if (!hit_left && !hit_right)
            return false;
        if (hit_left && !hit_right)
        {
            rec = rec_l;
            return true;
        }
        if (!hit_left && hit_right)
        {
            rec = rec_r;
            return true;
        }
        if (rec_l.t < rec_r.t)
            rec = rec_l;
        else
            rec = rec_r;
        return true;
    }
    
    // start_idx inclusive.     end_idx exclusive
    static BVHNode *construct_bvh(std::vector<Hittable *> &hittables, uint32_t start_idx, uint32_t end_idx)
    {
        if (end_idx - start_idx == 1)
            return new BVHNode{hittables[start_idx], hittables[start_idx], hittables[start_idx]->bbox};
        
        std::vector<AABB> bboxes{};
        for (uint32_t i = start_idx; i < end_idx; ++i)
            bboxes.push_back(hittables[i]->bbox);
        AABB bbox = AABB::union_aabbs(bboxes);
        uint32_t longest_axis = bbox.get_longest_axis();
        
        auto comparator = (longest_axis == 0) ? box_x_compare : (longest_axis == 1) ? box_y_compare : box_z_compare;
        std::sort(hittables.begin() + start_idx, hittables.begin() + end_idx, comparator);
        uint32_t mid = (end_idx + start_idx) / 2;
        
        return new BVHNode{construct_bvh(hittables, start_idx, mid), construct_bvh(hittables, mid, end_idx), bbox};
    }
};
