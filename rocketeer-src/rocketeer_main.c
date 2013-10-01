// test impl


#include "opengles_bridge.h"
#include "libqwqz.h"
#include "impl_main.h"
#include "rocketeer_main.h"
#include <chipmunk/chipmunk.h>
#include <chipmunk/ChipmunkDebugDraw.h>
#include <chipmunk/ChipmunkDemoShaderSupport.h>
#include <spine/spine.h>
#include <spine/extension.h>

static int RO = 0;

void _AtlasPage_createTexture (AtlasPage* self, const char* path) {
  //TODO: figure out how to map renderObject
	self->rendererObject = (void *)RO++;

  switch((int)self->rendererObject) {
    case 0:
      // size is important!!
      self->width = 256;
      self->height = 256;
      break;
    case 1:
      // size is important!!
      self->width = 2048;
      self->height = 2048;
      break;
  }
  LOGV("_AtlasPage_createTexture: %s\n", path);
}


void _AtlasPage_disposeTexture (AtlasPage* self) {
}


char* _Util_readFile (const char* path, int* length) {
	return _readFile(path, length);
}


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
static cpVect translate2 = {0, 0};
static cpFloat scale = 1.0;
static int doPhysics = 1;
static int doSpine = 1;
static int doMenu = 0;


static Skeleton* bgsSkeleton;
static AnimationStateData* bgsStateData;
static AnimationState* bgsState;
static Skeleton* skeleton;
static AnimationStateData* stateData;
static AnimationState* state;
static float verticeBuffer[8];
static float bgsScroll[3] = { 0.0, 0.0, 0.0 };


