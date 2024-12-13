#pragma  once

#include <format>
#include <numeric>
#include <vector>
#include "Shape.h"
#include "Light.h"
#include "Film.h"
#include "Camera.h"

const float T_MIN = 0.001f;
const float T_MAX = 1000.0f;

class Scene
{
public:
    uint32_t width = 0, height = 0;
    Camera *main_camera = nullptr;
    uint32_t maxdepth = 5;
    std::vector<Sphere> spheres{};
    std::vector<Triangle> triangles{};
    std::vector<Shape *> shapes{};
    DirnLight dirn_light{};
    std::vector<PointLight> pointlights{};
    BVHNode* bvh;
    bool anti_aliasing;
    uint32_t samples_per_pixel;
    bool gamma_correction;
    bool use_BVH;
    
    Scene () = default;
    
    void parse_scene_file (const char *input_filename, std::string& output_filename);
    
    void init ()
    {
        for (auto &sphere: spheres)
            shapes.push_back(&sphere);
        for (auto &triangle: triangles)
            shapes.push_back(&triangle);
        
        // TODO: clean this
        std::vector<Hittable*> objs{};
        for (auto &triangle: triangles)
            objs.push_back(&triangle);
        for (auto &sphere: spheres)
            objs.push_back(&sphere);
        
        bvh = BVHNode::construct_bvh(objs, 0, objs.size());
    }
    
    void terminate () const
    {
        delete main_camera;
        delete bvh;
        for (Shape* s : shapes)
            delete s->mat.texture;
    }
};

