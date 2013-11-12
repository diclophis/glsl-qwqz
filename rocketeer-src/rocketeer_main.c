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


//TODO: abstract chipmunk render system into bridge functions
#define GRABBABLE_MASK_BIT (1<<31)
cpShapeFilter GRAB_FILTER = {CP_NO_GROUP, GRABBABLE_MASK_BIT, GRABBABLE_MASK_BIT};
cpShapeFilter NOT_GRABBABLE_FILTER = {CP_NO_GROUP, ~GRABBABLE_MASK_BIT, ~GRABBABLE_MASK_BIT};

void ChipmunkDemoDefaultDrawImpl(cpSpace *space);

static void
DrawCircle(cpVect p, cpFloat a, cpFloat r, cpSpaceDebugColor outline, cpSpaceDebugColor fill, cpDataPointer *data)
{ChipmunkDebugDrawCircle(p, a, r, outline, fill);}

static void
DrawSegment(cpVect a, cpVect b, cpSpaceDebugColor color, cpDataPointer *data)
{ChipmunkDebugDrawSegment(a, b, color);}

static void
DrawFatSegment(cpVect a, cpVect b, cpFloat r, cpSpaceDebugColor outline, cpSpaceDebugColor fill, cpDataPointer *data)
{ChipmunkDebugDrawFatSegment(a, b, r, outline, fill);}

static void
DrawPolygon(int count, const cpVect *verts, cpFloat r, cpSpaceDebugColor outline, cpSpaceDebugColor fill, cpDataPointer *data)
{ChipmunkDebugDrawPolygon(count, verts, r, outline, fill);}

static void
DrawDot(cpFloat size, cpVect pos, cpSpaceDebugColor color, cpDataPointer *data)
{ChipmunkDebugDrawDot(size, pos, color);}

static cpSpaceDebugColor
ColorForShape(cpShape *shape, cpDataPointer *data)
{
  if(cpShapeGetSensor(shape)){
    return LAColor(1.0f, 0.1f);
  } else {
    cpBody *body = shape->body;
    
    if(cpBodyIsSleeping(body)){
      return LAColor(0.2f, 1.0f);
    } else if(body->node.idleTime > shape->space->sleepTimeThreshold) {
      return LAColor(0.66f, 1.0f);
    } else {
      uint32_t val = (uint32_t)shape->hashid;
      
      // scramble the bits up using Robert Jenkins' 32 bit integer hash function
      val = (val+0x7ed55d16) + (val<<12);
      val = (val^0xc761c23c) ^ (val>>19);
      val = (val+0x165667b1) + (val<<5);
      val = (val+0xd3a2646c) ^ (val<<9);
      val = (val+0xfd7046c5) + (val<<3);
      val = (val^0xb55a4f09) ^ (val>>16);
      
      GLfloat r = (GLfloat)((val>>0) & 0xFF);
      GLfloat g = (GLfloat)((val>>8) & 0xFF);
      GLfloat b = (GLfloat)((val>>16) & 0xFF);
      
      GLfloat max = (GLfloat)cpfmax(cpfmax(r, g), b);
      GLfloat min = (GLfloat)cpfmin(cpfmin(r, g), b);
      GLfloat intensity = (cpBodyIsStatic(body) ? 0.15f : 0.75f);
      
      // Saturate and scale the color
      if(min == max){
        return RGBAColor(intensity, 0.0f, 0.0f, 1.0f);
      } else {
        GLfloat coef = (GLfloat)intensity/(max - min);
        return RGBAColor(
          (r - min)*coef,
          (g - min)*coef,
          (b - min)*coef,
          1.0f
        );
      }
    }
  }
}


void ChipmunkDemoDefaultDrawImpl(cpSpace *space) {
  cpSpaceDebugDrawOptions drawOptions = {
    DrawCircle,
    DrawSegment,
    DrawFatSegment,
    DrawPolygon,
    DrawDot,

    CP_SPACE_DEBUG_DRAW_SHAPES | CP_SPACE_DEBUG_DRAW_CONSTRAINTS | CP_SPACE_DEBUG_DRAW_COLLISION_POINTS,

    {200.0f/255.0f, 210.0f/255.0f, 230.0f/255.0f, 1.0f},
    ColorForShape,
    {0.0f, 0.75f, 0.0f, 1.0f},
    {1.0f, 0.0f, 0.0f, 1.0f},
  };

  cpSpaceDebugDraw(space, &drawOptions);
}


