#include "opengles_bridge.h"
#include "libqwqz.h"

#include "libmodplug/modplug.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <OpenAL/al.h>
#include <OpenAL/alc.h>

int main_x(int argc, char* argv[]);

int main_x(int argc, char* argv[]) {
  int NUM_BUFFERS = 4;
  int BUFFER_SIZE = 1024 * 8;

  ALCdevice* device = NULL;
  ALCcontext* context = NULL;

  unsigned int channels = 0;
  unsigned int frequency = 0;
  unsigned int bits = 0;
  ALenum format = 0;
  ALuint source = 0;
  ModPlugFile *gCurrentSound2 = NULL;

  device = alcOpenDevice(NULL);
  context = alcCreateContext(device, NULL);
  alcMakeContextCurrent(context);

  bits = 16;
  frequency = 44100;
  channels = 2;

  format = 0;
  if (bits == 8) {
    if (channels == 1) {
      format = AL_FORMAT_MONO8;
    } else if (channels == 2) {
      format = AL_FORMAT_STEREO8;
    }
  } else if (bits == 16) {
    if (channels == 1) {
      format = AL_FORMAT_MONO16;
    } else if (channels == 2) {
      format = AL_FORMAT_STEREO16;
    }
  }
  
  ALint numBuffers = 0;
  int read = 0;

  ALuint *buffers = malloc(sizeof(ALuint *) * NUM_BUFFERS);
  alGenBuffers(NUM_BUFFERS, buffers);
  alGenSources(1, &source);

  unsigned int lenn = 0;
  char *buffer = qwqz_load("assets/sounds/0.mod", &lenn);
  gCurrentSound2 = ModPlug_Load(buffer, lenn);

  void *data = (void *)malloc(BUFFER_SIZE);

  while (numBuffers < NUM_BUFFERS) {
    read = ModPlug_Read(gCurrentSound2, data, BUFFER_SIZE);
    if (read == 0) {
      ModPlug_Seek(gCurrentSound2, 0);
    }
  
    alBufferData(buffers[numBuffers], format, data, read, frequency);
    alSourceQueueBuffers(source, 1, &buffers[numBuffers]);
    numBuffers++;
  }

  alSourcePlay(source);

  ALint state;
  alGetSourcei(source, AL_SOURCE_STATE, &state);
  assert(state == AL_PLAYING);

  alGetSourcei(source, AL_BUFFERS_QUEUED, &numBuffers);
  assert(numBuffers == NUM_BUFFERS);

  numBuffers = 0;

  ALuint buffer2 = 0;
  ALint buffersProcessed = 0;
  
  alGetSourcei(source, AL_BUFFERS_PROCESSED, &buffersProcessed);

  while(1) {
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &buffersProcessed);

    while(buffersProcessed) {

      buffersProcessed--;

      alSourceUnqueueBuffers(source, 1, &buffer2);

      read = ModPlug_Read(gCurrentSound2, data, BUFFER_SIZE);
      if (read == 0) {
        ModPlug_Seek(gCurrentSound2, 0);
      }

      alBufferData(buffer2, format, data, read, frequency);
      alSourceQueueBuffers(source, 1, &buffer2);
    }

    usleep(100000);
  }
}
