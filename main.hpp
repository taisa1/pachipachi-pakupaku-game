#ifndef MAIN_HPP
#include <GL/glut.h>
#include <chrono>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <dlib/opencv/cv_image.h>
#include <iostream>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <random>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_X (800)
#define WINDOW_Y (600)
#define WINDOW_NAME "game"
#define TEXTURE_HEIGHT (800)
#define TEXTURE_WIDTH (600)
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define FRAME_RATE 30

using namespace std::chrono;

void initGL(int argc, char *argv[]);
void init();
void setCallbackFunctions();

void glutDisplay();
void glutKeyboard(unsigned char key, int x, int y);
void glutIdle();
void drawBackground();

void drawOutlines(cv::Mat &img);

class Face {
public:
  std::vector<int> upper_mouth = {48, 49, 50, 51, 52, 53,
                                  54, 60, 61, 62, 63, 64};
  std::vector<int> lower_mouth = {55, 56, 57, 58, 59, 60, 66, 67};
  const std::vector<int> left_eye = {36, 37, 38, 39, 40, 41};
  const std::vector<int> right_eye = {42, 43, 44, 45, 46, 47};
  float center_x, center_y, radius, eyel_center_x, eyer_center_x, eyel_center_y,
      eyer_center_y, eye_radius;
  bool is_open;
  int no_detected_frame;
  dlib::full_object_detection shape;
  Face()
      : center_x(0), center_y(0), is_open(false), no_detected_frame(0),
        radius(50), eye_radius(30) {}
  void updatePos();
  void display(cv::Mat &img);
};

enum ItemType { BALL, STAR };

class Item {
public:
  std::mt19937 engine;

  const double GROUND_Z = 0.1;
  ItemType type;
  double x, y, z, r, vx, vy, vz, dt, g;
  int color_r, color_g, color_b, touching_frame;
  bool is_touching, is_dead, on_ground;
  double initial_x, initial_y;
  std::chrono::system_clock::time_point ground_time;
  Item()
      : x(0), y(-0.5), z(20.0), vy(0), vz(100.0), r(0.1), color_r(1.0),
        dt(0.005), g(9.8 * 5), is_touching(false), is_dead(false),
        on_ground(false) {
    init();
  }
  void init();
  void drawItem();
  void updateY() {}
  inline double get_ground_time();
  void up() {
    y += r;
    y = std::min(y, 1.0);
  }
  void down() {
    y -= r;
    y = std::max(y, -1.0);
  }
  void right() {
    x += r;
    x = std::min(x, 1.0);
  }
  void left() {
    x -= r;
    x = std::max(x, -1.0);
  }
  bool checkTouching(cv::Mat &img);
  bool checkTouching(Face &m, cv::Mat &img);
  void updateColor() {
    if (is_touching) {
      color_r = 0.0;
    } else {
      color_r = 1.0;
    }
  }
  void updatePos();
};

enum State { TITLE, PLAY, RESULT };

class gameState {
public:
  size_t item_id;
  int score;
  State state;
  gameState() : item_id(0), state(State::PLAY), score(0) {}
};

#define MAIN_HPP
#endif