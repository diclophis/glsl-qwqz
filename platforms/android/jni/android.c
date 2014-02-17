#include <jni.h>
#include <sys/time.h>
#include <time.h>
#include <android/log.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <pthread.h>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "opengles_bridge.h"
#include "libqwqz.h"
#include "impl_main.h"


static AAssetManager* mgr;
static AAssetDir* assetDir;

static JavaVM *g_Vm;
static JNIEnv *g_Env;
static JNIEnv *g_Env3;
//static jshortArray ab = NULL;
static jclass player;
jmethodID android_dumpAudio;
static int min_buffer;
static pthread_t audio_thread;
static int sWindowWidth  = 0;
static int sWindowHeight = 0;
static jobject activity;
int playing_audio = 0;
FILE *out = 0;

#define ASSET_BUFF 1

FILE *iosfopen(const char *filename, const char *mode) {

if (out) {
  //fclose(out);
}

//filename = filename + 7;

assert(mgr);

  AAsset* asset = AAssetManager_open(mgr, filename + 7, AASSET_MODE_UNKNOWN);
  if (asset != NULL) {

    off_t filesize = AAsset_getLength(asset);
    char *buf = (char *)malloc(filesize);
    LOGV("ok %s %d\n", filename, filesize);

    //char *tmp = mktemp("XXX");
    //LOGV("trying %s\n", tmp);

    out = fopen("/sdcard/XXX", "w"); //mkstemp("/tmp/XXX"); //fopen(tmp, "w");
    assert(out > 0);
    LOGV("got buffer address: %d\n", out);

    int nb_read = 0;
    while ((nb_read = AAsset_read(asset, buf, filesize)) > 0) {
      LOGV("need to buffer: %d\n", nb_read);
      LOGV("\n\n%d\n\n", strlen(buf));
      fwrite(buf, nb_read, 1, out);
    }

    fclose(out);

    out = fopen("/sdcard/XXX", "r");
    //fseek(out, 0);
    //rewind(out);

    //free(buf);

    //AAsset_close(asset);

    LOGV("loaded: %s\n", filename);

    return out;
  } else {
    LOGV("wtf %s\n", filename);
  }

  return 0;
}


void *pump_audio(void *);
void create_audio_thread();


void create_audio_thread() {
  playing_audio = 0;
  pthread_attr_t attr; 
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_create(&audio_thread, 0, pump_audio, NULL);
}


int Java_com_risingcode_qwqz_DemoActivity_initNative(
  JNIEnv * env, void* reserved, jobject thiz
);


void Java_com_risingcode_qwqz_DemoActivity_setMinBuffer(JNIEnv * env, jclass envClass, int size);
void Java_com_risingcode_qwqz_DemoRenderer_nativeOnSurfaceCreated(JNIEnv* env, jobject thiz);
void Java_com_risingcode_qwqz_DemoRenderer_nativeResize(JNIEnv* env, jobject thiz, jint width, jint height);
void Java_com_risingcode_qwqz_DemoGLSurfaceView_nativePause(JNIEnv*  env);
void Java_com_risingcode_qwqz_DemoGLSurfaceView_nativeResume(JNIEnv*  env);
void Java_com_risingcode_qwqz_DemoRenderer_nativeRender(JNIEnv* env);
void Java_com_risingcode_qwqz_DemoGLSurfaceView_nativeTouch(JNIEnv* env, jobject thiz, jfloat x, jfloat y, jint hitState);


void *pump_audio(void *userData) {
/*
  playing_audio = 1;
  short *b = new short[min_buffer];

  if (g_Env == NULL) {
    g_Vm->AttachCurrentThread(&g_Env, NULL);
  }

  if (ab == NULL) {
    jobject tmp;
    ab = g_Env->NewShortArray(min_buffer);
    tmp = ab;
    ab = (jshortArray)g_Env->NewGlobalRef(ab);
    g_Env->DeleteLocalRef(tmp);
  }

  if (android_dumpAudio == NULL) {
    android_dumpAudio = g_Env->GetStaticMethodID(player, "writeAudio", "([SII)V");
  }

  while (playing_audio) {
    //Engine::CurrentGameDoAudio(b, min_buffer * sizeof(short));
    g_Env->SetShortArrayRegion(ab, 0, min_buffer, b);
    g_Env->CallStaticVoidMethod(player, android_dumpAudio, ab, 0, min_buffer);
  }

  delete b;

  g_Vm->DetachCurrentThread();
*/
  return NULL;
}


void SimulationThreadCleanup() {
  g_Env3 = NULL;
  g_Env = NULL;
  android_dumpAudio = NULL;
  (*g_Vm)->DetachCurrentThread(g_Vm);
}


JNIEXPORT jint JNICALL JNI_OnLoad (JavaVM * vm, void * reserved) {
  g_Vm = vm;
  JNIEnv *env;
  jobject tmp;
  (*g_Vm)->GetEnv(g_Vm, (void **)&env, JNI_VERSION_1_6);
  player = (*env)->FindClass(env, "com/risingcode/qwqz/DemoActivity");
  tmp = player;
  player = (jclass)(*env)->NewGlobalRef(env, player);
  (*env)->DeleteLocalRef(env, tmp);
  return JNI_VERSION_1_6;
}


void Java_com_risingcode_qwqz_DemoActivity_setMinBuffer(
  JNIEnv * env, jclass envClass,
  int size
) {
  min_buffer = size / 4;
}


int Java_com_risingcode_qwqz_DemoActivity_initNative(
  JNIEnv * env, void* thiz, jobject reserved
) {

activity = (jobject)(*env)->NewGlobalRef(env, thiz);
mgr = AAssetManager_fromJava(env, reserved);
assetDir  = AAssetManager_openDir(mgr, "");

  return 0;
}


void Java_com_risingcode_qwqz_DemoRenderer_nativeOnSurfaceCreated(JNIEnv* env, jobject thiz) {
  /*
  if (Engine::CurrentGame()) {
    Engine::CurrentGameDestroyFoos();
    Engine::CurrentGameCreateFoos();
  } else {
  }
  */
}


void Java_com_risingcode_qwqz_DemoRenderer_nativeResize(JNIEnv* env, jobject thiz, jint width, jint height) {
  sWindowWidth = width;
  sWindowHeight = height;
  /*
  LOGV("DOES THIS GET CALLED\n");
  if (Engine::CurrentGame()) {
    Engine::CurrentGameResizeScreen(width, height);
    Engine::CurrentGameStart();
  } else {
    Engine::Start(game_index, sWindowWidth, sWindowHeight);
    create_audio_thread();
  }
  */

  impl_main(0, NULL, 0);
  impl_resize(width, height);

  LOGV("BOOM\n");
}


void Java_com_risingcode_qwqz_DemoGLSurfaceView_nativePause( JNIEnv*  env ) {
  //Engine::CurrentGamePause();
}


void Java_com_risingcode_qwqz_DemoGLSurfaceView_nativeResume( JNIEnv*  env ) {
  LOGV("Java_com_risingcode_qwqz_DemoGLSurfaceView_nativeResume\n");
}


void Java_com_risingcode_qwqz_DemoGLSurfaceView_nativeTouch(JNIEnv* env, jobject thiz, jfloat x, jfloat y, jint hitState) {
  impl_hit(x, y, (int)hitState);
}


void Java_com_risingcode_qwqz_DemoRenderer_nativeRender( JNIEnv*  env ) {
  //Engine::CurrentGameDrawScreen(0);
  impl_draw(0);
}
