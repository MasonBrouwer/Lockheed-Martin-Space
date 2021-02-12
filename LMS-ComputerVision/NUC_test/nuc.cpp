//////////////////////////////////////////////////////////////////////////////
/// 
/// Usage: NUC_test {input_image}.jpg {output_image}.jpg
/// 
//////////////////////////////////////////////////////////////////////////////
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "nuc_ops.h"
#include <vector>

using namespace cv;
using namespace std;

const int SIGMA_MIN = 0; //minimal std-dev of the Gaussian-weighting function
const int SIGMA_MAX = 8; //maximal std-dev of the Gaussian-weighting function
const float DELTA = 0.5; //step between two consecutive std-dev

int main(int argc, char** argv)
{
	Mat input_image;

	if (argc != 3)
	{
		cout << "Incorrect usage, please see documentation." << endl;
		return -1;
	}
	else
	{
		input_image = imread(argv[1], IMREAD_GRAYSCALE);
		if (input_image.empty())
		{
			cout << "Could not open or find the image" << endl;
			return -2;
		}
	}
	int width = input_image.size().width;
	int height = input_image.size().height;
	Mat output_image = Mat(height, width + 8 * SIGMA_MAX, 0);
	vector <int> flat_image;

	cout << "Converting image matrix..." << endl;

	// Convert Mat into 1D array
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			flat_image.push_back(input_image.at<uchar>(row, col));
		}
	}

	cout << "Fixing border..." << endl;

	// Border mirroring
	flat_image = mirror(flat_image, width, height, 4 * SIGMA_MAX);

	// MIRE stuff
	flat_image = automire(flat_image, width + 8 * SIGMA_MAX, height, SIGMA_MIN, SIGMA_MAX, DELTA);

	cout << "Restoring image matrix..." << endl;

	// Unconvert 1D array into Mat
	width = output_image.size().width;
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			output_image.at<uchar>(row, col) = flat_image[static_cast<_int64>(row) * width + col];
		}
	}

	cout << "Displaying images..." << endl;

	// Output Image before and after
	namedWindow("Input Image", WINDOW_AUTOSIZE); //WINDOW_NORMAL for resizable windows
	imshow("Input Image", input_image);
	namedWindow("Output Image", WINDOW_AUTOSIZE);
	imshow("Output Image", output_image);

	waitKey(0); // Wait for a keystroke in the window

	cout << "Finished." << endl;

	return 0;
}