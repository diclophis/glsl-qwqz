//


#include "opengles_bridge.h"
#include "libqwqz.h"
#include "pnglite.h"


static GLfloat ProjectionMatrix[16];

void qwqz_checkgl(const char *s) {
  // normally (when no error) just return
  const int lastGlError = glGetError();
  if (lastGlError == GL_NO_ERROR) return;
  
  LOGV("\n%s caused %04x %04x\n", s, lastGlError,  GL_INVALID_FRAMEBUFFER_OPERATION);
  switch (lastGlError)
  {
    case GL_INVALID_ENUM:      LOGV("GL_INVALID_ENUM\n\n");      break;
    case GL_INVALID_VALUE:     LOGV("GL_INVALID_VALUE\n\n");     break;
    case GL_INVALID_OPERATION: LOGV("GL_INVALID_OPERATION\n\n"); break;
    case GL_OUT_OF_MEMORY:     LOGV("GL_OUT_OF_MEMORY\n\n");     break;
    default: LOGV("unknown %d\n", lastGlError); return;
  }

  LOGV("exit!\n");

  exit(1);
}


qwqz_handle qwqz_create(void) {
  qwqz_handle e = malloc(sizeof(struct qwqz_handle_t));
  e->m_SpriteCount = 0;
  e->m_IsSceneBuilt = 0;
  e->m_IsScreenResized = 0;
  e->m_Batches = 0;

  e->g_lastFrameBuffer = -1;
  e->g_lastElementBuffer = -1;
  e->g_lastInterleavedBuffer = -1;
  e->m_NeedsBlendEnabled = 1;

  return e;
}

