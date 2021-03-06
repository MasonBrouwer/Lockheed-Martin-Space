#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

using namespace cv;
using namespace std;

vector<int> mirror(vector<int> image, int width, int height, int perimiter);

Mat mirror_cv(Mat image, int width, int height, int perimiter);

vector<int> automire(vector<int> image, int width, int height, int SIGMA_MIN, int SIGMA_MAX, float DELTA);

vector<int> mire(vector<int> image, float sigma, int width, int height);

Mat mire_cv(Mat image, float sigma, int width, int height);

float TV_column_norm(vector<int> image, int width, int height, int B);

float gaussian(int x, float sigma);

vector<int> specify_column(vector<int> image, int width, int height, int column, vector<int> target_values);

Mat specify_column_cv(Mat image, int width, int height, int column, Mat target_values, Mat sorted_column);

vector<vector<int>> column_sorting(vector<int> image, int width, int height);

vector<int> histo_column(vector<int> image, int width, int height, int column);

vector<vector<int>> target_histogram(vector<vector<int>> V_HISTOS, int width, int height, float sigma);

Mat target_histogram_cv(Mat V_HISTOS, int width, int height, float sigma);