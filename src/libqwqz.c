//

#include "opengles_bridge.h"
#include "libqwqz.h"
#include "pnglite.h"

static GLfloat ProjectionMatrix[16];

void qwqz_checkgl(const char *s) {
  // normally (when no error) just return
  const int lastGlError = glGetError();
  if (lastGlError == GL_NO_ERROR) return;
  
  LOGV("\n%s caused %04x %04x\n", s, lastGlError,  GL_INVALID_FRAMEBUFFER_OPERATION_EXT);
  switch (lastGlError)
  {
    case GL_INVALID_ENUM:      LOGV("GL_INVALID_ENUM\n\n");      break;
    case GL_INVALID_VALUE:     LOGV("GL_INVALID_VALUE\n\n");     break;
    case GL_INVALID_OPERATION: LOGV("GL_INVALID_OPERATION\n\n"); break;
    case GL_OUT_OF_MEMORY:     LOGV("GL_OUT_OF_MEMORY\n\n");     break;
    default: LOGV("unknown %d\n", lastGlError); return;
  }

  exit(1);
}


qwqz_handle qwqz_create() {
  qwqz_handle e = malloc(sizeof(struct qwqz_handle_t));
  e->m_SpriteCount = 0;
	e->m_IsSceneBuilt = 0;
	e->m_IsScreenResized = 0;
	//e->m_SimulationTime = 0.0;		
  e->m_Batches = 0;

  e->g_lastElementBuffer = -1;
  e->g_lastInterleavedBuffer = -1;




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
  e->g_TextureUniform3 = glGetUniformLocation(e->m_Program, "texture3");

  glVertexAttribPointer(e->g_PositionAttribute, 2, GL_SHORT, GL_FALSE, size_of_sprite, (char *)NULL + (0));
  glEnableVertexAttribArray(e->g_PositionAttribute);

  //TODO
  //glVertexAttribPointer(e->g_PositionAttribute, 2, GL_SHORT, GL_FALSE, ff->m_Stride, (char *)NULL + (0));
  //glVertexAttribPointer(e->g_TextureAttribute, 2, GL_FLOAT, GL_FALSE, ff->m_Stride, (char *)NULL + (2 * sizeof(GLshort)));

  //glEnableVertexAttribArray(sf->g_PositionAttribute);
  //glEnableVertexAttribArray(sf->g_TextureAttribute);

  free(msg);

  qwqz_checkgl("linkage_init");

  return 0;
}

int qwqz_draw(qwqz_handle e) {
  return 0;
}

void identity(GLfloat *m) {
  GLfloat t[16] = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0,
  };
  
  memcpy(m, t, sizeof(t));
}

void ortho(GLfloat *m, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat nearZ, GLfloat farZ) {
  
  GLfloat deltaX = right - left;
  GLfloat deltaY = top - bottom;
  GLfloat deltaZ = farZ - nearZ;
  
  GLfloat tmp[16];
  identity(tmp);
  
  if ((deltaX == 0) || (deltaY == 0) || (deltaZ == 0)) {
    LOGV("Invalid ortho\n");
    return;
  }
  
  tmp[0] = (2.0 / deltaX);
  tmp[12] = (-(right + left) / deltaX);
  tmp[5] = (2.0 / deltaY);
  tmp[13] = (-(top + bottom) / deltaY);
  tmp[10] = (-2.0 / deltaZ);
  tmp[14] = (-(nearZ + farZ) / deltaZ);
  
  memcpy(m, tmp, sizeof(tmp));
}

int qwqz_resize(qwqz_handle e, int width, int height) {
  e->m_ScreenWidth = width;
  e->m_ScreenHeight = height;
  e->m_ScreenAspect = e->m_ScreenWidth / e->m_ScreenHeight;
  e->m_ScreenHalfHeight = e->m_ScreenHeight * 0.5;
  glViewport(0, 0, e->m_ScreenWidth, e->m_ScreenHeight);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  e->m_IsScreenResized = 1;

  /*
  float m_Zoom2 = 1.0;

  float a = (-e->m_ScreenHalfHeight * e->m_ScreenAspect) * m_Zoom2;
  float b = (e->m_ScreenHalfHeight * e->m_ScreenAspect) * m_Zoom2;
  float c = (-e->m_ScreenHalfHeight) * m_Zoom2;
  float d = e->m_ScreenHalfHeight * m_Zoom2;
  float ee = 10.0;
  float ff = -10.0;

  identity(ProjectionMatrix);
  ortho(ProjectionMatrix, (a), (b), (c), (d), (ee), (ff));

  qwqz_checkgl("resize");
  */

  return 0;
}

