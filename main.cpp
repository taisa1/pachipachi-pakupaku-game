#include "cv.hpp"
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
std::chrono::system_clock::time_point start;
inline double get_time_sec() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now() - start)
             .count() /
         1000.0;
}

const int Item_N = 30;
cv::Mat frame, sobeled_frame, output_frame;
cv::VideoCapture cap;
void initGL(int argc, char *argv[]);
void init();
void setCallbackFunctions();

void glutDisplay();
void glutKeyboard(unsigned char key, int x, int y);
void glutIdle();
void drawBackground();

GLuint g_TextureHandles[3] = {0, 0, 0};
double g_angle1 = 0.0;
double g_angle2 = -3.14 / 3;
double g_distance = 5.0;
std::mt19937 engine;
std::uniform_real_distribution<float> ran(-1.0f, 1.0f);
dlib::frontal_face_detector detector;
dlib::shape_predictor predictor;
void drawOutlines(cv::Mat &img);

class Mouth {
public:
  const std::vector<int> upper_mouth = {50, 51, 52, 53, 54, 62, 63, 64};
  const std::vector<int> lower_mouth = {56, 57, 58, 59, 60, 66, 67, 68};
  const std::vector<int> left_eye = {36, 37, 38, 39, 40, 41};
  const std::vector<int> right_eye = {42, 43, 44, 45, 46, 47};
  float center_x, center_y, radius, eyel_center_x, eyer_center_x, eyel_center_y,
      eyer_center_y, eye_radius;
  bool is_open;
  int no_detected_frame;
  dlib::full_object_detection shape;
  Mouth()
      : center_x(0), center_y(0), is_open(false), no_detected_frame(0),
        radius(100), eye_radius(50) {}
  void updatePos() {
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
    is_open = (upper_mean - lower_mean > 1.5);
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
  }
  void display() {
    /* for (int i : upper_mouth) {
       int x = shape.part(i).x();
       int y = shape.part(i).y();
       cv::circle(frame, cv::Point(x, y), 2,
                  is_open ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255),
     -1);
     }
     for (int i : lower_mouth) {
       int x = shape.part(i).x();
       int y = shape.part(i).y();
       cv::circle(frame, cv::Point(x, y), 2,
                  is_open ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255),
     -1);
     }*/
    cv::circle(frame, cv::Point(center_x, center_y), radius,
               is_open ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), -1);
    cv::circle(frame, cv::Point(eyel_center_x, eyel_center_y), 50,
               is_open ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), -1);
    cv::circle(frame, cv::Point(eyer_center_x, eyer_center_y), 50,
               is_open ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), -1);
    /*cv::putText(frame, is_open ? "Mouth Open" : "Mouth Closed",
                cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1,
                is_open ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), 2);*/
  }
} mouth;
enum ItemType { BALL, STAR };
class Item {
public:
  const double GROUND_Z = 0.1;
  ItemType type;
  double x, y, z, r, vx, vy, vz, dt, g;
  int color_r, color_g, color_b, touching_frame;
  bool is_touching, is_dead, on_ground;
  double initial_x, initial_y;
  Item()
      : x(0), y(-0.5), z(2.0), vy(0), vz(10.0), r(0.03), color_r(1.0),
        dt(0.005), g(9.8 * 5), is_touching(false), is_dead(false),
        on_ground(false) {
    x = ran(engine)/2.0;
    y = (ran(engine) - 1.0) / 4.0;
    if (ran(engine) > 0.0) {
      type = BALL;
    } else {
      type = STAR;
    }
  }
  void drawItem() {
    if (is_dead)
      return;
    updatePos();
    updateColor();
    /* const int n = 20;
     glBegin(GL_POLYGON);
     glColor3d(color_r, 1.0, 1.0);
     for (int i = 0; i < n; i++) {
       glVertex3d(x + r * cos((double)i * 2.0 * M_PI / n),
                  y + r * sin((double)i * 2.0 * M_PI / n), z);
     }*/
    //球
    glPushMatrix();
    if (type == STAR) {
      glColor3d(1.0, 1.0, 0.0); //色の設定
    } else {
      glColor3d(0.0, 1.0, 0.0);
    }
    glTranslated(x, y, z); //平行移動値の設定
    glutSolidSphere(r, 20,
                    20); //引数：(半径, Z軸まわりの分割数, Z軸に沿った分割数)
    glPopMatrix();
    glEnd();
  }
  void updateY() {}
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
  bool checkTouching(cv::Mat &img) {
    if (!on_ground)
      return false;
    is_touching = false;
    for (int i = -5; i < 5; i++) {
      for (int j = -5; j < 5; j++) {
        int ny = static_cast<int>((1.0 - y) * img.rows / 2.0) + i;
        int nx = static_cast<int>((1.0 + x) * img.cols / 2.0) + j;
        if (ny < 0 || ny >= img.rows || nx < 0 || nx >= img.cols)
          continue;
        int intensity = img.at<cv::Vec3b>(ny, nx)[0];
        // std::cerr << ny << " " << nx << " " << intensity << std::endl;

        if (intensity > 254) {
          is_touching = true;
          // std::cerr << ny << " " << nx << " " << intensity << std::endl;
        }
      }
    }
    return is_touching;
  }
  bool checkTouching(Mouth &m, cv::Mat &img) {
    if (!on_ground)
      return false;
    is_touching = false;
    int ny = static_cast<int>((1.0 - y) * img.rows / 2.0);
    int nx = static_cast<int>((1.0 + x) * img.cols / 2.0);
    if (type == BALL && m.center_y - m.radius <= ny &&
        ny <= m.center_y + m.radius && m.center_x - m.radius <= nx &&
        nx <= m.center_x + m.radius) {
      is_touching = true;
    }
    if (type == STAR) {
      if (m.eyel_center_y - m.eye_radius <= ny &&
          ny <= m.eyel_center_y + m.eye_radius &&
          m.eyel_center_x - m.eye_radius <= nx &&
          nx <= m.eyel_center_x + m.eye_radius) {
        is_touching = true;
      }
      if (m.eyer_center_y - m.eye_radius <= ny &&
          ny <= m.eyer_center_y + m.eye_radius &&
          m.eyer_center_x - m.eye_radius <= nx &&
          nx <= m.eyer_center_x + m.eye_radius) {
        is_touching = true;
      }
    }
    if (is_touching) {
      touching_frame++;
      if (touching_frame >= 5) {
        is_dead = true;
      }
    } else {
      touching_frame = 0;
    }
    return is_touching;
  }
  void updateColor() {
    if (is_touching) {
      color_r = 0.0;
    } else {
      color_r = 1.0;
    }
  }
  void updatePos() {
    if (on_ground) {
      //  y -= dt * vy;
      // vy -= dt * g;
      //   if (y < -1 || y > 1) {
      // is_dead = true;
      // }
    } else {
      z -= dt * vz;
      if (z <= GROUND_Z) {
        std::cerr << "on_ground" << std::endl;
        on_ground = true;
      }
    }
  }
} item[Item_N];

