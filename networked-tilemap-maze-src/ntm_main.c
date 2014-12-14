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
static spRegionAttachment* lastAttachment = NULL;
static GLuint g_TextureOffset = -1;
static float flip = -0.0;
static float offX = 0.0;
static float offY = 0.0;

// render stuff
static float verticeBuffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static int debugGrid = 8;


float ntm_character_uv(float uv) {
  float characterUv = ((18.0 / 2048.0) * 12.0);
  return uv * characterUv;
}


float ntm_character_uw(float uw) {
  float characterUw = ((20.0 / 2048.0) * 1.0);
  float characterOffsetHeight = characterUw * 3.0;
  return uw * characterOffsetHeight;
}


int impl_hit(int x, int y, int s) {
  float scrh = 2.0 * (qwqz_engine->m_Zoom2 / 1.0);

  //skeleton->root->x = 
  offX = (((float)x / qwqz_engine->m_ScreenWidth) * scrh * (qwqz_engine->m_ScreenWidth / qwqz_engine->m_ScreenHeight)) - (0.5 * scrh * (qwqz_engine->m_ScreenWidth / qwqz_engine->m_ScreenHeight));
  //if (flip) {
  //  skeleton->root->x *= flip;
  //}
  //skeleton->root->y = 
  offY = ((scrh / 2.0) - (((float)y / qwqz_engine->m_ScreenHeight) * scrh));//  - (0.25 * scrh * ((qwqz_engine->m_ScreenWidth / qwqz_engine->m_ScreenHeight)));

  //spSkeleton_updateWorldTransform(skeleton);

  return 0;
}


int impl_draw(int b) {
  qwqz_tick_timer(&qwqz_engine->m_Timers[0]);

  int toxi = (qwqz_engine->m_Timers[0].m_SimulationTime * (0.333 * 0.5) * 1.0);
  int toyi = (qwqz_engine->m_Timers[0].m_SimulationTime * (0.333 * 0.5) * 0.5);
  float tox = ntm_character_uv(toxi % 2);
  float toy = ntm_character_uw(toyi % 26);

  int ax = (0 * 4);
  int ay = ((int)(qwqz_engine->m_Timers[0].m_SimulationTime * 1.0)) % 6;

  int si = ay; //((int)(qwqz_engine->m_Timers[0].m_SimulationTime * 4.0)) % 6;
  if (si == 1 || si == 3) {
    flip = 0.0;
  } else {
    flip = 1.0;
  }

  skeleton->flipX = flip; // so simple

  spAnimationState_update(state, qwqz_engine->m_Timers[0].step);
  spAnimationState_apply(state, skeleton);
  spSkeleton_updateWorldTransform(skeleton);

  glUniform1f(qwqz_engine->m_Linkages[0].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);
  glUniform2f(g_TextureOffset, tox + ((ntm_character_uv(ax) / 12.0) * 1.0), toy + ((ntm_character_uw(1.0) / 3.0) * (ay % 3)));

  glClearColor(0.01, 1.0, 0.01, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  qwqz_batch_clear(&qwqz_engine->m_Batches[0]);
  qwqz_engine->m_Batches[0].m_NeedsAttribs = 1;
  qwqz_batch_prepare(qwqz_engine, &qwqz_engine->m_Batches[0], &qwqz_engine->m_Linkages[0]);

  spSlot *s = skeleton->drawOrder[0];
  spRegionAttachment *ra = (spRegionAttachment *)s->attachment;

  if (s->attachment && s->attachment->type == SP_ATTACHMENT_REGION) {
    if (lastAttachment != ra) {
      lastAttachment = ra;
    }

    if (1) {
      spRegionAttachment_computeWorldVertices(ra, offX , offY, s->bone, verticeBuffer);
      qwqz_batch_add(&qwqz_engine->m_Batches[0], 0, verticeBuffer, NULL, ra->uvs);
    } else {
      for (int i=0; i<debugGrid; i++) {
        for (int j=0; j<debugGrid; j++) {
          spRegionAttachment_computeWorldVertices(ra, offX + (i * 18), offY + (j * 18), s->bone, verticeBuffer);
          qwqz_batch_add(&qwqz_engine->m_Batches[0], 0, verticeBuffer, NULL, ra->uvs);
        }
      }
    }
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

  float scrh = 2.0 * (128.0 / 1.0);
  skeleton->root->x = 0;
  skeleton->root->y = scrh * 0.5;
  spSkeleton_updateWorldTransform(skeleton);
 
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
  f2 = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/indexed_filled_quad.fsh");

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

  qwqz_batch_init(&qwqz_engine->m_Batches[0], &qwqz_engine->m_Linkages[0], (skeleton->slotCount) * debugGrid * debugGrid);

  glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
  int roboRegionRenderObject = (int)((spAtlasRegion *)((spRegionAttachment *)skeleton->drawOrder[0]->attachment)->rendererObject)->page->rendererObject; //TODO: fix this, fuck yea C
  glUniform1i(qwqz_engine->m_Linkages[0].g_TextureUniform, roboRegionRenderObject); //TODO: texture unit

  g_TextureOffset = glGetUniformLocation(program, "iTextureOffset");
  glUniform2f(g_TextureOffset, ntm_character_uv(0), ntm_character_uw(1));

  qwqz_engine->g_lastFrameBuffer = b;
  qwqz_engine->m_Zoom2 = 128.0 / 1.0;

  glActiveTexture(GL_TEXTURE0);

  return 0;
}
