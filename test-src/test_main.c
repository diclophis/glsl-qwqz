// test impl


#include "opengles_bridge.h"
#include "libqwqz.h"
#include "impl_main.h"
#include "test_main.h"
#include <spine/spine.h>
#include "spine_bridge.h"


static qwqz_handle qwqz_engine = NULL;
static float verticeBuffer[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
static float uvBuffer[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
static int renderBufferTexture = -1;


int impl_draw(int b) {

  qwqz_tick_timer(&qwqz_engine->m_Timers[0]);

  qwqz_bind_frame_buffer(qwqz_engine, b);

  //glClear(GL_COLOR_BUFFER_BIT);

  int max_shader_layers = 2;
  
  for (int i=0; i<max_shader_layers; i++) {
    glUseProgram(qwqz_engine->m_Linkages[i].m_Program);
    glUniform1f(qwqz_engine->m_Linkages[i].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);

    qwqz_batch_prepare(qwqz_engine, &qwqz_engine->m_Batches[i], &qwqz_engine->m_Linkages[i]);
    qwqz_batch_clear(&qwqz_engine->m_Batches[i]);
    qwqz_batch_add(&qwqz_engine->m_Batches[i], 0, verticeBuffer, NULL, uvBuffer);
    qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[i]);
  }

  return 0;
}


int impl_hit(int x, int y, int s) {
  return 0;
}


int impl_resize(int width, int height, int ew, int eh, int u) {
  if (qwqz_engine == NULL) {
    return 0;
  }
  
  glActiveTexture(GL_TEXTURE0);
  
  int resized = qwqz_resize(qwqz_engine, width, height, ew, eh, u);

  for (int i=0; i<2; i++) {
    glUseProgram(qwqz_engine->m_Linkages[i].m_Program);
    glUniform2f(qwqz_engine->m_Linkages[i].g_ResolutionUniform, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);
    qwqz_linkage_resize(qwqz_engine, &qwqz_engine->m_Linkages[i]);
  }

  verticeBuffer[0] = -1.0;
  verticeBuffer[1] = 2.0;
  
  verticeBuffer[2] = -1.0;
  verticeBuffer[3] = 0;

  verticeBuffer[4] = 1.0;
  verticeBuffer[5] = 0;
  
  verticeBuffer[6] = 1.0;
  verticeBuffer[7] = 2.0;
  
  return resized;
}


int impl_main(int argc, char** argv, GLuint b) {
  qwqz_engine = qwqz_create();

  GLuint v1 = 0;
  GLuint f1 = 0;
  GLuint f2 = 0;
  GLuint program = 0;

  qwqz_engine->m_Timers = (struct qwqz_timer_t *)malloc(sizeof(struct qwqz_timer_t) * 1);
  qwqz_timer_init(&qwqz_engine->m_Timers[0]);

  // render target
  renderBufferTexture = qwqz_buffer_texture_init(GL_TEXTURE0);
  qwqz_engine->FramebufferName = qwqz_buffer_target_init(renderBufferTexture);

  qwqz_engine->m_Linkages = (struct qwqz_linkage_t *)malloc(sizeof(struct qwqz_linkage_t) * 2);

  v1 = qwqz_compile(GL_VERTEX_SHADER, "assets/shaders/full_screen_first_pass.vsh");
  f1 = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/wtf3.fsh");
  f2 = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/graph.fsh");

  if (v1 && f1) {
    // Create and link the shader program
    program = glCreateProgram();
    glAttachShader(program, v1);
    glAttachShader(program, f1);

    qwqz_linkage_init(program, &qwqz_engine->m_Linkages[0]);
  }

  if (v1 && f2) {
    // Create and link the shader program
    program = glCreateProgram();
    glAttachShader(program, v1);
    glAttachShader(program, f2);
    
    qwqz_linkage_init(program, &qwqz_engine->m_Linkages[1]);
  }

  qwqz_engine->m_Batches = (struct qwqz_batch_t *)malloc(sizeof(struct qwqz_batch_t) * 2);

  qwqz_batch_init(&qwqz_engine->m_Batches[0], &qwqz_engine->m_Linkages[0], 1);
  qwqz_batch_init(&qwqz_engine->m_Batches[1], &qwqz_engine->m_Linkages[1], 1);

  return 0;
}
