// networked tilemap maze game

#include "opengles_bridge.h"
#include "libqwqz.h"
#include "impl_main.h"
#include <spine/spine.h>
#include <spine/extension.h>
#include "spine_bridge.h"
#include "impl_main.h"

// libqwqz stuff
static qwqz_handle qwqz_engine = NULL;

// spine stuff
static spSkeleton* skeleton;
static spAnimationStateData* stateData;
static spAnimationState* state;

// render stuff
static float verticeBuffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

int impl_hit(int x, int y, int s) {
  return 0;
}

int impl_draw(int b) {
  qwqz_tick_timer(&qwqz_engine->m_Timers[0]);
  glUniform1f(qwqz_engine->m_Linkages[0].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);

  spAnimationState_update(state, qwqz_engine->m_Timers[0].step);
  spAnimationState_apply(state, skeleton);
  spSkeleton_updateWorldTransform(skeleton);

  glClear(GL_COLOR_BUFFER_BIT);

  qwqz_batch_clear(&qwqz_engine->m_Batches[0]);
  qwqz_engine->m_Batches[0].m_NeedsAttribs = 1;
  qwqz_batch_prepare(qwqz_engine, &qwqz_engine->m_Batches[0], &qwqz_engine->m_Linkages[0]);

  spSlot *s = skeleton->drawOrder[0];
  spRegionAttachment *ra = (spRegionAttachment *)s->attachment;
  if (s->attachment && s->attachment->type == SP_ATTACHMENT_REGION) {
    spRegionAttachment_computeWorldVertices(ra, 0, 0, s->bone, verticeBuffer);
    qwqz_batch_add(&qwqz_engine->m_Batches[0], 0, verticeBuffer, NULL, ra->uvs);
  }

  glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
  qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[0]);

  return 0;
}

int impl_resize(int width, int height, int ew, int eh, int u) {
  int resized = qwqz_resize(qwqz_engine, width, height, ew, eh, u);

  for (int i=0; i<1; i++) {
    glUseProgram(qwqz_engine->m_Linkages[i].m_Program);
    glUniform2f(qwqz_engine->m_Linkages[i].g_ResolutionUniform, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);
    qwqz_linkage_resize(qwqz_engine, &qwqz_engine->m_Linkages[i]);
  }
 
  return resized;
}

int impl_main(int argc, char** argv, GLuint b) {
  GLuint v = 0;
  GLuint f2 = 0;
  GLuint program = 0;

  qwqz_engine = qwqz_create();
  qwqz_engine->m_Timers = (struct qwqz_timer_t *)malloc(sizeof(struct qwqz_timer_t) * 1);
  qwqz_engine->m_Linkages = (struct qwqz_linkage_t *)malloc(sizeof(struct qwqz_linkage_t) * 1);
  qwqz_engine->m_Batches = (struct qwqz_batch_t *)malloc(sizeof(struct qwqz_batch_t) * 1);

  qwqz_timer_init(&qwqz_engine->m_Timers[0]);

  v = qwqz_compile(GL_VERTEX_SHADER, "assets/shaders/spine_bone_texture_quad.vsh");
  f2 = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/filledquad.fsh");

  if (v && f2) {
    program = glCreateProgram();
    glAttachShader(program, v);
    glAttachShader(program, f2);
    qwqz_linkage_init(program, &qwqz_engine->m_Linkages[0]);
  }


  spAtlas* atlas = spAtlas_createFromFile("assets/spine/source.atlas", NULL);
  spSkeletonJson* json = spSkeletonJson_create(atlas);
  spSkeletonData *skeletonData = spSkeletonJson_readSkeletonDataFile(json, "assets/spine/source.json");
  skeleton = spSkeleton_create(skeletonData);
  stateData = spAnimationStateData_create(skeletonData);
  state = spAnimationState_create(stateData);
  spAnimationState_setAnimationByName(state, 0, "0", 1);

  qwqz_batch_init(&qwqz_engine->m_Batches[0], &qwqz_engine->m_Linkages[0], (skeleton->slotCount));

  skeleton->root->x = 0;
  skeleton->root->y = 8;
  spSkeleton_updateWorldTransform(skeleton);

  glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
  int roboRegionRenderObject = (int)((spAtlasRegion *)((spRegionAttachment *)skeleton->drawOrder[0]->attachment)->rendererObject)->page->rendererObject; //TODO: fix this, fuck yea C
  glUniform1i(qwqz_engine->m_Linkages[0].g_TextureUniform, roboRegionRenderObject); //TODO: texture unit

  qwqz_engine->g_lastFrameBuffer = b;
  qwqz_engine->m_Zoom2 = 32.0 / 1.0;

  glActiveTexture(GL_TEXTURE0);

  return 0;
}
