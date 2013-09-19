//

#include "opengles_bridge.h"
#include "libqwqz.h"

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
	e->m_SimulationTime = 0.0;		
  e->m_Batches = 0;

  // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
  e->FramebufferName = 0;
  glGenFramebuffers(1, &e->FramebufferName);
  glBindFramebuffer(GL_FRAMEBUFFER, e->FramebufferName);

  // The texture we're going to render to
  e->renderedTexture = 0;
  glGenTextures(1, &e->renderedTexture);

  glActiveTexture(GL_TEXTURE0);
  //glBindTexture(GL_TEXTURE_2D, qwqz_engine->renderedTexture);
// "Bind" the newly created texture : all future texture functions will modify this texture
  glBindTexture(GL_TEXTURE_2D, e->renderedTexture);

  e->m_RenderTextureWidth = 512;
  // Give an empty image to OpenGL ( the last "0" )
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, e->m_RenderTextureWidth, e->m_RenderTextureWidth, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

  // Poor filtering. Needed !
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  //extern void glFramebufferTexture2DEXT(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
  // Set "renderedTexture" as our colour attachement #0
  glFramebufferTexture2DOES(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, e->renderedTexture, 0);

  //glBindTexture(GL_TEXTURE_2D, 0);

  qwqz_checkgl("glFramebufferTexturOES");

  // Set the list of draw buffers.
  //GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0};
  //glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

  qwqz_checkgl("create");

  // Always check that our framebuffer is ok
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    LOGV("doh\n");
    return 0;
  }

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
