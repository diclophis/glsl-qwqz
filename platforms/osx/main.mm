// Vanilla MacOSX OpenGL App


#import <Foundation/Foundation.h>
#import <AppKit/NSImage.h>
#import <QuartzCore/QuartzCore.h>
#import <AudioToolbox/AudioToolbox.h>
#import <CoreAudio/CoreAudio.h>
#import <Accelerate/Accelerate.h>

extern "C" {
  #include "opengles_bridge.h"
  #include "libqwqz.h"
}


static int kWindowWidth = 1024;
static int kWindowHeight = 768;
static bool left_down = false;
static bool right_down = false;
static bool reset_down = false;
static bool debug_down = false;
static int game_index = 0;
static qwqz_handle qwqz_engine = NULL;


void draw(void) {
  qwqz_draw(qwqz_engine);
  glutSwapBuffers();
  glutPostRedisplay();
}


void resize(int width, int height) {
  kWindowWidth = width;
  kWindowHeight = height;
  qwqz_resize(qwqz_engine, (float)width, (float)height);
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
      if (key == 112) { // p
        //Engine::CurrentGamePause();
      } else if (key == 114) { // r
        //Engine::CurrentGameDestroyFoos();
        //Engine::CurrentGameCreateFoos();
        //Engine::CurrentGameStart();
      } else if (key == 115) { // s
        //if (game_index == 2) {
        //  game_index = 3;
        //} else {
        //  game_index = 2;
        //}

        //game_index++;
        if (game_index == 5) {
          game_index = 0;
        }

        //Engine::Start(game_index, kWindowWidth, kWindowHeight); //, textures, models, levels, sounds, NULL);
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

  qwqz_engine = qwqz_alloc();
  qwqz_init(qwqz_engine);

  glutMainLoop();

  [pool release];

  return 0;
}
