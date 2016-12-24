// rocketeer run and jump

#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) / 180.0 * M_PI)


#include "opengles_bridge.h"
#include "libqwqz.h"
#include "impl_main.h"
#include "rocketeer_main.h"
#include <chipmunk/chipmunk.h>
#include <chipmunk/ChipmunkDebugDraw.h>
#include <chipmunk/ChipmunkDemoShaderSupport.h>
#include <spine/spine.h>
#include <spine/extension.h>
#include "spine_bridge.h"
#include "chipmunk_bridge.h"




static qwqz_handle qwqz_engine = NULL;
static cpSpace *space;
static int doPhysics = 1;
static int doSpine = 1;
static int doShaderBg = 1;
static GLuint g_TextureOffset = -1;


static spSkeleton* bgsSkeleton;
static spAnimationStateData* bgsStateData;
static spAnimationState* bgsState;
static spSkeleton* skeleton;
static spAnimationStateData* stateData;
static spAnimationState* state;
static spRegionAttachment* lastAttachment = NULL;
static float verticeBuffer[8];
static float uvBuffer[8];
static float bgsScroll[9] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
static cpBody **bodies;
static int jumped = 0;

int impl_draw(int b) {
  qwqz_tick_timer(&qwqz_engine->m_Timers[0]);

  spAnimationState_update(state, qwqz_engine->m_Timers[0].step * 3.0);
  spAnimationState_apply(state, skeleton);
  spSkeleton_updateWorldTransform(skeleton);

  glUniform1f(qwqz_engine->m_Linkages[0].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);
  //glUniform2f(g_TextureOffset, 0) * (ay % 3)));

  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  qwqz_batch_clear(&qwqz_engine->m_Batches[0]);
  qwqz_engine->m_Batches[0].m_NeedsAttribs = 1;
  qwqz_batch_prepare(qwqz_engine, &qwqz_engine->m_Batches[0], &qwqz_engine->m_Linkages[0]);

  for (int i=0; i<skeleton->slotCount; i++) {
    spSlot *s = skeleton->drawOrder[i];
    spRegionAttachment *ra = (spRegionAttachment *)s->attachment;
    if (s->attachment->type == SP_ATTACHMENT_REGION) {
      float ox = 0; //qwqz_engine->m_Timers[0].m_SimulationTime * 40.0; //TODO: player movement
      spRegionAttachment_computeWorldVertices(ra, ox, 0.0, s->bone, verticeBuffer);
      qwqz_batch_add(&qwqz_engine->m_Batches[0], 0, verticeBuffer, NULL, ra->uvs);

      float rr = DEGREES_TO_RADIANS(s->bone->worldRotation);
      float r = DEGREES_TO_RADIANS(s->bone->worldRotation + ra->rotation);

      float x = s->bone->worldX + ((cosf(rr) * ra->x) - (sinf(rr) * ra->y));
      float y = s->bone->worldY + ((sinf(rr) * ra->x) + (cosf(rr) * ra->y));

      cpBody *body = bodies[i];

      cpVect newPos = cpv(x + ox, y);
      cpVect newVel = cpvmult(cpvsub(newPos, cpBodyGetPosition(body)), 1.0/qwqz_engine->m_Timers[0].step);

      float velocity_limit = 150;
      float velocity_mag = cpvlength(newVel);
      if (velocity_mag > velocity_limit) {
        float velocity_scale = velocity_limit / velocity_mag;
        newVel = cpvmult(newVel, velocity_scale < 0.00011 ? 0.00011: velocity_scale);
      }

      cpBodySetVelocity(body, newVel);
      cpBodySetPosition(body, newPos);

      cpBodySetAngle(body, r);
    }
  }

  glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
  qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[0]);

  if (doPhysics) {
    cpSpaceStep(space, qwqz_engine->m_Timers[0].step);

    // Draw the renderer contents and reset it back to the last tick's state.
    ChipmunkDebugDrawClearRenderer();
    ChipmunkDebugDrawPushRenderer();
    ChipmunkDemoDefaultDrawImpl(space);
    ChipmunkDebugDrawFlushRenderer();
    ChipmunkDebugDrawPopRenderer();
  }


  return 0;




