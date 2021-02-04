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

using namespace cv;
using namespace std;

const int SIGMA_MIN = 0; // minimal std-dev of the Gaussian-weighting function
const int SIGMA_MAX = 8; //maximal std-dev of the Gaussian-weighting function
const float DELTA = 0.5; //step between two consecutive std-dev

int main(int argc, char** argv)
{
	Mat input_image;
	Mat output_image;

	if (argc != 3)
	{
		cout << "Incorrect usage, please see documentation." << endl;
		return -1;
	}
	else
	{
		input_image = imread(argv[1], IMREAD_COLOR);
		if (input_image.empty())
		{
			cout << "Could not open or find the image" << endl;
			return -2;
		}
	}
	output_image = input_image;
	int height = output_image.size().height;
	int width = output_image.size().width;

	// Border stuff
	output_image = borders(output_image, width, height, 4 * SIGMA_MAX);

	// MIRE stuff

	// Output Image before and after
	namedWindow("Input Image", WINDOW_NORMAL);
	imshow("Input Image", input_image);
	namedWindow("Output Image", WINDOW_NORMAL);
	imshow("Output Image", output_image);

	waitKey(0); // Wait for a keystroke in the window

	cout << "Finished." << endl;

	return 0;
}