char *qwqz_load(const char *path) {
  FILE *fd = iosfopen(path, "rb");
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

  glLinkProgram(e->m_Program);
  glGetProgramiv(e->m_Program, GL_INFO_LOG_LENGTH, &l);
  msg = (char *)malloc(sizeof(char) * l);
  glGetProgramInfoLog(e->m_Program, l, NULL, msg);
  //LOGV("program info: %s\n", msg);

  //glUseProgram(e->m_Program);

  e->g_PositionAttribute = glGetAttribLocation(e->m_Program, "Position");
  e->g_TextureAttribute = glGetAttribLocation(e->m_Program, "Texture");
  e->g_ResolutionUniform = glGetUniformLocation(e->m_Program, "iResolution");

  e->ModelViewProjectionMatrix_location = glGetUniformLocation(e->m_Program, "ModelViewProjectionMatrix");
  //glUniformMatrix4fv(e->ModelViewProjectionMatrix_location, 1, GL_FALSE, ProjectionMatrix);

  e->g_TimeUniform = glGetUniformLocation(e->m_Program, "iGlobalTime");

  e->g_TextureUniform = glGetUniformLocation(e->m_Program, "texture1");
  e->g_TextureUniform2 = glGetUniformLocation(e->m_Program, "texture2");
  e->g_TextureUniform3 = glGetUniformLocation(e->m_Program, "texture3");

  free(msg);

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

void translate(qwqz_linkage e, GLfloat *m, float tx, float ty, float tz) {
    ProjectionMatrix[12] += (ProjectionMatrix[0] * tx + ProjectionMatrix[4] * ty + ProjectionMatrix[8] * tz);
    ProjectionMatrix[13] += (ProjectionMatrix[1] * tx + ProjectionMatrix[5] * ty + ProjectionMatrix[9] * tz);
    ProjectionMatrix[14] += (ProjectionMatrix[2] * tx + ProjectionMatrix[6] * ty + ProjectionMatrix[10] * tz);
    ProjectionMatrix[15] += (ProjectionMatrix[3] * tx + ProjectionMatrix[7] * ty + ProjectionMatrix[11] * tz);
    //glUniformMatrix4fv(e->ModelViewProjectionMatrix_location, 1, GL_FALSE, ProjectionMatrix);
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
  e->m_ScreenHalfWidth = e->m_ScreenWidth * 0.5;
  glViewport(0, 0, e->m_ScreenWidth, e->m_ScreenHeight);
  //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  e->m_IsScreenResized = 1;

  //LOGV("wtf: %d %d %f\n", width, height, e->m_ScreenAspect);

  float m_Zoom2 = 1.0;
  float a = (-e->m_ScreenHalfHeight * e->m_ScreenAspect) * m_Zoom2;
  float b = (e->m_ScreenHalfHeight * e->m_ScreenAspect) * m_Zoom2;
  float c = (-e->m_ScreenHalfHeight) * m_Zoom2;
  float d = e->m_ScreenHalfHeight * m_Zoom2;
  float ee = 10.0;
  float ff = -10.0;

  identity(ProjectionMatrix);
  ortho(ProjectionMatrix, (a), (b), (c), (d), (ee), (ff));

  return 0;
}

int qwqz_linkage_resize(qwqz_linkage e) {
  glUniformMatrix4fv(e->ModelViewProjectionMatrix_location, 1, GL_FALSE, ProjectionMatrix);
  return 0;
}

int qwqz_batch_init(qwqz_batch ff, qwqz_linkage e, int count) {

  //glUseProgram(e->m_Program);

  ff->m_NeedsAttribs = 1;
  
  size_t size_of_sprite = sizeof(struct qwqz_sprite_t);
  ff->m_Stride = size_of_sprite;

  ff->m_numInterleavedBuffers = 1;
  ff->m_InterleavedBuffers = (GLuint *)malloc(sizeof(GLuint) * (ff->m_numInterleavedBuffers));

  ff->m_numIndexBuffers = 1;
  ff->m_IndexBuffers = (GLuint *)malloc(sizeof(GLuint) * (ff->m_numIndexBuffers));

  ff->m_numSprites = count; // TODO: bone count!!!!
  int max_frame_count = ff->m_numSprites;
  ff->m_Sprites = (struct qwqz_sprite_t *)malloc(sizeof(struct qwqz_sprite_t) * ff->m_numSprites * 4);
  ff->indices = (GLushort *)malloc(max_frame_count * 6 * sizeof(GLushort));

  int i=0;
  
  //for (i=0; i<ff->m_numSprites * 4; i++) {
  //  //ff->m_Sprites[i] = (qwqz_sprite);
  //}

  //unsigned int i;
  for (i=0; i<max_frame_count; i++) {
    ff->indices[(i * 6) + 0] = (i * 4) + 1;
    ff->indices[(i * 6) + 1] = (i * 4) + 2;
    ff->indices[(i * 6) + 2] = (i * 4) + 0;
    ff->indices[(i * 6) + 3] = (i * 4) + 0;
    ff->indices[(i * 6) + 4] = (i * 4) + 2;
    ff->indices[(i * 6) + 5] = (i * 4) + 3;
  }

  glGenBuffers(ff->m_numIndexBuffers, ff->m_IndexBuffers);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ff->m_IndexBuffers[0]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_frame_count * 6 * sizeof(GLshort), ff->indices, GL_DYNAMIC_DRAW);

  glGenBuffers(ff->m_numInterleavedBuffers, ff->m_InterleavedBuffers);
  glBindBuffer(GL_ARRAY_BUFFER, ff->m_InterleavedBuffers[0]);

  /*
  if (0) {
    size_t size_of_sprite = sizeof(struct qwqz_sprite_t);
    glVertexAttribPointer(ll->g_PositionAttribute, 2, GL_SHORT, GL_FALSE, size_of_sprite, (char *)NULL + (0));
    glEnableVertexAttribArray(ll->g_PositionAttribute);
    glVertexAttribPointer(ll->g_TextureAttribute, 2, GL_FLOAT, GL_FALSE, size_of_sprite, (char *)NULL + (2 * sizeof(GLshort)));
    glEnableVertexAttribArray(ll->g_TextureAttribute);
  }
   */
  
  glVertexAttribPointer(e->g_PositionAttribute, 2, GL_SHORT, GL_FALSE, size_of_sprite, (char *)NULL + (0));
  glEnableVertexAttribArray(e->g_PositionAttribute);

  glVertexAttribPointer(e->g_TextureAttribute, 2, GL_FLOAT, GL_FALSE, size_of_sprite, (char *)NULL + (2 * sizeof(GLshort)));
  glEnableVertexAttribArray(e->g_TextureAttribute);
  
  glBufferData(GL_ARRAY_BUFFER, max_frame_count * 4 * ff->m_Stride, ff->m_Sprites, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  return 0;
}


void qwqz_batch_clear(qwqz_batch ff) {
  ff->m_numSpritesBatched = 0;
}


void qwqz_batch_prepare(qwqz_handle e, qwqz_batch ff, qwqz_linkage ll) {
  //translate(ll, NULL, 0, 0, 0);

  if (0 || ff->m_IndexBuffers[0] != e->g_lastElementBuffer) {
    e->g_lastElementBuffer = ff->m_IndexBuffers[0];
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, e->g_lastElementBuffer);
  }

  if (0 || ff->m_InterleavedBuffers[0] != e->g_lastInterleavedBuffer) {
    e->g_lastInterleavedBuffer = ff->m_InterleavedBuffers[0];
    glBindBuffer(GL_ARRAY_BUFFER, e->g_lastInterleavedBuffer);
  }

  if (0 || e->m_NeedsBlendEnabled) {
    e->m_NeedsBlendEnabled = 0;
    glEnable(GL_BLEND);
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  
  if (0 || ff->m_NeedsAttribs) {
    ff->m_NeedsAttribs = 0;
    size_t size_of_sprite = sizeof(struct qwqz_sprite_t);
    //glVertexAttribPointer(ll->g_PositionAttribute, 2, GL_SHORT, GL_FALSE, size_of_sprite, (char *)NULL + (0));
    //glEnableVertexAttribArray(ll->g_PositionAttribute);
    //glVertexAttribPointer(ll->g_TextureAttribute, 2, GL_FLOAT, GL_FALSE, size_of_sprite, (char *)NULL + (2 * sizeof(GLshort)));
    //glEnableVertexAttribArray(ll->g_TextureAttribute);
  }
}

void qwqz_batch_render(qwqz_handle e, qwqz_batch ff) {
  //assert(ff->m_numSpritesBatched > 0);

  if (ff->m_numSpritesBatched > 0) {
    size_t interleaved_buffer_size = (ff->m_numSpritesBatched * 4 * ff->m_Stride);
    //glBufferData(GL_ARRAY_BUFFER, interleaved_buffer_size, ff->m_Sprites, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, interleaved_buffer_size, ff->m_Sprites);

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ff->m_numSpritesBatched * 6 * sizeof(GLshort), ff->indices);

    // 1st [mode] parameter is what kind of primitive to render.
    // 2nd [count] parameter should be the number of elements to render. ie. the number of vertices
    // 3rd [type] parameter should be the type of the value in the 4th parameter.. can ONLY be either
    //  GL_UNSIGNED_BYTE or GL_UNSIGNED_SHORT or GL_UNSIGNED_INT
    // 4th [indices] parameter is a pointer to where the indices are stored.
    
    glDrawElements(GL_TRIANGLES, ff->m_numSpritesBatched * 6, GL_UNSIGNED_SHORT, (GLvoid*)((char*)NULL));
    //glDrawElements(GL_TRIANGLES, ff->m_numSpritesBatched * 6, GL_UNSIGNED_SHORT, ff->m_Sprites);
  }
}


