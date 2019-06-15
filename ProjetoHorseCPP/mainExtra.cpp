#include <iostream>
#include <opencv2/opencv.hpp>
#include <cstdlib>

using namespace std;
using namespace cv;

/*
    Using HSV - Hue, saturation and value
    Version 1:
        For dark: H: 2..15, 110..180, 80..255
        for Light: H: 15..20, 80..190, 110..255
        for Black: H: 0..150, S: 30..100, V: 10..100
        for White: H: 0..180, S: 0..30, V: 100..255
    Version 2:
    inRange(hsvImage, Scalar(2,118,60), Scalar(14, 255, 190), hsvdark);
    inRange(hsvImage, Scalar(12,31,191), Scalar(20, 255, 255), hsvlight);
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
    for(int i = 87; i <= 92; i++) {
    cout << "i = " << i << endl;
    Mat im = imread("images/horse"+to_string(i)+".jpg",IMREAD_COLOR);
    //Mat im = imread("images/horse23.jpg",IMREAD_COLOR);
    Mat imWithoutBackground; // Variable that receives the image without background

    // Removing background
    remove_background(im,imWithoutBackground);

    // Variables to store the number of pixels found in a specific boundary for each color
    int qtdNonZeroWhite, qtdNonZeroBrown, qtdNonZeroLight, qtdNonZeroBlack;

    // Detecting color
    detectColor(imWithoutBackground, qtdNonZeroWhite, qtdNonZeroBrown, qtdNonZeroLight, qtdNonZeroBlack); // passing the image without background and the variables that are going to store the amount of pixels founded

    // Detecting predominant color (Entire Body)
    string horsePredominantColor;
    predominantColor(horsePredominantColor, qtdNonZeroWhite, qtdNonZeroBrown, qtdNonZeroLight, qtdNonZeroBlack);

    // Detecting predominant color (Horse's legs)
    string horseLegColor;
    bool hasABlackleg = false; // Boolean variable used to verify if at least one of the legs is of color black, although predominant color may not be black.
    detectLegColor(im, horseLegColor, hasABlackleg);

    // Calculating percentage of each color (Entire body)
    float percentageBlack, percentageWhite, percentageBrown, percentageLight, total;

    total = qtdNonZeroBlack + qtdNonZeroBrown + qtdNonZeroLight + qtdNonZeroWhite;

    percentageBlack = (qtdNonZeroBlack / total) * 100;
    percentageBrown = (qtdNonZeroBrown / total) * 100;
    percentageWhite = (qtdNonZeroWhite / total) * 100;
    percentageLight = (qtdNonZeroLight / total) * 100;

    // Detecting equine coats
    string horseCoat;
    detectCoat(percentageWhite, percentageBlack, percentageBrown, percentageLight, horsePredominantColor, horseCoat, horseLegColor, hasABlackleg);


    cout.precision(2);
    cout.setf(ios::fixed);

    cout << "Porcentagem de cada cor: " << endl;
    cout << "Brown: " << percentageBrown << "%" << endl;
    cout << "White: " << percentageWhite << "%" << endl;
    cout << "Light: " << percentageLight << "%" << endl;
    cout << "Black: " << percentageBlack << "%" << endl;

    cout << "\n\nPredominant Color: " << horsePredominantColor << endl;
    cout << "Horse's Leg Color: " << horseLegColor << endl;
    cout << "Horse's Coat: " << horseCoat << endl;

    namedWindow("Horse", WINDOW_NORMAL);
    namedWindow("Image Without Background", WINDOW_NORMAL);
    resizeWindow("Horse", 300,200);
    resizeWindow("Image Without Background",300,200);

    imshow("Horse",im);
    imshow("Image Without Background",imWithoutBackground);

    waitKey();
    system("cls");
    }
    return 0;
}

// *********************************************************************************************************************************

void detectColor(Mat& imagein, int& qtdNonZeroWhite, int& qtdNonZeroBrown, int& qtdNonZeroLight, int& qtdNonZeroBlack) {

    // Variables to store binary image of each color
    Mat imLight, imBrown, imWhite, imBlack;

    // Converting image from RGB to HSV
    Mat hsvImage;
    cvtColor(imagein, hsvImage, COLOR_BGR2HSV);

    // Check if array elements lie between the elements of two other arrays (upper bound and lower bound).
    inRange(hsvImage, Scalar(2,118,60), Scalar(14, 255, 190), imBrown); // Brown (red)
    inRange(hsvImage, Scalar(12,31,191), Scalar(20, 255, 255), imLight); // Light
    inRange(hsvImage, Scalar(0,0,101), Scalar(180, 30, 255), imWhite); // White
    inRange(hsvImage, Scalar(0,0,10), Scalar(180, 117, 100), imBlack); // Black

    // Amount of non black pixels
    qtdNonZeroWhite = countNonZero(imWhite);
    qtdNonZeroBrown = countNonZero(imBrown);
    qtdNonZeroLight = countNonZero(imLight);
    qtdNonZeroBlack = countNonZero(imBlack);
}

// *********************************************************************************************************************************

void remove_background(Mat& imagein, Mat& imageOut) {
    Mat gray, threshImage, maskInv;

    int threshold_value = 238;
    int const max_BINARY_value = 255; // White
    // int threshold_type = 1;

    // Converting from RGB to Gray
    cvtColor(imagein, gray, COLOR_BGR2GRAY);

    // Removing noise with non local means
    fastNlMeansDenoising(gray, gray, 25);

    // Thresholding image
    threshold( gray, threshImage, threshold_value, max_BINARY_value,THRESH_BINARY);

    // Inverting Thresholded image (changing 0 to 1, and vice versa)
    bitwise_not(threshImage, maskInv);

    // (Morphological Operation) Kernel (Structuring element) - Applying erosion to remove border around horse
    Mat kernel = Mat::ones(Size(6,6), CV_8U);
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
    // ROI (Region of Interest) - legs of the horse
    Mat legsHorseROI = image(Range(image.rows / 1.3, image.rows), Range(image.cols / 10, image.cols));

    // Removing background
    Mat legsHorseROIWithoutBackground;
    remove_background(legsHorseROI, legsHorseROIWithoutBackground);

    // Variables to store the number of pixels found in a specific boundary for each color
    int qtdNonZeroWhite, qtdNonZeroBrown, qtdNonZeroLight, qtdNonZeroBlack;

    // Detecting colors
    detectColor(legsHorseROIWithoutBackground, qtdNonZeroWhite, qtdNonZeroBrown, qtdNonZeroLight, qtdNonZeroBlack);

    // Predominant color (Horse's legs)
    predominantColor(horseLegColor, qtdNonZeroWhite, qtdNonZeroBrown, qtdNonZeroLight, qtdNonZeroBlack);

    // Calculating percentage of brown and black
    float total, percentageBlack, percentageBrown;
    total = qtdNonZeroBlack + qtdNonZeroBrown + qtdNonZeroLight + qtdNonZeroWhite;
    percentageBlack = (qtdNonZeroBlack  / total) * 100;
    percentageBrown = (qtdNonZeroBrown / total ) * 100;

    // true if there's possibly at least one black leg
    hasABlackLeg = (percentageBlack >= percentageBrown && percentageBlack >= 20.00);
}

// *********************************************************************************************************************************

void detectCoat(float percentageWhite, float percentageBlack, float percentageBrown, float percentageLight, string horsePredominantColor, string& horseCoat, string horseLegColor, bool hasABlackLeg) {
    // if predominant color is brown or Color is black, but, more than 20 percentage is brown, ( In almost all black Horses the percentage of brown is less than 16 percent).
    if(horsePredominantColor == "Brown" || (horsePredominantColor == "Black" && percentageBrown >= 20.00)) {
        // if horse's leg is black or there's at least one leg black
        if((horseLegColor == "Black") || (horseLegColor != "Black" && hasABlackLeg)) {
            horseCoat = "Castanho";
        } else {
            horseCoat = "Alaza";
        }
    } else if(horsePredominantColor == "Light") {
        if (horseLegColor == "Black" || (horseLegColor != "Black" && hasABlackLeg)) {
            horseCoat = "Baia";
        } else {
            horseCoat = "Palomino/Alaza";
        }
    } else {
        horseCoat = "Neither \"Alaza\" nor \"Castanho\"";
    }
}
