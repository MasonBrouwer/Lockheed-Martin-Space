/**
*   Non-Uniformity Correction (NUC)
* 
*   This project contains code that can be used to perform the NUC image processing algorithm.
* 
*   Date: 2021 Jan 31
*   Authors: CSE498 Capstone, Lockheed-Martin Space Team
* 
*   Relevant OpenCV Documentation:
*       https://docs.opencv.org/3.4/d5/dc4/tutorial_adding_images.html
* 
* 
*   NOTES:
*       As of right now, this code is only using the sample images that are included with the OpenCV Library. This will change in the future.
*/

#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

// Note: We're NOT using the standard namespace here to avoid collisions between the beta variable and std::beta in c++17
using std::cin;
using std::cout;
using std::endl;

/**
*   Function to test the filter2d() function from OpenCV
*   https://docs.opencv.org/3.4/d4/dbd/tutorial_filter_2d.html
*/
int test_filter2d(int argc, char** argv)
{
    // Declare variables
    Mat src, dst;

    Mat kernel;
    Point anchor;
    double delta;
    int ddepth;
    int kernel_size;
    const char* window_name = "filter2D Demo";

    const char* imageName = argc >= 2 ? argv[1] : "lena.jpg";

    // Loads an image
    src = imread(samples::findFile(imageName), IMREAD_COLOR);

    if (src.empty())
    {
        printf(" Error opening image\n");
        printf(" Program Arguments: [image_name -- default lena.jpg\n");
        return EXIT_FAILURE;
    }

    // Initialize arguments for the filter
    anchor = Point(-1, -1);
    delta = 0;
    ddepth = -1;

    // Loop - Will filter the image with different kernel sizes each 0.5 seconds
    int ind = 0;
    for (;;)
    {
        // Update kernel size for a normalized box filter
        kernel_size = 3 + 2 * (ind % 5);
        kernel = Mat::ones(kernel_size, kernel_size, CV_32F) / (float)(kernel_size * kernel_size);

        // Apply filter
        filter2D(src, dst, ddepth, kernel, anchor, delta, BORDER_DEFAULT);
        imshow(window_name, dst);

        char c = (char)waitKey(500);
        // Press 'ESC' to exit the program

        if (c == 27)
        {
            break;
        }

        ind++;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
    // From the OpenCV tutorial "tutorial_adding_images"
    // The equation:    g(x) = (1 - alpha)*img1(x)  +  alpha*img2(x)
    // By varying alpha from 0 -> 1, this operator can be used to perform a temporal cross-dissolve between two images or videos, as seen in slide shows and film productions

    double alpha = 0.5; // Default value of alpha, subject to change via user input
    double beta;    // Will be the result of (1 - alpha)
    double input;   // User input value for alpha
    double gamma = 0.0; // To be added to the result of the two pictures

    Mat src1;   // The first image
    Mat src2;   // The second image
    Mat output; // The outputted result of adding these two images

    cout << " Simple Linear Blender " << endl;
    cout << "-----------------------" << endl;
    cout << "* Enter alpha [0.0-1.0]: ";
    cin >> input;

    // We use the user provided alpha if it is between 0 and 1
    if (input >= 0 && input <= 1)
    {
        alpha = input;
    }

    //
    // WARNING: In order for the operation to work, the two images must be of the same size (height and width) and type
    src1 = imread(samples::findFile("LinuxLogo.jpg"));
    src2 = imread(samples::findFile("WindowsLogo.jpg"));

    // Error checking, make sure the images were found
    if (src1.empty())
    {
        cout << "Error loading src1" << endl; 
        return EXIT_FAILURE;
    }
    if (src2.empty())
    {
        cout << "Error loading src2" << endl;
        return EXIT_FAILURE;
    }

    beta = (1.0 - alpha);

    // addWeighted() produces:
    // output = (alpha*src1) + (beta*src2) + gamma
    addWeighted(src1, alpha, src2, beta, gamma, output);

    // Create the window to show the images and wait for the user to end the program
    imshow("Linear Blend", output);
    waitKey(0);

    // test_filter2d(argc, argv);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
