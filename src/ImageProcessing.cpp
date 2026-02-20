#include "ImageProcessing.hpp"

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <iostream>



std::vector<float> ImageProcessing::GrayScaleGridConverter(ImageData& image, const int HEIGHT, const int WIDTH) //
{
    image.image = imread(image.path, IMREAD_GRAYSCALE);

    // Error Handling
    while (image.image.empty()) {
        std::cout << "Image File " << "Not Found" << std::endl;
        //loadImage(image);
        image.image = imread(image.path, IMREAD_GRAYSCALE);
        //return -1;
    }

    Mat img = image.image;

    int W = img.size().width;
    int H = img.size().height;



    // compute width of tile
    float w = W/WIDTH;

    // compute number of rows
    const int rows = int(H/w);

    // Define our grid
    std::vector<float> VolumGrid;

    int counter = 0; // counter for the vector

    for (int i=0; i<rows; i++)
    {
        int y1 = (i*w);
        int y2 = (i+1)*w;
        
        if ( i == rows-1) y2 = H;

        for (int j=0; j < WIDTH; j++)
        {
            int x1 = j*w;
            int x2 = (j+1)*w;

            if (j == WIDTH - 1)  x2 = W;

            Mat tempImg = img( Range(y1, y2), Range(x1, x2) );


            if (tempImg.empty() || tempImg.rows == 0 || tempImg.cols == 0) {
                std::cout << "tempImg is empty! Check your ranges." << std::endl;
            } else {
                cv::Scalar tempVal = cv::mean(tempImg);
                VolumGrid.push_back( (tempVal.val[0] /255)*10);
                counter++;
            }
        }
    }



    return VolumGrid;
}

int ImageProcessing::SobelEdgeDetection(ImageData& image){
    
    cv::Mat sobelx, sobely, gradient;

    Mat img = image.image;

    // Apply Sobel operator
    cv::Sobel(img, sobelx, CV_64F, 1, 0, 3);
    cv::Sobel(img, sobely, CV_64F, 0, 1, 3);

    // Compute gradient magnitude
    cv::magnitude(sobelx, sobely, gradient);

    // Convert to 8-bit image
    cv::Mat gradient_abs;
    cv::convertScaleAbs(gradient, gradient_abs);

    // Display result
    cv::imshow("Sobel Edge Detection", gradient_abs);

    cv::waitKey(0);
    return 1;
}