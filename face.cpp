#include "main.hpp"
double dist(dlib::point &p1, dlib::point &p2) {
  return std::sqrt((p1.x() - p2.x()) * (p1.x() - p2.x()) +
                   (p1.y() - p2.y()) * (p1.y() - p2.y()));
}
double calcEar(dlib::point &p1, dlib::point &p2, dlib::point &p3,
               dlib::point &p4, dlib::point &p5, dlib::point &p6) {
  return (dist(p2, p6) + dist(p3, p5)) / 2.0 / dist(p1, p4);
}
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
  bool mouthopen =
      calcEar(shape.part(60), shape.part(61), shape.part(63), shape.part(64),
              shape.part(65), shape.part(67)) > 0.2;
  if (is_mouthopen != mouthopen) {
    mouth_count++;
    is_mouthopen = mouthopen;
  }
  bool lefteyeopen =
      calcEar(shape.part(36), shape.part(37), shape.part(38), shape.part(39),
              shape.part(40), shape.part(41)) > 0.2;
  if (is_lefteyeopen != lefteyeopen) {
    lefteye_count++;
    is_lefteyeopen = lefteyeopen;
  }
  bool righteyeopen =
      calcEar(shape.part(42), shape.part(43), shape.part(44), shape.part(45),
              shape.part(46), shape.part(47)) > 0.2;
  if (is_righteyeopen != righteyeopen) {
    righteye_count++;
    is_righteyeopen = righteyeopen;
  }
}

void Face::display(cv::Mat &img) {

  // std::cerr << is_mouthope << std::endl;
  img.setTo(cv::Scalar(0, 0, 0));
  cv::Scalar mouth_color = cv::Scalar(90, 200, 0);
  if (is_mouthopen) {
    cv::circle(img, cv::Point(center_x, center_y), radius, mouth_color, -1);
  } else {
    cv::line(img, cv::Point(center_x - radius / 2, center_y),
             cv::Point(center_x + radius / 2, center_y), mouth_color, 5);
  }
  cv::Scalar eye_color = cv::Scalar(0, 200, 200);
  if (is_lefteyeopen) {
    cv::circle(img, cv::Point(eyel_center_x, eyel_center_y), eye_radius,
               eye_color, -1);
  } else {
    cv::line(img, cv::Point(eyel_center_x - eye_radius / 2, eyel_center_y),
             cv::Point(eyel_center_x + eye_radius / 2, eyel_center_y),
             eye_color, 5);
  }
  if (is_righteyeopen) {
    cv::circle(img, cv::Point(eyer_center_x, eyer_center_y), eye_radius,
               eye_color, -1);
  } else {
    cv::line(img, cv::Point(eyer_center_x - eye_radius / 2, eyer_center_y),
             cv::Point(eyer_center_x + eye_radius / 2, eyer_center_y),
             eye_color, 5);
  }
  /*cv::putText(img, is_mouthopen ? "Mouth Open" : "Mouth Closed",
              cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1,
              is_mouthopen ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255),
     2);*/
}