#ifndef CV_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>

void convertColorToGray(cv::Mat &input, cv::Mat &processed);
void edgeExtract(cv::Mat &input, cv::Mat &processed);

#define CV_HPP
#endif