static qwqz_handle qwqz_engine = NULL;
static cpSpace *space;
static int doPhysics = 1;
static int doSpine = 1;
static int doShaderBg = 1;


static spSkeleton* bgsSkeleton;
static spAnimationStateData* bgsStateData;
static spAnimationState* bgsState;
static spSkeleton* skeleton;
static spAnimationStateData* stateData;
static spAnimationState* state;
static float verticeBuffer[8];
static float uvBuffer[8];
static float bgsScroll[9] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
static cpBody **bodies;
static int jumped = 0;

int impl_draw() {

  qwqz_tick_timer(&qwqz_engine->m_Timers[0]);

  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  if (doShaderBg) {
    qwqz_batch_clear(&qwqz_engine->m_Batches[1]);
    qwqz_batch_clear(&qwqz_engine->m_Batches[2]);

    verticeBuffer[0] = -qwqz_engine->m_ScreenHalfWidth;
    verticeBuffer[1] = -qwqz_engine->m_ScreenHalfHeight;

    verticeBuffer[2] = -qwqz_engine->m_ScreenHalfWidth;
    verticeBuffer[3] = qwqz_engine->m_ScreenHalfHeight;

    verticeBuffer[4] = qwqz_engine->m_ScreenHalfWidth;
    verticeBuffer[5] = qwqz_engine->m_ScreenHalfHeight;

    verticeBuffer[6] = qwqz_engine->m_ScreenHalfWidth;
    verticeBuffer[7] = -qwqz_engine->m_ScreenHalfHeight;

    uvBuffer[0] = 0.0;
    uvBuffer[1] = 0.0;
    uvBuffer[2] = 0.0;
    uvBuffer[3] = 0.0;
    uvBuffer[4] = 0.0;
    uvBuffer[5] = 0.0;
    uvBuffer[6] = 0.0;
    uvBuffer[7] = 0.0;

    qwqz_batch_add(&qwqz_engine->m_Batches[1], 0, verticeBuffer, NULL, uvBuffer);
    qwqz_batch_add(&qwqz_engine->m_Batches[2], 0, verticeBuffer, NULL, uvBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, qwqz_engine->FramebufferName);
    glViewport(0, 0, qwqz_engine->m_RenderTextureWidth, qwqz_engine->m_RenderTextureWidth); // Render on the whole framebuffer, complete from the lower left corner to the upper right

    glUseProgram(qwqz_engine->m_Linkages[1].m_Program);
    glUniform2f(qwqz_engine->m_Linkages[1].g_ResolutionUniform, qwqz_engine->m_RenderTextureWidth, qwqz_engine->m_RenderTextureWidth);
    glUniform1f(qwqz_engine->m_Linkages[1].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);

    translate(&qwqz_engine->m_Linkages[1], NULL, 0, 0, 0);

    //TODO: abstract render passes
    {
      qwqz_batch_prepare(qwqz_engine, &qwqz_engine->m_Batches[1]);
      size_t size_of_sprite = sizeof(struct qwqz_sprite_t);
      glVertexAttribPointer(qwqz_engine->m_Linkages[1].g_PositionAttribute, 2, GL_SHORT, GL_FALSE, size_of_sprite, (char *)NULL + (0));
      glEnableVertexAttribArray(qwqz_engine->m_Linkages[1].g_PositionAttribute);
      glVertexAttribPointer(qwqz_engine->m_Linkages[1].g_TextureAttribute, 2, GL_FLOAT, GL_FALSE, size_of_sprite, (char *)NULL + (2 * sizeof(GLshort)));
      glEnableVertexAttribArray(qwqz_engine->m_Linkages[1].g_TextureAttribute);
    }

    qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[1]);

    // Render to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);

    glUseProgram(qwqz_engine->m_Linkages[2].m_Program);
    glUniform2f(qwqz_engine->m_Linkages[2].g_ResolutionUniform, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);
    glUniform1f(qwqz_engine->m_Linkages[2].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);
    glUniform1i(qwqz_engine->m_Linkages[2].g_TextureUniform, 2);

    translate(&qwqz_engine->m_Linkages[2], NULL, 0, 0, 0);

    qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[2]);
  }


  if (doSpine) {
    if (1) {
      qwqz_batch_clear(&qwqz_engine->m_Batches[0]);

      bgsSkeleton->root->scaleX = 1.0;
      bgsSkeleton->root->scaleY = 1.0;

      spSkeleton_updateWorldTransform(bgsSkeleton);

      for (int a=0; a<9; a++) {
        float spd_m = 1.0 + (float)(a / 3);
        float spd_x = 24.0;
        float total_w = 1024.0;

        bgsScroll[a] += -spd_x * qwqz_engine->m_Timers[0].step * spd_m;

        if (bgsScroll[a] < -(total_w)) {
          bgsScroll[a] = total_w * 2;
        }

        int c = a;
        spSlot *s = bgsSkeleton->drawOrder[c];
        spRegionAttachment *ra = (spRegionAttachment *)s->attachment;
        if (s->attachment->type == ATTACHMENT_REGION) {
          spRegionAttachment_computeWorldVertices(ra, bgsScroll[a], 0.0, s->bone, verticeBuffer);
          qwqz_batch_add(&qwqz_engine->m_Batches[0], 0, verticeBuffer, NULL, ra->uvs);
        }
      }

      glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
      glUniform2f(qwqz_engine->m_Linkages[0].g_ResolutionUniform, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);
      glUniform1f(qwqz_engine->m_Linkages[0].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);

      glUniform1i(qwqz_engine->m_Linkages[0].g_TextureUniform, 1);

      translate(&qwqz_engine->m_Linkages[0], NULL, 0, 0, 0);

      //TODO: abstract render passes
      {
        qwqz_batch_prepare(qwqz_engine, &qwqz_engine->m_Batches[0]);
        size_t size_of_sprite = sizeof(struct qwqz_sprite_t);
        glVertexAttribPointer(qwqz_engine->m_Linkages[0].g_PositionAttribute, 2, GL_SHORT, GL_FALSE, size_of_sprite, (char *)NULL + (0));
        glEnableVertexAttribArray(qwqz_engine->m_Linkages[0].g_PositionAttribute);
        glVertexAttribPointer(qwqz_engine->m_Linkages[0].g_TextureAttribute, 2, GL_FLOAT, GL_FALSE, size_of_sprite, (char *)NULL + (2 * sizeof(GLshort)));
        glEnableVertexAttribArray(qwqz_engine->m_Linkages[0].g_TextureAttribute);
      }

      qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[0]);
    }

    if (1) {
      qwqz_batch_clear(&qwqz_engine->m_Batches[0]);

      skeleton->root->scaleX = 1.0;
      skeleton->root->scaleY = 1.0;

      spAnimationState_update(state, qwqz_engine->m_Timers[0].step * 3.125);
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

      for (int i=0; i<skeleton->slotCount; i++) {
        spSlot *s = skeleton->drawOrder[i];
        spRegionAttachment *ra = (spRegionAttachment *)s->attachment;
        if (s->attachment->type == ATTACHMENT_REGION) {
          float ox = qwqz_engine->m_Timers[0].m_SimulationTime * 40.0; //TODO: player movement
          spRegionAttachment_computeWorldVertices(ra, ox, 0.0, s->bone, verticeBuffer);
          qwqz_batch_add(&qwqz_engine->m_Batches[0], 0, verticeBuffer, NULL, ra->uvs);

          float rr = DEGREES_TO_RADIANS(s->bone->worldRotation);
          float r = DEGREES_TO_RADIANS(s->bone->worldRotation + ra->rotation);

          float x = s->bone->worldX + ((cosf(rr) * ra->x) - (sinf(rr) * ra->y));
          float y = s->bone->worldY + ((sinf(rr) * ra->x) + (cosf(rr) * ra->y));

          cpBody *body = bodies[i];

          cpVect newPos = cpv(x + ox, y);
          cpBodySetVelocity(body, cpvmult(cpvsub(newPos, cpBodyGetPosition(body)), 1.0/qwqz_engine->m_Timers[0].step));
          cpBodySetPosition(body, newPos);

          cpBodySetAngle(body, r);
        }
      }

      glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
      glUniform2f(qwqz_engine->m_Linkages[0].g_ResolutionUniform, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);
      glUniform1f(qwqz_engine->m_Linkages[0].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);

      glUniform1i(qwqz_engine->m_Linkages[0].g_TextureUniform, 0);

      translate(&qwqz_engine->m_Linkages[0], NULL, 0, 0, 0);

      //TODO: abstract render passes
      {
        qwqz_batch_prepare(qwqz_engine, &qwqz_engine->m_Batches[0]);
        size_t size_of_sprite = sizeof(struct qwqz_sprite_t);
        glVertexAttribPointer(qwqz_engine->m_Linkages[0].g_PositionAttribute, 2, GL_SHORT, GL_FALSE, size_of_sprite, (char *)NULL + (0));
        glEnableVertexAttribArray(qwqz_engine->m_Linkages[0].g_PositionAttribute);
        glVertexAttribPointer(qwqz_engine->m_Linkages[0].g_TextureAttribute, 2, GL_FLOAT, GL_FALSE, size_of_sprite, (char *)NULL + (2 * sizeof(GLshort)));
        glEnableVertexAttribArray(qwqz_engine->m_Linkages[0].g_TextureAttribute);
      }

      qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[0]);
    }
  }

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
}


