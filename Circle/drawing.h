#pragma once

#include <vector>

// Assumes a 4 channel RGBA image
void DrawCircle(std::vector<unsigned char>& imageData, int width, int height,
                int centerX, int centerY, int radius,
                unsigned char r, unsigned char g, unsigned char b, unsigned char a);

// Assumes a 4 channel RGBA image
void DrawFilledCircle(std::vector<unsigned char>& imageData, int width, int height,
                      int centerX, int centerY, int radius,
                      unsigned char r, unsigned char g, unsigned char b, unsigned char a);

// Assumes a 4 channel RGBA image
void DrawLine(std::vector<unsigned char>& imageData, int width, int height,
              int x0, int y0, int x1, int y1,
              unsigned char r, unsigned char g, unsigned char b, unsigned char a);

// Function to save a png
bool SavePNG(const std::vector<unsigned char>& imageData, int width, int height, int channels, const char* filename);
