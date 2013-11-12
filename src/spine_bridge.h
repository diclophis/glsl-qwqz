static int RO = 0;

void _spAtlasPage_createTexture (spAtlasPage* self, const char* path) {
  //TODO: figure out how to map renderObject
	self->rendererObject = (void *)RO++;

  switch((int)self->rendererObject) {
    case 0:
      // size is important!!
      self->width = 2048;
      self->height = 2048;
      break;
    case 1:
      // size is important!!
      self->width = 2048;
      self->height = 2048;
      break;
  }
  LOGV("_AtlasPage_createTexture: %s\n", path);
}


void _spAtlasPage_disposeTexture (spAtlasPage* self) {
}


char* _spUtil_readFile (const char* path, int* length) {
	return _readFile(path, length);
}