int impl_resize(int width, int height) {
  qwqz_resize(qwqz_engine, width, height);

  return 0;
}


int impl_main(int argc, char** argv) {

  qwqz_engine = qwqz_create();

  GLuint v = 0;
  GLuint f2 = 0;
  GLuint program = 0;

  if (doPhysics) {
    ChipmunkDebugDrawInit();

    space = cpSpaceNew();
    cpSpaceSetGravity(space, cpv(0, -200));

    cpShape *shape;
    cpBody *body;
    cpBody *staticBody = cpSpaceGetStaticBody(space);

    //foor
    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-1000, 0), cpv(1000, 0), 0.0f));
    cpShapeSetElasticity(shape, 0.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    // Add lots of boxes.
    for(int i=0; i<15; i++){
      for(int j=0; j<=i; j++){
        body = cpSpaceAddBody(space, cpBodyNew(1.0f, cpMomentForBox(1.0f, 30.0f, 30.0f)));

        cpBodySetPosition(body, cpv(j*43 - i*16, 600 + i*64));
        
        shape = cpSpaceAddShape(space, cpBoxShapeNew(body, 30.0f, 30.0f, 0.5f));
        cpShapeSetElasticity(shape, 1.0f);
        cpShapeSetFriction(shape, 1.0f);
        cpGroup boxGroup = 1;
        shape->filter.group = boxGroup;
      }
    }

    // Add a ball to make things more interesting
    cpFloat radius = 15.0f;
    body = cpSpaceAddBody(space, cpBodyNew(10.0f, cpMomentForCircle(10.0f, 0.0f, radius, cpvzero)));
    cpBodySetPosition(body, cpv(0, 440 + radius+5));

    shape = cpSpaceAddShape(space, cpCircleShapeNew(body, radius, cpvzero));
    cpShapeSetElasticity(shape, 0.0f);
    cpShapeSetFriction(shape, 0.9f);
  }

  qwqz_engine->m_Timers = (struct qwqz_timer_t *)malloc(sizeof(struct qwqz_timer_t) * 1);
  qwqz_timer_init(&qwqz_engine->m_Timers[0]);

  int t0 = qwqz_texture_init(GL_TEXTURE0, "assets/spine/robot.png");
  int t1 = qwqz_texture_init(GL_TEXTURE1, "assets/spine/bgs.png");

  LOGV("fix implied assumption about texture bindings %d %d\n", t0, t1);

  qwqz_engine->m_Linkages = (struct qwqz_linkage_t *)malloc(sizeof(struct qwqz_linkage_t) * 3);
  qwqz_engine->m_Batches = (struct qwqz_batch_t *)malloc(sizeof(struct qwqz_batch_t) * 3);

  if (doSpine) {
    {
      spAtlas* atlas = spAtlas_readAtlasFile("assets/spine/robot.atlas");
      spSkeletonJson* json = spSkeletonJson_create(atlas);
      spSkeletonData *skeletonData = spSkeletonJson_readSkeletonDataFile(json, "assets/spine/robot.json");
      skeleton = spSkeleton_create(skeletonData);
      stateData = spAnimationStateData_create(skeletonData);
      spAnimationStateData_setMixByName(stateData, "walk_alt", "jump", 0.75);
      spAnimationStateData_setMixByName(stateData, "jump", "walk_alt", 0.75);
      state = spAnimationState_create(stateData);
      spAnimationState_setAnimationByName(state, 0, "walk_alt", 1);

      spAtlas *atlas2 = spAtlas_readAtlasFile("assets/spine/bgs.atlas");
      spSkeletonJson *json2 = spSkeletonJson_create(atlas2);
      spSkeletonData *skeletonData2 = spSkeletonJson_readSkeletonDataFile(json2, "assets/spine/bgs.json");
      bgsSkeleton = spSkeleton_create(skeletonData2);
      bgsStateData = spAnimationStateData_create(skeletonData2);
      bgsState = spAnimationState_create(bgsStateData);
      spAnimationState_setAnimationByName(bgsState, 0, "default", 1);
    }

    v = qwqz_compile(GL_VERTEX_SHADER, "assets/shaders/spine_bone_texture_quad.vsh");
    f2 = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/filledquad.fsh");

    if (v && f2) {
      program = glCreateProgram();
      glAttachShader(program, v);
      glAttachShader(program, f2);
      qwqz_linkage_init(program, &qwqz_engine->m_Linkages[0]);
    }

    qwqz_batch_init(&qwqz_engine->m_Batches[0], &qwqz_engine->m_Linkages[0], (bgsSkeleton->slotCount * 3) + skeleton->slotCount);

    for (int i=0; i<9; i++) {
      bgsScroll[i] = (2 - (i % 3)) * 1024.0;
    }

    skeleton->root->scaleX = 1.0;
    skeleton->root->scaleY = 1.0;

    spSkeleton_updateWorldTransform(skeleton);

    bodies = (cpBody **)malloc(sizeof(cpBody *) * skeleton->slotCount);

    for (int i=0; i<skeleton->slotCount; i++) {
      spSlot *s = skeleton->drawOrder[i];
      if (s->attachment->type == ATTACHMENT_REGION) {

        spRegionAttachment *ra = (spRegionAttachment *)s->attachment;

        float rr = DEGREES_TO_RADIANS(s->bone->worldRotation);
        float r = DEGREES_TO_RADIANS(s->bone->worldRotation + ra->rotation);

        float x = s->bone->worldX + ((cosf(rr) * ra->x) - (sinf(rr) * ra->y));
        float y = s->bone->worldY + ((sinf(rr) * ra->x) + (cosf(rr) * ra->y));

        cpBody *body;
        cpShape *shape;

        body = cpBodyNew(INFINITY, cpMomentForBox(INFINITY, ra->width, ra->height));
        bodies[i] = body;

        cpBodySetAngle(body, r);
        cpBodySetPosition(body, cpv(x, y));
        
        shape = cpSpaceAddShape(space, cpBoxShapeNew(body, ra->width, ra->height, 0.0f));
        cpShapeSetElasticity(shape, 0.0f);
        cpShapeSetFriction(shape, 1.0f);
        cpGroup spineGroup = 2;
        shape->filter.group = spineGroup;
      }
    }
  }

  if (doShaderBg) {
    qwqz_engine->m_RenderTextureWidth = 256;

    GLuint v = 0;
    GLuint f = 0;
    GLuint f2 = 0;
    GLuint program = 0;

    // render target
    int renderBufferTexture = qwqz_buffer_texture_init(GL_TEXTURE2);
    qwqz_engine->FramebufferName = qwqz_buffer_target_init(renderBufferTexture);

    v = qwqz_compile(GL_VERTEX_SHADER, "assets/shaders/basic.vsh");
    f = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/thunder_and_lightning.fsh");
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
    }

    qwqz_batch_init(&qwqz_engine->m_Batches[1], &qwqz_engine->m_Linkages[1], 1);
    qwqz_batch_init(&qwqz_engine->m_Batches[2], &qwqz_engine->m_Linkages[2], 1);
  }

  return 0;
}
