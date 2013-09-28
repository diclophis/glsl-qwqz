// 

struct qwqz_sprite_t {
  GLshort vertex[2];
  GLfloat texture[2];
};
typedef struct qwqz_sprite_t * qwqz_sprite;

struct qwqz_batch_t {
  int m_numSpritesBatched;
  int m_numSprites;
  int m_numInterleavedBuffers;
  int m_numIndexBuffers;
  struct qwqz_sprite_t *m_Sprites;
	GLuint *m_InterleavedBuffers;
	GLuint *m_IndexBuffers;
  size_t m_Stride;
};
typedef struct qwqz_batch_t * qwqz_batch;

struct qwqz_handle_t {
  int m_SpriteCount;
  int m_IsSceneBuilt;
  int m_IsScreenResized;
  float m_ScreenWidth;
  float m_ScreenHeight;
  float m_ScreenAspect;
  float m_ScreenHalfHeight;
  struct qwqz_batch_t *m_Batches;
  struct qwqz_linkage_t *m_Linkages;
  struct qwqz_timer_t *m_Timers;



  /*
  int m_Program;
  GLuint g_PositionAttribute;
  GLuint g_ResolutionUniform;
  GLuint g_TimeUniform;

  int m_Program2;
  GLuint g_PositionAttribute2;
  GLuint g_ResolutionUniform2;
  GLuint g_TimeUniform2;
  */

  GLuint FramebufferName;
  GLuint renderedTexture;
  GLuint depthrenderbuffer;
  int m_RenderTextureWidth;

  GLuint g_lastTexture;
  GLuint g_lastElementBuffer;
  GLuint g_lastInterleavedBuffer;
  GLuint g_lastVertexArrayObject;
};
typedef struct qwqz_handle_t * qwqz_handle;

struct qwqz_timer_t {
  double t1;
  double t2;
  float m_SimulationTime;
  float step;
};
typedef struct qwqz_timer_t * qwqz_timer;

struct qwqz_linkage_t {
  int m_Program;
  GLuint g_PositionAttribute;
  GLuint g_TextureAttribute;
  GLuint g_ResolutionUniform;
  GLuint g_TimeUniform;
  GLuint g_TextureUniform;
  GLuint g_TextureUniform2;
  GLuint g_TextureUniform3;
};
typedef struct qwqz_linkage_t * qwqz_linkage;

void qwqz_checkgl(const char *s);
qwqz_handle qwqz_create();
char *qwqz_load(const char *path);
int qwqz_draw(qwqz_handle e);
int qwqz_resize(qwqz_handle e, int width, int height);
int qwqz_batch_init(qwqz_batch ff);
void qwqz_batch_add(qwqz_batch ff, int renderObject, float *vertices, float *color, float *uv);
void qwqz_batch_clear(qwqz_batch ff);
void qwqz_batch_render(qwqz_handle e, qwqz_batch ff);


int qwqz_linkage_init(GLuint program, qwqz_linkage e);
int qwqz_compile(GLuint type, const char *path);
int qwqz_texture_init(GLuint u, const char *path);
int qwqz_buffer_texture_init();
int qwqz_buffer_target_init(int texture);
int qwqz_timer_init(qwqz_timer t);
void qwqz_tick_timer(qwqz_timer t);
void translate(float *m, float x, float y, float z);
