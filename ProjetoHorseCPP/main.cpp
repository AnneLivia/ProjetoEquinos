#include <iostream>
#include <opencv2/opencv.hpp>
#include <cstdlib>

using namespace std;
using namespace cv;

/*
    Using HSV - Hue, saturation and value
    Where, Version 1:
        For dark brown: H: 2..15, 110..180, 80..255
        for Light Brown: H: 15..20, 80..190, 110..255
        for Black: H: 0..150, S: 30..100, V: 10..100
        for White: H: 0..180, S: 0..30, V: 100..255
    Version 2:
    inRange(hsvImage, Scalar(2,118,60), Scalar(14, 255, 190), hsvdarkBrown);
    inRange(hsvImage, Scalar(12,31,191), Scalar(20, 255, 255), hsvlightBrown);
    inRange(hsvImage, Scalar(0,0,101), Scalar(180, 30, 255), hsvwhite);
    inRange(hsvImage, Scalar(0,0,10), Scalar(180, 117, 100), hsvblack);
*/

void remove_background(Mat& imagein, Mat& imageOut);
void detectColor(Mat& imagein, int& qtdNonZeroWhite, int& qtdNonZeroBrown, int& qtdNonZeroLight, int& qtdNonZeroBlack);
void predominantColor(string& horsePredominantColor, int qtdNonZeroWhite, int qtdNonZeroBrown, int qtdNonZeroLight, int qtdNonZeroBlack);
void detectLegColor(Mat& image, string& horseLegColor, bool& hasABlackLeg);
void detectCoat(float percentageWhite, float percentageBlack, float percentageBrown, float percentageLight, string horsePredominantColor, string& horseCoat, string horseLegColor, bool hasABlackLeg);

int main()
{
    for(int i = 3; i <= 129; i++) {
    cout << "i = " << i << endl;
    Mat im = imread("images/horse"+to_string(i)+".jpg",CV_LOAD_IMAGE_COLOR);
    //Mat im = imread("images/horse43.jpg",CV_LOAD_IMAGE_COLOR);
    Mat imWithoutBackground; // Variable that receives the image without background

    // Removing background
    remove_background(im,imWithoutBackground);

    // Variables that are going to store the number of pixels that were found in a specific boundary for each color
    int qtdNonZeroWhite, qtdNonZeroBrown, qtdNonZeroLight, qtdNonZeroBlack;

    // Detecting color based on a range of value
    detectColor(imWithoutBackground, qtdNonZeroWhite, qtdNonZeroBrown, qtdNonZeroLight, qtdNonZeroBlack); // passing the image without background and the variables that are going to store the amount of pixels founded

    // String that receives the horse's predominant color
    string horsePredominantColor;
    // Detecting what is the predominant color
    predominantColor(horsePredominantColor, qtdNonZeroWhite, qtdNonZeroBrown, qtdNonZeroLight, qtdNonZeroBlack);

    // String that receives the horse's leg color
    string horseLegColor;
    // Check if there is at least one black leg when the predominant color is not black
    bool hasABlackleg = false;
    // Detecting leg color
    detectLegColor(im, horseLegColor, hasABlackleg);

    // Calculating and storing percentage of each color
    float percentageBlack, percentageWhite, percentageBrown, percentageLight, total;

    total = qtdNonZeroBlack + qtdNonZeroBrown + qtdNonZeroLight + qtdNonZeroWhite;

    percentageBlack = (qtdNonZeroBlack / total) * 100;
    percentageBrown = (qtdNonZeroBrown / total) * 100;
    percentageWhite = (qtdNonZeroWhite / total) * 100;
    percentageLight = (qtdNonZeroLight / total) * 100;

    // String to store horse's coat
    string horseCoat;
    // Detecting horse's coat
    detectCoat(percentageWhite, percentageBlack, percentageBrown, percentageLight, horsePredominantColor, horseCoat, horseLegColor, hasABlackleg);

    // To show on the screen a float number with 2 numbers after the decimal point.
    cout.precision(2);
    cout.setf(ios::fixed);

    cout << "Porcentagem de cada cor: " << endl;
    cout << "Dark Brown: " << percentageBrown << "%" << endl;
    cout << "White: " << percentageWhite << "%" << endl;
    cout << "Light: " << percentageLight << "%" << endl;
    cout << "Black: " << percentageBlack << "%" << endl;

    cout << "\n\nPredominant Color: " << horsePredominantColor << endl;
    //cout << "Horse's Leg Color: " << horseLegColor << endl;
    cout << "Horse's Coat: " << horseCoat << endl;

    // Creating and resizing widows to show original image and image without background
    namedWindow(horsePredominantColor, WINDOW_NORMAL);
    namedWindow("Image Without Background", WINDOW_NORMAL);
    resizeWindow(horsePredominantColor, 500,400);
    resizeWindow("Image Without Background",500,400);

    // showing images
    imshow(horsePredominantColor,im); // showing original image
    imshow("Image Without Background",imWithoutBackground); // Showing image without background

    waitKey();
    system("cls");
    }
    return 0;
}

