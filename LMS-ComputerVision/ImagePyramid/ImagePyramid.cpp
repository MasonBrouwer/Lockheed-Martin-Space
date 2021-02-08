//////////////////////////////////////////////////////////////////////////////
/// Image Pyramid
//////////////////////////////////////////////////////////////////////////////
///
/// 
/// Relevant OpenCV documenatation:
/// https://docs.opencv.org/3.4/d5/d0f/tutorial_js_pyramids.html
/// https://docs.opencv.org/3.4/d4/d1f/tutorial_pyramids.html
/// 
/// Usage:
///     Choose one or multiple options:
///         -rev : Will pyramid the image upwards instead of downwards. Note that this process
///                cannot restore resolution to a downsampled image.
///         -pyr : Name of image to pyramid will follow this argument. If no image is provided, 
///                an error will be thrown.
///         -val : Number of pyramid operations will follow this argument. If no value is 
///                provided, or zero is entered an error will be thrown. If this tag is not used
///                the program will default to one Gaussian or Laplacian pyramid operation.
///     Use both commands to pyramid an image upward.
///         ex: 
///             Pyramid -rev -pyr {originalImage}.jpg
/// 
//////////////////////////////////////////////////////////////////////////////
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <chrono>
#include <iostream>

using namespace cv;
using namespace std;
using namespace std::chrono;

Mat original_image;
Mat pyramid_image;

int main(int argc, char** argv)
{
    auto start = high_resolution_clock::now(); //benchmarking

    bool pyramid_flag = false;
    bool reverse_flag = false;
    int pyramid_value = 1;

    /// process arguments ///
    if (argc > 6 || argc < 2)
    {
        cout << "Incorrect usage, please see documentation." << endl;
        return -1;
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "-pyr") == 0)
            {
                pyramid_flag = true;
                i++;

                if (i >= argc) {
                    cout << "Could not open or find the image" << endl;
                    return -2;
                }

                cout << "Opening " << argv[i] << " to pyramid..." << endl;

                original_image = imread(argv[i], IMREAD_COLOR);
                pyramid_image = original_image;

                if (original_image.empty())
                {
                    cout << "Could not open or find the image" << endl;
                    return -3;
                }
            }
            else if (strcmp(argv[i], "-val") == 0)
            {
                i++;

                if (i >= argc) {
                    cout << "Invalid value, \"\" is not an integer greater than zero" << endl;
                    return -4;
                }

                pyramid_value = atoi(argv[i]);

                if (pyramid_value == 0)
                {
                    cout << "Invalid value, \"" << argv[i] << "\" is not an integer greater than zero" << endl;
                    return -5;
                }
            }
            else if (strcmp(argv[i], "-rev") == 0)
            {
                reverse_flag = true;
            }
            else
            {
                cout << "Unknown argument: " << argv[i] << endl;
                return -6;
            }
        }
    }


    /// Pyramid image ///
    if (pyramid_flag)
    {

        //TODO: Check if there is a way to avoid use of a for loop here, the value to divide/multiply 
        //(if not 2) by will cause issues depending on number of pixels in the original image hence the 
        //current loop use.

        //Apply Image Pyramid operation
        for (; pyramid_value > 0; pyramid_value--) {

            if (reverse_flag) //pyramid up (*2)
            {
                pyrUp(pyramid_image, pyramid_image, Size(pyramid_image.cols * 2, pyramid_image.rows * 2));
                cout << "Upsampling image..." << endl;
            }
            else //pyramid down (/2)
            {
                pyrDown(pyramid_image, pyramid_image, Size(pyramid_image.cols / 2, pyramid_image.rows / 2));
                cout << "Downsampling image..." << endl;
            }
        }
        

        // finish benchmarking and return runtime
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Total runtime: " << duration.count() << " microseconds" << endl;

        cout << "Displaying original and resulting images..." << endl;

        /// Display original and resulting images ///
        namedWindow("Original Image", WINDOW_NORMAL);
        imshow("Original Image", original_image);
        namedWindow("Image Pyramid", WINDOW_NORMAL);
        imshow("Image Pyramid", pyramid_image);

        waitKey(0); // Wait for a keystroke in the window
    }

    cout << "Finished." << endl;

    return 0;
}