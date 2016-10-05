// Vanilla Linux OpenGL+GLUT+SOIL App

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>

#include <dirent.h>

/*
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(kWindowWidth, kWindowHeight);
  glutInitWindowPosition(256,256);
  glutCreateWindow("simple");
  glutKeyboardFunc(processNormalKeys);
  glutKeyboardUpFunc(processNormalKeys);
  glutIgnoreKeyRepeat(1);
  glutMouseFunc(processMouse);
  glutMotionFunc(processMouseMotion);
  glutDisplayFunc(draw);
  glutReshapeFunc(resize);
*/


#include "opengles_bridge.h"
#include "libqwqz.h"
#include "impl_main.h"

FILE *iosfopen(const char *filename, const char *mode) {
  return fopen(filename, mode);
}

static int kWindowWidth = 320;
static int kWindowHeight = 568;
static int left_down = 0;
static int right_down = 0;
static int reset_down = 0;
static int debug_down = 0;


void draw(void) {
  impl_draw(0);
  glutSwapBuffers();
  glutPostRedisplay();
}


void resize(int width, int height) {
  kWindowWidth = width;
  kWindowHeight = height;
  impl_resize((float)width, (float)height, (float)width, (float)height, 0);
  glViewport(0, 0, width, height);
}


void processMouse(int button, int state, int x, int y) {
  switch (state) {
    case GLUT_DOWN:
      impl_hit(x, y, 0);
      break;
    case GLUT_UP:
      impl_hit(x, y, 2);
      break;
  }

  glutPostRedisplay();
}


void processMouseMotion(int x, int y) {
  impl_hit(x, y, 1);
}


void processNormalKeys(unsigned char key, int x, int y) {
  ///LOGV("key: %d %c\n", key, key);
  if (key == 32) {
    if (debug_down) {
      impl_hit(0, 0, 2);
    } else {
      impl_hit(0, 0, 0);
    }
    debug_down = !debug_down;

  } else if (key == 49) {
    if (debug_down) {
      //Engine::CurrentGameHit(0, 0, 2);
    } else {
      //Engine::CurrentGameHit(0, 0, 0);
    }
    debug_down = !debug_down;
  } else if (key == 110) {
    if (left_down) {
      //Engine::CurrentGameHit(0, 1024, 2);
    } else {
      //Engine::CurrentGameHit(0, 1024, 0);
    }
    left_down = !left_down;
  } else if (key == 109) {
    if (right_down) {
      //Engine::CurrentGameHit(1024, 1024, 2);
    } else {
      //Engine::CurrentGameHit(1024, 1024, 0);
    }
    right_down = !right_down;
  } else {
    if (reset_down) {

    } else {
      if (key == 112) { // p
        //Engine::CurrentGamePause();
      } else if (key == 114) { // r
        //Engine::CurrentGameDestroyFoos();
        //Engine::CurrentGameCreateFoos();
        //Engine::CurrentGameStart();
      } else if (key == 115) { // s
      }
    }
    reset_down = !reset_down;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(kWindowWidth, kWindowHeight);
  glutInitWindowPosition(1000, 500);
  glutCreateWindow("main");
  glutDisplayFunc(draw);
  //glutOverlayDisplayFunc(draw);
  glutKeyboardFunc(processNormalKeys);
  glutKeyboardUpFunc(processNormalKeys);
  glutIgnoreKeyRepeat(1);
  glutMouseFunc(processMouse);
  glutMotionFunc(processMouseMotion);
  glutReshapeFunc(resize);
  //glutFullScreen();

  if (0 == impl_main(argc, argv, 0)) {
    glutMainLoop();
  }

  return 0;
}
