#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <dlib/opencv/cv_image.h>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace dlib;

int main() {
  try {
    frontal_face_detector detector = get_frontal_face_detector();
    shape_predictor predictor;
    deserialize("/home/denjo/opencv/facial-landmarks-recognition/"
                "shape_predictor_68_face_landmarks.dat") >>
        predictor;

    std::vector<int> mouthPoints = {48, 49, 50, 51, 52, 53, 54,
                                    55, 56, 57, 58, 59, 60, 61};
    std::vector<int> upper_mouth = {50, 51, 52, 53, 54, 62, 63, 64};
    std::vector<int> lower_mouth = {56, 57, 58, 59, 60, 66, 67, 68};
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
      std::cerr << "Error: Couldn't open the camera." << std::endl;
      return -1;
    }

    while (true) {
      cv::Mat frame;
      cap >> frame;
      if (frame.empty()) {
        break;
      }

      // Dlibの形状予測器に変換
      dlib::cv_image<dlib::bgr_pixel> cimg(frame);

      std::vector<dlib::rectangle> faces = detector(cimg);
      // std::vector<full_object_detection> shapes;

      for (const auto &face : faces) {
        full_object_detection shape = predictor(cimg, face);
        //  shapes.push_back(shape);

        int upperMouthMean = 0;
        int lowerMouthMean = 0;

        for (int i : upper_mouth) {
          upperMouthMean += shape.part(i).y();
        }

        for (int i : lower_mouth) {
          lowerMouthMean += shape.part(i).y();
        }

        upperMouthMean /= upper_mouth.size();
        lowerMouthMean /= lower_mouth.size();

        bool isMouthOpen = (lowerMouthMean - upperMouthMean) > 3;

        // 結果を描画
        for (int i : mouthPoints) {
          int x = shape.part(i).x();
          int y = shape.part(i).y();
          cv::circle(
              frame, cv::Point(x, y), 2,
              isMouthOpen ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), -1);
        }

        cv::putText(frame, isMouthOpen ? "Mouth Open" : "Mouth Closed",
                    cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1,
                    isMouthOpen ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255),
                    2);
      }

      // 結果を表示
      cv::imshow("Mouth Open Detection", frame);

      // キー入力を待つ
      if (cv::waitKey(1) == 10) {
        break;
      }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;

  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    return -1;
  }
}
