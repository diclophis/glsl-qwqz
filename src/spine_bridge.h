
void _spAtlasPage_createTexture (spAtlasPage* self, const char* path) {
  //TODO: figure out how to map renderObject
  int outW = -1;
  int outH = -1;

  int unit = GL_TEXTURE0 + RO;
  int t0 = qwqz_texture_init(unit, path, &outW, &outH);
	self->rendererObject = (void *)RO;

  RO++;

  self->width = outW;
  self->height = outH;

  //int t0 = -1;
  //self->width = 2048;
  //self->height = 2048;

  LOGV("_AtlasPage_createTexture: %s %d => %d (%d %d)\n", path, unit, t0, outW, outH);
}


void _spAtlasPage_disposeTexture (spAtlasPage* self) {
}


char* _spUtil_readFile (const char* path, int* length) {
	return _readFile(path, length);
}