/*
  qwqz_tick_timer(&qwqz_engine->m_Timers[0]);

  // not needed explicitly given that doShaderBg draws the to the entire screen
  qwqz_bind_frame_buffer(qwqz_engine, b);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  //glViewport(0, 0, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);

  if (1) {
  
  if (doShaderBg) {
    qwqz_bind_frame_buffer(qwqz_engine, qwqz_engine->FramebufferName);
    //glViewport(0, 0, qwqz_engine->m_RenderTextureWidth, qwqz_engine->m_RenderTextureWidth);
    
    //glBindFramebuffer(GL_FRAMEBUFFER, b); //
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(qwqz_engine->m_Linkages[1].m_Program);
    glUniform1f(qwqz_engine->m_Linkages[1].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);

    if (1) {
      qwqz_batch_prepare(qwqz_engine, &qwqz_engine->m_Batches[1], &qwqz_engine->m_Linkages[1]);
      qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[1]);
    }
    
    if (1) {
      qwqz_bind_frame_buffer(qwqz_engine, b);
      glUseProgram(qwqz_engine->m_Linkages[2].m_Program);
      glUniform1f(qwqz_engine->m_Linkages[2].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);
      qwqz_batch_prepare(qwqz_engine, &qwqz_engine->m_Batches[2], &qwqz_engine->m_Linkages[2]);
      qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[2]);
    }
  }

  }
  
  if (1) {
  
  qwqz_bind_frame_buffer(qwqz_engine, b);

  if (doSpine) {
    if (1) {
      qwqz_batch_clear(&qwqz_engine->m_Batches[0]);

      bgsSkeleton->root->scaleX = 1.0;
      bgsSkeleton->root->scaleY = 1.0;

      spSkeleton_updateWorldTransform(bgsSkeleton);

      int bgsRegionRenderObject = (int)((spAtlasRegion *)((spRegionAttachment *)bgsSkeleton->drawOrder[0]->attachment)->rendererObject)->page->rendererObject; //TODO: fix this, fuck yea C

      for (int a=0; a<9; a++) {
        float spd_m = 1.0 + (float)(a / 3);
        float spd_x = 200.0;
        float total_w = 1024.0;

        bgsScroll[a] += -spd_x * qwqz_engine->m_Timers[0].step * spd_m;

        if (bgsScroll[a] < -(total_w)) {
          bgsScroll[a] = total_w * 2;
        }

        int c = a;
        spSlot *s = bgsSkeleton->drawOrder[c];
        spRegionAttachment *ra = (spRegionAttachment *)s->attachment;
        if (s->attachment->type == SP_ATTACHMENT_REGION) {
          spRegionAttachment_computeWorldVertices(ra, bgsScroll[a], 0.0, s->bone, verticeBuffer);
          qwqz_batch_add(&qwqz_engine->m_Batches[0], 0, verticeBuffer, NULL, ra->uvs);
        }
      }
      
      if (1) {
        glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
        glUniform1f(qwqz_engine->m_Linkages[0].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);
        glUniform1i(qwqz_engine->m_Linkages[0].g_TextureUniform, bgsRegionRenderObject); //TODO: this is the texture unit for spine background
        qwqz_batch_prepare(qwqz_engine, &qwqz_engine->m_Batches[0], &qwqz_engine->m_Linkages[0]);
        qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[0]);
      }
    }

    if (1) {
      qwqz_batch_clear(&qwqz_engine->m_Batches[0]);

      skeleton->root->scaleX = 1.0;
      skeleton->root->scaleY = 1.0;

      spAnimationState_update(state, qwqz_engine->m_Timers[0].step * 13.0); //3.125
      spAnimationState_apply(state, skeleton);
      spSkeleton_updateWorldTransform(skeleton);

      if (0 == jumped && qwqz_engine->m_Timers[0].m_SimulationTime > 3.0) {
        jumped = 1;
        spAnimationState_addAnimationByName(state, 0, "jump", 0, 0); // trackIndex, name, loop, delay
        spAnimationState_addAnimationByName(state, 0, "walk_alt", 1, 0);
      }

      if (qwqz_engine->m_Timers[0].m_SimulationTime > 6.0) {
        jumped = 0;
        qwqz_engine->m_Timers[0].m_SimulationTime = 0;
      }

      int roboRegionRenderObject = (int)((spAtlasRegion *)((spRegionAttachment *)skeleton->drawOrder[0]->attachment)->rendererObject)->page->rendererObject; //TODO: fix this, fuck yea C
      //LOGV("wtf %d\n", roboRegionRenderObject);

      for (int i=0; i<skeleton->slotCount; i++) {
        spSlot *s = skeleton->drawOrder[i];
        spRegionAttachment *ra = (spRegionAttachment *)s->attachment;
        if (s->attachment->type == SP_ATTACHMENT_REGION) {
          float ox = 0; //qwqz_engine->m_Timers[0].m_SimulationTime * 40.0; //TODO: player movement
          spRegionAttachment_computeWorldVertices(ra, ox, 0.0, s->bone, verticeBuffer);
          qwqz_batch_add(&qwqz_engine->m_Batches[0], 0, verticeBuffer, NULL, ra->uvs);

          float rr = DEGREES_TO_RADIANS(s->bone->worldRotation);
          float r = DEGREES_TO_RADIANS(s->bone->worldRotation + ra->rotation);

          float x = s->bone->worldX + ((cosf(rr) * ra->x) - (sinf(rr) * ra->y));
          float y = s->bone->worldY + ((sinf(rr) * ra->x) + (cosf(rr) * ra->y));

          cpBody *body = bodies[i];

          cpVect newPos = cpv(x + ox, y);
          cpVect newVel = cpvmult(cpvsub(newPos, cpBodyGetPosition(body)), 1.0/qwqz_engine->m_Timers[0].step);

          float velocity_limit = 150;
          float velocity_mag = cpvlength(newVel);
          if (velocity_mag > velocity_limit) {
            float velocity_scale = velocity_limit / velocity_mag;
            newVel = cpvmult(newVel, velocity_scale < 0.00011 ? 0.00011: velocity_scale);
          }

          cpBodySetVelocity(body, newVel);
          cpBodySetPosition(body, newPos);

          cpBodySetAngle(body, r);
        }
      }

      if (1) {
        glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
        glUniform1f(qwqz_engine->m_Linkages[0].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);
        glUniform1i(qwqz_engine->m_Linkages[0].g_TextureUniform, roboRegionRenderObject); //TODO: texture unit
        qwqz_batch_prepare(qwqz_engine, &qwqz_engine->m_Batches[0], &qwqz_engine->m_Linkages[0]);
        qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[0]);
      }
    }
  }
    
  }

*/
}


