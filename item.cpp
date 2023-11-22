#include "main.hpp"

void Item::init() {
  std::random_device seed_gen;
  engine.seed(seed_gen());
  std::uniform_real_distribution<float> ran(-1.0f, 1.0f);
  x = ran(engine) / 2.0;
  y = (ran(engine) - 1.0) / 4.0;
  float r = ran(engine);
  std::cerr << r << std::endl;
  if (r > 0.0) {
    type = BALL;
    y = -abs(y);
  } else {
    type = STAR;
    y = abs(y);
  }
}
void Item::drawItem() {
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

bool Item::checkTouching(cv::Mat &img) {
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
bool Item::checkTouching(Face &m, cv::Mat &img) {
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
inline double Item::get_ground_time() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now() - ground_time)
             .count() /
         1000.0;
}
void Item::updatePos() {
  if (on_ground) {
    if (get_ground_time() > 2.0) {
      is_dead = true;
    }
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
      ground_time = std::chrono::system_clock::now();
    }
  }
}