enum State { TITLE, PLAY, RESULT };

class gameState {
public:
  size_t item_id;
  int score;
  State state;
  gameState() : item_id(0), state(State::PLAY), score(0) {}
} game;

void drawBitmapString(void *font, char *string) {
  glPushAttrib(GL_CURRENT_BIT);

  /* ビットマップ文字列の描画 */
  while (*string)
    glutBitmapCharacter(font, *string++);

  glPopAttrib();
}

int main(int argc, char *argv[]) {
  /* OpenGLの初期化 */
  initGL(argc, argv);

  /* このプログラム特有の初期化 */
  init();

  /* コールバック関数の登録 */
  setCallbackFunctions();

  /* メインループ */
  glutMainLoop();

  return 0;
}

void initGL(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(WINDOW_X, WINDOW_Y);
  glutCreateWindow(WINDOW_NAME);
}
double prev_time;

void init() {
  std::random_device seed_gen;
  engine.seed(seed_gen());
  detector = dlib::get_frontal_face_detector();
  dlib::deserialize("/home/denjo/opencv/facial-landmarks-recognition/"
                    "shape_predictor_68_face_landmarks.dat") >>
      predictor;
  glClearColor(0.2, 0.2, 0.2, 0.2);
  glGenTextures(3, g_TextureHandles);
  cap.open(0, cv::CAP_V4L2);
  cap.set(cv::CAP_PROP_FPS, FRAME_RATE);
  cap.set(cv::CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
  if (!cap.isOpened()) {
    printf("no input video\n");
    return;
  }
  for (int i = 0; i < 3; i++) {
    glBindTexture(GL_TEXTURE_2D, g_TextureHandles[i]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, i);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  }
  /* cap >> frame;
   dlib::cv_image<dlib::bgr_pixel> cimg(frame);
   std::vector<dlib::rectangle> faces = detector(cimg);
   mouth.shape = predictor(cimg, faces[0]);
   mouth.updatePos();
   edgeExtract(frame, output_frame);
   glBindTexture(GL_TEXTURE_2D, g_TextureHandles[0]);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, output_frame.cols,
   output_frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, output_frame.data);*/
  // set_texture();
  start = std::chrono::system_clock::now();
  prev_time = get_time_sec();
}

void setCallbackFunctions() {
  glutDisplayFunc(glutDisplay);
  glutKeyboardFunc(glutKeyboard);
  glutIdleFunc(glutIdle);
}

void glutKeyboard(unsigned char key, int x, int y) {
  switch (key) {
  /*case 'w':
    item.up();
    break;
  case 'a':
    item.left();
    break;
  case 's':
    item.down();
    break;
  case 'd':
    item.right();
    break;*/
  case 'q':
  case 'Q':
  case '\033':
    exit(0);
  }
  glutPostRedisplay();
}
char txt[20];

void glutDisplay() {
  if (game.state == State::PLAY) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, 1.0, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(g_distance * cos(g_angle2) * sin(g_angle1),
              g_distance * sin(g_angle2),
              g_distance * cos(g_angle2) * cos(g_angle1), 0.0, 0.0, 0.0, 0.0,
              1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    drawBackground();
    for (size_t i = 0; i < game.item_id; i++) {
      item[i].drawItem();
    }
    // drawOutlines(output_frame);
    glFlush();
    glDisable(GL_DEPTH_TEST);

    glutSwapBuffers();
  }
  // drawBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, txt);
}

void glutIdle() {
  if (game.state == State::PLAY) {

    cap >> frame;
    if (frame.empty()) {
      printf("no frame\n");
      return;
    }
    cv::flip(frame, frame, 1);
    dlib::cv_image<dlib::bgr_pixel> cimg(frame);
    //   std::cerr << "loaded" << std::endl;
    std::vector<dlib::rectangle> faces = detector(cimg);
    //   std::cerr << "detected" << std::endl;
    if (!faces.empty()) {
      mouth.no_detected_frame = 0;
      mouth.shape = predictor(cimg, faces[0]);
      mouth.updatePos();
      mouth.display();
    } else {
      mouth.no_detected_frame++;
      if (mouth.no_detected_frame > 10) {
        std::cerr << "cannot detect mouth" << std::endl;
      } else {
        mouth.display();
      }
    }
    // std::cerr << "predicted" << std::endl;

    // std::cerr << mouth.center_x << " " << mouth.center_y << std::endl;
    // std::cerr << mouth.is_open << std::endl;
    //   mouth.display();

    // convertColorToGray(frame, output_frame);
    // edgeExtract(frame, output_frame);
    double cur_time = get_time_sec();
    if (game.item_id + 1 < Item_N && prev_time + 2.0 < cur_time) {
      prev_time += 2.0;
      std::cerr << cur_time << std::endl;
      game.item_id++;
    }
    for (int i = 0; i < game.item_id; i++) {
      item[i].checkTouching(mouth, frame);
    }
    glBindTexture(GL_TEXTURE_2D, g_TextureHandles[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR,
                 GL_UNSIGNED_BYTE, frame.data);
    // int k = cv::waitKey(33);
    sprintf(txt, "item_id = %ld", game.item_id);
  } else if (game.state == State::RESULT) {
    //  std::cerr << "result" << std::endl;
  }
  glutPostRedisplay();
}

void drawBackground() {
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_POLYGON);
  glColor3d(1.0, 1.0, 1.0); //色の設定

  // glColor3d(1.0, 0.0, 0.0);
  glTexCoord2d(0.0, 1.0);
  glVertex3d(-1.0, -1.0, 0.0);
  // glColor3d(1.0, 1.0, 0.0);
  glTexCoord2d(1.0, 1.0);
  glVertex3d(1.0, -1.0, 0.0);
  // glColor3d(0.0, 1.0, 1.0);
  glTexCoord2d(1.0, 0.0);
  glVertex3d(1.0, 1.0, 0.0);
  // glColor3d(0.0, 0.0, 0.0);
  glTexCoord2d(0.0, 0.0);
  glVertex3d(-1.0, 1.0, 0.0);

  glEnd();
  glDisable(GL_TEXTURE_2D);
}

void drawOutlines(cv::Mat &img) {
  glLineWidth(10.0);
  for (int y = 0; y < img.rows; y++) {
    cv::Vec3b *src = img.ptr<cv::Vec3b>(y);
    double ny = 1.0 - 2.0 * y / img.rows;
    for (int x = 0; x < img.cols; x++) {
      int b = src[x][0];
      if (b > 0) {
        double nx = 2.0 * x / img.cols - 1.0;
        glBegin(GL_LINES);
        glVertex3f(nx, ny, 0);
        glVertex3f(nx, ny, 0.3);
        glEnd();
      }
    }
  }
}
