#pragma once

#include "util/VecMath.h"
#include "FreeImage.h"

class Film
{
private:
    uint32_t width, height;
    std::vector<unsigned char> pixels{};
    
public:
    Film(uint32_t w, uint32_t h) : width(w), height(h)
    {
        pixels = std::vector<unsigned char>(width * height * 3);
    }
    
    void commit(uint32_t x, uint32_t y, vec3 color)
    {
        auto r = static_cast<unsigned char>(color.x * 255.99f);
        auto g = static_cast<unsigned char>(color.y * 255.99f);
        auto b = static_cast<unsigned char>(color.z * 255.99f);
        
        pixels[(y * width + x) * 3 + 0] = r;
        pixels[(y * width + x) * 3 + 1] = g;
        pixels[(y * width + x) * 3 + 2] = b;
    }
    
    void write_to_file_png(const char* output_filename)
    {
        FreeImage_Initialise();
        FIBITMAP* bitmap = FreeImage_Allocate(static_cast<int>(width), static_cast<int>(height), 24);
        if (!bitmap)
        {
            std::cerr << "Failed to allocate bitmap" << std::endl;
            FreeImage_DeInitialise();
            return;
        }
        
        for (int y = 0; y < height; y++)
        {
            BYTE* bits = FreeImage_GetScanLine(bitmap, y);
            for (int x = 0; x < width; x++)
            {
                bits[FI_RGBA_RED] = pixels[(y * width + x) * 3];
                bits[FI_RGBA_GREEN] = pixels[(y * width + x) * 3 + 1];
                bits[FI_RGBA_BLUE] = pixels[(y * width + x) * 3 + 2];
                bits += 3;
            }
        }
        if (!FreeImage_Save(FIF_PNG, bitmap, output_filename))
            std::cerr << "Failed to save PNG file: " << output_filename << std::endl;
        
        FreeImage_Unload(bitmap);
        FreeImage_DeInitialise();
    }
};
