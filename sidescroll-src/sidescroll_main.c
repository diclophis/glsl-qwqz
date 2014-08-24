// sidescroll like flappy bird

//TODO: move this into libqwqz
#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) / 180.0 * M_PI)
#define RADIANS_TO_DEGREES(__ANGLE__) ((__ANGLE__) * (180.0 / M_PI));

#include "opengles_bridge.h"
#include "libqwqz.h"
#include "impl_main.h"
#include "sidescroll_main.h"
#include <chipmunk/chipmunk.h>
#include <chipmunk/ChipmunkDebugDraw.h>
#include <chipmunk/ChipmunkDemoShaderSupport.h>
#include <spine/spine.h>
#include <spine/extension.h>
#include "spine_bridge.h"

/* We must always include pt.h in our protothreads code. */
#include "pt/pt.h"

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
   
    if (body->userData) {
      return LAColor(0.0f, 0.0f);
    }

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

// libqwqz stuff
static qwqz_handle qwqz_engine = NULL;
static qwqz_audio_stream qwqz_audio = NULL;
static int started_audio = 0;
static int gRenderPhysicsDebug = 0;
static float time_on_ground = -0.1;
static int counting_on_ground = 0.0;

// chipmunk stuff
static cpSpace *space;
static cpBody **bodies;
static float gChipmunkJumpPower = 200000.0;
static float gChipmunkGravity = -1200.0;
static float gChipmunkPlayerMass = 1000.0;
static float gChipmunkPlayerElasticity = 0.85;
static float gChipmunkGroundElasticity = 0.33;
static float gChipmunkPlayerSpeed = 75.0;

// spine stuff
static spSkeleton* bgsSkeleton;
static spAnimationStateData* bgsStateData;
static spAnimationState* bgsState;
static spSkeleton* skeleton;
static spAnimationStateData* stateData;
static spAnimationState* state;
static float verticeBuffer1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static float verticeBuffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static float bgsScroll[] = { 0.0, 0.0 }; //, 0.0, 0.0, 0.0, 0.0 }; //, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
static int num_bg = 0;
static int bg_range = 0;
static int bg_first = 0;
static int bg_last = 0;

////protothreads
//static struct pt pt1, pt2;
///* Two flags that the two protothread functions use. */
//static int protothread1_flag, protothread2_flag;
//
//
///**
// * This is a very small example that shows how to use
// * protothreads. The program consists of two protothreads that wait
// * for each other to toggle a variable.
// */
///**
// * The first protothread function. A protothread function must always
// * return an integer, but must never explicitly return - returning is
// * performed inside the protothread statements.
// *
// * The protothread function is driven by the main loop further down in
// * the code.
// */
//static int
//protothread1(struct pt *pt)
//{
//  /* A protothread function must begin with PT_BEGIN() which takes a
//     pointer to a struct pt. */
//  PT_BEGIN(pt);
//
//  /* We loop forever here. */
//  while(1) {
//    /* Wait until the other protothread has set its flag. */
//    PT_WAIT_UNTIL(pt, protothread2_flag != 0);
//    //LOGV("Protothread 1 running\n");
//
//    /* We then reset the other protothread's flag, and set our own
//       flag so that the other protothread can run. */
//    protothread2_flag = 0;
//    protothread1_flag = 1;
//
//    /* And we loop. */
//  }
//
//  /* All protothread functions must end with PT_END() which takes a
//     pointer to a struct pt. */
//  PT_END(pt);
//}
//
///**
// * The second protothread function. This is almost the same as the
// * first one.
// */
//static int
//protothread2(struct pt *pt)
//{
//  PT_BEGIN(pt);
//
//  while(1) {
//    /* Let the other protothread run. */
//    protothread2_flag = 1;
//
//    /* Wait until the other protothread has set its flag. */
//    PT_WAIT_UNTIL(pt, protothread1_flag != 0);
//    //LOGV("Protothread 2 running\n");
//    
//    /* We then reset the other protothread's flag. */
//    protothread1_flag = 0;
//
//    /* And we loop. */
//  }
//  PT_END(pt);
//}
///**
//* Finally, we have the main loop. Here is where the protothreads are
//* initialized and scheduled. First, however, we define the
//* protothread state variables pt1 and pt2, which hold the state of
//* the two protothreads.
//*/
//
///*
//* Then we schedule the two protothreads by repeatedly calling their
//* protothread functions and passing a pointer to the protothread
//* state variables as arguments.
//*/

