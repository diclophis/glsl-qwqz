// 

struct qwqz_sprite_t {
  GLshort vertex[2];
};
typedef struct qwqz_sprite_t * qwqz_sprite;

struct qwqz_batch_t {
  struct qwqz_sprite_t *m_Sprites;
  int m_numInterleavedBuffers;
  int m_numIndexBuffers;
	GLuint *m_InterleavedBuffers;
	GLuint *m_IndexBuffers;
  size_t m_Stride;
};
typedef struct qwqz_batch_t * qwqz_batch;

struct qwqz_handle_t {
  int m_SpriteCount;
  int m_IsSceneBuilt;
  int m_IsScreenResized;
  float m_SimulationTime;
  int m_Program;
  int m_EnabledState;
  float m_ScreenWidth;
  float m_ScreenHeight;
  float m_ScreenAspect;
  float m_ScreenHalfHeight;
  struct qwqz_batch_t *m_Batches;
  GLuint g_PositionAttribute;
  GLuint g_ResolutionUniform;
  GLuint g_TimeUniform;
  double t1;
  double t2;
};
typedef struct qwqz_handle_t * qwqz_handle;

void qwqz_checkgl(const char *s);
qwqz_handle qwqz_alloc();
int qwqz_init(qwqz_handle e);
char *qwqz_load(const char *path);
int qwqz_shader();
int qwqz_link(qwqz_handle e);
int qwqz_draw(qwqz_handle e);
int qwqz_resize(qwqz_handle e, float width, float height);
int qwqz_batch_init();
