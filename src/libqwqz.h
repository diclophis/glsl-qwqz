// 

struct qwqz_sprite_t {
  GLshort vertex[2];
};
typedef struct qwqz_sprite_t * qwqz_sprite;

struct qwqz_batch_t {
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
  float m_SimulationTime;
  float m_ScreenWidth;
  float m_ScreenHeight;
  float m_ScreenAspect;
  float m_ScreenHalfHeight;
  struct qwqz_batch_t *m_Batches;
  struct qwqz_linkage_t *m_Linkages;


  double t1;
  double t2;

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
};
typedef struct qwqz_handle_t * qwqz_handle;

struct qwqz_linkage_t {
  int m_Program;
  GLuint g_PositionAttribute;
  GLuint g_ResolutionUniform;
  GLuint g_TimeUniform;
  GLuint g_TextureUniform;
};
typedef struct qwqz_linkage_t * qwqz_linkage;

void qwqz_checkgl(const char *s);
qwqz_handle qwqz_create();
char *qwqz_load(const char *path);
int qwqz_draw(qwqz_handle e);
int qwqz_resize(qwqz_handle e, float width, float height);
int qwqz_batch_init(qwqz_batch ff);
int qwqz_linkage_init(GLuint program, qwqz_linkage e);
int qwqz_compile(GLuint type, const char *path);
