// test impl


#include "opengles_bridge.h"
#include "libqwqz.h"
#include "impl_main.h"
#include "test_main.h"
#include <spine/spine.h>
#include "spine_bridge.h"


static qwqz_handle qwqz_engine = NULL;
static float verticeBuffer[12];
static float uvBuffer[12];
static int renderBufferTexture = -1;
static int booted = 0;


int impl_draw(int b) {
  qwqz_tick_timer(&qwqz_engine->m_Timers[0]);

  // not needed explicitly given that doShaderBg draws the to the entire screen
  //glBindFramebuffer(GL_FRAMEBUFFER, b);

  glUniform1f(qwqz_engine->m_Linkages[0].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);

  //if (0 == booted) {
  //  booted = 1;
    glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
    glActiveTexture(GL_TEXTURE0);
    glBindFramebuffer(GL_FRAMEBUFFER, b);
  //}

  qwqz_batch_prepare(qwqz_engine, &qwqz_engine->m_Batches[0], &qwqz_engine->m_Linkages[0]);
  qwqz_batch_clear(&qwqz_engine->m_Batches[0]);
  qwqz_batch_add(&qwqz_engine->m_Batches[0], 0, verticeBuffer, NULL, uvBuffer);
  
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[0]);

  //glUseProgram(qwqz_engine->m_Linkages[1].m_Program);
//  glUniform1f(qwqz_engine->m_Linkages[1].g_TextureUniform, renderBufferTexture);
  //qwqz_batch_prepare(qwqz_engine, &qwqz_engine->m_Batches[1], &qwqz_engine->m_Linkages[1]);
//  glActiveTexture(GL_TEXTURE1);
//  glBindFramebuffer(GL_FRAMEBUFFER, b);
//  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//  qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[1]);



  return 0;
}


int impl_hit(int x, int y, int s) {
  return 0;
}


int impl_resize(int width, int height, int u) {
  if (qwqz_engine == NULL) {
    return 0;
  }
  
  LOGV("resizing to %d %d\n", width, height);
  int resized = qwqz_resize(qwqz_engine, width, height, u);

  for (int i=0; i<1; i++) {
    glUseProgram(qwqz_engine->m_Linkages[i].m_Program);
    glUniform2f(qwqz_engine->m_Linkages[i].g_ResolutionUniform, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);

    qwqz_linkage_resize(qwqz_engine, &qwqz_engine->m_Linkages[i]);
  }

//
//  verticeBuffer[0] = -1;
//  verticeBuffer[1] = -1;
//  
//  verticeBuffer[2] = -1;
//  verticeBuffer[3] = 1;
//  
//  verticeBuffer[4] = 1;
//  verticeBuffer[5] = 1;
//  
//  verticeBuffer[6] = 1;
//  verticeBuffer[7] = -1;


//
//  verticeBuffer[8] = 1;
//  verticeBuffer[9] = 1;
//  
//  verticeBuffer[10] = -1;
//  verticeBuffer[11] = 1;
//  
//
  /*
  uvBuffer[0] = -1;
  uvBuffer[1] = -0.5;
  
  uvBuffer[2] = 1;
  uvBuffer[3] = -0.5;
  
  uvBuffer[4] = -1;
  uvBuffer[5] = 0.5;
  
  uvBuffer[6] = 1;
  uvBuffer[7] = -0.5;
  
  uvBuffer[8] = 1;
  uvBuffer[9] = 0.5;
  
  uvBuffer[10] = -1;
  uvBuffer[11] = 0.25;
   */
  
  verticeBuffer[0] = -width;
  verticeBuffer[1] = -height;
  
  verticeBuffer[2] = -width;
  verticeBuffer[3] = height;
  
  verticeBuffer[4] = width;
  verticeBuffer[5] = height;
  
  verticeBuffer[6] = width;
  verticeBuffer[7] = -height;

 
//  uvBuffer[0] = 0.0;
//  uvBuffer[1] = 0.0;
//  uvBuffer[2] = 0.0;
//  uvBuffer[3] = 0.0;
//  uvBuffer[4] = 0.0;
//  uvBuffer[5] = 0.0;
//  uvBuffer[6] = 0.0;
//  uvBuffer[7] = 0.0;
  
  return resized;
}


int impl_main(int argc, char** argv, GLuint b) {
  qwqz_engine = qwqz_create();
  qwqz_checkgl("create\n");

  //qwqz_engine->m_RenderTextureWidth = 512;

  GLuint v1 = 0;
  GLuint f1 = 0;
  GLuint v2 = 0;
  GLuint f2 = 0;
  GLuint program = 0;

  qwqz_engine->m_Timers = (struct qwqz_timer_t *)malloc(sizeof(struct qwqz_timer_t) * 1);
  qwqz_timer_init(&qwqz_engine->m_Timers[0]);

  // render target
  renderBufferTexture = qwqz_buffer_texture_init(GL_TEXTURE0);
  qwqz_engine->FramebufferName = qwqz_buffer_target_init(renderBufferTexture);

  qwqz_engine->m_Linkages = (struct qwqz_linkage_t *)malloc(sizeof(struct qwqz_linkage_t) * 2);

  v1 = qwqz_compile(GL_VERTEX_SHADER, "assets/shaders/full_screen_first_pass.vsh");
  f1 = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/graph.fsh");

  if (v1 && f1) {
    // Create and link the shader program
    program = glCreateProgram();
    glAttachShader(program, v1);
    qwqz_checkgl("attachA\n");
    glAttachShader(program, f1);
    qwqz_checkgl("attach\n");

    qwqz_linkage_init(program, &qwqz_engine->m_Linkages[0]);
    qwqz_checkgl("link init\n");
  }

//  v2 = qwqz_compile(GL_VERTEX_SHADER, "assets/shaders/full_screen_second_pass.vsh");
//  f2 = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/wtf.fsh");
//
//  if (v2 && f2) {
//    // Create and link the shader program
//    program = glCreateProgram();
//    glAttachShader(program, v2);
//    qwqz_checkgl("attachC\n");
//    glAttachShader(program, f2);
//    qwqz_checkgl("attachD\n");
//
//    qwqz_linkage_init(program, &qwqz_engine->m_Linkages[1]);
//    qwqz_checkgl("link init 2\n");
//  }

  qwqz_engine->m_Batches = (struct qwqz_batch_t *)malloc(sizeof(struct qwqz_batch_t) * 2);

  qwqz_batch_init(&qwqz_engine->m_Batches[0], &qwqz_engine->m_Linkages[0], 1);
  //qwqz_batch_init(&qwqz_engine->m_Batches[1], &qwqz_engine->m_Linkages[1], 1);
  qwqz_checkgl("batch init\n");

  qwqz_checkgl("main\n");

  return 0;
}
