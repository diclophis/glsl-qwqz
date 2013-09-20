//

#include "opengles_bridge.h"
#include "libqwqz.h"
#include "pnglite.h"

void qwqz_checkgl(const char *s) {
  // normally (when no error) just return
  const int lastGlError = glGetError();
  if (lastGlError == GL_NO_ERROR) return;
  
  LOGV("\n%s caused\n", s);
  switch (lastGlError)
  {
    case GL_INVALID_ENUM:      LOGV("GL_INVALID_ENUM\n\n");      break;
    case GL_INVALID_VALUE:     LOGV("GL_INVALID_VALUE\n\n");     break;
    case GL_INVALID_OPERATION: LOGV("GL_INVALID_OPERATION\n\n"); break;
    case GL_OUT_OF_MEMORY:     LOGV("GL_OUT_OF_MEMORY\n\n");     break;
  }
}


qwqz_handle qwqz_create() {
  qwqz_handle e = malloc(sizeof(struct qwqz_handle_t));
  e->m_SpriteCount = 0;
	e->m_IsSceneBuilt = 0;
	e->m_IsScreenResized = 0;
	//e->m_SimulationTime = 0.0;		
  e->m_Batches = 0;



  return e;
}

char *qwqz_load(const char *path) {
  FILE *fd = fopen(path, "rb");
  if (fd) {
    fseek(fd, 0, SEEK_END);
    unsigned int len = ftell(fd);
    rewind(fd);
    char *buffer = NULL;
    buffer = (char *)malloc(sizeof(char) * (len + 1));
    fseek(fd, 0, SEEK_SET); //NOTE: android offset != 0
    int read = fread(buffer, sizeof(char), len, fd);
    rewind(fd);
    fclose(fd);
    if (read > 0) {
      buffer[len] = '\0';
      return buffer;
    } else {
      return NULL;
    }
  } else {
    LOGV("file %s does not exist\n", path);
    return NULL;
  }
}


int qwqz_linkage_init(GLuint program, qwqz_linkage e) {
  e->m_Program = program;

  char *msg = NULL;
  int l = 0;
  size_t size_of_sprite = sizeof(struct qwqz_sprite_t);

  glLinkProgram(e->m_Program);
  glGetProgramiv(e->m_Program, GL_INFO_LOG_LENGTH, &l);
  msg = (char *)malloc(sizeof(char) * l);
  glGetProgramInfoLog(e->m_Program, l, NULL, msg);
  LOGV("program info: %s\n", msg);

  glUseProgram(e->m_Program);

  e->g_PositionAttribute = glGetAttribLocation(e->m_Program, "Position");
  e->g_ResolutionUniform = glGetUniformLocation(e->m_Program, "iResolution");
  e->g_TimeUniform = glGetUniformLocation(e->m_Program, "iGlobalTime");
  e->g_TextureUniform = glGetUniformLocation(e->m_Program, "texture1");
  e->g_TextureUniform2 = glGetUniformLocation(e->m_Program, "texture2");

  glVertexAttribPointer(e->g_PositionAttribute, 2, GL_SHORT, GL_FALSE, size_of_sprite, (char *)NULL + (0));
  glEnableVertexAttribArray(e->g_PositionAttribute);

  free(msg);

  qwqz_checkgl("linkage_init");

  return 0;
}

int qwqz_draw(qwqz_handle e) {
  return 0;
}

int qwqz_resize(qwqz_handle e, float width, float height) {
  e->m_ScreenWidth = width;
  e->m_ScreenHeight = height;
  e->m_ScreenAspect = e->m_ScreenWidth / e->m_ScreenHeight;
  e->m_ScreenHalfHeight = e->m_ScreenHeight * 0.5;
  glViewport(0, 0, e->m_ScreenWidth, e->m_ScreenHeight);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  e->m_IsScreenResized = 1;

  qwqz_checkgl("resize");

  return 0;
}

int qwqz_batch_init(qwqz_batch ff) {

  int max_frame_count = 1;

  size_t size_of_sprite = sizeof(struct qwqz_sprite_t);
  ff->m_Stride = size_of_sprite;

  GLushort *indices = (GLushort *)malloc(max_frame_count * 6 * sizeof(GLushort));

  ff->m_numInterleavedBuffers = 1;
  ff->m_InterleavedBuffers = (GLuint *)malloc(sizeof(GLuint) * (ff->m_numInterleavedBuffers));

  ff->m_numIndexBuffers = 1;
  ff->m_IndexBuffers = (GLuint *)malloc(sizeof(GLuint) * (ff->m_numIndexBuffers));

  ff->m_numSprites = 4;
  ff->m_Sprites = (struct qwqz_sprite_t *)malloc(sizeof(struct qwqz_sprite_t) * ff->m_numSprites);

  glGenBuffers(ff->m_numInterleavedBuffers, ff->m_InterleavedBuffers);
  glBindBuffer(GL_ARRAY_BUFFER, ff->m_InterleavedBuffers[0]);
  glBufferData(GL_ARRAY_BUFFER, max_frame_count * 4 * ff->m_Stride, NULL, GL_DYNAMIC_DRAW);

  for (unsigned int i=0; i<max_frame_count; i++) {
    indices[(i * 6) + 0] = (i * 4) + 1;
    indices[(i * 6) + 1] = (i * 4) + 2;
    indices[(i * 6) + 2] = (i * 4) + 0;
    indices[(i * 6) + 3] = (i * 4) + 0;
    indices[(i * 6) + 4] = (i * 4) + 2;
    indices[(i * 6) + 5] = (i * 4) + 3;
  }

  glGenBuffers(ff->m_numIndexBuffers, ff->m_IndexBuffers);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ff->m_IndexBuffers[0]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_frame_count * 6 * sizeof(GLshort), indices, GL_DYNAMIC_DRAW);

  ff->m_Sprites[0].vertex[0] = -1.0;
  ff->m_Sprites[0].vertex[1] = -1.0;

  ff->m_Sprites[1].vertex[0] = -1.0;
  ff->m_Sprites[1].vertex[1] = 1.0;

  ff->m_Sprites[2].vertex[0] = 1.0;
  ff->m_Sprites[2].vertex[1] = 1.0;

  ff->m_Sprites[3].vertex[0] = 1.0;
  ff->m_Sprites[3].vertex[1] = -1.0;

  size_t interleaved_buffer_size = (1 * 4 * ff->m_Stride);
  glBufferData(GL_ARRAY_BUFFER, interleaved_buffer_size, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, interleaved_buffer_size, ff->m_Sprites);

  free(indices);
  free(ff->m_Sprites);
  free(ff->m_InterleavedBuffers);
  free(ff->m_IndexBuffers);

  qwqz_checkgl("batch_init");

  return 0;
}

