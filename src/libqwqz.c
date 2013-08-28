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


qwqz_handle qwqz_create(const char *vsh, const char *fsh) {
  qwqz_handle e = malloc(sizeof(struct qwqz_handle_t));
  e->m_SpriteCount = 0;
	e->m_IsSceneBuilt = 0;
	e->m_IsScreenResized = 0;
	e->m_SimulationTime = 0.0;		
  e->m_Program = 0;
  e->m_EnabledState = 0;
  e->m_Batches = 0;
  //e->m_Program = qwqz_shader(vsh, fsh);

  //int qwqz_shader() {

  char *b = NULL;
  char *msg = NULL;
  int l;
  GLuint v = 0;
  GLuint f = 0;
  GLuint program = 0;

  // Compile the vertex shader
  b = qwqz_load(vsh); //"assets/shaders/basic.vsh");
  if (b) {
    const char *vs = b;
    LOGV("vertex source: %s\n", vs);

    v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vs, NULL);
    glCompileShader(v);
    glGetShaderiv(v, GL_INFO_LOG_LENGTH, &l);
    msg = (char *)malloc(sizeof(char) * l);
    glGetShaderInfoLog(v, l, NULL, msg);
    //LOGV("vertex shader info: %s\n", msg);

    free(b);
    free(msg);
  }

  // Compile the fragment shader
  b = qwqz_load(fsh); //"assets/shaders/gears.fsh");
  if (b) {
    const char *fs = b;
    LOGV("fragment source: %s\n", fs);

    f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &fs, NULL);
    glCompileShader(f);
    glGetShaderiv(f, GL_INFO_LOG_LENGTH, &l);
    msg = (char *)malloc(sizeof(char) * l);
    glGetShaderInfoLog(f, l, NULL, msg);
    LOGV("fragment shader info: %s\n", msg);

    free(b);
    free(msg);
  }

  if (v && f) {

    // Create and link the shader program
    program = glCreateProgram();
    glAttachShader(program, v);
    glAttachShader(program, f);
    e->m_Program = program;

    struct timeval tim;
    gettimeofday(&tim, NULL);
    e->t1 = tim.tv_sec + (tim.tv_usec / 1000000.0);

    e->m_Batches = (struct qwqz_batch_t *)malloc(sizeof(struct qwqz_batch_t) * 1);
    qwqz_batch_init(e, e->m_Batches[0]);
  }

  qwqz_checkgl("create");

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


int qwqz_link(qwqz_handle e) {
  
  char *msg = NULL;
  int l = 0;

  glLinkProgram(e->m_Program);
  glGetProgramiv(e->m_Program, GL_INFO_LOG_LENGTH, &l);
  msg = (char *)malloc(sizeof(char) * l);
  glGetProgramInfoLog(e->m_Program, l, NULL, msg);

  glUseProgram(e->m_Program);

  e->g_PositionAttribute = glGetAttribLocation(e->m_Program, "Position");
  e->g_ResolutionUniform = glGetUniformLocation(e->m_Program, "iResolution");
  e->g_TimeUniform = glGetUniformLocation(e->m_Program, "iGlobalTime");

  size_t size_of_sprite = sizeof(struct qwqz_sprite_t);
  glVertexAttribPointer(e->g_PositionAttribute, 2, GL_SHORT, GL_FALSE, size_of_sprite, (char *)NULL + (0));
  glEnableVertexAttribArray(e->g_PositionAttribute);

  free(msg);
  
  e->m_EnabledState = 1;

  return 0;
}

int qwqz_draw(qwqz_handle e) {
  {
    struct timeval tim;
    gettimeofday(&tim, NULL);
    e->t2 = tim.tv_sec + (tim.tv_usec / 1000000.0);
    float step = e->t2 - e->t1;
    e->t1 = e->t2;
    e->m_SimulationTime += step;
  }

  if (e->m_Batches && e->m_IsScreenResized) {
    if (!e->m_EnabledState) {
      qwqz_link(e);
    } else {
      glUniform2f(e->g_ResolutionUniform, e->m_ScreenWidth, e->m_ScreenHeight);
      glUniform1f(e->g_TimeUniform, e->m_SimulationTime);
      glDrawElements(GL_TRIANGLES, 1 * 6, GL_UNSIGNED_SHORT, (GLvoid*)((char*)NULL));
    }
  }

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
  return 0;
}

int qwqz_batch_init(qwqz_handle e, qwqz_batch ff) {

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

  return 0;
}
