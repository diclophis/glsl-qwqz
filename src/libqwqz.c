//

#include "MemoryLeak.h"

qwqz_handle qwqz_alloc() {
  return malloc(sizeof(struct qwqz_handle_t));
}

int qwqz_init(qwqz_handle e) {

  e->m_SpriteCount = 0;
	e->m_IsSceneBuilt = 0;
	e->m_IsScreenResized = 0;
	e->m_SimulationTime = 0.0;		
  e->m_Program = 0;
  e->m_EnabledState = 0;

  e->m_Program = qwqz_shader();

  /*
    gl.useProgram(screenProgram);
    target.framebuffer = gl.createFramebuffer();
    target.renderbuffer = gl.createRenderbuffer();
    // Create vertex buffer (2 triangles)
    buffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([ - 1.0, - 1.0, 1.0, - 1.0, - 1.0, 1.0, 1.0, - 1.0, 1.0, 1.0, - 1.0, 1.0 ]), gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(screenVertexPosition, 2, gl.FLOAT, false, 0, 0);
  */

  return 0;
}

char *qwqz_load(const char *path) {
  FILE *fd = fopen(path, "rb");
  if (fd) {
    fseek(fd, 0, SEEK_END);
    unsigned int len = ftell(fd);
    rewind(fd);
    char *buffer = NULL;
    buffer = (char *)malloc(sizeof(char) * len);
    fseek(fd, 0, SEEK_SET); //NOTE: android offset != 0
    int read = fread(buffer, sizeof(char), len, fd);
    rewind(fd);
    fclose(fd);
    if (read > 0) {
      return buffer;
    } else {
      return NULL;
    }
  } else {
    LOGV("%s does not exist\n", path);
    return NULL;
  }
}

int qwqz_shader() {
  char *b = NULL;
  char *msg = NULL;
  int l;
  GLuint v = 0;
  GLuint f = 0;
  GLuint program = 0;
  b = qwqz_load("assets/shaders/basic.vsh");
  if (b) {
    const char *vs = b;
    LOGV("vertex source: %s\n", vs);

    // Compile the vertex shader
    v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vs, NULL);
    glCompileShader(v);
    glGetShaderiv(v, GL_INFO_LOG_LENGTH, &l);
    msg = (char *)malloc(sizeof(char) * l);
    glGetShaderInfoLog(v, l, NULL, msg);
    LOGV("vertex shader info: %s\n", msg);

    free(b);
    free(msg);
  }

  // Compile the fragment shader
  b = qwqz_load("assets/shaders/basic.fsh");
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
    LOGV("created program: %d\n", program);
  }

  //m_StateFoo = new StateFoo(program);

  return 0;
}

int qwqz_link(qwqz_handle e) {
  
  char *msg = NULL;
  int l = 0;

  glLinkProgram(e->m_Program);
  glGetProgramiv(e->m_Program, GL_INFO_LOG_LENGTH, &l);
  msg = (char *)malloc(sizeof(char) * l);
  glGetProgramInfoLog(e->m_Program, l, NULL, msg);
  LOGV("program info: %s\n", msg);

  glUseProgram(e->m_Program);

  //g_PositionAttribute = glGetAttribLocation(m_Program, "Position");
  //g_TextureAttribute = glGetAttribLocation(m_Program, "InCoord");
  // Get the locations of the uniforms so we can access them
  //ModelViewProjectionMatrix_location = glGetUniformLocation(m_Program, "ModelViewProjectionMatrix");

  e->m_EnabledState = 1;

  return 0;
}

int qwqz_draw(qwqz_handle e) {
	if (e->m_IsScreenResized) {
    if (!e->m_EnabledState) {
      qwqz_link(e);
    }


  }

  return 0;
}

int qwqz_resize(qwqz_handle e, float width, float height) {
  LOGV("resized %f %f\n", width, height);
  e->m_ScreenWidth = width;
  e->m_ScreenHeight = height;
  e->m_ScreenAspect = e->m_ScreenWidth / e->m_ScreenHeight;
  e->m_ScreenHalfHeight = e->m_ScreenHeight * 0.5;
  glViewport(0, 0, e->m_ScreenWidth, e->m_ScreenHeight);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  e->m_IsScreenResized = 1;
  return 0;
}