int qwqz_compile(GLuint type, const char *vsh) {
  int l = 0;
  GLuint v = 0;
  char *b = qwqz_load(vsh);
  char *msg = NULL;
  if (b) {
    const char *vs = b;
    v = glCreateShader(type);
    glShaderSource(v, 1, &vs, NULL);
    glCompileShader(v);
    glGetShaderiv(v, GL_INFO_LOG_LENGTH, &l);
    msg = (char *)malloc(sizeof(char) * l);
    glGetShaderInfoLog(v, l, NULL, msg);
    LOGV("shader info: %s\n", msg);

    free(b);
    free(msg);
  }

  qwqz_checkgl("compile");

  return v;
}


int qwqz_texture_init(GLuint unit, const char *path) {
  png_t tex;
  //char* data = qwqz_load("assets/textures/0.png");
  FILE *fp = fopen(path, "rb");
  unsigned char* data;
  GLuint textureHandle;

  png_init(0, 0);
  //fseek(m_TextureFileHandles->at(i)->fp, m_TextureFileHandles->at(i)->off, 0);
  png_open_read(&tex, 0, fp);
  data = (unsigned char*)malloc(tex.width * tex.height * tex.bpp);
  for(int i=0; i < tex.width*tex.height*tex.bpp; ++i) {
    data[i] = 0;
  }
  png_get_data(&tex, data);

  //unsigned int* inPixel32;
  unsigned short* outPixel16;

  void *textureData = data;
  void *tempData = malloc(tex.height * tex.width * sizeof(unsigned short));

  //inPixel32 = (unsigned int *)textureData;
  outPixel16 = (unsigned short *)tempData;

  //Convert "RRRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA" to "RRRRGGGGBBBBAAAA"
  for (int i=0; i<(tex.height * tex.width); i++) {
    unsigned int inP = ((unsigned int *)textureData)[i];
    outPixel16[i] = ((((inP >> 0) & 0xFF) >> 4) << 12) | ((((inP >> 8) & 0xFF) >> 4) << 8) | ((((inP >> 16) & 0xFF) >> 4) << 4) | ((((inP >> 24) & 0xFF) >> 4) << 0);
  }

  glGenTextures(1, &textureHandle);

  glActiveTexture(unit);
  glBindTexture(GL_TEXTURE_2D, textureHandle);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  int useSwizzledBits = 1;
  if (useSwizzledBits) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, tempData);
  } else {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  }
  
  int generateMipMap = 1;
  if (generateMipMap) {
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  free(data);
  free(tempData);

  qwqz_checkgl("texture_init");

  return textureHandle;
}


int qwqz_buffer_texture_init() {
  // The texture we're going to render to
  GLuint renderedTexture = 0;
  glGenTextures(1, &renderedTexture);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, renderedTexture);

  int m_RenderTextureWidth = 512;
  // Give an empty image to OpenGL ( the last "0" )
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderTextureWidth, m_RenderTextureWidth, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

  // Poor filtering. Needed !
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  //glBindTexture(GL_TEXTURE_2D, 0);

  return renderedTexture;
}


int qwqz_buffer_target_init(renderedTexture) {

  // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
  GLuint FramebufferName = 0;
  glGenFramebuffers(1, &FramebufferName);
  glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

  // Set "renderedTexture" as our colour attachement #0
  glFramebufferTexture2DOES(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);

  qwqz_checkgl("glFramebufferTexturOES");

  qwqz_checkgl("create");

  // Always check that our framebuffer is ok
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    LOGV("doh\n");
    return 0;
  } else {
    return FramebufferName;
  }
}


int qwqz_timer_init(qwqz_timer timer) {
  timer->m_SimulationTime = 0.0;
  timer->t1 = 0.0;
  timer->t2 = 0.0;
  struct timeval tim;
  gettimeofday(&tim, NULL);
  timer->t1 = tim.tv_sec + (tim.tv_usec / 1000000.0);
  return 0;
}


void qwqz_tick_timer(qwqz_timer timer) {
  struct timeval tim;
  gettimeofday(&tim, NULL);
  timer->t2 = tim.tv_sec + (tim.tv_usec / 1000000.0);
  float step = timer->t2 - timer->t1;
  timer->t1 = timer->t2;
  timer->m_SimulationTime += step;
}
