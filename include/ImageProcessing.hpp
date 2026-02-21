#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;

struct ImageData
{
    Mat image;
    Mat edge;
    cv::Point2f CenterPoint;
    float height;
    float width;
    int rows;
    int columns;
    std::string path;
    std::vector<float> grid;
    std::vector<Vec2f> lines;
    cv::Mat depthMap;
};

class ImageProcessing {
public:
    ImageProcessing() = default;
    std::vector<float> GrayScaleGridConverter(ImageData& image, const int HEIGHT, const int WIDTH);
    int SobelEdgeDetection(ImageData& image);
    int CannyEdgeDetection(ImageData& image);
    int HoughLineTransform(ImageData& image);
    std::vector<std::pair<Vec2f, Vec2f>> ParallelLineDetector(ImageData& image);
    int ParallelogramFinder(std::vector<std::pair<Vec2f, Vec2f>> ParallelPair);
    int Intersection(ImageData& image);
    void creatDepthScale(ImageData& image);
};