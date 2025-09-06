#define _CRT_SECURE_NO_WARNINGS // for STB

#include "drawing.h"
#include <cstdio>
#include <cmath>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb/stb_image_write.h"

// Helper function to set a pixel with bounds checking
// Assumes a 4 channel RGBA image
static inline void SetPixel(std::vector<unsigned char>& imageData, int width, int height, int x, int y,
                    unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        int index = (y * width + x) * 4;
        imageData[index + 0] = r; // Red
        imageData[index + 1] = g; // Green
        imageData[index + 2] = b; // Blue
        imageData[index + 3] = a; // Alpha
    }
}

// Assumes a 4 channel RGBA image
void DrawCircle(std::vector<unsigned char>& imageData, int width, int height,
                int centerX, int centerY, int radius,
                unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    // Validate image data size
    if (imageData.size() < static_cast<size_t>(width * height * 4))
    {
        printf("Error: Image data too small for specified dimensions\n");
        return;
    }

    // Validate radius
    if (radius < 0)
    {
        printf("Error: Invalid radius\n");
        return;
    }

    if (radius == 0)
    {
        SetPixel(imageData, width, height, centerX, centerY, r, g, b, a);
        return;
    }

    // Bresenham's circle algorithm
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    // Draw initial points and their symmetric counterparts
    while (x <= y)
    {
        // Draw 8 symmetric points
        SetPixel(imageData, width, height, centerX + x, centerY + y, r, g, b, a);
        SetPixel(imageData, width, height, centerX - x, centerY + y, r, g, b, a);
        SetPixel(imageData, width, height, centerX + x, centerY - y, r, g, b, a);
        SetPixel(imageData, width, height, centerX - x, centerY - y, r, g, b, a);
        SetPixel(imageData, width, height, centerX + y, centerY + x, r, g, b, a);
        SetPixel(imageData, width, height, centerX - y, centerY + x, r, g, b, a);
        SetPixel(imageData, width, height, centerX + y, centerY - x, r, g, b, a);
        SetPixel(imageData, width, height, centerX - y, centerY - x, r, g, b, a);

        // Update decision parameter
        if (d < 0)
        {
            d = d + 4 * x + 6;
        }
        else
        {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

// Assumes a 4 channel RGBA image
void DrawLine(std::vector<unsigned char>& imageData, int width, int height,
              int x0, int y0, int x1, int y1,
              unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    // Validate image data size
    if (imageData.size() < static_cast<size_t>(width * height * 4))
    {
        printf("Error: Image data too small for specified dimensions\n");
        return;
    }

    // Bresenham's line algorithm
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    int x = x0;
    int y = y0;

    while (true)
    {
        // Check if pixel is within bounds
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
            // Calculate pixel index (RGBA format)
            int index = (y * width + x) * 4;
            imageData[index + 0] = r; // Red
            imageData[index + 1] = g; // Green
            imageData[index + 2] = b; // Blue
            imageData[index + 3] = a; // Alpha
        }

        // Check if we've reached the end point
        if (x == x1 && y == y1) break;

        // Calculate next point
        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y += sy;
        }
    }
}

// Assumes a 4 channel RGBA image - draws a filled circle
void DrawFilledCircle(std::vector<unsigned char>& imageData, int width, int height,
                      int centerX, int centerY, int radius,
                      unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    // Validate image data size
    if (imageData.size() < static_cast<size_t>(width * height * 4))
    {
        printf("Error: Image data too small for specified dimensions\n");
        return;
    }

    // Validate radius
    if (radius < 0)
    {
        printf("Error: Invalid radius\n");
        return;
    }

    if (radius == 0)
    {
        SetPixel(imageData, width, height, centerX, centerY, r, g, b, a);
        return;
    }

    // Use Bresenham's circle algorithm to find boundary points and fill horizontally
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y)
    {
        // Fill horizontal lines between symmetric points
        // Top and bottom halves
        for (int i = centerX - x; i <= centerX + x; i++)
        {
            SetPixel(imageData, width, height, i, centerY + y, r, g, b, a); // Bottom
            SetPixel(imageData, width, height, i, centerY - y, r, g, b, a); // Top
        }
        
        // Fill horizontal lines for the wider parts
        if (x != y) // Avoid drawing the same line twice when x == y
        {
            for (int i = centerX - y; i <= centerX + y; i++)
            {
                SetPixel(imageData, width, height, i, centerY + x, r, g, b, a); // Bottom-wide
                SetPixel(imageData, width, height, i, centerY - x, r, g, b, a); // Top-wide
            }
        }

        // Update decision parameter
        if (d < 0)
        {
            d = d + 4 * x + 6;
        }
        else
        {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

bool SavePNG(const std::vector<unsigned char>& imageData, int width, int height, int channels, const char* filename)
{
    // Validate input parameters
    if (imageData.empty() || width <= 0 || height <= 0 || channels <= 0)
    {
        printf("Error: Invalid image parameters\n");
        return false;
    }

    // Check if we have enough data
    size_t expectedSize = static_cast<size_t>(width) * height * channels;
    if (imageData.size() < expectedSize)
    {
        printf("Error: Not enough image data. Expected: %zu, Got: %zu\n", expectedSize, imageData.size());
        return false;
    }

    // Use stb_image_write to save the PNG
    int result = stbi_write_png(filename, width, height, channels,
                               imageData.data(),
                               width * channels);

    if (result == 0)
    {
        printf("Error: Failed to write PNG file: %s\n", filename);
        return false;
    }

    printf("Successfully saved PNG: %s\n", filename);
    return true;
}
