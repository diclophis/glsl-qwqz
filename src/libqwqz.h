// 

int impl_draw(int b);
int impl_resize(int width, int height, int u);
int impl_main(int argc, char** argv, GLuint b);
int impl_hit(int x, int y, int s);

struct qwqz_sprite_t {
  GLfloat vertex[2];
  GLfloat texture[2];
};
typedef struct qwqz_sprite_t * qwqz_sprite;

struct qwqz_batch_t {
  int m_numSpritesBatched;
  int m_numSprites;
  int m_numInterleavedBuffers;
  int m_numIndexBuffers;
  int m_NeedsAttribs;
  struct qwqz_sprite_t *m_Sprites;
  GLushort *indices;
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
  float m_ScreenHalfWidth;
  struct qwqz_batch_t *m_Batches;
  struct qwqz_linkage_t *m_Linkages;
  struct qwqz_timer_t *m_Timers;

  GLuint FramebufferName;
  GLuint renderedTexture;
  GLuint depthrenderbuffer;
  int m_RenderTextureWidth;

  GLuint g_lastFrameBuffer;
  GLuint g_lastTexture;
  GLuint g_lastElementBuffer;
  GLuint g_lastInterleavedBuffer;
  GLuint g_lastVertexArrayObject;
  GLuint m_NeedsBlendEnabled;
};
typedef struct qwqz_handle_t * qwqz_handle;

struct qwqz_timer_t {
  double t1;
  double t2;
  float m_SimulationTime;
  float step;
  float accum;
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
  GLuint ModelViewProjectionMatrix_location;
};
typedef struct qwqz_linkage_t * qwqz_linkage;

void identity(GLfloat *m);
void ortho(GLfloat *m, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat nearZ, GLfloat farZ);

void qwqz_checkgl(const char *s);
qwqz_handle qwqz_create(void);
char *qwqz_load(const char *path);
int qwqz_draw(qwqz_handle e);
int qwqz_resize(qwqz_handle e, int width, int height, int u);
int qwqz_batch_init(qwqz_batch ff, qwqz_linkage e, int count);
void qwqz_batch_add(qwqz_batch ff, int renderObject, float *vertices, float *color, float *uv);
void qwqz_batch_clear(qwqz_batch ff);
void qwqz_batch_prepare(qwqz_handle e, qwqz_batch ff, qwqz_linkage ll);
void qwqz_batch_render(qwqz_handle e, qwqz_batch ff);

int qwqz_linkage_init(GLuint program, qwqz_linkage e);
int qwqz_compile(GLuint type, const char *path);
int qwqz_texture_init(GLuint u, const char *path, int *w, int *h);
int qwqz_buffer_texture_init(GLuint t);
int qwqz_buffer_target_init(GLuint texture);
int qwqz_timer_init(qwqz_timer t);
int qwqz_tick_timer(qwqz_timer t);
void translate(qwqz_linkage e, float *m, float x, float y, float z);

int qwqz_linkage_resize(qwqz_handle ee, qwqz_linkage e);

FILE *iosfopen(const char *filename, const char *mode);
void qwqz_bind_frame_buffer(qwqz_handle e, GLuint buffer);
