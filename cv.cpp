#include "cv.hpp"

void convertColorToGray(cv::Mat &input, cv::Mat &processed) {
  cv::Mat temp;
  cv::cvtColor(input, processed, cv::COLOR_RGB2GRAY);
  // cv::cvtColor(temp, processed,
  //           cv::COLOR_GRAY2BGR); //これをしないとサイズ変わる
}
void edgeExtract(cv::Mat &input, cv::Mat &processed) {
  cv::Mat grayscaled, blured, sobeled;
  cv::flip(input, input, 1);
  // cv::GaussianBlur(input, blured, cv::Size(23, 23), 0.0);
  convertColorToGray(input, grayscaled);
  // cv::blur(grayscaled, blured, cv::Size(2, 2));
  cv::Canny(grayscaled, processed, 50, 100);
  cv::cvtColor(processed, processed, cv::COLOR_GRAY2RGB);
}
