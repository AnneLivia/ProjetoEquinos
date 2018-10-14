#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/*
    Color BGR (RGB) found:

    Light Brown:
    R = 74 a 243
    G = 46 a 211
    B = 24 a 170

    Dark Brown:
    R = 29 a 175
    G = 30 a 152
    B = 16 a 134

    White:
    R = 63 a 226
    G = 63 a 226
    B = 61 a 224

    Black:
    R = 26 a 141
    G = 22 a 139
    B = 19 a 140

    Using HSV - Hue, saturation and value
    Where:
        For dark brown: H: 2..15, 110..180, 80..255
        for Light Brown: H: 15..20, 80..190, 110..255
        for Black: H: 0..150, S: 30..100, V: 10..100
        for White: H: 0..180, S: 0..30, V: 100..255
*/

void remove_background(Mat& imagein, Mat& imageOut);
void detectColor(Mat& imagein, int& qtdlight, int& qtdbrownDark, int& qtdLightDark, int& qtdNonZeroBlack);

int main()
{
    Mat im = imread("images/horse23.jpg",CV_LOAD_IMAGE_COLOR);
    Mat imWithoutBackground; // Variable that receives the image without background

    // Removing background
    remove_background(im,imWithoutBackground);

    int qtdNonZeroWhite, qtdNonZeroDarkBrown, qtdNonZeroLightBrown, qtdNonZeroBlack; // Variables that are going to store the number of pixels that were found in a specific boundary

    // Detecting color based on a range of value
    detectColor(imWithoutBackground, qtdNonZeroWhite, qtdNonZeroDarkBrown, qtdNonZeroLightBrown, qtdNonZeroBlack); // passing the image without background and the variables that are going to store the amount of pixels founded

    string colorHorse = ""; // String that receives the horse's color according to the conditions bellow

    if(qtdNonZeroDarkBrown > qtdNonZeroWhite && qtdNonZeroDarkBrown > qtdNonZeroLightBrown && qtdNonZeroDarkBrown > qtdNonZeroBlack)
        colorHorse = "Horse's color: Dark Brown";
    else if (qtdNonZeroWhite > qtdNonZeroDarkBrown && qtdNonZeroWhite > qtdNonZeroLightBrown && qtdNonZeroWhite > qtdNonZeroBlack)
        colorHorse = "Horse's color: White";
    else if (qtdNonZeroBlack > qtdNonZeroDarkBrown && qtdNonZeroBlack > qtdNonZeroLightBrown && qtdNonZeroBlack > qtdNonZeroWhite)
        colorHorse = "Horse's color: Black";
    else
        colorHorse = "Horse's color: Light Brown";

    namedWindow(colorHorse,WINDOW_NORMAL); // Creating a window that in which the name is the colorHorse string
    namedWindow("Image Without Background",WINDOW_NORMAL); // Window that is going to show the image without background

    resizeWindow(colorHorse,500,400); // Specifying the size of window: width: 500, height: 400
    resizeWindow("Image Without Background",500,400);

    imshow(colorHorse,im); // showing original image

    imshow("Image Without Background",imWithoutBackground); // Showing image without background

    moveWindow(colorHorse, 150, 100);

    float total = qtdNonZeroBlack + qtdNonZeroDarkBrown + qtdNonZeroLightBrown + qtdNonZeroWhite; // Count the total pixel found

    // To show on the screen a float number with 2 numbers after the decimal point.
    cout.precision(2);
    cout.setf(ios::fixed);

    cout << "Quantidade de pixels encontrados em cada comparacao: " << endl;
    cout << "Dark Brown: " << (qtdNonZeroDarkBrown / total) * 100 << "%" << endl;
    cout << "White: " << (qtdNonZeroWhite / total) * 100 << "%" << endl;
    cout << "Light Brown: " << (qtdNonZeroLightBrown / total) * 100 << "%" << endl;
    cout << "Black: " << (qtdNonZeroBlack / total) * 100 << "%" << endl;

    cout << "\n\n" << colorHorse << endl;

    waitKey();

    return 0;
}

// *********************************************************************************************************************************

void detectColor(Mat& imagein, int& qtdNonZeroWhite, int& qtdNonZeroDarkBrown, int& qtdNonZeroLightBrown, int& qtdNonZeroBlack) {

    Mat imBrownLight, imBrownDark, imWhite, imBlack; // Variables to store the result of the bellow function

    // Checks if array elements lie between the elements of two other arrays (upper bound and lower bound).
    // dst(i) = lowerb <= array[i] <= upperb
    // That is, dst (I) is set to 255 (all 1 -bits) if src (I) is within the specified 1D, 2D, 3D, ... box and 0 otherwise.

    // RGB Image
    //inRange(imagein, Scalar(24,46,74), Scalar(170,211,243), imBrownLight); // in case its light brown: B >= 24 && B <= 170, G >= 46 && G <= 211 && R >= 74 && R <= 243
    //inRange(imagein, Scalar(16,30,29), Scalar(134,152,175), imBrownDark); // Same as above for color dark brown
    //inRange(imagein, Scalar(61,63,63), Scalar(224,226,226), imWhite); // color white
    //inRange(imagein, Scalar(19,22,26), Scalar(140,139,141), imBlack); // color black

    // Converting image from RGB to HSV
    Mat hsvImage;
    cvtColor(imagein, hsvImage, CV_BGR2HSV);

    // HSV Image
    inRange(hsvImage, Scalar(2,110,80), Scalar(15, 180, 255), imBrownDark); // Dark Brown
    inRange(hsvImage, Scalar(15,80,110), Scalar(20, 190, 255), imBrownLight); // Light Brown
    inRange(hsvImage, Scalar(0,0,100), Scalar(180, 30, 255), imWhite); // White
    inRange(hsvImage, Scalar(0,30,10), Scalar(150, 100, 100), imBlack); // Black


    // Getting the amount of non black pixels to check how many were found from each color
    qtdNonZeroWhite = countNonZero(imWhite);
    qtdNonZeroDarkBrown = countNonZero(imBrownDark);
    qtdNonZeroLightBrown = countNonZero(imBrownLight);
    qtdNonZeroBlack = countNonZero(imBlack);
}

// *********************************************************************************************************************************

void remove_background(Mat& imagein, Mat& imageOut) {
    Mat gray, threshImage, maskInv;

    int threshold_value = 238; // Threshold to check
    int const max_BINARY_value = 255; // Where the value is above the thresh "threshold_value", this value will be replace in its respective pixel
    // int threshold_type = 1;

    // Convert the image to Gray
    cvtColor(imagein, gray, COLOR_BGR2GRAY);

    // Remove noise using non local means
    fastNlMeansDenoising(gray, gray, 25);

    // Create Threshold
    threshold( gray, threshImage, threshold_value, max_BINARY_value,THRESH_BINARY);

    // Erode image (Morphological Operation)
    Mat kernel = Mat::ones(Size(6,6), CV_8U); // Kernel to do the morphological operation

    // Invert Threshold
    bitwise_not(threshImage, maskInv);

    // Applying erosion to remove border around horse
    erode(maskInv, maskInv, kernel);

    // Crop
    bitwise_and(imagein, imagein, imageOut, maskInv = maskInv);
}
