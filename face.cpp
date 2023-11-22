#include "main.hpp"
void Face::updatePos() {

  center_x = 0;
  center_y = 0;
  eyel_center_x = 0;
  eyel_center_y = 0;
  eyer_center_x = 0;
  eyer_center_y = 0;
  float upper_mean = 0;
  float lower_mean = 0;
  for (auto i : upper_mouth) {
    center_x += shape.part(i).x();
    center_y += shape.part(i).y();
    // std::cerr << shape.part(i).x() << " " << shape.part(i).y() <<
    // std::endl;
    upper_mean += shape.part(i).y();
  }
  upper_mean /= upper_mouth.size();
  for (auto i : lower_mouth) {
    center_x += shape.part(i).x();
    center_y += shape.part(i).y();
    lower_mean += shape.part(i).y();
  }
  lower_mean /= lower_mouth.size();
  center_x /= upper_mouth.size() + lower_mouth.size();
  center_y /= upper_mouth.size() + lower_mouth.size();
  //   std::cerr << upper_mean << " " << lower_mean << std::endl;
  for (auto i : left_eye) {
    eyel_center_x += shape.part(i).x();
    eyel_center_y += shape.part(i).y();
  }
  eyel_center_x /= left_eye.size();
  eyel_center_y /= left_eye.size();
  for (auto i : right_eye) {
    eyer_center_x += shape.part(i).x();
    eyer_center_y += shape.part(i).y();
  }
  eyer_center_x /= right_eye.size();
  eyer_center_y /= right_eye.size();
  int upperMouthMax = -100000000;
  int lowerMouthMin = 10000000;
  int upperMouthMin = 100000000;
  int lowerMouthMax = -100000000;

  for (int i : upper_mouth) {
    // upperMouthMean += shape.part(i).y();
    upperMouthMax = std::max(upperMouthMax, (int)(shape.part(i).y()));
    upperMouthMin = std::min(upperMouthMin, (int)(shape.part(i).y()));
  }

  for (int i : lower_mouth) {
    lowerMouthMin = std::min(lowerMouthMin, (int)(shape.part(i).y()));
    lowerMouthMax = std::max(lowerMouthMax, (int)(shape.part(i).y()));
  }

  // upperMouthMean /= upper_mouth.size();
  // lowerMouthMean /= lower_mouth.size();
  is_open = std::fabs((float)(lowerMouthMin - upperMouthMax) /
                      (float)(lowerMouthMax - upperMouthMin)) > 0.1;
}

void Face::display(cv::Mat &img) {
  /* for (int i : upper_mouth) {
     int x = shape.part(i).x();
     int y = shape.part(i).y();
     cv::circle(img, cv::Point(x, y), 2,
                is_open ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255),
   -1);
   }
   for (int i : lower_mouth) {
     int x = shape.part(i).x();
     int y = shape.part(i).y();
     cv::circle(img, cv::Point(x, y), 2,
                is_open ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255),
   -1);
   }*/
  // std::cerr << is_open << std::endl;
  cv::circle(img, cv::Point(center_x, center_y), radius,
             is_open ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), -1);
  cv::circle(img, cv::Point(eyel_center_x, eyel_center_y), eye_radius,
             cv::Scalar(0, 255, 255), -1);
  cv::circle(img, cv::Point(eyer_center_x, eyer_center_y), eye_radius,
             cv::Scalar(0, 255, 255), -1);
  /*cv::putText(img, is_open ? "Mouth Open" : "Mouth Closed",
              cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1,
              is_open ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), 2);*/
}