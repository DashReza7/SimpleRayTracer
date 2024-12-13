#include <iostream>
#include <fstream>
#include <sstream>
#include <format>
#include <stack>
#include "Scene.h"

void Scene::parse_scene_file(const char *input_filename, std::string &output_filename)
{
    std::ifstream file{input_filename};
    if (!file)
        throw std::runtime_error(std::format("Error opening scene file: {}", input_filename));
    
    // set defaults
    width = 640;
    height = 480;
    maxdepth = 5;
    output_filename = "raytrace.png";
    float atten_const = 1.0f;
    float atten_linear = 0.0f;
    float atten_quadratic = 0.0f;
    int vertex_idx = 0;
    std::vector<vec3> vertexes;
    int vertnormal_idx = 0;
    std::vector<std::pair<vec3, vec3>> vertnormals;
    vec3 ambient = vec3{0.0f, 0.0f, 0.0f};
    vec3 diffuse = vec3{0.0f, 0.0f, 0.0f};
    vec3 specular = vec3{0.0f, 0.0f, 0.0f};
    float shininess = 0.0f;
    vec3 emission = vec3{0.0f, 0.0f, 0.0f};
    this->anti_aliasing = false;
    this->samples_per_pixel = 5;
    this->gamma_correction = false;
    float fuzziness = 0.0f;
    float dielectric_idx = 0.0f;
    // allow only one directional light
    bool is_dirn_light_set = false;
    this->use_BVH = false;
    std::string texture;
    vec3 texture_color1;
    vec3 texture_color2;
    float texture_scale;
    
    // keep track of transformations
    mat4 cur_transform = mat4{1.0f};
    mat4 cur_inv_transform = mat4{1.0f};
    std::stack<mat4> transform_stack;
    std::stack<mat4> inv_transform_stack;
    
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss{line};
        std::vector<std::string> tokens;
        
        std::string token;
        while (iss >> token)
            tokens.push_back(token);
        
        if (tokens.empty() || tokens[0][0] == '#')
            continue;
        
        std::string command = tokens[0];
        if (command == "size")
        {
            width = std::stoi(tokens[1]);
            height = std::stoi(tokens[2]);
        }
        else if (command == "maxdepth")
        {
            maxdepth = std::stoi(tokens[1]);
        }
        else if (command == "output")
        {
            output_filename = tokens[1];
        }
        else if (command == "camera")
        {
            vec3 look_from = vec3{std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])};
            vec3 look_at = vec3{std::stof(tokens[4]), std::stof(tokens[5]), std::stof(tokens[6])};
            vec3 up = normalize(vec3{std::stof(tokens[7]), std::stof(tokens[8]), std::stof(tokens[9])});
            float fov = std::stof(tokens[10]);
            main_camera = new Camera{fov, static_cast<float>(width) / static_cast<float>(height), up, look_from,
                                     look_at};
        }
        else if (command == "sphere")
        {
            vec3 center{std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])};
            float radius = std::stof(tokens[4]);
            Texture *tex = nullptr;
            if (texture == "solid")
                tex = new SolidColor(texture_color1);
            else if (texture == "checker")
                tex = new CheckerTexture(texture_color1, texture_color2, texture_scale);
            else if (texture == "image")
                tex = new ImageTexture("../Resources/earthmap.jpg");
            spheres.emplace_back(center, radius,
                                 Material{diffuse, specular, shininess, ambient, emission, fuzziness, dielectric_idx, tex},
                                 cur_transform, cur_inv_transform);
        }
        else if (command == "sphereMoving")
        {
            vec3 center1{std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])};
            vec3 center2{std::stof(tokens[4]), std::stof(tokens[5]), std::stof(tokens[6])};
            float radius = std::stof(tokens[7]);
            Texture *tex = nullptr;
            if (texture == "solid")
                tex = new SolidColor(texture_color1);
            else if (texture == "checker")
                tex = new CheckerTexture(texture_color1, texture_color2, texture_scale);
            else if (texture == "image")
                tex = new ImageTexture("../Resources/earthmap.jpg");
            spheres.emplace_back(center1, center2, radius,
                                 Material{diffuse, specular, shininess, ambient, emission, fuzziness, dielectric_idx, tex},
                                 cur_transform, cur_inv_transform);
        }
        else if (command == "maxverts")
        {
            vertexes = std::vector<vec3>{std::stoul(tokens[1])};
        }
        else if (command == "maxvertnormals")
        {
            vertnormals = std::vector<std::pair<vec3, vec3>>{std::stoul(tokens[1])};
        }
        else if (command == "vertex")
        {
            vertexes[vertex_idx++] = vec3{std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])};
        }
        else if (command == "vertexnormal")
        {
            vertnormals[vertnormal_idx++] = std::pair<vec3, vec3>{
                    vec3{std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])},
                    vec3{std::stof(tokens[4]), std::stof(tokens[5]), std::stof(tokens[6])}};
        }
        else if (command == "tri")
        {
            // TODO: apply texture and probably other things you've done for the sphere
            vec4 tr_v1 = cur_transform * vec4{vertexes[std::stoi(tokens[1])], 1.0f};
            vec4 tr_v2 = cur_transform * vec4{vertexes[std::stoi(tokens[2])], 1.0f};
            vec4 tr_v3 = cur_transform * vec4{vertexes[std::stoi(tokens[3])], 1.0f};
            triangles.emplace_back(vec3{tr_v1.x, tr_v1.y, tr_v1.z}, vec3{tr_v2.x, tr_v2.y, tr_v2.z},
                                   vec3{tr_v3.x, tr_v3.y, tr_v3.z},
                                   Material{diffuse, specular, shininess, ambient, emission, fuzziness,
                                            dielectric_idx, nullptr});
        }
        else if (command == "trinormal")
        {
            throw std::runtime_error("trinormal not implemented yet!");
        }
        else if (command == "translate")
        {
            vec3 direction = vec3{std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])};
            cur_transform = cur_transform * get_translation_matrix(direction);
            cur_inv_transform = get_translation_matrix(direction * -1) * cur_inv_transform;
        }
        else if (command == "rotate")
        {
            vec3 axis = normalize(vec3{std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])});
            float angle = std::stof(tokens[4]);
            while (angle >= 360.0f)
                angle -= 360.0f;
            while (angle <= -360.0f)
                angle += 360.0f;
            
            cur_transform = cur_transform * get_rotation_matrix(axis, radians(angle));
            cur_inv_transform = get_rotation_matrix(axis, radians(-angle)) * cur_inv_transform;
        }
        else if (command == "scale")
        {
            vec3 scale_vec = vec3{std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])};
            cur_transform = cur_transform * mat4{vec4{scale_vec, 1.0f}};
            cur_inv_transform =
                    mat4{vec4{1.0f / scale_vec[0], 1.0f / scale_vec[1], 1.0f / scale_vec[2], 1.0f}} * cur_inv_transform;
        }
        else if (command == "pushTransform")
        {
            transform_stack.push(cur_transform);
            inv_transform_stack.push(cur_inv_transform);
        }
        else if (command == "popTransform")
        {
            cur_transform = transform_stack.top();
            transform_stack.pop();
            
            cur_inv_transform = inv_transform_stack.top();
            inv_transform_stack.pop();
        }
        else if (command == "directional")
        {
            if (is_dirn_light_set)
                throw std::runtime_error("multiple directional lights not implemented yet.");
            
            vec3 direction_to_light = vec3{std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])};
            direction_to_light = normalize(cur_transform * direction_to_light);
            
            vec3 color = vec3{std::stof(tokens[4]), std::stof(tokens[5]), std::stof(tokens[6])};
            dirn_light = DirnLight{color, direction_to_light};
            
            is_dirn_light_set = true;
        }
        else if (command == "point")
        {
            vec4 tmp_position =
                    cur_transform * vec4{std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]), 1.0f};
            vec3 position = vec3{tmp_position.x / tmp_position.w, tmp_position.y / tmp_position.w,
                                 tmp_position.z / tmp_position.w};
            vec3 color = vec3{std::stof(tokens[4]), std::stof(tokens[5]), std::stof(tokens[6])};
            pointlights.emplace_back(color, position, atten_const, atten_linear, atten_quadratic);
        }
        else if (command == "attenuation")
        {
            atten_const = std::stof(tokens[1]);
            atten_linear = std::stof(tokens[2]);
            atten_quadratic = std::stof(tokens[3]);
        }
        else if (command == "ambient")
        {
            ambient.x = std::stof(tokens[1]);
            ambient.y = std::stof(tokens[2]);
            ambient.z = std::stof(tokens[3]);
        }
        else if (command == "diffuse")
        {
            diffuse.x = std::stof(tokens[1]);
            diffuse.y = std::stof(tokens[2]);
            diffuse.z = std::stof(tokens[3]);
        }
        else if (command == "specular")
        {
            specular.x = std::stof(tokens[1]);
            specular.y = std::stof(tokens[2]);
            specular.z = std::stof(tokens[3]);
        }
        else if (command == "shininess")
        {
            shininess = std::stof(tokens[1]);
        }
        else if (command == "emission")
        {
            emission.x = std::stof(tokens[1]);
            emission.y = std::stof(tokens[2]);
            emission.z = std::stof(tokens[3]);
        }
        else if (command == "anti_aliasing")
        {
            if (tokens[1] == "true")
                this->anti_aliasing = true;
            else if (tokens[1] == "false")
                this->anti_aliasing = false;
        }
        else if (command == "samples_per_pixel")
        {
            this->samples_per_pixel = std::stoi(tokens[1]);
        }
        else if (command == "gamma_correction")
        {
            if (tokens[1] == "true")
                this->gamma_correction = true;
            else if (tokens[1] == "false")
                this->gamma_correction = false;
        }
        else if (command == "fuzziness")
        {
            fuzziness = std::stof(tokens[1]);
        }
        else if (command == "dielectric_idx")
        {
            dielectric_idx = std::stof(tokens[1]);
        }
        else if (command == "use_bvh")
        {
            if (tokens[1] == "true")
                this->use_BVH = true;
            else if (tokens[1] == "false")
                this->use_BVH = false;
        }
        else if (command == "texture")
        {
            if (tokens[1] == "null")
                texture = "";
            else if (tokens[1] == "solid")
            {
                texture = "solid";
                texture_color1 = vec3{std::stof(tokens[2]), std::stof(tokens[3]), std::stof(tokens[4])};
            }
            else if (tokens[1] == "checker")
            {
                texture = "checker";
                texture_color1 = vec3{std::stof(tokens[2]), std::stof(tokens[3]), std::stof(tokens[4])};
                texture_color2 = vec3{std::stof(tokens[5]), std::stof(tokens[6]), std::stof(tokens[7])};
                texture_scale = std::stof(tokens[8]);
            }
            else if (tokens[1] == "image")
            {
                texture = "image";
            }
        }
        else
            throw std::runtime_error(std::format("Invalid command: {}", command));
    }
    
    file.close();
}

