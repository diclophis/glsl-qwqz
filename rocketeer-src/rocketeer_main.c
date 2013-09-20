// test impl


#include "opengles_bridge.h"
#include "libqwqz.h"
#include "impl_main.h"
#include "rocketeer_main.h"


static qwqz_handle qwqz_engine = NULL;


void impl_draw() {

  qwqz_tick_timer(&qwqz_engine->m_Timers[0]);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);

  glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
  glUniform2f(qwqz_engine->m_Linkages[0].g_ResolutionUniform, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);
  glUniform1f(qwqz_engine->m_Linkages[0].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);
   
  glDrawElements(GL_TRIANGLES, 1 * 6, GL_UNSIGNED_SHORT, (GLvoid*)((char*)NULL));
}


void impl_resize(int width, int height) {
  qwqz_resize(qwqz_engine, (float)width, (float)height);
}


int impl_main(int argc, char** argv) {

  qwqz_engine = qwqz_create();

  GLuint v = 0;
  GLuint f2 = 0;
  GLuint program = 0;

  int t0 = qwqz_texture_init();

  qwqz_engine->m_Timers = (struct qwqz_timer_t *)malloc(sizeof(struct qwqz_timer_t) * 1);
  qwqz_timer_init(&qwqz_engine->m_Timers[0]);

  //TODO: why does this have to happen before linking?
  qwqz_engine->m_Batches = (struct qwqz_batch_t *)malloc(sizeof(struct qwqz_batch_t) * 1);
  qwqz_batch_init(&qwqz_engine->m_Batches[0]);

  qwqz_engine->m_Linkages = (struct qwqz_linkage_t *)malloc(sizeof(struct qwqz_linkage_t) * 1);

  v = qwqz_compile(GL_VERTEX_SHADER, "assets/shaders/basic.vsh");
  f2 = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/texquad.fsh");

  if (v && f2) {
    program = glCreateProgram();
    glAttachShader(program, v);
    glAttachShader(program, f2);
    qwqz_linkage_init(program, &qwqz_engine->m_Linkages[0]);

    LOGV("impled %d\n", t0);
  
    return 0;
  }

  return 1;
}
