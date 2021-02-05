
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "nuc_ops.h"
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

// From borders.cpp from srcmire_2
vector<int> mirror(vector<int> image, int width, int height, int perimiter)
{
	vector<int> output_image((static_cast<_int64>(width) * 2 * perimiter) * height); // 1D image with added perimeter

	// copy image into the center of output_image
	for (int column = 0; column < width; column++)
	{
		for (int row = 0; row < height; row++)
		{
			output_image[static_cast<_int64>(row) * (static_cast<_int64>(width) + 2 * static_cast<_int64>(perimiter)) + perimiter + column]
				= image[static_cast<_int64>(row) * width + column];
		}
	}
	
	// copy left side
	for (int column = -perimiter; column < 0; column++)
	{
		for (int row = 0; row < height; row++)
		{
			output_image[static_cast<_int64>(row) * (static_cast<_int64>(width) + 2 * static_cast<_int64>(perimiter)) + perimiter + column]
				= image[static_cast<_int64>(row) * width - column];
		}
	}
	
	// copy right side
	for (int column = width; column < width + perimiter; column++)
	{
		for (int row = 0; row < height; row++)
		{
			output_image[static_cast<_int64>(row) * (static_cast<_int64>(width) + 2 * static_cast<_int64>(perimiter)) + perimiter + column]
				= image[static_cast<_int64>(row) * width +(2 * static_cast<_int64>(width) - column - 1)];
		}
	}
	
	return output_image;
}