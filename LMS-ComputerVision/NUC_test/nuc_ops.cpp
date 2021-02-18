
// TODO: optimize these functions (many don't need to create intermediate image vectors, might be faster to modify the original image arg than to return an image vector)

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

float M_PI = 3.14159265358979323846;

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

Mat mirror_cv(Mat image, int width, int height, int perimiter)
{
	// Create output Mat and clone input mat to avoid changing it
	Mat output_image = Mat(height, width + 2 * perimiter, 0);
	Mat temp = image.clone();

	// Obtain left and right border components as well as the image itself
	Mat left = temp(Range::all(), Range(0, perimiter));
	Mat middle = image.clone();
	Mat right = temp(Range::all(), Range(width - perimiter, width));

	// Mirror border components
	flip(left, left, +1);
	flip(right, right, +1);

	// Concatenate images together horizontally
	hconcat(left, middle, left);
	hconcat(left, right, output_image);

	return output_image;
}

vector<int> automire(vector<int> image, int width, int height, int SIGMA_MIN, int SIGMA_MAX, float DELTA)
{
	//vector<int> output_image(static_cast<_int64>(width) * height);
	vector<int> temp_image(static_cast<_int64>(width) * height);
	float sigma_current;
	float sigma_best;
	float TV_current;
	float TV_min;

	cout << "Initializing MIRE algorithm..." << endl;

	// Initialize
	sigma_best = SIGMA_MIN;
	for (int i = 0; i < width * height; i++)
	{
		temp_image[i] = image[i];
	}
	if (SIGMA_MIN == 0)
	{
		TV_current = TV_column_norm(temp_image, width, height, 4 * SIGMA_MAX);
	}
	else 
	{
		TV_current = TV_column_norm(mire(temp_image, SIGMA_MIN, width, height), width, height, 4 * SIGMA_MAX);
	}
	TV_min = TV_current;
	sigma_current = SIGMA_MIN;

	cout << "Guessing best sigma..." << endl;

	// Guess the best Sigma
	/*
	int T = round((SIGMA_MAX - SIGMA_MIN) / DELTA)+1;
	for (int i = 0; i < width * height; i++)
	{
		for (int j = 0; j < width * height; j++)
		{
			temp_image[j] = image[j];
		}

		TV_current = TV_column_norm(mire(image, sigma_current, width, height), width, height, 4 * SIGMA_MAX);

		if (TV_current < TV_min)
		{
			sigma_best = sigma_current;
			TV_min = TV_current;
		}

		cout << "current sigma: " << sigma_current << endl;

		sigma_current = sigma_current + DELTA;
	}
	*/
	sigma_best = 2.5;

	cout << "Applying MIRE with best sigma..." << endl;

	// Apply MIRE with the best sigma
	if (sigma_best != 0)
	{
		image = mire(image, sigma_best, width, height);
	}
	cout << "SIGMA_BEST: " << sigma_best << endl;

	return image;
}

vector<int> mire(vector<int> image, float sigma, int width, int height)
{
	cout << "test1" << endl;

	vector<vector<int>> v;
	vector<vector<int>> c_sorted;
	c_sorted = column_sorting(image, width, height); // takes a long time

	cout << "test2" << endl;

	v = target_histogram(c_sorted, width, height, sigma);
	cout << "test3" << endl;
	int N = round(4 * sigma);
	for (int column = N; column < width - N; column++)
	{
		image = specify_column(image, width, height, column, v[static_cast<_int64>(column) - N]); // takes a long time
	}

	cout << "test4" << endl;

	return(image);
}

Mat mire_cv(Mat image, float sigma, int width, int height)
{
	// Sort columns
	Mat sorted = Mat(height, image.size().width, 0);
	cv::sort(image, sorted, SORT_EVERY_COLUMN + SORT_ASCENDING); // 'sorted' now holds sorted columns

	// Make target histogram
	Mat v = target_histogram_cv(sorted, image.size().width, height, sigma); // runtime could be improved

	// Modify image
	int N = round(4 * sigma);
	for (int column = N; column < image.size().width - N; column++)
	{
		image = specify_column_cv(image, image.size().width, height, column, v.col(column - N), sorted.col(column)); // runtime could be improved
	}

	// Return image with borders cropped
	return image(Range(0, height), Range(N, width + N));
}

