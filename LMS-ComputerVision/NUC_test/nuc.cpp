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
#include <chrono>

using namespace cv;
using namespace std;
using namespace std::chrono;

const int SIGMA = 4;

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
	Mat output_image = Mat(height, width + 8 * SIGMA, 0);
	vector <int> flat_image;
	
	cout << "Border mirroring..." << endl;

	auto start = high_resolution_clock::now(); //benchmarking

	// Border mirroring (runtime 600)
	output_image = mirror_cv(input_image, width, height, 4 * SIGMA);

	cout << "Running MIRE algorithm..." << endl;

	// MIRE stuff (runtime 6,760,000)
	output_image = mire_cv(output_image, SIGMA, width, height);

	// finish benchmarking and return runtime
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "Total runtime: " << duration.count() << " microseconds" << endl;

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