void qwqz_batch_add(qwqz_batch ff, int _renderObject, float *vertices, float *color, float *uv) {
  int i=0;
  for (i=0; i<4; i++) {
    int batched_times_four = (ff->m_numSpritesBatched * 4) + i;
    
    ff->m_Sprites[(batched_times_four)].vertex[0] = vertices[0 + (i * 2)];
    ff->m_Sprites[(batched_times_four)].vertex[1] = vertices[1 + (i * 2)];
    
    ff->m_Sprites[(batched_times_four)].texture[0] = uv[0 + (i * 2)];
    ff->m_Sprites[(batched_times_four)].texture[1] = uv[1 + (i * 2)];
  }

  ff->m_numSpritesBatched++;
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
    LOGV("shader info: %s %s %d\n", vsh, msg, v);

    free(b);
    free(msg);
  }

  return v;
}


int qwqz_texture_init(GLuint unit, const char *path, int *w, int *h) {
  png_t tex;
  //char* data = qwqz_load("assets/textures/0.png");
  FILE *fp = iosfopen(path, "rb");
  unsigned char* data;
  GLuint textureHandle;

  png_init(0, 0);
  //fseek(m_TextureFileHandles->at(i)->fp, m_TextureFileHandles->at(i)->off, 0);
  png_open_read(&tex, 0, fp);
  *w = tex.width;
  *h = tex.height;
  data = (unsigned char*)malloc(tex.width * tex.height * tex.bpp);
  int i=0;
  for(i=0; i < tex.width*tex.height*tex.bpp; ++i) {
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
  for (i=0; i<(tex.height * tex.width); i++) {
    unsigned int inP = ((unsigned int *)textureData)[i];
    outPixel16[i] = ((((inP >> 0) & 0xFF) >> 4) << 12) | ((((inP >> 8) & 0xFF) >> 4) << 8) | ((((inP >> 16) & 0xFF) >> 4) << 4) | ((((inP >> 24) & 0xFF) >> 4) << 0);
  }

  glGenTextures(1, &textureHandle);

  if (unit > GL_TEXTURE0) {
    glActiveTexture(unit);
  }
  
  glBindTexture(GL_TEXTURE_2D, textureHandle);

  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  int useSwizzledBits = 1;
  if (useSwizzledBits) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, tempData);
  } else {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  }
  
  int generateMipMap = 0;
  if (generateMipMap) {
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  free(data);
  free(tempData);

  //LOGV("created opengles texture: %d\n", textureHandle);
  return textureHandle;
}




int qwqz_buffer_texture_init(GLuint t) {
  // The texture we're going to render to
  GLuint renderedTexture = 0;
  glGenTextures(1, &renderedTexture);

  glActiveTexture(t);
  glBindTexture(GL_TEXTURE_2D, renderedTexture);

  int m_RenderTextureWidth = 512;
  
  unsigned char* data;
  
  data = (unsigned char*)malloc(m_RenderTextureWidth * m_RenderTextureWidth * 8);
  int i=0;
  for(i=0; i < m_RenderTextureWidth * m_RenderTextureWidth * 8; ++i) {
    data[i] = 0;
  }
  
  // Give an empty image to OpenGL ( the last "0" )
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderTextureWidth, m_RenderTextureWidth, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  // Poor filtering. Needed !
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  return renderedTexture;
}


int qwqz_buffer_target_init(GLuint renderedTexture) {
  // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
  GLuint FramebufferName = 0;
  glGenFramebuffers(1, &FramebufferName);
  glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

  // Set "renderedTexture" as our colour attachement #0
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);

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

void qwqz_bind_frame_buffer(qwqz_handle e, GLuint buffer) {
  if (e->g_lastFrameBuffer != buffer) {
    e->g_lastFrameBuffer = buffer;
    glBindFramebuffer(GL_FRAMEBUFFER, buffer);
  }
}
