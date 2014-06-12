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
  return 0;

  qwqz_tick_timer(&qwqz_engine->m_Timers[0]);

  //glActiveTexture(GL_TEXTURE0);
  //glBindFramebuffer(GL_FRAMEBUFFER, b);
  qwqz_bind_frame_buffer(qwqz_engine, b);

  glClear(GL_COLOR_BUFFER_BIT);

  int max_shader_layers = 1;
  
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
  
  //LOGV("resizing to %d %d %d %d\n", width, height, ew, eh);
  int resized = qwqz_resize(qwqz_engine, width, height, ew, eh, u);

  for (int i=0; i<2; i++) {
    glUseProgram(qwqz_engine->m_Linkages[i].m_Program);
    //if (1) {
    //if (ew != width) {
    //  glUniform2f(qwqz_engine->m_Linkages[i].g_ResolutionUniform, eh , ew);
    //} else {
      glUniform2f(qwqz_engine->m_Linkages[i].g_ResolutionUniform, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);
    //}
    //} else {
      //glUniform2f(qwqz_engine->m_Linkages[i].g_ResolutionUniform, eh, ew);
    //}

    qwqz_linkage_resize(qwqz_engine, &qwqz_engine->m_Linkages[i]);
  }
  
//  verticeBuffer[0] = -1.0 * (float)ew;
//  verticeBuffer[1] = -0.5 * (float)eh;
//  
//  verticeBuffer[2] = 1.0 * (float)ew;
//  verticeBuffer[3] = -0.5 * (float)eh;
//  
//  verticeBuffer[4] = -1.0 * (float)ew;
//  verticeBuffer[5] = 0.5 * (float)eh;
//  
//  verticeBuffer[6] = 1.0 * (float)ew;
//  verticeBuffer[7] = -0.5 * (float)eh;
//  
//  verticeBuffer[8] = 1.0 * (float)ew;
//  verticeBuffer[9] = 0.5 * (float)eh;
//  
//  verticeBuffer[10] = -1.0 * (float)ew;
//  verticeBuffer[11] = 0.5 * (float)eh;
  
  
//  if (1) {
  
//  -0.5f, 0.5f, 0f,    // Left top         ID: 0
//  080.-0.5f, -0.5f, 0f,   // Left bottom      ID: 1
//  081.0.5f, -0.5f, 0f,    // Right bottom     ID: 2
//  082.0.5f, 0.5f, 0f      // Right left       ID: 3
  
//OK
//  verticeBuffer[0] = -((float)width / 4.0);
//  verticeBuffer[1] = ((float)height / 4.0);
//    
//  verticeBuffer[2] = -((float)width / 4.0);
//  verticeBuffer[3] = -((float)height / 4.0);
//    
//  verticeBuffer[4] = ((float)width / 4.0);
//  verticeBuffer[5] = -((float)height / 4.0);
//
//  verticeBuffer[6] = ((float)width / 4.0);
//  verticeBuffer[7] = ((float)height / 4.0);
  
//  
  verticeBuffer[0] = -1.0;
  verticeBuffer[1] = 2.0;
  
  verticeBuffer[2] = -1.0;
  verticeBuffer[3] = 0;

  verticeBuffer[4] = 1.0;
  verticeBuffer[5] = 0;
  
  verticeBuffer[6] = 1.0;
  verticeBuffer[7] = 2.0;
  


    //verticeBuffer[6] = ((float)width / 2.2);
    //verticeBuffer[7] = 0; //((float)height * 0.2);
//  } else {
//    verticeBuffer[0] = -ew / 2;
//    verticeBuffer[1] = -eh;
//    
//    verticeBuffer[2] = -ew / 2;
//    verticeBuffer[3] = eh;
//    
//    verticeBuffer[4] = ew / 2;
//    verticeBuffer[5] = eh;
//    
//    verticeBuffer[6] = ew / 2;
//    verticeBuffer[7] = -eh;
//  }
  
  
  
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
  f1 = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/balls.fsh");
  f2 = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/graph.fsh");

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

  if (v1 && f2) {
    // Create and link the shader program
    program = glCreateProgram();
    glAttachShader(program, v1);
    qwqz_checkgl("attachA\n");
    glAttachShader(program, f2);
    qwqz_checkgl("attach\n");
    
    qwqz_linkage_init(program, &qwqz_engine->m_Linkages[1]);
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
  qwqz_batch_init(&qwqz_engine->m_Batches[1], &qwqz_engine->m_Linkages[1], 1);
  
  qwqz_checkgl("batch init\n");

  qwqz_checkgl("main\n");

  return 0;
}
