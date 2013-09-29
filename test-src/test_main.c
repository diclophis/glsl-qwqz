// test impl


#include "opengles_bridge.h"
#include "libqwqz.h"
#include "impl_main.h"
#include "test_main.h"
#include <spine/spine.h>
#include <spine/extension.h>


static qwqz_handle qwqz_engine = NULL;
static float verticeBuffer[8];
static float uvBuffer[8];

void _AtlasPage_createTexture (AtlasPage* self, const char* path) {
	self->rendererObject = 0;
	self->width = 123;
	self->height = 456;
  LOGV("_AtlasPage_createTexture: %s\n", path);
}

void _AtlasPage_disposeTexture (AtlasPage* self) {
}

char* _Util_readFile (const char* path, int* length) {
	return _readFile(path, length);
}


int impl_draw() {
  qwqz_tick_timer(&qwqz_engine->m_Timers[0]);

  qwqz_batch_clear(&qwqz_engine->m_Batches[0]);
  qwqz_batch_clear(&qwqz_engine->m_Batches[1]);

  verticeBuffer[0] = -1.0;
  verticeBuffer[1] = -1.0;

  verticeBuffer[2] = -1.0;
  verticeBuffer[3] = 1.0;

  verticeBuffer[4] = 1.0;
  verticeBuffer[5] = 1.0;

  verticeBuffer[6] = 1.0;
  verticeBuffer[7] = -1.0;

  uvBuffer[0] = 0.0;
  uvBuffer[1] = 0.0;
  uvBuffer[2] = 0.0;
  uvBuffer[3] = 0.0;
  uvBuffer[4] = 0.0;
  uvBuffer[5] = 0.0;
  uvBuffer[6] = 0.0;
  uvBuffer[7] = 0.0;

  qwqz_batch_add(&qwqz_engine->m_Batches[0], 0, verticeBuffer, NULL, uvBuffer);
  qwqz_batch_add(&qwqz_engine->m_Batches[1], 0, verticeBuffer, NULL, uvBuffer);

  glBindFramebuffer(GL_FRAMEBUFFER, qwqz_engine->FramebufferName);
  glViewport(0, 0, qwqz_engine->m_RenderTextureWidth, qwqz_engine->m_RenderTextureWidth); // Render on the whole framebuffer, complete from the lower left corner to the upper right

  glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
  glUniform2f(qwqz_engine->m_Linkages[0].g_ResolutionUniform, qwqz_engine->m_RenderTextureWidth, qwqz_engine->m_RenderTextureWidth);
  glUniform1f(qwqz_engine->m_Linkages[0].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);

  translate(&qwqz_engine->m_Linkages[0], NULL, 0, 0, 0);

  qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[0]);

  // Render to the screen
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);

  glUseProgram(qwqz_engine->m_Linkages[1].m_Program);
  glUniform2f(qwqz_engine->m_Linkages[1].g_ResolutionUniform, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);
  glUniform1f(qwqz_engine->m_Linkages[1].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);

  translate(&qwqz_engine->m_Linkages[1], NULL, 0, 0, 0);

  qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[1]);
   
  qwqz_draw(qwqz_engine);

  return 0;
}


int impl_resize(int width, int height) {
  qwqz_resize(qwqz_engine, width, height);
}


int impl_main(int argc, char** argv) {
  if (argc == 3) {
    qwqz_engine = qwqz_create();

    qwqz_engine->m_RenderTextureWidth = 512;

    GLuint v = 0;
    GLuint f = 0;
    GLuint f2 = 0;
    GLuint program = 0;

    qwqz_engine->m_Timers = (struct qwqz_timer_t *)malloc(sizeof(struct qwqz_timer_t) * 1);
    qwqz_timer_init(&qwqz_engine->m_Timers[0]);

    // render target
    int renderBufferTexture = qwqz_buffer_texture_init();
    qwqz_engine->FramebufferName = qwqz_buffer_target_init(renderBufferTexture);

    qwqz_engine->m_Linkages = (struct qwqz_linkage_t *)malloc(sizeof(struct qwqz_linkage_t) * 2);

    v = qwqz_compile(GL_VERTEX_SHADER, argv[1]);
    f = qwqz_compile(GL_FRAGMENT_SHADER, argv[2]);
    f2 = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/testquad.fsh");

    if (v && f && f2) {
      // Create and link the shader program
      program = glCreateProgram();
      glAttachShader(program, v);
      glAttachShader(program, f);
      qwqz_linkage_init(program, &qwqz_engine->m_Linkages[0]);

      program = glCreateProgram();
      glAttachShader(program, v);
      glAttachShader(program, f2);
      qwqz_linkage_init(program, &qwqz_engine->m_Linkages[1]);
    }

    qwqz_engine->m_Batches = (struct qwqz_batch_t *)malloc(sizeof(struct qwqz_batch_t) * 2);

    qwqz_batch_init(&qwqz_engine->m_Batches[0], &qwqz_engine->m_Linkages[0], 1);
    qwqz_batch_init(&qwqz_engine->m_Batches[1], &qwqz_engine->m_Linkages[1], 1);
  }

  return 0;
}

