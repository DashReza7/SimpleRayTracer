#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <array>
#include "FreeImage.h"

class RTWImage
{
public:
    
    static std::vector<std::vector<std::array<uint8_t, 3>>> load(const char *filename)
    {
        FreeImage_Initialise();
        FIBITMAP *bitmap = FreeImage_Load(FIF_JPEG, filename);
        if (!bitmap)
        {
            std::cerr << "Failed to load image: " << filename << std::endl;
            FreeImage_DeInitialise();
            exit(EXIT_FAILURE);
        }
        
        FIBITMAP *rgbBitmap = FreeImage_ConvertTo24Bits(bitmap);
        FreeImage_Unload(bitmap);
        if (!rgbBitmap)
        {
            std::cerr << "Failed to convert image to 24-bit." << std::endl;
            FreeImage_DeInitialise();
            exit(EXIT_FAILURE);
        }
        
        // Get image dimensions
        int width = FreeImage_GetWidth(rgbBitmap);
        int height = FreeImage_GetHeight(rgbBitmap);
//        int pitch = FreeImage_GetPitch(rgbBitmap); // Bytes per row, includes padding
        
        std::vector<std::vector<std::array<uint8_t, 3>>> imageData(height, std::vector<std::array<uint8_t, 3>>(width));
        
        // Read pixel data
        for (int y = 0; y < height; ++y)
        {
            BYTE *row = FreeImage_GetScanLine(rgbBitmap, y); // Bitmap rows are stored bottom-up
            for (int x = 0; x < width; ++x)
            {
                imageData[y][x] = {row[x * 3 + FI_RGBA_RED],     // Red
                                   row[x * 3 + FI_RGBA_GREEN],   // Green
                                   row[x * 3 + FI_RGBA_BLUE]};  // Blue
            }
        }
        FreeImage_Unload(rgbBitmap);
        FreeImage_DeInitialise();
        
        return imageData;
    }
};
