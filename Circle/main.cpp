#include "drawing.h"

#include <vector>
#include <cstdio>
#include <direct.h>
#include <cmath>

static const size_t c_imageWidth = 256;
static const size_t c_imageHeight = 256;
static const size_t c_numTimeSteps = 11;

static const float c_pi = 3.14159265f;
static const float c_twoPi = 2.0f * c_pi;

inline float LerpF(float a, float b, float t)
{
	return a + t * (b - a);
}

inline int LerpI(int a, int b, float t)
{
	return (int)(LerpF((float)a, (float)b, t) + 0.5f);
}

inline float AngleLerp(float angle1, float angle2, float t)
{
	/*
	float diff = angle2 - angle1;
	if (diff < -c_pi)
		angle2 += c_twoPi;
	else if (diff > c_pi)
		angle1 += c_twoPi;
	float result = LerpF(angle1, angle2, t);
	if (result < 0.0f)
		result += c_twoPi;
	else if (result >= c_twoPi)
		result -= c_twoPi;
	return result;
	*/

	if (angle2 < angle1)
		angle2 += c_twoPi;

	float diff = angle2 - angle1;

	if (diff <= c_pi)
	{
		return LerpF(angle1, angle2, t);
	}
	else
	{
		return LerpF(angle2, angle1 + c_twoPi, 1.0f - t);
	}
}

void DrawAndSaveInterpolationFrame(const char* fileName, float angle1, float angle2, float t)
{
	std::vector<unsigned char> imageData(c_imageWidth * c_imageHeight * 4, 255);

	DrawCircle(imageData, c_imageWidth, c_imageHeight,
		c_imageWidth / 2, c_imageHeight / 2, 110,
		0, 0, 0, 255);

	int point1x = int(std::cos(angle1) * 110.0f + c_imageWidth / 2);
	int point1y = int(std::sin(angle1) * 110.0f + c_imageWidth / 2);

	int point2x = int(std::cos(angle2) * 110.0f + c_imageWidth / 2);
	int point2y = int(std::sin(angle2) * 110.0f + c_imageWidth / 2);

	int pointAx = LerpI(point1x, point2x, t);
	int pointAy = LerpI(point1y, point2y, t);

	float angle3 = AngleLerp(angle1, angle2, t);
	int point3x = int(std::cos(angle3) * 110.0f + c_imageWidth / 2);
	int point3y = int(std::sin(angle3) * 110.0f + c_imageWidth / 2);

	DrawFilledCircle(imageData, c_imageWidth, c_imageHeight,
		point1x, point1y, 8,
		0, 255, 0, 255);

	DrawFilledCircle(imageData, c_imageWidth, c_imageHeight,
		point2x, point2y, 8,
		255, 0, 0, 255);

	DrawFilledCircle(imageData, c_imageWidth, c_imageHeight,
		pointAx, pointAy, 8,
		255, 128, 0, 255);

	DrawFilledCircle(imageData, c_imageWidth, c_imageHeight,
		point3x, point3y, 8,
		255, 0, 255, 255);

	DrawLine(imageData, c_imageWidth, c_imageHeight, c_imageWidth / 2, c_imageHeight / 2, pointAx, pointAy, 255, 128, 0, 255);
	DrawLine(imageData, c_imageWidth, c_imageHeight, c_imageWidth / 2, c_imageHeight / 2, point3x, point3y, 255, 0, 255, 255);

	// Save the image
	SavePNG(imageData, c_imageWidth, c_imageHeight, 4, fileName);
}

int main(int argc, char** argv)
{
	_mkdir("out");
	for (size_t timeSteps = 0; timeSteps < c_numTimeSteps; timeSteps++)
	{
		float t = (float)timeSteps / (float)(c_numTimeSteps - 1);
		char fileName[256];
		sprintf_s(fileName, "out/test%02zu.png", timeSteps);
		DrawAndSaveInterpolationFrame(fileName, 0.5f, 3.0f, t);
	}

	return 0;
}
