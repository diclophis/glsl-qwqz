
static inline float fastAbs(float x) { return (x < 0) ? -x : x; }
static inline GLfloat fastSinf(GLfloat x)
{
	// fast sin function; maximum error is 0.001
	const float P = 0.225;
	
	x = x * M_1_PI;
	int k = (int) round(x);
	x = x - k;
    
	float y = (4 - 4 * fastAbs(x)) * x;
    
	y = P * (y * fastAbs(y) - y) + y;
    
	return (k&1) ? -y : y;	
}

/*
 *  FileHandle.h
 *  MemoryLeak
 *
 *  Created by Jon Bardin on 04/20/12.
 *  GPL
 *
 */

struct FileHandle {
	FILE *fp;
	unsigned int off;
	unsigned int len;
  const char *name;
};

enum {
  MODELS,
  SOUNDS,
  TEXTURES,
  LEVELS
};
// Jon Bardin GPL


class StateFoo {

public:

  GLuint g_lastTexture;
  GLuint g_lastElementBuffer;
  GLuint g_lastInterleavedBuffer;
  GLuint g_lastVertexArrayObject;
  GLuint ModelViewProjectionMatrix_location;
  int m_LastBufferIndex;
  bool m_EnabledStates;

  GLuint g_PositionAttribute;
  GLuint g_TextureAttribute;
  GLuint m_Program;
  StateFoo(GLuint program);
  ~StateFoo();
  void Reset();
  void Link();

};
/*
 *  foo.h
 *  MemoryLeak
 *
 *  Created by Jon Bardin on 9/6/10.
 *  GPL
 *
 */

typedef struct
{
  GLshort vertex[2];
  GLfloat texture[2];
} SpriteFoo;

typedef struct
{
  GLfloat vertex[3];
  GLfloat normal[3];
  GLfloat texture[3];
  GLshort xyz[3];
} ModelFoo;


#ifdef __cplusplus

extern "C" {

#endif

struct foofoo {

#ifdef __cplusplus

	foofoo()
	{
    m_AnimationDuration = 0;
    m_numFaces = 0;
    m_numFrames = 0;
    m_numInterleavedBuffers = 0;
    m_numTextureBuffers = 0;
    m_numNormalBuffers = 0;
    m_numIndexBuffers = 0;
    m_AnimationStart = 0;
    m_AnimationEnd = 0;
    m_Stride = 0;
    m_numVertexArrayObjects = 0;
    m_numBuffers = 0;
    m_NumBatched = 0;
    m_NumBatchedElements = 0;
    m_numSpriteFoos = 0;
    m_numModelFoos = 0;
    m_BufferCount = 0;
    m_NeedsCopy = true;
	}

	~foofoo()
	{
    //LOGV("dealloc foofoo %d %d\n", m_numInterleavedBuffers, m_numIndexBuffers);
    glDeleteBuffers(m_numBuffers, m_VerticeBuffers);
    glDeleteBuffers(m_numBuffers, m_IndexBuffers);
    glDeleteBuffers(m_numNormalBuffers, m_NormalBuffers);
    glDeleteBuffers(m_numTextureBuffers, m_TextureBuffer);
    glDeleteBuffers(m_numInterleavedBuffers, m_InterleavedBuffers);
    glDeleteBuffers(m_numIndexBuffers, m_IndexBuffers);

    if (m_numBuffers > 0) {
      free(m_VerticeBuffers);
    }

    if (m_numIndexBuffers > 0) {
      free(m_IndexBuffers);
    }

    if (m_numNormalBuffers > 0) {
      free(m_NormalBuffers);
    }

    if (m_numTextureBuffers > 0) {
      free(m_TextureBuffer);
    }

    if (m_numInterleavedBuffers > 0) {
      free(m_InterleavedBuffers);
    }

    if (m_numSpriteFoos > 0) {
      free(m_SpriteFoos);
    }

    if (m_numModelFoos > 0) {
      free(m_ModelFoos);
    }

    if (m_numFaces > 0) {
      free(m_IndexFoo);
    }

#ifdef HAS_VAO

    glDeleteVertexArraysOES(m_numVertexArrayObjects, m_VertexArrayObjects);

    if (m_numVertexArrayObjects > 0) {
      free(m_VertexArrayObjects);
    }

#endif

	}

#endif

	int m_numBuffers;
	GLuint *m_VerticeBuffers;
	GLuint *m_NormalBuffers;
	GLuint *m_IndexBuffers;
	GLuint *m_TextureBuffer;
	GLuint *m_InterleavedBuffers;
	GLuint *m_VertexArrayObjects;
	GLuint m_Texture;
  float m_AnimationDuration;
	int m_numFaces;
	int m_numFrames;
  int m_numInterleavedBuffers;
  int m_numTextureBuffers;
  int m_numNormalBuffers;
  int m_numIndexBuffers;
  int m_numVertexArrayObjects;
  int m_numSpriteFoos;
  int m_numModelFoos;
	int m_AnimationStart;
	int m_AnimationEnd;
  int m_NumBatched;
  int m_NumBatchedElements;
  size_t m_Stride;
  bool m_NeedsCopy;
  SpriteFoo *m_SpriteFoos;
  ModelFoo *m_ModelFoos;
  GLshort *m_IndexFoo;
  int m_BufferCount;
  float m_texCoordWidth;
  float m_texCoordHeight;

};

#ifdef __cplusplus

}

#endif
// Jon Bardin GPL


class nodexyz {

public:

  int node_index;
  int x;
  int y;
  int z;


  bool operator==(const nodexyz &other) const {
    return (this->x == other.x && this->y == other.y);
  }


  bool operator!=(const nodexyz &other) const {
    return !(*this == other);
  }


};
