#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;

struct ImageData
{
    Mat image;
    float height;
    float width;
    std::string path;
    std::vector<float> grid;
};

class ImageProcessing {
public:
    ImageProcessing() = default;
    std::vector<float> GrayScaleGridConverter(ImageData& image, const int HEIGHT, const int WIDTH);
    int SobelEdgeDetection(ImageData& image);
};