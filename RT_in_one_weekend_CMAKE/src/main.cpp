#include <iostream>
#include <fstream>
#include <format>
#include <random>
#include <vector>
#include <limits>
#include <numeric>
#include <chrono>
#include "utils.h"

using namespace std;
#pragma region declarations
void run();
#pragma endregion


random_device rd;
mt19937 gen;
const float T_MIN = 0.001f;
const float T_MAX = 1000.0f;
const unsigned int SPP = 1;

int main()
{
	try
	{
		run();
	}
	catch (const std::exception& e)
	{
		cerr << e.what() << endl;
		return 1;
	}

	std::cout << "The end!" << endl;
	return 0;
}

vec3 get_pixel_color(const ray& r, const vector<sphere>& spheres, int depth = 1)
{
	hit_record rec;
	rec.t = std::numeric_limits<float>::max();
	hit_record temp_rec;
	int hit_idx = -1;
	for (int i = 0; i < spheres.size(); i++)
		if (spheres[i].hit(r, T_MIN, T_MAX, temp_rec) && temp_rec.t < rec.t)
		{
			hit_idx = i;
			rec = temp_rec;
		}

	if (hit_idx == -1)
	{
		//return vec3{ 1.0f, 1.0f, 1.0f };
		float t = 0.5f * (r.direction.y + 1.0f);
		return vec3{ 1.0f, 1.0f, 1.0f } *(1.0f - t) + vec3{ 0.5f, 0.7f, 1.0f } *t;
	}
	else
	{
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && spheres[hit_idx].mat->scatter(r, rec, attenuation, scattered))
			return attenuation * get_pixel_color(scattered, spheres, depth + 1);
		else
			return vec3{ 0.0f, 0.0f, 0.0f };
	}
}

vector<sphere> get_random_spheres()
{
	vector<sphere> spheres{};
	uniform_real_distribution<float> dis(0.0f, 1.0f);
	uniform_real_distribution<float> dis_min1_1(-1.0f, 1.0f);
	spheres.push_back(sphere{ vec3{0.0f, -1000.0f, 0.0f}, 1000.0f, new lambertian{vec3{0.5f, 0.5f, 0.5f}} });
	for (int i = -11; i < 11; i++)
		for (int j = -11; j < 11; j++)
		{
			float choose_mat = dis(gen);
			vec3 center{ i + 0.9f * dis(gen), 0.2f, j + 0.9f * dis(gen) };
			if (norm(center - vec3{ 4.0f, 0.2f, 0.0f }) > 0.9f)
			{
				if (choose_mat < 0.8f) // diffuse
					spheres.push_back(sphere{ center, 0.2f, new lambertian{vec3{dis(gen) * dis(gen), dis(gen) * dis(gen), dis(gen) * dis(gen)}} });
				else if (choose_mat < 0.95f) // metal
					spheres.push_back(sphere{ center, 0.2f, new metal{vec3{0.5f * dis(gen), 0.5f * dis(gen), 0.5f * dis(gen)}, 0.5f * dis(gen)} });
				else // glass
					spheres.push_back(sphere{ center, 0.2f, new dielectric{1.5f} });
			}
		}
	spheres.push_back(sphere{ vec3{0.0f, 1.0f, 0.0f}, 1.0f, new dielectric{1.5f} });
	spheres.push_back(sphere{ vec3{-4.0f, 1.0f, 0.0f}, 1.0f, new lambertian{vec3{0.4f, 0.2f, 0.1f}} });
	spheres.push_back(sphere{ vec3{4.0f, 1.0f, 0.0f}, 1.0f, new metal{vec3{0.7f, 0.6f, 0.5f}, 0.0f} });

	return spheres;
}

void run()
{
	gen = mt19937(rd());
	init_utils(gen);
	uniform_real_distribution<float> dis{ 0.0f, 1.0f };

	int width = 800, height = 400;
	vec3 look_from = vec3{ 5.0f, 1.0f, 2.0f };
	vec3 look_at = vec3{ -15.0f, 0.0f, -5.0f };
	camera main_camera{ 45.0f, 2.0f, vec3{0.0f, 1.0f, 0.0f}, look_from, look_at };

	vector<vector<vec3uc>> output_pixels(height, vector<vec3uc>(width));

	// define world
	vector<sphere> spheres = get_random_spheres();

	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			vector<float> sample_Us(SPP);
			vector<float> sample_Vs(sample_Us.size());
			for (int k = 0; k < sample_Us.size(); k++)
			{
				sample_Us[k] = (float)(j + dis(gen)) / width;
				sample_Vs[k] = (float)(i + dis(gen)) / height;
			}

			vector<vec3> pixel_colors;
			for (int k = 0; k < sample_Us.size(); k++)
				pixel_colors.push_back(get_pixel_color(main_camera.get_ray(sample_Us[k], sample_Vs[k]), spheres));
			vec3 pixel_color(0.0f, 0.0f, 0.0f);
			for (vec3 col : pixel_colors)
				pixel_color += col;
			pixel_color = pixel_color / static_cast<float>(pixel_colors.size());
			unsigned char r = static_cast<unsigned char>(pixel_color.x * 255);
			unsigned char g = static_cast<unsigned char>(pixel_color.y * 255);
			unsigned char b = static_cast<unsigned char>(pixel_color.z * 255);
			//r -= r % 20;
			//g -= g % 20;
			//b -= b % 20;
			output_pixels[i][j] = vec3uc(r, g, b);
		}
		std::cout << format("Progress: {:.2f}%", 100.0f * (i + 1) / height) << endl;
	}
	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	std::cout << "Rendering time: " << std::chrono::duration_cast<chrono::seconds>(end - begin).count() << " seconds" << endl;

	string output = "";
	output += std::format("P3\n{} {}\n255\n", width, height);
	for (int i = height - 1; i >= 0; i--)
		for (int j = 0; j < width; j++)
			output += format("{} {} {}\n", output_pixels[i][j].r, output_pixels[i][j].g, output_pixels[i][j].b);
	ofstream file("render.ppm");
	file << output;
	file.close();

	// clean resources
	for (sphere& sp : spheres)
		delete sp.mat;
}