// Compute TV-norm among columns (avoiding columns added by mirroring)
float TV_column_norm(vector<int> image, int width, int height, int B)
{
	float TV = 0;
	for (int column = B; column < width - B; column++)
	{
		for (int row = 0; row < height; row++)
		{
			TV = TV + abs((image[static_cast<_int64>(row) * width + column + 1]) - 
				image[static_cast<_int64>(row) * width + column]);
		}
	}
	return TV;
}

float gaussian(int x, float sigma)
{
	return(1 / (sigma * sqrt(2 * M_PI)) * exp(-(x*x) / (2 * sigma*sigma)));
}

vector<int> specify_column(vector<int> image, int width, int height, int column, vector<int> target_values)
{
	// sort v_column and put it in column_sorted
	vector<int> column_sorted;
	for (int i = 0; i < height; i++)
	{
		column_sorted.push_back(image[i * static_cast<_int64>(width) + column]);
	}
	sort(column_sorted.begin(), column_sorted.end());
	// for each entry v_column(i) find indice j of column_sorted such that solumn_sorted(j) == v_column(i) 
	// change v_column(j) to v_column(j) = target_values(j)
	for (int i = 0; i < height; i++)
	{
		int temp = image[i * static_cast<_int64>(width) + column];
		for (int j = 0; j < height; j++)
		{
			if (temp == column_sorted[j])
			{
				image[i * static_cast<_int64>(width) + column] = target_values[j];
			}
		}
	}
	return image;
}

Mat specify_column_cv(Mat image, int width, int height, int column, Mat target_values, Mat sorted_column)
{
	// for each entry v_column(i) find indice j of column_sorted such that solumn_sorted(j) == v_column(i) 
	// change v_column(j) to v_column(j) = target_values(j)
	for (int i = 0; i < height; i++)
	{
		int temp = image.at<uchar>(i, column);
		for (int j = 0; j < height; j++)
		{
			if (temp == sorted_column.at<uchar>(j, 0))
			{
				image.at<uchar>(i, column) = target_values.at<uchar>(j, 0);
			}
		}
	}

	return image;
}

vector<vector<int>> column_sorting(vector<int> image, int width, int height)
{
	vector<vector<int>> V_HISTOS;
	for (int i = 0; i < width; i++)
	{
		V_HISTOS.push_back(histo_column(image, width, height, i));
	}
	return(V_HISTOS);
}

vector<int> histo_column(vector<int> image, int width, int height, int column)
{
	vector<int> v;
	for (int col = 0; col < height; col++)
	{
		v.push_back(image[static_cast<_int64>(col) * width + column]);
	}
	sort(v.begin(), v.end());
	return(v);
}

vector<vector<int>> target_histogram(vector<vector<int>> V_HISTOS, int width, int height, float sigma)
{
	vector<vector<int>> FINAL;
	int N = round(4 * sigma);
	for (int center = N; center < width - N; center++)
	{
		vector<int> v;
		for (int vline = 0; vline < height; vline++)
		{
			float temp = 0;
			for (int vcolumn = center - N; vcolumn < center + N + 1; vcolumn++)
			{
				temp = temp + gaussian((center - vcolumn), sigma) * (V_HISTOS[vcolumn][vline]);
			}
			v.push_back(temp);
		}
		FINAL.push_back(v);
	}
	return(FINAL);
}

Mat target_histogram_cv(Mat V_HISTOS, int width, int height, float sigma)
{
	Mat final = Mat(height, 0, 0);
	int N = round(4 * sigma);
	for (int center = N; center < width - N; center++)
	{
		Mat v = Mat(height, 1, 0);
		for (int vline = 0; vline < height; vline++)
		{
			float temp = 0;
			for (int vcolumn = center - N; vcolumn < center + N + 1; vcolumn++)
			{
				temp = temp + gaussian((center - vcolumn), sigma) * (V_HISTOS.at<uchar>(vline, vcolumn));
			}
			v.at<uchar>(vline, 0) = temp;
		}
		hconcat(final, v, final);
	}
	return(final);
}