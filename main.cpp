#include "main.hpp"
#include "cv.hpp"

const int Item_N = 30;
cv::Mat frame, sobeled_frame, output_frame;
cv::VideoCapture cap;

dlib::frontal_face_detector detector;
dlib::shape_predictor predictor;
std::chrono::system_clock::time_point start;
inline double get_time_sec() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now() - start)
             .count() /
         1000.0;
}

GLuint g_TextureHandles[3] = {0, 0, 0};
double g_angle1 = 0.0;
double g_angle2 = 0.0;
double g_distance = 15;
double prev_time, end_time;
;

Face face;
Item item[Item_N];
gameState game;
bool is_end;

void drawBitmapString(void *font, char *string) {
  glPushAttrib(GL_CURRENT_BIT);

  /* ビットマップ文字列の描画 */
  while (*string) {
    glutBitmapCharacter(font, *string++);
  }

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
void init() {
  is_end = false;
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
}

void setCallbackFunctions() {
  glutDisplayFunc(glutDisplay);
  glutKeyboardFunc(glutKeyboard);
  glutIdleFunc(glutIdle);
  glutMouseFunc(glutMouse);
}
void glutMouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      if (game.state == TITLE) {
        game.state = PLAY;
        start = std::chrono::system_clock::now();
        prev_time = get_time_sec();
      }
    }
  }
}
void glutKeyboard(unsigned char key, int x, int y) {
  switch (key) {
  /*case 'w':
    item.up();エフェクト

スコア表示
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
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, 1.0, 0.1, 100);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(g_distance * cos(g_angle2) * sin(g_angle1),
            g_distance * sin(g_angle2) - 0.5,
            g_distance * cos(g_angle2) * cos(g_angle1), 0.0, 0.0, 0.0, 0.0, 1.0,
            0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  if (game.state == State::TITLE) {
    glRasterPos2f(0.0, -0.3);
    glColor3f(1.0, 1.0, 1.0);
    drawBitmapString(GLUT_BITMAP_HELVETICA_18, "click to start");
    glRasterPos2f(0.0, 0.3);
    glColor3f(1.0, 1.0, 1.0);
    drawBitmapString(GLUT_BITMAP_HELVETICA_18, "PACHIPACHI PAKUPAKU GAME");
  } else if (game.state == State::PLAY) {

    drawBackground();
    glRasterPos2f(-3.0, 0.0);
    glColor3d(1.0, 1.0, 1.0);
    char str[20];
    sprintf(str, "score : %d", game.score);
    drawBitmapString(GLUT_BITMAP_HELVETICA_18, str);
    for (size_t i = 0; i < game.item_id; i++) {
      item[i].drawItem();
    }

    drawFullscreen();
    // drawOutlines(output_frame);
  } else if (game.state == State::RESULT) {
    glRasterPos2f(0.0, -0.3);
    glColor3f(1.0, 1.0, 1.0);
    drawBitmapString(GLUT_BITMAP_HELVETICA_18, "Thank you for playing!");
    glRasterPos2f(0.0, 0.3);
    glColor3f(1.0, 1.0, 1.0);
    char str[20];
    sprintf(str, "your score : %d", game.score);
    drawBitmapString(GLUT_BITMAP_HELVETICA_18, str);
  }
  // drawBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, txt);
  glFlush();

  glDisable(GL_DEPTH_TEST);

  glutSwapBuffers();
}

void glutIdle() {
  if (game.state == State::TITLE) {

  } else if (game.state == State::PLAY) {
    cap >> frame;
    if (frame.empty()) {
      printf("no frame\n");
      return;
    }
    cv::flip(frame, frame, 1);
    brighter(frame);
    dlib::cv_image<dlib::bgr_pixel> cimg(frame);
    //   std::cerr << "loaded" << std::endl;
    std::vector<dlib::rectangle> faces = detector(cimg);
    cv::resize(frame, frame, cv::Size(), (double)1 / 128, (double)1 / 128);
    cv::resize(frame, frame, cv::Size(), 128, 128);
    //   std::cerr << "detected" << std::endl;
    if (!faces.empty()) {
      face.no_detected_frame = 0;
      face.shape = predictor(cimg, faces[0]);
      face.updatePos();
      face.display(frame);
    } else {
      face.no_detected_frame++;
      if (face.no_detected_frame > 10) {
        std::cerr << "cannot detect face" << std::endl;
      } else {
        face.display(frame);
      }
    }
    // std::cerr << "predicted" << std::endl;

    // std::cerr << face.center_x << " " << face.center_y << std::endl;
    // std::cerr << face.is_open << std::endl;
    //   face.display();

    // convertColorToGray(frame, output_frame);
    // edgeExtract(frame, output_frame);

    double cur_time = get_time_sec();
    if (game.item_id + 1 < Item_N && prev_time + 2.0 < cur_time) {
      prev_time += 2.0;
      std::cerr << "t = " << cur_time << ", score = " << game.score
                << std::endl;
      game.item_id++;
    }
    for (int i = 0; i < game.item_id; i++) {
      item[i].checkTouching(face, frame, game);
    }
    glBindTexture(GL_TEXTURE_2D, g_TextureHandles[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR,
                 GL_UNSIGNED_BYTE, frame.data);
    // int k = cv::waitKey(33);
    sprintf(txt, "item_id = %ld", game.item_id);
    if (game.color_a > 0.0) {
      game.color_a = std::max(0.0, game.color_a - 0.06);
    }
    if (game.item_id == Item_N - 1) {
      int dead_count = 0;
      for (int i = 0; i < game.item_id; i++) {
        dead_count += (item[i].is_dead);
      }
      if (dead_count == Item_N - 1) {
        if (!is_end) {
          end_time = get_time_sec();
          is_end = true;
        } else {
          if (cur_time - end_time > 2.0) {
            game.state = State::RESULT;
          }
        }
      }
    }
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
  glBegin(GL_LINES);
  glVertex3f(-1, -1, 0);
  glVertex3f(-1, -1, g_distance);
  glBegin(GL_LINES);
  glVertex3f(1, -1, 0);
  glVertex3f(1, -1, g_distance);
  glBegin(GL_LINES);
  glVertex3f(-1, 1, 0);
  glVertex3f(-1, 1, g_distance);
  glBegin(GL_LINES);
  glVertex3f(1, 1, 0);
  glVertex3f(1, 1, g_distance);
  glEnd();
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

void drawFullscreen() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST); // 深度テストを無効化
  glColor4f(game.color_r, game.color_g, game.color_b, game.color_a);
  glBegin(GL_POLYGON);
  // glColor3d(1.0, 0.0, 0.0);
  glVertex3d(-1.0, -1.0, 14);
  // glColor3d(1.0, 1.0, 0.0);
  glVertex3d(1.0, -1.0, 14);
  // glColor3d(0.0, 1.0, 1.0);
  glVertex3d(1.0, 1.0, 14);
  // glColor3d(0.0, 0.0, 0.0);
  glVertex3d(-1.0, 1.0, 14);
  glEnd();
  glEnable(GL_DEPTH_TEST); // 描画後に深度テストを有効化
  glDisable(GL_BLEND);
}
