//////////////////////////////////////////////////////////////////////////////
/// Camera Calibration and Undistortion
//////////////////////////////////////////////////////////////////////////////
///
/// Temporary saved commands: 
/// cd C:\Users\mason\source\repos\LMS_ComputerVision\x64\Debug
/// LMS_ComputerVision chessboard_distorted.jpg cabin_distorted.jpg
/// 
/// Relevant OpenCV documenatation:
/// https://docs.opencv.org/3.4/dc/dbb/tutorial_py_calibration.html
/// https://docs.opencv.org/3.4/da/d54/group__imgproc__transform.html#ga69f2545a8b62a6b0fc2ee060dc30559d
/// https://docs.opencv.org/3.4/d9/d0c/group__calib3d.html#ga93efa9b0aa890de240ca32b11253dd4a
/// https://docs.opencv.org/3.4/dd/d1a/group__imgproc__feature.html#ga354e0d7c86d0d9da75de9b9701a9a87e
/// 
/// Usage:
///     Choose one or both options:
///         -cal : Use all images in the same directory as the executable with the naming scheme
///                cal_*.jpg for calibration. Will save calibration information in calibration.xml.
///         -und : Undistort image following this argument. If no camera calibration is available,
///                an error will be thrown.
///     Use both commands to calibrate camera and distort image.
///         ex: 
///             Undistort -cal -und {distortedImage}.jpg
/// 
//////////////////////////////////////////////////////////////////////////////
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <chrono>
#include <iostream>

using namespace cv;
using namespace std;
using namespace std::chrono;

// variables for refining corner calculations. Useful for benchmarking
int corner_size = 10;
int corner_count = 30;
float corner_eps = 0.1;

Mat distorted_image;
Mat undistorted_image;
Mat camera_matrix, distorsion_coefficients;

int main(int argc, char** argv)
{
    auto start = high_resolution_clock::now(); //benchmarking

    bool undistort_flag = false;
    bool calibrate_flag = false;

    /// process arguments ///
    if (argc > 4 || argc < 2)
    {
        cout << "Incorrect usage, please see documentation." << endl;
        return -1;
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "-und") == 0)
            {
                undistort_flag = true;
                i++;

                cout << "Opening " << argv[i] << " for undistortion..." << endl;

                distorted_image = imread(argv[i], IMREAD_COLOR); //TODO: catch errors
                if (distorted_image.empty())
                {
                    cout << "Could not open or find the image" << endl;
                    return -2;
                }
            }
            else if (strcmp(argv[i], "-cal") == 0)
            {
                calibrate_flag = true;
            }
            else
            {
                cout << "Unknown argument: " << argv[i] << endl;
                return -3;
            }
        }
    }

    /// Calibrate camera ///
    if (calibrate_flag)
    {
        // dimensions of interrior corners of chessboard
        int board_height = 6;
        int board_width = 6;
        Size board_size(board_height, board_width);
        Size image_size;

        Mat grey;
        vector<vector<Point3f>> object_points;
        vector<vector<Point2f>> image_points;

        vector<Mat> distorted_chessboards;
        vector<String> filenames;

        cout << "Finding and processing calibration images..." << endl;

        glob("cal_*.jpg", filenames, false); //collect all filenames matching cal_*.jpg
        if (filenames.size() == 0)
        {
            cout << "No calibration images matching cal_*.jpg found." << endl;
            return -4;
        }
        for (size_t i = 0; i < filenames.size(); i++)
        {
            Mat distorted_chessboard = imread(filenames[i]);
            if (distorted_chessboard.empty()) { continue; }
            cvtColor(distorted_chessboard, grey, COLOR_BGR2GRAY);
            image_size = distorted_chessboard.size();

            vector<Point2f> corners;
            bool found = false;
            found = findChessboardCorners(distorted_chessboard, board_size, corners);
            if (found) 
            {
                cornerSubPix(grey, corners, Size(corner_size, corner_size), Size(-1, -1),
                    TermCriteria(TermCriteria::COUNT|TermCriteria::EPS, corner_count,corner_eps)); //refine corners
                image_points.push_back(corners);

                drawChessboardCorners(distorted_chessboard, board_size, corners, found);

                vector<Point3f> objects;
                for (int i = 0; i < board_height; i++) {
                    for (int j = 0; j < board_width; j++) {
                        objects.push_back(Point3f((float)i, (float)j, 0));
                    }
                }
                object_points.push_back(objects);
            }
        }

        cout << "Creating camera calibration information..." << endl;

        /// Calibrate camera ///
        vector<Mat> rvecs, tvecs; //rotation and translation vectors
        vector<Point3f> objects;

        calibrateCamera(object_points, image_points, image_size, camera_matrix, distorsion_coefficients, rvecs, tvecs);

        cout << "Saving camera calibration information..." << endl;

        // save to file
        FileStorage file("calibration.xml", FileStorage::WRITE);
        file << "camera_matrix" << camera_matrix;
        file << "distorsion_coefficients" << distorsion_coefficients;
        file.release();
    }

    /// Undistort image ///
    if (undistort_flag)
    { 

        cout << "Undistorting image..." << endl;

        if (calibrate_flag) //call from memory
        {
            undistort(distorted_image, undistorted_image, camera_matrix, distorsion_coefficients);
        }
        else //call from storage
        {
            FileStorage file("calibration.xml", FileStorage::READ);
            file["camera_matrix"] >> camera_matrix;
            file["distorsion_coefficients"] >> distorsion_coefficients;

            undistort(distorted_image, undistorted_image, camera_matrix, distorsion_coefficients);

            file.release();
        }

        // finish benchmarking and return runtime
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Total runtime: " << duration.count() << " microseconds" << endl;

        cout << "Displaying distorted and undistorted images..." << endl;

        /// Display distorted and undistorted images ///
        namedWindow("Distorted Image", WINDOW_NORMAL);
        imshow("Distorted Image", distorted_image);
        namedWindow("Undistorted Image", WINDOW_NORMAL);
        imshow("Undistorted Image", undistorted_image);

        waitKey(0); // Wait for a keystroke in the window
    }

    cout << "Finished." << endl;

    return 0;
}