int impl_resize(int width, int height, int ew, int eh, int u) {
  int resized = qwqz_resize(qwqz_engine, width, height, ew, eh, u);

  for (int i=0; i<qwqz_engine->m_LinkageCount; i++) {
    glUseProgram(qwqz_engine->m_Linkages[i].m_Program);
    glUniform2f(qwqz_engine->m_Linkages[i].g_ResolutionUniform, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);
    qwqz_linkage_resize(qwqz_engine, &qwqz_engine->m_Linkages[i]);
  }

  spSkeleton_updateWorldTransform(skeleton);

  ChipmunkDebugDrawResizeRenderer(width, height);
  glUniform2f(ChipmunkDebugDrawPushRenderer(), qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);
 
  return resized;
}

int impl_hit(int x, int y, int s) {
  spAnimationState_addAnimationByName(state, 0, "jump", 0, 0); // trackIndex, name, loop, delay
  spAnimationState_addAnimationByName(state, 0, "walk_alt", 1, 0);
  return 0;
}

int impl_main(int argc, char** argv, GLuint b) {

  qwqz_engine = qwqz_create();
  qwqz_alloc_timers(qwqz_engine, 1);
  qwqz_alloc_linkages(qwqz_engine, 4);
  qwqz_alloc_batches(qwqz_engine, 4);

  GLuint program = 0;

  if (doPhysics) {
    ChipmunkDebugDrawInit();

    space = cpSpaceNew();
    cpSpaceSetGravity(space, cpv(0, -200));
    cpSpaceSetIterations(space, 1);
    cpSpaceSetCollisionSlop(space, 1.0);

    cpShape *shape;
    cpBody *body;
    cpBody *staticBody = cpSpaceGetStaticBody(space);

    //foor
    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-1000, 0), cpv(1000, 0), 0.0f));
    cpShapeSetElasticity(shape, 0.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-300, 0), cpv(-600, 400), 0.0f));
    cpShapeSetElasticity(shape, 0.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(300, 0), cpv(600, 400), 0.0f));
    cpShapeSetElasticity(shape, 0.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    for(int i=0; i<25; i++) {
      for(int j=0; j<=i; j++) {
        float m = 10000.0;

        if (rand() > (RAND_MAX / 2)) {
          // Add lots of boxes.
          body = cpSpaceAddBody(space, cpBodyNew(m, cpMomentForBox(m, 30.0f, 30.0f)));

          cpBodySetPosition(body, cpv(j*35 - i*16, 300 + i*128));
          
          shape = cpSpaceAddShape(space, cpBoxShapeNew(body, 30.0f, 30.0f, 0.0f));
          cpShapeSetElasticity(shape, 0.01f);
          cpShapeSetFriction(shape, 100.0f);

          //TODO: random collide with self?
          //cpGroup boxGroup = 1;
          //shape->filter.group = boxGroup;
        } else {
          // Add a ball to make things more interesting
          cpFloat radius = 15.0f;
          body = cpSpaceAddBody(space, cpBodyNew(m, cpMomentForCircle(m, 0.0f, radius, cpvzero)));

          cpBodySetPosition(body, cpv(j*35 - i*16, 300 + i*128));

          shape = cpSpaceAddShape(space, cpCircleShapeNew(body, radius, cpvzero));
          cpShapeSetElasticity(shape, 0.0f);
          cpShapeSetFriction(shape, 100.0f);
        }
      }
    }
  }


  if (doSpine) {
    {
      spAtlas* atlas = spAtlas_createFromFile("assets/spine/robot.atlas", NULL);
      spSkeletonJson* json = spSkeletonJson_create(atlas);
      spSkeletonData *skeletonData = spSkeletonJson_readSkeletonDataFile(json, "assets/spine/robot.json");
      assert(skeletonData);
      skeleton = spSkeleton_create(skeletonData);
      stateData = spAnimationStateData_create(skeletonData);
      spAnimationStateData_setMixByName(stateData, "walk_alt", "jump", 0.75);
      spAnimationStateData_setMixByName(stateData, "jump", "walk_alt", 0.75);
      state = spAnimationState_create(stateData);
      spAnimationState_setAnimationByName(state, 0, "walk_alt", 1);

      spAtlas* atlas2 = spAtlas_createFromFile("assets/spine/bgs.atlas", NULL);
      spSkeletonJson *json2 = spSkeletonJson_create(atlas2);
      spSkeletonData *skeletonData2 = spSkeletonJson_readSkeletonDataFile(json2, "assets/spine/bgs.json");
      bgsSkeleton = spSkeleton_create(skeletonData2);
      bgsStateData = spAnimationStateData_create(skeletonData2);
      bgsState = spAnimationState_create(bgsStateData);
      spAnimationState_setAnimationByName(bgsState, 0, "default", 1);
    }

    qwqz_stack_shader_linkage(qwqz_engine,
      "assets/shaders/spine_bone_texture_quad.vsh",
      "assets/shaders/indexed_filled_quad.fsh");

    qwqz_batch_init(&qwqz_engine->m_Batches[0],
      &qwqz_engine->m_Linkages[0], (bgsSkeleton->slotCount * 3) + skeleton->slotCount);

    glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
    int roboRegionRenderObject = (int)((spAtlasRegion *)((spRegionAttachment *)skeleton->drawOrder[0]->attachment)->rendererObject)->page->rendererObject; //TODO: fix this, fuck yea C
    glUniform1i(qwqz_engine->m_Linkages[0].g_TextureUniform, roboRegionRenderObject); //TODO: texture unit

    g_TextureOffset = glGetUniformLocation(program, "iTextureOffset");
    glUniform2f(g_TextureOffset, 0, 0);

    qwqz_engine->g_lastFrameBuffer = b;
    qwqz_engine->m_Zoom2 = 256.0 / 1.0;

    glActiveTexture(GL_TEXTURE0);

    skeleton->root->scaleX = 1.0;
    skeleton->root->scaleY = 1.0;

    spSkeleton_updateWorldTransform(skeleton);

    bodies = (cpBody **)malloc(sizeof(cpBody *) * skeleton->slotCount);

    for (int i=0; i<skeleton->slotCount; i++) {
      spSlot *s = skeleton->drawOrder[i];
      if (s->attachment->type == SP_ATTACHMENT_REGION) {

        spRegionAttachment *ra = (spRegionAttachment *)s->attachment;

        float rr = DEGREES_TO_RADIANS(s->bone->worldRotation);
        float r = DEGREES_TO_RADIANS(s->bone->worldRotation + ra->rotation);

        float x = s->bone->worldX + ((cosf(rr) * ra->x) - (sinf(rr) * ra->y));
        float y = s->bone->worldY + ((sinf(rr) * ra->x) + (cosf(rr) * ra->y));

        cpBody *body;
        cpShape *shape;

        body = cpBodyNew(INFINITY, cpMomentForBox(INFINITY, ra->width, ra->height));
        body->userData = (void *)1;
        bodies[i] = body;

        cpBodySetAngle(body, r);
        cpBodySetPosition(body, cpv(x, y));

        shape = cpSpaceAddShape(space, cpBoxShapeNew(body, ra->width * 0.5, ra->height * 0.5, 10.0f));
        cpShapeSetElasticity(shape, 0.0f);
        cpShapeSetFriction(shape, 1.0f);
        cpGroup spineGroup = 2;
        shape->filter.group = spineGroup;
      }
    }
  }


  if (0 && doShaderBg) {
    qwqz_engine->m_RenderTextureWidth = 256;

    GLuint v = 0;
    GLuint f = 0;
    GLuint f2 = 0;
    GLuint program = 0;

    // render target
    int renderBufferTexture = qwqz_buffer_texture_init(GL_TEXTURE2);
    qwqz_engine->FramebufferName = qwqz_buffer_target_init(renderBufferTexture);

    v = qwqz_compile(GL_VERTEX_SHADER, "assets/shaders/basic.vsh");
    f = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/flower.fsh");
    f2 = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/rocketeer_background.fsh");

    if (v && f && f2) {
      // Create and link the shader program
      program = glCreateProgram();
      glAttachShader(program, v);
      glAttachShader(program, f);
      qwqz_linkage_init(program, &qwqz_engine->m_Linkages[1]);

      program = glCreateProgram();
      glAttachShader(program, v);
      glAttachShader(program, f2);
      qwqz_linkage_init(program, &qwqz_engine->m_Linkages[2]);
      //GL_TEXTURE2
      //glUniform1i(qwqz_engine->m_Linkages[1].g_TextureUniform, 2);
      glUniform1i(qwqz_engine->m_Linkages[2].g_TextureUniform, 2);
    }

    qwqz_batch_init(&qwqz_engine->m_Batches[1], &qwqz_engine->m_Linkages[1], 1);
    qwqz_batch_init(&qwqz_engine->m_Batches[2], &qwqz_engine->m_Linkages[2], 1);
  }

  return 0;
}
