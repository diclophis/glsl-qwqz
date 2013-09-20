// Vanilla Linux OpenGL+GLUT+SOIL App

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>

#include "opengles_bridge.h"
#include "libqwqz.h"

static int game_index = 0;
static int left_down = 0;
static int right_down = 0;
static int reset_down = 0;
static int debug_down = 0;
int kWindowWidth = 0;
int kWindowHeight = 0;


void draw(void) {
  impl_draw();
  glutPostRedisplay();
}


void resize(int width, int height) {
  kWindowWidth = width;
  kWindowHeight = height;
  impl_resize((float)width, (float)height);
}


void processMouse(int button, int state, int x, int y) {
  switch (state) {
    case GLUT_DOWN:
      //Engine::CurrentGameHit(x, y, 0);
      break;
    case GLUT_UP:
      //Engine::CurrentGameHit(x, y, 2);
      break;
  }
}


void processMouseMotion(int x, int y) {
  //Engine::CurrentGameHit(x, y, 1);
}


void processNormalKeys(unsigned char key, int x, int y) {
  LOGV("key: %d %c\n", key, key);
  if (key == 49) {
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
      //Engine::Start(game_index, kWindowWidth, kWindowHeight);
    }
    reset_down = !reset_down;
  }
}


int main(int argc, char** argv) {
  char *wh;
  
  wh = emscripten_run_script_string("(function(){return document.body.offsetWidth;})()");
  kWindowWidth = atoi(wh);

  wh = emscripten_run_script_string("(function(){return document.body.offsetHeight;})()");
  kWindowHeight = atoi(wh);

  LOGV("%s %d %d\n", wh, kWindowWidth, kWindowHeight);

  glutInit(&argc,argv);
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(kWindowWidth, kWindowHeight);
  glutCreateWindow("does emscripten GLUT wrapper set window.title?, do I need a title? //TODO");
  glutKeyboardFunc(processNormalKeys);
  glutKeyboardUpFunc(processNormalKeys);
  glutMouseFunc(processMouse);
  glutMotionFunc(processMouseMotion);
  glutDisplayFunc(draw);
  glutReshapeFunc(resize);

  if (0 == impl_main(argc, argv)) {
    glutMainLoop();
  }

  return 0;
}
