#include "cv.hpp"

void convertColorToGray(cv::Mat &input, cv::Mat &processed) {
  cv::Mat temp;
  cv::cvtColor(input, temp, cv::COLOR_RGB2GRAY);
  cv::cvtColor(temp, processed,
               cv::COLOR_GRAY2BGR); //これをしないとサイズ変わる
}
void edgeExtract(cv::Mat &input, cv::Mat &processed) {
  cv::Mat grayscaled, blured, sobeled;
  // cv::GaussianBlur(input, blured, cv::Size(23, 23), 0.0);
  convertColorToGray(input, grayscaled);
  cv::blur(grayscaled, blured, cv::Size(2, 2));
  cv::Sobel(blured, sobeled, -1, 1, 1);
  cv::threshold(sobeled, processed, 10, 255, cv::THRESH_BINARY);
}
