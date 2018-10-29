#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/*
    Using HSV - Hue, saturation and value
    Where:
        For dark brown: H: 2..15, 110..180, 80..255
        for Light Brown: H: 15..20, 80..190, 110..255
        for Black: H: 0..150, S: 30..100, V: 10..100
        for White: H: 0..180, S: 0..30, V: 100..255
*/

void remove_background(Mat& imagein, Mat& imageOut);
void detectColor(Mat& imagein, int& qtdNonZeroWhite, int& qtdNonZeroDarkBrown, int& qtdNonZeroLightBrown, int& qtdNonZeroBlack);
void predominantColor(string& horsePredominantColor, int qtdNonZeroWhite, int qtdNonZeroDarkBrown, int qtdNonZeroLightBrown, int qtdNonZeroBlack);
void detectLegColor(Mat& image, string& horseLegColor);
void detectCoat(float percentageWhite, float percentageBlack, float percentageDarkBrown, float percentageLightBrown, string horsePredominantColor, string& horseCoat, string horseLegColor);

int main()
{
    Mat im = imread("images/horse43.jpg",CV_LOAD_IMAGE_COLOR);
    Mat imWithoutBackground; // Variable that receives the image without background

    // Removing background
    remove_background(im,imWithoutBackground);

    // Variables that are going to store the number of pixels that were found in a specific boundary for each color
    int qtdNonZeroWhite, qtdNonZeroDarkBrown, qtdNonZeroLightBrown, qtdNonZeroBlack;

    // Detecting color based on a range of value
    detectColor(imWithoutBackground, qtdNonZeroWhite, qtdNonZeroDarkBrown, qtdNonZeroLightBrown, qtdNonZeroBlack); // passing the image without background and the variables that are going to store the amount of pixels founded

    // String that receives the horse's predominant color
    string horsePredominantColor;
    // Detecting what is the predominant color
    predominantColor(horsePredominantColor, qtdNonZeroWhite, qtdNonZeroDarkBrown, qtdNonZeroLightBrown, qtdNonZeroBlack);

    // String that receives the horse's leg color
    string horseLegColor;
    // Detecting leg color
    detectLegColor(im, horseLegColor);

    // Calculating and storing percentage of each color
    float percentageBlack, percentageWhite, percentageDarkBrown, percentageLightBrown, total;

    total = qtdNonZeroBlack + qtdNonZeroDarkBrown + qtdNonZeroLightBrown + qtdNonZeroWhite;

    percentageBlack = (qtdNonZeroBlack / total) * 100;
    percentageDarkBrown = (qtdNonZeroDarkBrown / total) * 100;
    percentageWhite = (qtdNonZeroWhite / total) * 100;
    percentageLightBrown = (qtdNonZeroLightBrown / total) * 100;

    // String to store horse's coat
    string horseCoat;
    // Detecting horse's coat
    detectCoat(percentageWhite, percentageBlack, percentageDarkBrown, percentageLightBrown, horsePredominantColor, horseCoat, horseLegColor);

    // To show on the screen a float number with 2 numbers after the decimal point.
    cout.precision(2);
    cout.setf(ios::fixed);

    cout << "Quantidade de pixels encontrados em cada comparacao: " << endl;
    cout << "Dark Brown: " << percentageDarkBrown << "%" << endl;
    cout << "White: " << percentageWhite << "%" << endl;
    cout << "Light: " << percentageLightBrown << "%" << endl;
    cout << "Black: " << percentageBlack << "%" << endl;

    cout << "\n\nPredominant Color: " << horsePredominantColor << endl;
    cout << "Horse's Leg Color: " << horseLegColor << endl;
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

    return 0;
}

// *********************************************************************************************************************************

void detectColor(Mat& imagein, int& qtdNonZeroWhite, int& qtdNonZeroDarkBrown, int& qtdNonZeroLightBrown, int& qtdNonZeroBlack) {

    Mat imBrownLight, imBrownDark, imWhite, imBlack; // Variables to store the result of the bellow function

    // Converting image from RGB to HSV
    Mat hsvImage;
    cvtColor(imagein, hsvImage, CV_BGR2HSV);

    // Checks if array elements lie between the elements of two other arrays (upper bound and lower bound).
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

// *********************************************************************************************************************************

void predominantColor(string& horsePredominantColor, int qtdNonZeroWhite, int qtdNonZeroDarkBrown, int qtdNonZeroLightBrown, int qtdNonZeroBlack) {
    if(qtdNonZeroDarkBrown > qtdNonZeroWhite &&
       qtdNonZeroDarkBrown > qtdNonZeroLightBrown &&
       qtdNonZeroDarkBrown > qtdNonZeroBlack)
        horsePredominantColor = "Dark Brown";
    else if (qtdNonZeroWhite > qtdNonZeroDarkBrown &&
             qtdNonZeroWhite > qtdNonZeroLightBrown &&
             qtdNonZeroWhite > qtdNonZeroBlack)
        horsePredominantColor = "White";
    else if (qtdNonZeroBlack > qtdNonZeroDarkBrown &&
             qtdNonZeroBlack > qtdNonZeroLightBrown &&
             qtdNonZeroBlack > qtdNonZeroWhite)
        horsePredominantColor = "Black";
    else
        horsePredominantColor = "Light Brown";
}

// *********************************************************************************************************************************

void detectLegColor(Mat& image, string& horseLegColor) {
    // getting only the legs of the horse, using ROI (Region of Interest) technique
    Mat legsHorseROI = image(Range(image.rows / 1.3, image.rows), Range(image.cols / 10, image.cols));
    imshow("l", legsHorseROI);
    // LegsHorse to store image without background
    Mat legsHorseROIWithoutBackground;

    // Removing background
    remove_background(legsHorseROI, legsHorseROIWithoutBackground);

    // Variable to store amount of pixels founded for each color
    int qtdNonZeroWhite, qtdNonZeroDarkBrown, qtdNonZeroLightBrown, qtdNonZeroBlack;

    // Detecting colors
    detectColor(legsHorseROIWithoutBackground, qtdNonZeroWhite, qtdNonZeroDarkBrown, qtdNonZeroLightBrown, qtdNonZeroBlack);

    // Checking predominant color from the horse's legs
    // predominantColor(horseLegColor, qtdNonZeroWhite, qtdNonZeroDarkBrown, qtdNonZeroLightBrown, qtdNonZeroBlack);

    // Checking for a while only two colors, dark brown and black
    if(qtdNonZeroDarkBrown > qtdNonZeroBlack)
        horseLegColor = "Dark Brown";
    else
        horseLegColor = "Black";
}

// *********************************************************************************************************************************

void detectCoat(float percentageWhite, float percentageBlack, float percentageDarkBrown, float percentageLightBrown, string horsePredominantColor, string& horseCoat, string horseLegColor) {

    if(horsePredominantColor == "Dark Brown") {
        if((percentageBlack >= 10.0 && horseLegColor == "Black") || horseLegColor == "Black") {
            horseCoat = "Castanho";
        } else {
            horseCoat = "Alaza";
        }
    }
}

