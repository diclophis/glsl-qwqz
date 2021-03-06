// Vanilla Linux OpenGL+GLUT+SOIL App

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>

#include "opengles_bridge.h"
#include "libqwqz.h"
#include "impl_main.h"


FILE *iosfopen(const char *filename, const char *mode) {
  return fopen(filename, mode);
}


// Adds the given callback function to listen to the changes in browser window size.
//void set_window_resize_handler(void *userData, void (*handlerFunc)(int newWidth, int newHeight, void *userData));


static int game_index = 0;
static int left_down = 0;
static int right_down = 0;
static int reset_down = 0;
static int debug_down = 0;
int kWindowWidth = 0;
int kWindowHeight = 0;


void draw(void) {
  impl_draw(0);
  glutPostRedisplay();
}


void resize(int width, int height, void *userData) {
  kWindowWidth = width;
  kWindowHeight = height;
  impl_resize(width, height, width, height, 0);
  glutInitWindowSize(kWindowWidth, kWindowHeight);
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
}


void processMouseMotion(int x, int y) {
  impl_hit(x, y, 1);
}


void processNormalKeys(unsigned char key, int x, int y) {
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
      //Engine::Start(game_index, kWindowWidth, kWindowHeight);
    }
    reset_down = !reset_down;
  }
}


int main(int argc, char** argv) {
  printf("starting impl_main XXX\n");

  char *wh = NULL;
  
  wh = emscripten_run_script_string("(function(){return document.body.offsetWidth;})()");
  kWindowWidth = atoi(wh);
  wh = emscripten_run_script_string("(function(){return document.body.offsetHeight;})()");
  kWindowHeight = atoi(wh);

  glutInit(&argc,argv);
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("");
  glutKeyboardFunc(processNormalKeys);
  glutKeyboardUpFunc(processNormalKeys);
  glutMouseFunc(processMouse);
  glutMotionFunc(processMouseMotion);
  glutDisplayFunc(draw);
  //set_window_resize_handler(0, resize);

  if (0 == impl_main(argc, argv, 0)) {
    resize(kWindowWidth, kWindowHeight, 0);
    glutMainLoop();
  }

  return 0;
}