int impl_hit(int x, int y, int s) {
  if (!started_audio) {
    started_audio = 1;
    qwqz_audio_play(qwqz_audio);
  }
  
  if (s == 0) {
    cpBody *body = bodies[0];
    cpVect jump = cpv(0.0, gChipmunkJumpPower);
    cpBodyApplyImpulseAtLocalPoint(body, jump, cpv(0, 0));
    if (gChipmunkPlayerSpeed < 2000.0) {
      gChipmunkPlayerSpeed += 100.0;
    }
  }
  
  return 0;
}

int impl_draw(int b) {
  qwqz_tick_timer(&qwqz_engine->m_Timers[0]);
  if (qwqz_engine->m_Timers[0].step < 0.1) {
    float dx = ((-gChipmunkPlayerSpeed * qwqz_engine->m_Timers[0].step));

    // physics tick
    cpSpaceStep(space, qwqz_engine->m_Timers[0].step);
    // background tick
    for (int a=0; a<bg_range; a++) {
      bgsScroll[a] += dx;
    }
  } else {
    cpSpaceStep(space, 0.0); //tick the phsyics to warm up the JS optimizer
  }

  if (bgsScroll[bg_first] <= -((float)(bg_range / 2) * bgsSkeleton->data->bones[1]->length * 4.0)) {
    bgsScroll[bg_first] = floor(bgsScroll[bg_last] + (bgsSkeleton->data->bones[1]->length * 4.0) - 4.0);
    bg_first++;
    bg_last++;
    if (bg_first > (bg_range - 1)) {
      bg_first = 0;
    }
    if (bg_last > (bg_range - 1)) {
      bg_last = 0;
    }
  }

  bgsSkeleton->root->scaleX = 1.0;
  bgsSkeleton->root->scaleY = 1.0;

  skeleton->root->scaleX = 1.0;
  skeleton->root->scaleY = 1.0;
  
  //glClear(GL_COLOR_BUFFER_BIT);

    spSkeleton_updateWorldTransform(bgsSkeleton);

    qwqz_batch_clear(&qwqz_engine->m_Batches[0]);
    qwqz_engine->m_Batches[0].m_NeedsAttribs = 1;
    qwqz_batch_prepare(qwqz_engine, &qwqz_engine->m_Batches[0], &qwqz_engine->m_Linkages[0]);

    for (int a=0; a<bg_range; a++) {
      for (int c=0; c<num_bg; c++) {
        spSlot *s = bgsSkeleton->drawOrder[c];
        spRegionAttachment *ra = (spRegionAttachment *)s->attachment;
        if (s->attachment && s->attachment->type == SP_ATTACHMENT_REGION) {
          float offX = (bgsScroll[a]);
          spRegionAttachment_computeWorldVertices(ra, offX, 0.0, s->bone, verticeBuffer1);
          qwqz_batch_add(&qwqz_engine->m_Batches[0], 0, verticeBuffer1, NULL, ra->uvs);
        }
      }
    }
    
    glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
    qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[0]);

    qwqz_batch_clear(&qwqz_engine->m_Batches[1]);
    qwqz_engine->m_Batches[1].m_NeedsAttribs = 1;
    qwqz_batch_prepare(qwqz_engine, &qwqz_engine->m_Batches[1], &qwqz_engine->m_Linkages[1]);

    for (int i=0; i<skeleton->slotCount; i++) {
      spSlot *s = skeleton->drawOrder[i];
      spRegionAttachment *ra = (spRegionAttachment *)s->attachment;
      if (s->attachment->type == SP_ATTACHMENT_REGION) {
        float rr = DEGREES_TO_RADIANS(s->bone->worldRotation);
        float x = s->bone->worldX + ((cosf(rr) * ra->x) - (sinf(rr) * ra->y));
        float y = s->bone->worldY + ((sinf(rr) * ra->x) + (cosf(rr) * ra->y));

        cpBody *body = bodies[i];
        cpVect bodyOff = cpBodyGetPosition(body);
        cpVect newVel = cpBodyGetVelocity(body);
        if (newVel.y < 0.0) {
          gChipmunkPlayerSpeed -= 10.0;
        }

        if (gChipmunkPlayerSpeed < 0.0) {
          gChipmunkPlayerSpeed = 0.0;
        }

        //LOGV("foo %f\n", bodyOff.y);
        if (bodyOff.y <= 68.9 && counting_on_ground == 0) {
          counting_on_ground = 1;
          //LOGV("hit ground\n");
          qwqz_audio_play(qwqz_audio);
        }

        if (bodyOff.y <= 69.0 && counting_on_ground == 1) {
          //LOGV("counting\n");
          time_on_ground += qwqz_engine->m_Timers[0].step;
        }

        if (bodyOff.y > 72.0 && counting_on_ground == 1) {
          counting_on_ground = 0;
          //LOGV("got airborne\n");
        }

        if (bodyOff.y > 1024.0) {
          //counting_on_ground = 0;
          bodyOff.y = 1024.0;
          //LOGV("got airborne\n");
        }
        
        if (time_on_ground >= 4.0) {
          //time_on_ground = 0.0;
          //qwqz_audio_play(qwqz_audio);

          //vel limit
          //float velocity_limit = 1;
          //float velocity_mag = cpvlength(newVel);
          //if (velocity_mag > velocity_limit) {
          //  float velocity_scale = velocity_limit / velocity_mag;
          //  newVel = cpvmult(newVel, velocity_scale < 0.00011 ? 0.00011: velocity_scale);
          //}
          //cpBodySetVelocity(body, cpv(0.0, -newVel.y));
          //cpBodySetPosition(body, cpv(bodyOff.x, 100.0));
        }

        bodyOff.x = 0.0;

        cpBodySetPosition(body, bodyOff); //cpv(0.0, bodyOff.y));

        float r = cpBodyGetAngle(body);
        ra->rotation = RADIANS_TO_DEGREES(r);
        if (abs(ra->rotation) > 90) {
          cpBodySetAngle(body, 0);
        };
        spRegionAttachment_updateOffset(ra);

        spRegionAttachment_computeWorldVertices(ra, (bodyOff.x) - x, (bodyOff.y) - y, s->bone, verticeBuffer);
        qwqz_batch_add(&qwqz_engine->m_Batches[1], 0, verticeBuffer, NULL, ra->uvs);
      }
    }

    glUseProgram(qwqz_engine->m_Linkages[1].m_Program);
    qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[1]);

  if (gRenderPhysicsDebug) {
    // Draw the renderer contents and reset it back to the last tick's state.

    ChipmunkDebugDrawClearRenderer();

    glUniform2f(ChipmunkDebugDrawPushRenderer(), qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);

    ChipmunkDemoDefaultDrawImpl(space);

    ChipmunkDebugDrawFlushRenderer();
    ChipmunkDebugDrawPopRenderer();
  }

  qwqz_audio_fill(qwqz_audio);

  return 0;
}