int qwqz_batch_init(qwqz_batch ff) {

  size_t size_of_sprite = sizeof(struct qwqz_sprite_t);
  ff->m_Stride = size_of_sprite;

  ff->m_numInterleavedBuffers = 1;
  ff->m_InterleavedBuffers = (GLuint *)malloc(sizeof(GLuint) * (ff->m_numInterleavedBuffers));

  ff->m_numIndexBuffers = 1;
  ff->m_IndexBuffers = (GLuint *)malloc(sizeof(GLuint) * (ff->m_numIndexBuffers));

  ff->m_numSprites = 18; // TODO: bone count!!!!
  int max_frame_count = ff->m_numSprites;
  ff->m_Sprites = (struct qwqz_sprite_t *)malloc(sizeof(struct qwqz_sprite_t) * ff->m_numSprites * 4);
  GLushort *indices = (GLushort *)malloc(max_frame_count * 6 * sizeof(GLushort));

  glGenBuffers(ff->m_numInterleavedBuffers, ff->m_InterleavedBuffers);
  glBindBuffer(GL_ARRAY_BUFFER, ff->m_InterleavedBuffers[0]);
  glBufferData(GL_ARRAY_BUFFER, max_frame_count * 4 * ff->m_Stride, NULL, GL_DYNAMIC_DRAW);
  //glBindBuffer(GL_ARRAY_BUFFER, 0);

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
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  /*
  ff->m_Sprites[0].vertex[0] = -1.0;
  ff->m_Sprites[0].vertex[1] = -1.0;

  ff->m_Sprites[1].vertex[0] = -1.0;
  ff->m_Sprites[1].vertex[1] = 1.0;

  ff->m_Sprites[2].vertex[0] = 1.0;
  ff->m_Sprites[2].vertex[1] = 1.0;

  ff->m_Sprites[3].vertex[0] = 1.0;
  ff->m_Sprites[3].vertex[1] = -1.0;

  verticeBuffer[0] = -1.0;
  verticeBuffer[1] = -1.0;
  verticeBuffer[2] = -1.0;
  verticeBuffer[3] = 1.0;
  verticeBuffer[4] = 1.0;
  verticeBuffer[5] = 1.0;
  verticeBuffer[6] = 1.0;
  verticeBuffer[7] = -1.0;
  */

  qwqz_checkgl("batch_init");

  return 0;
}


void qwqz_batch_clear(qwqz_batch ff) {
  ff->m_numSpritesBatched = 0;
}


