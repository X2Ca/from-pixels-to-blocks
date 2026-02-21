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

    image.rows = rows;
    image.columns = WIDTH;

    if (rows > HEIGHT ) std::cerr << "[Error] The number of rows doesn't  match";

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


    if (counter != rows * WIDTH){
        std::cerr << "There is a problem with the size of your image\n";
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

int ImageProcessing::CannyEdgeDetection(ImageData& image){
    Mat img = imread(image.path);

    
    // Convert to graycsale
    Mat img_gray;
    cvtColor(img, img_gray, COLOR_BGR2GRAY);
    // Blur the image for better edge detection
    Mat img_blur;
    GaussianBlur(img_gray, img_blur, Size(3,3), 0);

     // Canny edge detection
    Mat edges;
    Canny(img_blur, edges, 100, 200, 3, false);
    // Display canny edge detected image
    imshow("Canny edge detection", edges);

    image.edge = edges;


    waitKey();
    return 0;    

}

int ImageProcessing::HoughLineTransform(ImageData& image)
{

    Mat canny_edge = image.edge;
    Mat cdst;

    // Copy edges to the images that will display the results in BGR
    cvtColor(canny_edge, cdst, COLOR_GRAY2BGR);
    // Standard Hough Line Transform
    std::vector<Vec2f> lines; // will hold the results of the detection
    HoughLines(canny_edge, lines, 1, CV_PI/180, 150, 0, 0 ); // runs the actual detection
    // Draw the lines
    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0], theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        line( cdst, pt1, pt2, Scalar(0,0,255), 1, LINE_AA);
    }

    

    imshow("Hough line transform", cdst);
    waitKey();

    image.lines = lines;

    return 0;
}

std::vector<std::pair<Vec2f, Vec2f>> ImageProcessing::ParallelLineDetector(ImageData& image)
{
    std::vector<Vec2f> lines = image.lines;

    Mat parallel;
    cvtColor(image.edge, parallel, COLOR_GRAY2BGR);

    std::vector<std::pair<Vec2f, Vec2f>> parallelPairs;

    for (size_t i = 0; i < lines.size(); i++)
    {
        for (size_t j = i + 1; j < lines.size(); j++)
        {
            if (std::abs(lines[i][1] - lines[j][1]) < 0.01f)
            {
                parallelPairs.emplace_back(lines[i], lines[j]);
            }
        }
    }


    for (int k=0; k<parallelPairs.size(); k++){
        Vec2f i = parallelPairs[k].first;
        Vec2f j = parallelPairs[k].second;

        Scalar color(rand() % 256, rand() % 256, rand() % 256);

        float rho = i[0], theta = i[1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        line( parallel, pt1, pt2, color, 1, LINE_AA);

        rho = j[0], theta = j[1];

        a = cos(theta), b = sin(theta);
        x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        line( parallel, pt1, pt2, color, 1, LINE_AA);

    } 

    imshow("Parralel", parallel);
    waitKey();

    return parallelPairs ;
}

int ImageProcessing::ParallelogramFinder(std::vector<std::pair<Vec2f, Vec2f>> parallelPair)
{

    for (int k=0; k< parallelPair.size(); k++){
        Vec2f i = parallelPair[k].first;
        Vec2f j = parallelPair[k].second;

        
    }
    return 0;
}

int ImageProcessing::Intersection(ImageData& image)
{
    cv::Point2f point;
    std::vector<Vec2f> lines = image.lines;
    std::vector<std::pair<double, double>> intersection;

    cv::Point2f CenterPoint;
    int MaxCount = 0;

    float tolerance = 10.5f;

    Mat graphImage ;
    cvtColor(image.edge, graphImage, COLOR_GRAY2BGR);



    for ( int i=0; i<lines.size(); i++){
        Vec2f line1 = lines[i];
        for ( int j=0; j<lines.size(); j++){
            Vec2f line2 = lines[j];

            float rho1 = line1[0], theta1 = line1[1];
            float rho2 = line2[0], theta2 = line2[1];

            double a1 = cos(theta1), b1 = sin(theta1);
            double a2 = cos(theta2), b2 = sin(theta2);

            double det = a1*b2 - a2*b1;
            if (std::abs(det) < 1e-10) continue; // lines are parallel

            double x = (b2*rho1 - b1*rho2)/det;
            double y = (a1*rho2 - a2*rho1)/det;

            circle(graphImage, cv::Point2f(x,y), 0, cv::Scalar(0, 0, 255), 2);

            intersection.push_back({x, y});
        }
    }

    for (int i=0; i<intersection.size(); i++){
        int counter = 0;
        for ( int j=i+1; j<intersection.size(); j++){
            if (abs(intersection[i].first - intersection[j].first) < tolerance && abs(intersection[i].second - intersection[j].second) < tolerance ){
                counter++;
            }
        }
        if (counter > MaxCount){
            MaxCount = counter ;
            CenterPoint = cv::Point2f(intersection[i].first, intersection[i].second);
        }
    }

    circle(graphImage, CenterPoint, 5, cv::Scalar(0, 0, 255), 7);

    image.CenterPoint = CenterPoint; 

    imshow("Intersection", graphImage);
    waitKey();


    
    return 0;
}

void ImageProcessing::creatDepthScale(ImageData& image)
{
    Mat depth ;
    cvtColor(image.edge, depth, COLOR_GRAY2BGR);
    circle(depth, image.CenterPoint, 5, cv::Scalar(0, 0, 255), 7);

    Point pt1, pt2;

    pt1.x = 0, pt1.y =  image.edge.size().height;
    pt2.x = image.edge.size().width, pt2.y = image.edge.size().height;

    line( depth, pt1, image.CenterPoint, cv::Scalar(0, 0, 255), 1, LINE_AA);
    line( depth, pt2, image.CenterPoint, cv::Scalar(0, 0, 255), 1, LINE_AA);

    imshow("Depth Scale", depth);
    waitKey();

    



}