int impl_draw() {

  qwqz_tick_timer(&qwqz_engine->m_Timers[0]);

  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);

  if (doPhysics) {
    cpSpaceStep(space, qwqz_engine->m_Timers[0].step);

    // Draw the renderer contents and reset it back to the last tick's state.
    ChipmunkDebugDrawClearRenderer();
    ChipmunkDebugDrawPushRenderer();

    ChipmunkDemoDefaultDrawImpl(space);

    ChipmunkDebugDrawFlushRenderer();
    ChipmunkDebugDrawPopRenderer();
  }

  if (doMenu) {
    glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
    glUniform2f(qwqz_engine->m_Linkages[0].g_ResolutionUniform, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);
    glUniform1f(qwqz_engine->m_Linkages[0].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);

    glUniform1i(qwqz_engine->m_Linkages[0].g_TextureUniform, 0);
    glUniform1i(qwqz_engine->m_Linkages[0].g_TextureUniform2, 1);
    glUniform1i(qwqz_engine->m_Linkages[0].g_TextureUniform3, 2);
     
    glDrawElements(GL_TRIANGLES, 1 * 6, GL_UNSIGNED_SHORT, (GLvoid*)((char*)NULL));
  }

  if (doSpine) {
    if (1) {
      qwqz_batch_clear(&qwqz_engine->m_Batches[0]);

      bgsSkeleton->root->scaleX = 1.0;
      bgsSkeleton->root->scaleY = 1.0;

      AnimationState_update(bgsState, qwqz_engine->m_Timers[0].step * 0.1);
      AnimationState_apply(bgsState, bgsSkeleton);
      Skeleton_updateWorldTransform(bgsSkeleton);

      for (int a=0; a<3; a++) {
        float bgw = 280.0 * 3.0;
        float spd = 1.0 + (float)a;

        bgsScroll[a] += -100.0 * qwqz_engine->m_Timers[0].step * spd;

        if (bgsScroll[a] < -bgw) {
          bgsScroll[a] = 0.0;
        }

        float scx = -(bgw / 2) + bgsScroll[a];
        for (int i=0; i<3; i++) {
          for (int j=0; j<3; j++) {
            int c = (a * 3) + j;
            Slot *s = bgsSkeleton->drawOrder[c];
            RegionAttachment *ra = (RegionAttachment *)s->attachment;
            if (s->attachment->type == ATTACHMENT_REGION) {
              RegionAttachment_computeVertices(ra, scx, 0.0, s->bone, verticeBuffer);
              qwqz_batch_add(&qwqz_engine->m_Batches[0], 0, verticeBuffer, NULL, ra->uvs);
            }
          }
          scx += bgw;
        }
      }

      glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
      glUniform2f(qwqz_engine->m_Linkages[0].g_ResolutionUniform, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);
      glUniform1f(qwqz_engine->m_Linkages[0].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);

      glUniform1i(qwqz_engine->m_Linkages[0].g_TextureUniform, 1);

      translate(&qwqz_engine->m_Linkages[0], NULL, 0, 0, 0);

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

      skeleton->root->scaleX = 0.75;
      skeleton->root->scaleY = 0.75;

      AnimationState_update(state, qwqz_engine->m_Timers[0].step * 0.25);
      AnimationState_apply(state, skeleton);
      Skeleton_updateWorldTransform(skeleton);

      for (int i=0; i<skeleton->slotCount; i++) {
        Slot *s = skeleton->drawOrder[i];
        RegionAttachment *ra = (RegionAttachment *)s->attachment;
        if (s->attachment->type == ATTACHMENT_REGION) {
          RegionAttachment_computeVertices(ra, 0.0, -200.0, s->bone, verticeBuffer);
          qwqz_batch_add(&qwqz_engine->m_Batches[0], 0, verticeBuffer, NULL, ra->uvs);
        }
      }

      glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
      glUniform2f(qwqz_engine->m_Linkages[0].g_ResolutionUniform, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);
      glUniform1f(qwqz_engine->m_Linkages[0].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);

      glUniform1i(qwqz_engine->m_Linkages[0].g_TextureUniform, 0);

      translate(&qwqz_engine->m_Linkages[0], NULL, 0, 0, 0);

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
    cpSpaceSetIterations(space, 30);
    cpSpaceSetGravity(space, cpv(0, -100));
    cpSpaceSetSleepTimeThreshold(space, 0.5f);
    cpSpaceSetCollisionSlop(space, 0.5f);

    cpBody *body, *staticBody = cpSpaceGetStaticBody(space);
    cpShape *shape;

    // Create segments around the edge of the screen.
    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-320,-240), cpv(-320,240), 0.0f));
    cpShapeSetElasticity(shape, 1.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(320,-240), cpv(320,240), 0.0f));
    cpShapeSetElasticity(shape, 1.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-320,-240), cpv(320,-240), 0.0f));
    cpShapeSetElasticity(shape, 1.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    // Add lots of boxes.
    for(int i=0; i<14; i++){
      for(int j=0; j<=i; j++){
        body = cpSpaceAddBody(space, cpBodyNew(1.0f, cpMomentForBox(1.0f, 30.0f, 30.0f)));
        cpBodySetPosition(body, cpv(j*32 - i*16, 1000 - i*64));
        
        shape = cpSpaceAddShape(space, cpBoxShapeNew(body, 30.0f, 30.0f, 0.5f));
        cpShapeSetElasticity(shape, 0.0f);
        cpShapeSetFriction(shape, 0.8f);
      }
    }

    // Add a ball to make things more interesting
    cpFloat radius = 15.0f;
    body = cpSpaceAddBody(space, cpBodyNew(10.0f, cpMomentForCircle(10.0f, 0.0f, radius, cpvzero)));
    cpBodySetPosition(body, cpv(0, -240 + radius+5));

    shape = cpSpaceAddShape(space, cpCircleShapeNew(body, radius, cpvzero));
    cpShapeSetElasticity(shape, 0.0f);
    cpShapeSetFriction(shape, 0.9f);
  }

  qwqz_engine->m_Timers = (struct qwqz_timer_t *)malloc(sizeof(struct qwqz_timer_t) * 1);
  qwqz_timer_init(&qwqz_engine->m_Timers[0]);

  int t0 = qwqz_texture_init(GL_TEXTURE0, "assets/spine/elle.png");
  int t1 = qwqz_texture_init(GL_TEXTURE1, "assets/spine/bgs.png");

  qwqz_engine->m_Linkages = (struct qwqz_linkage_t *)malloc(sizeof(struct qwqz_linkage_t) * 1);

  if (doMenu) {
    v = qwqz_compile(GL_VERTEX_SHADER, "assets/shaders/basic.vsh");
    f2 = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/texquad.fsh");
    if (v && f2) {
      if (doMenu) {
        program = glCreateProgram();
        glAttachShader(program, v);
        glAttachShader(program, f2);
        qwqz_linkage_init(program, &qwqz_engine->m_Linkages[0]);
      }
    }
  }

  if (doSpine) {
    {
      Atlas* atlas = Atlas_readAtlasFile("assets/spine/elle.atlas");
      SkeletonJson* json = SkeletonJson_create(atlas);
      SkeletonData *skeletonData = SkeletonJson_readSkeletonDataFile(json, "assets/spine/elle.json");
      skeleton = Skeleton_create(skeletonData);
      stateData = AnimationStateData_create(skeletonData);
      state = AnimationState_create(stateData);
      //AnimationStateData_setMixByName(stateData, "walk", "jump", 0.2);
      //AnimationStateData_setMixByName(stateData, "jump", "walk", 0.4);
      AnimationState_setAnimationByName(state, "run", 1);

      Atlas *atlas2 = Atlas_readAtlasFile("assets/spine/bgs.atlas");
      SkeletonJson *json2 = SkeletonJson_create(atlas2);
      SkeletonData *skeletonData2 = SkeletonJson_readSkeletonDataFile(json2, "assets/spine/bgs.json");
      bgsSkeleton = Skeleton_create(skeletonData2);
      bgsStateData = AnimationStateData_create(skeletonData2);
      bgsState = AnimationState_create(bgsStateData);
      AnimationState_setAnimationByName(bgsState, "default", 1);
    }

    v = qwqz_compile(GL_VERTEX_SHADER, "assets/shaders/spine.vsh");
    f2 = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/filledquad.fsh");

    if (v && f2) {
      program = glCreateProgram();
      glAttachShader(program, v);
      glAttachShader(program, f2);
      qwqz_linkage_init(program, &qwqz_engine->m_Linkages[0]);
    }

    qwqz_engine->m_Batches = (struct qwqz_batch_t *)malloc(sizeof(struct qwqz_batch_t) * 1);
    qwqz_batch_init(&qwqz_engine->m_Batches[0], &qwqz_engine->m_Linkages[0], (bgsSkeleton->slotCount * 3) + skeleton->slotCount);
  }

  return 0;
}
