#include "cv.hpp"
#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
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

const int BALL_N = 10;

void initGL(int argc, char *argv[]);
void init();
void setCallbackFunctions();

void glutDisplay();
void glutKeyboard(unsigned char key, int x, int y);
void glutIdle();
void drawBackground();

GLuint g_TextureHandles[3] = {0, 0, 0};

class Ball {
public:
  double x, y, r, vx, vy, dt, g;
  int color_r, touching_frame;
  bool is_touching, is_dead;
  Ball()
      : x(0), y(0), vy(0), r(0.03), color_r(1.0), dt(0.005), g(9.8),
        is_touching(false), is_dead(false) {}
  void drawBall() {
    if (is_dead)
      return;
    updatePos();
    updateColor();
    const int n = 20;
    glBegin(GL_POLYGON);
    glColor3d(color_r, 1.0, 1.0);
    for (int i = 0; i < n; i++) {
      glVertex2d(x + r * cos((double)i * 2.0 * M_PI / n),
                 y + r * sin((double)i * 2.0 * M_PI / n));
    }
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
    is_touching = false;
    for (int i = -5; i < 5; i++) {
      for (int j = -5; j < 5; j++) {
        int ny = static_cast<int>((1.0 - y) * img.rows / 2.0) + i;
        int nx = static_cast<int>((1.0 + x) * img.cols / 2.0) + j;
        if (ny < 0 || ny >= img.rows || nx < 0 || nx >= img.cols)
          continue;
        int intensity = img.at<cv::Vec3b>(ny, nx)[0];
        // std::cerr << ny << " " << nx << " " << intensity << std::endl;

        if (intensity > 0) {
          is_touching = true;
          // std::cerr << ny << " " << nx << " " << intensity << std::endl;
        }
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
    y += dt * vy;
    vy -= dt * g;
    if (y < -1) {
      is_dead = true;
    }
  }
} ball[BALL_N];

enum State { TITLE, PLAY, RESULT };

class gameState {
public:
  int ball_id;
  State state;
  gameState() : ball_id(0), state(State::PLAY) {}
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
  glutInitDisplayMode(GLUT_RGBA);
  glutInitWindowSize(WINDOW_X, WINDOW_Y);
  glutCreateWindow(WINDOW_NAME);
}

void init() {
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
  cap >> frame;
  edgeExtract(frame, output_frame);
  glBindTexture(GL_TEXTURE_2D, g_TextureHandles[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, output_frame.cols, output_frame.rows,
               0, GL_BGR, GL_UNSIGNED_BYTE, output_frame.data);
  // set_texture();
}

void setCallbackFunctions() {
  glutDisplayFunc(glutDisplay);
  glutKeyboardFunc(glutKeyboard);
  glutIdleFunc(glutIdle);
}

void glutKeyboard(unsigned char key, int x, int y) {
  switch (key) {
  /*case 'w':
    ball.up();
    break;
  case 'a':
    ball.left();
    break;
  case 's':
    ball.down();
    break;
  case 'd':
    ball.right();
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
    drawBackground();
    ball[game.ball_id].drawBall();
  }
  // drawBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, txt);
  glFlush();
}

void glutIdle() {
  if (game.state == State::PLAY) {

    cap >> frame;
    if (frame.empty()) {
      printf("no frame\n");
      return;
    }
    // convertColorToGray(frame, output_frame);
    edgeExtract(frame, output_frame);
    ball[game.ball_id].checkTouching(output_frame);
    glBindTexture(GL_TEXTURE_2D, g_TextureHandles[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, output_frame.cols, output_frame.rows,
                 0, GL_BGR, GL_UNSIGNED_BYTE, output_frame.data);
    // int k = cv::waitKey(33);
    if (ball[game.ball_id].is_dead) {
      game.ball_id++;
      std::cerr << "ball_id = " << game.ball_id << std::endl;
    }
    if (game.ball_id >= BALL_N) {
      game.state = State::RESULT;
      // std::cerr << "result" << std::endl;
    }
    sprintf(txt, "ball_id = %d", game.ball_id);
  } else if (game.state == State::RESULT) {
    //  std::cerr << "result" << std::endl;
  }
  glutPostRedisplay();
}

void drawBackground() {
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_POLYGON);

  // glColor3d(1.0, 0.0, 0.0);
  glTexCoord2d(0.0, 1.0);
  glVertex2d(-0.9, -0.9);
  // glColor3d(1.0, 1.0, 0.0);
  glTexCoord2d(1.0, 1.0);
  glVertex2d(0.9, -0.9);
  // glColor3d(0.0, 1.0, 1.0);
  glTexCoord2d(1.0, 0.0);
  glVertex2d(0.9, 0.9);
  // glColor3d(0.0, 0.0, 0.0);
  glTexCoord2d(0.0, 0.0);
  glVertex2d(-0.9, 0.9);

  glEnd();
  glDisable(GL_TEXTURE_2D);
}
