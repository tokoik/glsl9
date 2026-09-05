/* OpenGL */
#if defined(__APPLE__)
#  define GL_SILENCE_DEPRECATION
#  include <GLUT/glut.h>
#  include <OpenGL/glext.h>
#else
#  if defined(_MSC_VER)
#    define _USE_MATH_DEFINES
#    define _CRT_SECURE_NO_WARNINGS
#  else
#    define GL_GLEXT_PROTOTYPES
#  endif
#  include <GL/glut.h>
#  include <GL/glext.h>
#endif
#include <math.h>
#include "scene.h"

/*
** タイルの描画
*/
static void tile(double w, double d, int nw, int nd)
{
  /* タイルの色 */
  static const GLfloat color[][4] = {
    { 0.6f, 0.6f, 0.6f, 1.0f },
    { 0.3f, 0.3f, 0.3f, 1.0f }
  };
  static const GLfloat specular[] = {
    0.2f, 0.2f, 0.2f, 1.0f
  };
  
  /* タイルの材質 */
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 40.0f);

  glNormal3d(0.0, 1.0, 0.0);
  glBegin(GL_QUADS);
  for (int j = 0; j < nd; ++j) {
    GLdouble dj = d * j, djd = dj + d;

    for (int i = 0; i < nw; ++i) {
      GLdouble wi = w * i, wiw = wi + w;

      glColor3fv(color[(i + j) & 1]);
      glVertex3d(wi,  0.0, dj);
      glVertex3d(wi,  0.0, djd);
      glVertex3d(wiw, 0.0, djd);
      glVertex3d(wiw, 0.0, dj);
    }
  }
  glEnd();
}

/*
** 箱の描画
*/
static void box(double x, double y, double z)
{
  /* 頂点データ */
  static const GLdouble vertex[][3] = {
    { 0.0, 0.0, 0.0 },
    {   x, 0.0, 0.0 },
    {   x,   y, 0.0 },
    { 0.0,   y, 0.0 },
    { 0.0, 0.0,   z },
    {   x, 0.0,   z },
    {   x,   y,   z },
    { 0.0,   y,   z },
  };
  
  /* 面データ */
  static const GLdouble *face[][4] = {
    { vertex[0], vertex[1], vertex[2], vertex[3] },
    { vertex[1], vertex[5], vertex[6], vertex[2] },
    { vertex[5], vertex[4], vertex[7], vertex[6] },
    { vertex[4], vertex[0], vertex[3], vertex[7] },
    { vertex[4], vertex[5], vertex[1], vertex[0] },
    { vertex[3], vertex[2], vertex[6], vertex[7] },
  };
  
  /* 面の法線ベクトル */
  static const GLdouble normal[][3] = {
    { 0.0, 0.0,-1.0 },
    { 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0 },
    {-1.0, 0.0, 0.0 },
    { 0.0,-1.0, 0.0 },
    { 0.0, 1.0, 0.0 },
  };
  
  /* 箱の色 */
  static const GLfloat color[] = {
    0.8f, 0.8f, 0.2f, 1.0f
  };
  static const GLfloat specular[] = {
    0.1f, 0.1f, 0.1f, 1.0f
  };

  /* 箱の材質 */
  glColor3fv(color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0f);

  glBegin(GL_QUADS);
  for (int j = 0; j < 6; j++) {
    glNormal3dv(normal[j]);
    for (int i = 4; --i >= 0;) {
      glVertex3dv(face[j][i]);
    }
  }
  glEnd();
}

/*
** シーンの描画
*/
void scene(double t)
{
  static const double r = 1.5;
  double wt = 2.0 * M_PI * t;

  /* タイルを描く */
  glPushMatrix();
  glTranslated(-3.0, -2.0, -3.0);
  tile(1.0, 1.0, 6, 6);
  glPopMatrix();
  
  /* 箱を描く */
  glPushMatrix();
  glTranslated(-1.0, -1.5, -1.0);
  box(2.0, 1.0, 2.0);
  glPopMatrix();
  
  /* 球を描く */
  glPushMatrix();
  glTranslated(r * cos(wt), 1.0, r * sin(wt));
  static const GLfloat red[] = { 0.8f, 0.2f, 0.2f, 1.0f };
  static const GLfloat specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
  glColor3fv(red);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0f);
  glutSolidSphere(0.9, 32, 16);
  glPopMatrix();
}