// *********************************************************************************************************************************

void detectColor(Mat& imagein, int& qtdNonZeroWhite, int& qtdNonZeroBrown, int& qtdNonZeroLight, int& qtdNonZeroBlack) {

    Mat imLight, imBrown, imWhite, imBlack; // Variables to store the result of the bellow function

    // Converting image from RGB to HSV
    Mat hsvImage;
    cvtColor(imagein, hsvImage, CV_BGR2HSV);

    // Checks if array elements lie between the elements of two other arrays (upper bound and lower bound).
    inRange(hsvImage, Scalar(2,118,60), Scalar(14, 255, 190), imBrown); // Dark Brown
    inRange(hsvImage, Scalar(12,31,191), Scalar(20, 255, 255), imLight); // Light Brown
    inRange(hsvImage, Scalar(0,0,101), Scalar(180, 30, 255), imWhite); // White
    inRange(hsvImage, Scalar(0,0,10), Scalar(180, 117, 100), imBlack); // Black


    // Getting the amount of non black pixels to check how many were found from each color
    qtdNonZeroWhite = countNonZero(imWhite);
    qtdNonZeroBrown = countNonZero(imBrown);
    qtdNonZeroLight = countNonZero(imLight);
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

// *********************************************************************************************************************************

void predominantColor(string& horsePredominantColor, int qtdNonZeroWhite, int qtdNonZeroBrown, int qtdNonZeroLight, int qtdNonZeroBlack) {
    if(qtdNonZeroBrown > qtdNonZeroWhite &&
       qtdNonZeroBrown > qtdNonZeroLight &&
       qtdNonZeroBrown > qtdNonZeroBlack)
        horsePredominantColor = "Brown";
    else if (qtdNonZeroWhite > qtdNonZeroBrown &&
             qtdNonZeroWhite > qtdNonZeroLight &&
             qtdNonZeroWhite > qtdNonZeroBlack)
        horsePredominantColor = "White";
    else if (qtdNonZeroBlack > qtdNonZeroBrown &&
             qtdNonZeroBlack > qtdNonZeroLight &&
             qtdNonZeroBlack > qtdNonZeroWhite)
        horsePredominantColor = "Black";
    else
        horsePredominantColor = "Light";
}

// *********************************************************************************************************************************

void detectLegColor(Mat& image, string& horseLegColor, bool& hasABlackLeg) {
    // getting only the legs of the horse, using ROI (Region of Interest) technique
    Mat legsHorseROI = image(Range(image.rows / 1.3, image.rows), Range(image.cols / 10, image.cols));

    // LegsHorse to store image without background
    Mat legsHorseROIWithoutBackground;

    // Removing background
    remove_background(legsHorseROI, legsHorseROIWithoutBackground);

    // Variable to store amount of pixels founded for each color
    int qtdNonZeroWhite, qtdNonZeroBrown, qtdNonZeroLight, qtdNonZeroBlack;

    // Detecting colors
    detectColor(legsHorseROIWithoutBackground, qtdNonZeroWhite, qtdNonZeroBrown, qtdNonZeroLight, qtdNonZeroBlack);

    // Checking predominant color from the horse's legs
    predominantColor(horseLegColor, qtdNonZeroWhite, qtdNonZeroBrown, qtdNonZeroLight, qtdNonZeroBlack);

    // Calculating percentage of each color
    float total, percentageBlack, percentageBrown;
    total = qtdNonZeroBlack + qtdNonZeroBrown + qtdNonZeroLight + qtdNonZeroWhite;
    percentageBlack = (qtdNonZeroBlack  / total) * 100;
    percentageBrown = (qtdNonZeroBrown / total ) * 100;

    // true if there's possibly a black leg, although the predominant color is other
    hasABlackLeg = (percentageBlack >= percentageBrown && percentageBlack >= 20.00);
}

// *********************************************************************************************************************************

void detectCoat(float percentageWhite, float percentageBlack, float percentageBrown, float percentageLight, string horsePredominantColor, string& horseCoat, string horseLegColor, bool hasABlackLeg) {
    // if predominant color is brown or Color is black, which means that the predominant color is black, but, more than 20 percentage is dark brown, ( In almost all black Horses the percentage of brown is less than 16 percent).
    if(horsePredominantColor == "Brown" || (horsePredominantColor == "Black" && percentageBrown >= 20.00)) {
        // if horse's leg is black, it means that it maybe has a black mane, tail, etc. (Sometimes, other color like white can be predominant on the leg, so the algorithm must check if there's at least one leg black
        if((horseLegColor == "Black") || (horseLegColor != "Black" && hasABlackLeg)) {
            horseCoat = "Castanho";
        } else {
            horseCoat = "Alaza";
        }
    } else {
        horseCoat = "Neither \"Alaza\" nor \"Castanho\"";
    }
}
