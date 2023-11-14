#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
cv::Mat frame, sobeled_frame, output_frame;
cv::VideoCapture cap;
void convertColorToGray(cv::Mat &input, cv::Mat &processed);
void edgeExtract(cv::Mat &input, cv::Mat &processed);