int impl_resize(int width, int height, int ew, int eh, int u) {

  int resized = qwqz_resize(qwqz_engine, width, height, ew, eh, u);

  for (int i=0; i<2; i++) {
    glUseProgram(qwqz_engine->m_Linkages[i].m_Program);
    glUniform2f(qwqz_engine->m_Linkages[i].g_ResolutionUniform, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);
    qwqz_linkage_resize(qwqz_engine, &qwqz_engine->m_Linkages[i]);
  }
 
  if (gRenderPhysicsDebug) {
    ChipmunkDebugDrawResizeRenderer(width, height);
  }

  return resized;
}


int impl_main(int argc, char** argv, GLuint b) {
  num_bg = 0;
  bg_range = 2;
  bg_first = 0;
  bg_last = (bg_range - 1);

  qwqz_engine = qwqz_create();

  GLuint v = 0;
  GLuint f2 = 0;
  GLuint program = 0;
  GLuint program2 = 0;

  if (gRenderPhysicsDebug) {
    ChipmunkDebugDrawInit();
  }

  space = cpSpaceNew();
  cpSpaceSetGravity(space, cpv(0, gChipmunkGravity));
  cpSpaceSetIterations(space, 4);
  //cpSpaceSetDamping(space, 0.99);
  //cpSpaceSetCollisionSlop(space, 1.0);

  cpBody *body = 0;
  cpBody *staticBody = cpSpaceGetStaticBody(space);

  //foor
  cpShape *shape;
  shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-256, -48.0), cpv(256, -48.0), 48.0 * 2.0));
  cpShapeSetElasticity(shape, gChipmunkGroundElasticity);
  cpShapeSetFriction(shape, 0.0f);
  cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

  // Add lots of boxes.
  for(int i=0; i<0; i++) {
    for(int j=0; j<=i; j++) {
      float m = 1.0;
      body = cpSpaceAddBody(space, cpBodyNew(m, cpMomentForBox(m, 30.0f, 30.0f)));

      cpBodySetPosition(body, cpv(j*43 - i*16, 600 + i*64));
        
      shape = cpSpaceAddShape(space, cpBoxShapeNew(body, 30.0f, 30.0f, 0.0f));
      cpShapeSetElasticity(shape, 0.0f);
      cpShapeSetFriction(shape, 0.0f);
    }
  }

  qwqz_engine->m_Timers = (struct qwqz_timer_t *)malloc(sizeof(struct qwqz_timer_t) * 1);
  qwqz_timer_init(&qwqz_engine->m_Timers[0]);

  qwqz_engine->m_Linkages = (struct qwqz_linkage_t *)malloc(sizeof(struct qwqz_linkage_t) * 2);
  qwqz_engine->m_Batches = (struct qwqz_batch_t *)malloc(sizeof(struct qwqz_batch_t) * 2);

  spAtlas* atlas = spAtlas_createFromFile("assets/spine/player.atlas", NULL);
  spSkeletonJson* json = spSkeletonJson_create(atlas);
  spSkeletonData *skeletonData = spSkeletonJson_readSkeletonDataFile(json, "assets/spine/player.json");
  assert(skeletonData);
  skeleton = spSkeleton_create(skeletonData);
  stateData = spAnimationStateData_create(skeletonData);
  state = spAnimationState_create(stateData);
  spAnimationState_setAnimationByName(state, 0, "default", 1);

  spAtlas *atlas2 = spAtlas_createFromFile("assets/spine/background.atlas", NULL);
  spSkeletonJson *json2 = spSkeletonJson_create(atlas2);
  spSkeletonData *skeletonData2 = spSkeletonJson_readSkeletonDataFile(json2, "assets/spine/background.json");
  bgsSkeleton = spSkeleton_create(skeletonData2);
  bgsStateData = spAnimationStateData_create(skeletonData2);
  bgsState = spAnimationState_create(bgsStateData);
  //spAnimationState_setAnimationByName(bgsState, 0, "default", 1);

  v = qwqz_compile(GL_VERTEX_SHADER, "assets/shaders/spine_bone_texture_quad.vsh");
  f2 = qwqz_compile(GL_FRAGMENT_SHADER, "assets/shaders/filledquad.fsh");

  if (v && f2) {
    program = glCreateProgram();
    glAttachShader(program, v);
    glAttachShader(program, f2);
    qwqz_linkage_init(program, &qwqz_engine->m_Linkages[0]);

    program2 = glCreateProgram();
    glAttachShader(program2, v);
    glAttachShader(program2, f2);
    qwqz_linkage_init(program2, &qwqz_engine->m_Linkages[1]);
  }

  qwqz_batch_init(&qwqz_engine->m_Batches[0], &qwqz_engine->m_Linkages[0], (bgsSkeleton->slotCount * bg_range));

  num_bg = bgsSkeleton->slotCount;
  
  for (int i=0; i<bg_range; i++) {
    float f = (float)(i - (i / 2)) * (bgsSkeleton->data->bones[1]->length * 4.0); // * bg_scale));
    bgsScroll[i] = f;
  }

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
      float y = s->bone->worldY + ((sinf(rr) * ra->x) + (cosf(rr) * ra->y)) + 300.0;

      cpBody *body;
      float m = gChipmunkPlayerMass;
      body = cpSpaceAddBody(space, cpBodyNew(m, cpMomentForBox(m, ra->width * ra->scaleX * 1.0, ra->height * ra->scaleY * 1.0)));
      bodies[i] = body;

      cpBodySetAngle(body, r);
      cpBodySetPosition(body, cpv(x, y));

      shape = cpSpaceAddShape(space, cpBoxShapeNew(body, ra->width * ra->scaleX * 1.0, ra->height * ra->scaleY * 1.0, 0.0f));
      cpShapeSetElasticity(shape, gChipmunkPlayerElasticity);
      cpShapeSetFriction(shape, 0.0f);
    }
  }

  glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
  int bgsRegionRenderObject = (int)((spAtlasRegion *)((spRegionAttachment *)bgsSkeleton->drawOrder[1]->attachment)->rendererObject)->page->rendererObject; //TODO: fix this, fuck yea C
  glUniform1i(qwqz_engine->m_Linkages[0].g_TextureUniform, bgsRegionRenderObject); //TODO: this is the texture unit for spine background

  glUseProgram(qwqz_engine->m_Linkages[1].m_Program);
  int roboRegionRenderObject = (int)((spAtlasRegion *)((spRegionAttachment *)skeleton->drawOrder[0]->attachment)->rendererObject)->page->rendererObject; //TODO: fix this, fuck yea C
  glUniform1i(qwqz_engine->m_Linkages[1].g_TextureUniform, roboRegionRenderObject); //TODO: texture unit
  
  qwqz_batch_init(&qwqz_engine->m_Batches[1], &qwqz_engine->m_Linkages[1], (skeleton->slotCount));
  
  qwqz_engine->g_lastFrameBuffer = b;

  qwqz_engine->m_Zoom2 = 256.0;

  glActiveTexture(GL_TEXTURE0);

//  /* Initialize the protothread state variables with PT_INIT(). */
//  PT_INIT(&pt1);
//  PT_INIT(&pt2);
  
  qwqz_audio_bind_device();
  qwqz_audio = qwqz_create_audio_stream("assets/sounds/0.mod");
  
  return 0;
}
