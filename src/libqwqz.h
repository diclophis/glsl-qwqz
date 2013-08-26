// 

struct qwqz_sprite_t {
  GLshort vertex[2];
  //GLfloat texture[2];
};
typedef struct qwqz_sprite_t * qwqz_sprite;

struct qwqz_batch_t {

  struct qwqz_sprite_t *m_Sprites;

  int m_numInterleavedBuffers;
  int m_numIndexBuffers;

	GLuint *m_InterleavedBuffers;
	GLuint *m_IndexBuffers;

  size_t m_Stride;
/*
	int m_numBuffers;
	GLuint *m_VerticeBuffers;
	GLuint *m_NormalBuffers;
	GLuint *m_TextureBuffer;
	GLuint *m_VertexArrayObjects;
	GLuint m_Texture;
  float m_AnimationDuration;
	int m_numFaces;
	int m_numFrames;
  int m_numInterleavedBuffers;
  int m_numTextureBuffers;
  int m_numNormalBuffers;
  int m_numVertexArrayObjects;
  int m_numSpriteFoos;
  int m_numModelFoos;
	int m_AnimationStart;
	int m_AnimationEnd;
  int m_NumBatched;
  int m_NumBatchedElements;
  bool m_NeedsCopy;
  SpriteFoo *m_SpriteFoos;
  ModelFoo *m_ModelFoos;
  GLshort *m_IndexFoo;
  int m_BufferCount;
  float m_texCoordWidth;
  float m_texCoordHeight;
*/
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
  qwqz_batch *m_Batches;
  GLuint g_PositionAttribute;
  GLuint g_ResolutionUniform;
  GLuint g_TimeUniform;
};
typedef struct qwqz_handle_t * qwqz_handle;


qwqz_handle qwqz_alloc();

int qwqz_init(qwqz_handle e);
char *qwqz_load(const char *path);
int qwqz_shader();

int qwqz_link(qwqz_handle e);
int qwqz_draw(qwqz_handle e);
int qwqz_resize(qwqz_handle e, float width, float height);
qwqz_batch qwqz_batch_create();