void qwqz_batch_render(qwqz_handle e, qwqz_batch ff) {

  if (1 || ff->m_IndexBuffers[0] != e->g_lastElementBuffer) {
    e->g_lastElementBuffer = ff->m_IndexBuffers[0];
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, e->g_lastElementBuffer);
    //LOGV("abc %d\n", e->g_lastElementBuffer);
  }

  if (1 || ff->m_InterleavedBuffers[0] != e->g_lastInterleavedBuffer) {
    e->g_lastInterleavedBuffer = ff->m_InterleavedBuffers[0];
    //glBindBuffer(GL_ARRAY_BUFFER, e->g_lastInterleavedBuffer);
    //LOGV("efg %d\n", e->g_lastInterleavedBuffer);
  }

  qwqz_checkgl("wtf");

  //size_t interleaved_buffer_size = (ff->m_numSpritesBatched * 4 * ff->m_Stride);
  //glBufferData(GL_ARRAY_BUFFER, interleaved_buffer_size, NULL, GL_DYNAMIC_DRAW); // GL_STATIC_DRAW might be faster...
  //glBufferSubData(GL_ARRAY_BUFFER, 0, interleaved_buffer_size, ff->m_Sprites);

  //LOGV("render: %d\n", ff->m_numSpritesBatched);

  size_t interleaved_buffer_size = (ff->m_numSpritesBatched * 4 * ff->m_Stride);
  //glBindBuffer(GL_ARRAY_BUFFER, ff->m_InterleavedBuffers[0]);
  glBufferData(GL_ARRAY_BUFFER, interleaved_buffer_size, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, interleaved_buffer_size, ff->m_Sprites);
  
  //if (!sf->m_EnabledStates) {
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glActiveTexture(GL_TEXTURE0);
    //glEnableVertexAttribArray(sf->g_PositionAttribute);
    //glEnableVertexAttribArray(sf->g_TextureAttribute);
  //  sf->m_EnabledStates = true;
  //}

  //LOGV("render: %d\n", ff->m_numSpritesBatched);
  qwqz_checkgl("wtf2");
  
  glDrawElements(GL_TRIANGLES, ff->m_numSpritesBatched * 6, GL_UNSIGNED_SHORT, (GLvoid*)((char*)NULL));

  //LOGV("render: %d\n", ff->m_numSpritesBatched);
}



void qwqz_batch_add(qwqz_batch ff, int renderObject, float *vertices, float *color, float *uv) {
  for (int i=0; i<4; i++) {
    int batched_times_four = (ff->m_numSpritesBatched * 4) + i;
    
    //ff->m_Sprites[(batched_times_four)].vertex[0] = 0;
    
    ff->m_Sprites[(batched_times_four)].vertex[0] = vertices[0 + (i * 2)]; //(x + m_Position[0]);
    ff->m_Sprites[(batched_times_four)].vertex[1] = vertices[1 + (i * 2)]; //(y + m_Position[1]);
    //LOGV("%f %f\n", uv[0 + (i * 2)], uv[1 + (i * 2)]);
    
    ff->m_Sprites[(batched_times_four)].texture[0] = 0.0; //uv[0 + (i * 2)];
    ff->m_Sprites[(batched_times_four)].texture[1] = 0.0; //uv[1 + (i * 2)];
  }

  /*
  ff->m_Sprites[0].vertex[0] = -1.0;
  ff->m_Sprites[0].vertex[1] = -1.0;
  ff->m_Sprites[1].vertex[0] = -1.0;
  ff->m_Sprites[1].vertex[1] = 1.0;
  ff->m_Sprites[2].vertex[0] = 1.0;
  ff->m_Sprites[2].vertex[1] = 1.0;
  ff->m_Sprites[3].vertex[0] = 1.0;
  ff->m_Sprites[3].vertex[1] = -1.0;
  */

  for (int i=0; i<4; i++) {
    ff->m_Sprites[(i)].texture[0] = 0.0; //uv[0 + (i * 2)];
    ff->m_Sprites[(i)].texture[1] = 0.0; //uv[1 + (i * 2)];
  }

  /*
  verticeBuffer[0] = -1.0;
  verticeBuffer[1] = -1.0;
  verticeBuffer[2] = -1.0;
  verticeBuffer[3] = 1.0;
  verticeBuffer[4] = 1.0;
  verticeBuffer[5] = 1.0;
  verticeBuffer[6] = 1.0;
  verticeBuffer[7] = -1.0;
  */

  //LOGV("added\n");


      //attachment.rendererObject.page.rendererObject,
      //ff->m_Sprites[cbi].vertex[0] = vertices[0];

      /*
      vertices[1],
      vertices[6],
      vertices[7],
      vertices[2],
      vertices[3],
      vertices[4],
      vertices[5],
      //skeleton.r * slot.r,
      //skeleton.g * slot.g,
      //skeleton.b * slot.b,
      //skeleton.a * slot.a,
      uvs[0], uvs[1],
      uvs[4], uvs[5]
      */

  ff->m_numSpritesBatched++;
  
  qwqz_checkgl("add");
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
  timer->step = step;
}
