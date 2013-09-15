// test impl


#include "opengles_bridge.h"
#include "libqwqz.h"
#include "impl_main.h"
#include "test_main.h"


static qwqz_handle qwqz_engine = NULL;


void impl_draw() {
  //TODO: timer
  {
    struct timeval tim;
    gettimeofday(&tim, NULL);
    qwqz_engine->t2 = tim.tv_sec + (tim.tv_usec / 1000000.0);
    float step = qwqz_engine->t2 - qwqz_engine->t1;
    qwqz_engine->t1 = qwqz_engine->t2;
    qwqz_engine->m_SimulationTime += step;
  }

  //TODO: dumbness
  if (qwqz_engine->m_Linkages && qwqz_engine->m_Batches && qwqz_engine->m_IsScreenResized) {
    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, qwqz_engine->FramebufferName);
    glViewport(0, 0, qwqz_engine->m_RenderTextureWidth, qwqz_engine->m_RenderTextureWidth); // Render on the whole framebuffer, complete from the lower left corner to the upper right

    glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
    glUniform2f(qwqz_engine->m_Linkages[0].g_ResolutionUniform, qwqz_engine->m_RenderTextureWidth, qwqz_engine->m_RenderTextureWidth);
    glUniform1f(qwqz_engine->m_Linkages[0].g_TimeUniform, qwqz_engine->m_SimulationTime);

    glDrawElements(GL_TRIANGLES, 1 * 6, GL_UNSIGNED_SHORT, (GLvoid*)((char*)NULL));

    // Render to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);

    glUseProgram(qwqz_engine->m_Linkages[1].m_Program);
    glUniform2f(qwqz_engine->m_Linkages[1].g_ResolutionUniform, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);
    glUniform1f(qwqz_engine->m_Linkages[1].g_TimeUniform, qwqz_engine->m_SimulationTime);
    glUniform1i(qwqz_engine->m_Linkages[1].g_TextureUniform, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, qwqz_engine->renderedTexture);
     
    glDrawElements(GL_TRIANGLES, 1 * 6, GL_UNSIGNED_SHORT, (GLvoid*)((char*)NULL));
  }

  qwqz_draw(qwqz_engine);
}


void impl_resize(int width, int height) {
  qwqz_resize(qwqz_engine, (float)width, (float)height);
}


int impl_main(int argc, char** argv) {
  if (argc == 3) {
    qwqz_engine = qwqz_create();

    GLuint v = 0;
    GLuint f = 0;
    GLuint f2 = 0;
    GLuint program = 0;

    //TODO: timer abstraction
    struct timeval tim;
    gettimeofday(&tim, NULL);
    qwqz_engine->t1 = tim.tv_sec + (tim.tv_usec / 1000000.0);

    //TODO: why does this have to happen before linking?
    qwqz_engine->m_Batches = (struct qwqz_batch_t *)malloc(sizeof(struct qwqz_batch_t) * 1);
    qwqz_batch_init(&qwqz_engine->m_Batches[0]);

    qwqz_engine->m_Linkages = (struct qwqz_linkage_t *)malloc(sizeof(struct qwqz_linkage_t) * 2);

    v = qwqz_compile(GL_VERTEX_SHADER, argv[1]);
    f = qwqz_compile(GL_FRAGMENT_SHADER, argv[2]);
    f2 = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/texquad.fsh");

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
    
      return 0;
    }
  }

  return 1;
}
