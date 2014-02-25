// Vanilla MacOSX OpenGL App


#import <Foundation/Foundation.h>
#import <AppKit/NSImage.h>
#import <QuartzCore/QuartzCore.h>
#import <AudioToolbox/AudioToolbox.h>
#import <CoreAudio/CoreAudio.h>
#import <Accelerate/Accelerate.h>

#include "opengles_bridge.h"
#include "libqwqz.h"
#include "impl_main.h"

FILE *iosfopen(const char *filename, const char *mode) {
  return fopen(filename, mode);
}

static int kWindowWidth = 320;
static int kWindowHeight = 568;
static bool left_down = false;
static bool right_down = false;
static bool reset_down = false;
static bool debug_down = false;


void draw(void) {
  impl_draw(0);
  glutSwapBuffers();
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
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(kWindowWidth, kWindowHeight);
  glutInitWindowPosition(1000, 500);
  glutCreateWindow("main");
  glutDisplayFunc(draw);
  glutKeyboardFunc(processNormalKeys);
  glutKeyboardUpFunc(processNormalKeys);
  glutIgnoreKeyRepeat(true);
  glutMouseFunc(processMouse);
  glutMotionFunc(processMouseMotion);
  glutReshapeFunc(resize);

#if defined(__APPLE__) && !defined (VMDMESA)
  int swap_interval = 1;
  CGLContextObj cgl_context = CGLGetCurrentContext();
  CGLSetParameter(cgl_context, kCGLCPSwapInterval, &swap_interval);
#endif

  if (0 == impl_main(argc, argv, 0)) {
    glutMainLoop();
  }

  [pool release];

  return 0;
}
