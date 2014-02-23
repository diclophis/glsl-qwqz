#include "opengles_bridge.h"
#include "libqwqz.h"
#include <spine/spine.h>
#include <spine/extension.h>
#include "spine_bridge.h"


static void *RO = 0;


void _spAtlasPage_createTexture (spAtlasPage* self, const char* path) {
  //TODO: figure out how to map renderObject
  int outW = -1;
  int outH = -1;

  int r = (int)RO;
  int unit = GL_TEXTURE0 + r;
  int t0 = qwqz_texture_init(unit, path, &outW, &outH);
  self->rendererObject = RO;

  RO++;

  self->width = outW;
  self->height = outH;

  //LOGV("_AtlasPage_createTexture: %s %d => %d (%d %d)\n", path, unit, t0, outW, outH);
}


void _spAtlasPage_disposeTexture (spAtlasPage* self) {
}


char* _spUtil_readFile (const char* path, int* length) {
  return _readFile(path, length);
}
