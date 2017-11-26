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
#include <spine/GLUtils.h>
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
static spSkeleton* skeletonTop;
static spAnimationStateData* stateData;
static spAnimationState* state;
static spRegionAttachment* lastAttachment = NULL;
static float verticeBuffer[8];
static float uvBuffer[8];
static float bgsScroll[9] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
static cpBody **bodies;
static int jumped = 0;


static unsigned short quadTriangles[6] = {0, 1, 2, 2, 3, 0};
static spTwoColorBatcher* batcher = 0;
static spMesh* mesh = 0;
static spSkeletonClipping* _clipper;
static float* _worldVertices;

// A single vertex with UV 
typedef struct Vertex {
   // Position in x/y plane
   float x, y;

   // UV coordinates
   float u, v;

   // Color, each channel in the range from 0-1
   // (Should really be a 32-bit RGBA packed color)
   float r, g, b, a;
} Vertex;

/*
enum BlendMode {
   // See http://esotericsoftware.com/git/spine-runtimes/blob/spine-libgdx/spine-libgdx/src/com/esotericsoftware/spine/BlendMode.java#L37
   // for how these translate to OpenGL source/destination blend modes.
   BLEND_NORMAL,
   BLEND_ADDITIVE,
   BLEND_MULTIPLY,      
   BLEND_SCREEN
}
*/

#define MAX_VERTICES_PER_ATTACHMENT 2048
static float worldVerticesPositions[MAX_VERTICES_PER_ATTACHMENT];
static Vertex vertices[MAX_VERTICES_PER_ATTACHMENT];


// Little helper function to add a vertex to the scratch buffer. Index will be increased
// by one after a call to this function.
void addVertex(float x, float y, float u, float v, float r, float g, float b, float a, int* index) {
   Vertex* vertex = &vertices[*index];
   vertex->x = x;
   vertex->y = y;
   vertex->u = u;
   vertex->v = v;
   vertex->r = r;
   vertex->g = g;
   vertex->b = b;
   vertex->a = a;
   *index += 1;
}


int impl_draw(int b) {
  //qwqz_bind_frame_buffer(qwqz_engine, b);
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  qwqz_tick_timer(&qwqz_engine->m_Timers[0]);

  if (doPhysics) {
    //TODO: figure out better step code
    float phstep = qwqz_engine->m_Timers[0].step / 4.0;
    int p=0;
    for (p=0; p<4; p++) {
      cpSpaceStep(space, phstep);
    }
  }

  if (doPhysics) {
    // Draw the renderer contents and reset it back to the last tick's state.
    ChipmunkDebugDrawPushRenderer();
    ChipmunkDebugDrawClearRenderer();
    ChipmunkDemoDefaultDrawImpl(space);
    ChipmunkDebugDrawFlushRenderer();
    ChipmunkDebugDrawPopRenderer();
  }

  spAnimationState_update(state, qwqz_engine->m_Timers[0].step * 1.0);
  spAnimationState_apply(state, skeletonTop);
  spSkeleton_updateWorldTransform(skeletonTop);

  //glUniform1f(qwqz_engine->m_Linkages[0].g_TimeUniform, qwqz_engine->m_Timers[0].m_SimulationTime);

  //qwqz_batch_clear(&qwqz_engine->m_Batches[0]);
  //qwqz_engine->m_Batches[0].m_NeedsAttribs = 1;
  //qwqz_batch_prepare(qwqz_engine, &qwqz_engine->m_Batches[0], &qwqz_engine->m_Linkages[0]);

  for (int i=0; i<skeletonTop->slotsCount; i++) {
    spSlot *slot = skeletonTop->drawOrder[i];

    spAttachment* attachment = slot->attachment;
    if (!attachment) continue;

    // Fetch the blend mode from the slot and
    // translate it to the engine blend mode
    /*
    BlendMode engineBlendMode;
    switch (slot->data->blendMode) {
       case SP_BLEND_MODE_NORMAL:
          engineBlendMode = BLEND_NORMAL;
          break;
       case SP_BLEND_MODE_ADDITIVE:
          engineBlendMode = BLEND_ADDITIVE;
          break;
       case SP_BLEND_MODE_MULTIPLY:
          engineBlendMode = BLEND_MULTIPLY;
          break;
       case SP_BLEND_MODE_SCREEN:
          engineBlendMode = BLEND_SCREEN;
          break;
       default:
          // unknown Spine blend mode, fall back to
          // normal blend mode
          engineBlendMode = BLEND_NORMAL;
    }
    */

    // Calculate the tinting color based on the skeleton's color
    // and the slot's color. Each color channel is given in the
    // range [0-1], you may have to multiply by 255 and cast to
    // and int if your engine uses integer ranges for color channels.
    float tintR = 0; //skeletonTop->r * slot->r;
    float tintG = 0; //skeletonTop->g * slot->g;
    float tintB = 0; //skeletonTop->b * slot->b;
    float tintA = 0; //skeletonTop->a * slot->a;

    // Fill the vertices array depending on the type of attachment
    //Texture* texture = 0;
    int vertexIndex = 0;
    if (attachment->type == SP_ATTACHMENT_REGION) {
       // Cast to an spRegionAttachment so we can get the rendererObject
       // and compute the world vertices
       spRegionAttachment* regionAttachment = (spRegionAttachment*)attachment;

       // Our engine specific Texture is stored in the spAtlasRegion which was
       // assigned to the attachment on load. It represents the texture atlas
       // page that contains the image the region attachment is mapped to
       //texture = (Texture*)((spAtlasRegion*)regionAttachment->rendererObject)->page->rendererObject;

       // Computed the world vertices positions for the 4 vertices that make up
       // the rectangular region attachment. This assumes the world transform of the
       // bone to which the slot (and hence attachment) is attached has been calculated
       // before rendering via spSkeleton_updateWorldTransform
       spRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, worldVerticesPositions, 0, 2);

      //LOGV("%f\n", worldVerticesPositions[0]);

/*
       // Create 2 triangles, with 3 vertices each from the region's
       // world vertex positions and its UV coordinates (in the range [0-1]).
       addVertex(worldVerticesPositions[0], worldVerticesPositions[1],
              regionAttachment->uvs[0], regionAttachment->uvs[1],
              tintR, tintG, tintB, tintA, &vertexIndex);

       addVertex(worldVerticesPositions[2], worldVerticesPositions[3],
              regionAttachment->uvs[2], regionAttachment->uvs[3],
              tintR, tintG, tintB, tintA, &vertexIndex);

       addVertex(worldVerticesPositions[4], worldVerticesPositions[5],
              regionAttachment->uvs[4], regionAttachment->uvs[5],
              tintR, tintG, tintB, tintA, &vertexIndex);

       addVertex(worldVerticesPositions[4], worldVerticesPositions[5],
              regionAttachment->uvs[4], regionAttachment->uvs[5],
              tintR, tintG, tintB, tintA, &vertexIndex);

       addVertex(worldVerticesPositions[6], worldVerticesPositions[7],
              regionAttachment->uvs[6], regionAttachment->uvs[7],
              tintR, tintG, tintB, tintA, &vertexIndex);

       addVertex(worldVerticesPositions[0], worldVerticesPositions[1],
              regionAttachment->uvs[0], regionAttachment->uvs[1],
              tintR, tintG, tintB, tintA, &vertexIndex);
*/

            //// Draw the mesh we created for the attachment
            ////engine_drawMesh(vertices, 0, vertexIndex, texture, engineBlendMode);
            
            //qwqz_batch_add(&qwqz_engine->m_Batches[0], 0, worldVerticesPositions, NULL, regionAttachment->uvs);
    } else if (attachment->type == SP_ATTACHMENT_MESH) {
       // Cast to an spMeshAttachment so we can get the rendererObject
       // and compute the world vertices
       spMeshAttachment* mesh = (spMeshAttachment*)attachment;

       // Check the number of vertices in the mesh attachment. If it is bigger
       // than our scratch buffer, we don't render the mesh. We do this here
       // for simplicity, in production you want to reallocate the scratch buffer
       // to fit the mesh.
       if (mesh->super.worldVerticesLength > MAX_VERTICES_PER_ATTACHMENT) continue;

       // Our engine specific Texture is stored in the spAtlasRegion which was
       // assigned to the attachment on load. It represents the texture atlas
       // page that contains the image the mesh attachment is mapped to
       //texture = (Texture*)((spAtlasRegion*)mesh->rendererObject)->page->rendererObject;

       // Computed the world vertices positions for the vertices that make up
       // the mesh attachment. This assumes the world transform of the
       // bone to which the slot (and hence attachment) is attached has been calculated
       // before rendering via spSkeleton_updateWorldTransform
       spVertexAttachment_computeWorldVertices(SUPER(mesh), slot, 0, mesh->super.worldVerticesLength, worldVerticesPositions, 0, 2);

       // Mesh attachments use an array of vertices, and an array of indices to define which
       // 3 vertices make up each triangle. We loop through all triangle indices
       // and simply emit a vertex for each triangle's vertex.
       for (int i = 0; i < mesh->trianglesCount; ++i) {
          int index = mesh->triangles[i] << 1;
          //addVertex(worldVerticesPositions[index], worldVerticesPositions[index + 1],
          //       mesh->uvs[index], mesh->uvs[index + 1],
          //       tintR, tintG, tintB, tintA, &vertexIndex);
       
          //qwqz_batch_add(&qwqz_engine->m_Batches[0], 0, worldVerticesPositions, NULL, mesh->uvs);
       }
       //LOGV("%d \n", mesh->trianglesCount);
    }
  }

  //glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
  //qwqz_batch_render(qwqz_engine, &qwqz_engine->m_Batches[0]);

	float* uvs = 0;
	float* vertices = _worldVertices;
	int verticesCount = 0;
	unsigned short* triangles = 0;
	int trianglesCount = 0;
	//float r = 0, g = 0, b = 0, a = 0;
	//float dr = 0, dg = 0, db = 0, da = _premultipliedAlpha ? 1 : 0;
	for (int i = 0, n = skeletonTop->slotsCount; i < n; i++) {
		spSlot* slot = skeletonTop->drawOrder[i];
		if (!slot->attachment) continue;
		//CCTexture *texture = 0;
		switch (slot->attachment->type) {
		case SP_ATTACHMENT_REGION: {
			spRegionAttachment* attachment = (spRegionAttachment*)slot->attachment;
			spRegionAttachment_computeWorldVertices(attachment, slot->bone, vertices, 0, 2);
			//texture = [self getTextureForRegion:attachment];
			uvs = attachment->uvs;
			verticesCount = 8;
			triangles = quadTriangles;
			trianglesCount = 6;
			r = attachment->color.r;
			g = attachment->color.g;
			b = attachment->color.b;
			a = attachment->color.a;
			break;
		}
		case SP_ATTACHMENT_MESH: {
			spMeshAttachment* attachment = (spMeshAttachment*)slot->attachment;
			spVertexAttachment_computeWorldVertices(SUPER(attachment), slot, 0, attachment->super.worldVerticesLength, vertices, 0, 2);
			//texture = [self getTextureForMesh:attachment];
			uvs = attachment->uvs;
			verticesCount = attachment->super.worldVerticesLength;
			triangles = attachment->triangles;
			trianglesCount = attachment->trianglesCount;
			r = attachment->color.r;
			g = attachment->color.g;
			b = attachment->color.b;
			a = attachment->color.a;
			break;
		}
		case SP_ATTACHMENT_CLIPPING: {
			spClippingAttachment* clip = (spClippingAttachment*)slot->attachment;
			spSkeletonClipping_clipStart(_clipper, slot, clip);
		}
		default: ;
		}
/*	
		if (texture) {
			if (slot->data->blendMode != blendMode) {
				blendMode = slot->data->blendMode;
				switch (slot->data->blendMode) {
				case SP_BLEND_MODE_ADDITIVE:
					[self setBlendMode:[CCBlendMode addMode]];
					srcBlend = !_premultipliedAlpha ? GL_SRC_ALPHA : GL_ONE;
					dstBlend = GL_ONE;
					break;
				case SP_BLEND_MODE_MULTIPLY:
					[self setBlendMode:[CCBlendMode multiplyMode]];
					srcBlend = GL_DST_COLOR;
					dstBlend = GL_ONE_MINUS_SRC_ALPHA;
					break;
				case SP_BLEND_MODE_SCREEN:
					[self setBlendMode:screenMode];
					srcBlend = GL_ONE;
					dstBlend = GL_ONE_MINUS_SRC_COLOR;
					break;
				default:
					[self setBlendMode:_premultipliedAlpha ? [CCBlendMode premultipliedAlphaMode] : [CCBlendMode alphaMode]];
					srcBlend = !_premultipliedAlpha ? GL_SRC_ALPHA : GL_ONE;
					dstBlend = GL_ONE_MINUS_SRC_ALPHA;
				}
			}

			if (_premultipliedAlpha) {
				a *= skeletonTop->color.a * slot->color.a;
				r *= skeletonTop->color.r * slot->color.r * a;
				g *= skeletonTop->color.g * slot->color.g * a;
				b *= skeletonTop->color.b * slot->color.b * a;
			} else {
				a *= skeletonTop->color.a * slot->color.a;
				r *= skeletonTop->color.r * slot->color.r;
				g *= skeletonTop->color.g * slot->color.g;
				b *= skeletonTop->color.b * slot->color.b;
			}

			self.texture = texture;
			CGSize size = texture.contentSize;
			GLKVector2 center = GLKVector2Make(size.width / 2.0, size.height / 2.0);
			GLKVector2 extents = GLKVector2Make(size.width / 2.0, size.height / 2.0);
			if (_skipVisibilityCheck || CCRenderCheckVisbility(transform, center, extents)) {
			*/

				if (spSkeletonClipping_isClipping(_clipper)) {
					spSkeletonClipping_clipTriangles(_clipper, vertices, verticesCount, triangles, trianglesCount, uvs, 2);
					vertices = _clipper->clippedVertices->items;
					verticesCount = _clipper->clippedVertices->size;
					uvs = _clipper->clippedUVs->items;
					triangles = _clipper->clippedTriangles->items;
					trianglesCount = _clipper->clippedTriangles->size;
				}
				
				if (trianglesCount > 0) {
					if (!self.twoColorTint) {
						//CCRenderBuffer buffer = [renderer enqueueTriangles:(trianglesCount / 3) andVertexes:verticesCount withState:self.renderState globalSortOrder:0];
						for (int i = 0; i * 2 < verticesCount; ++i) {
							//CCVertex vertex;
              /*
							vertex.position = GLKVector4Make(vertices[i * 2], vertices[i * 2 + 1], 0.0, 1.0);
							vertex.color = GLKVector4Make(r, g, b, a);
							vertex.texCoord1 = GLKVector2Make(uvs[i * 2], 1 - uvs[i * 2 + 1]);
							if (_effect) {
								spColor light;
								spColor dark;
								light.r = r;
								light.g = g;
								light.b = b;
								light.a = a;
								dark.r = dark.g = dark.b = dark.a = 0;
								_effect->transform(_effect, &vertex.position.x, &vertex.position.y, &vertex.texCoord1.s, &vertex.texCoord1.t, &light, &dark);
								vertex.color.r = light.r;
								vertex.color.g = light.g;
								vertex.color.b = light.b;
								vertex.color.a = light.a;
							}
							CCRenderBufferSetVertex(buffer, i, CCVertexApplyTransform(vertex, transform));
              */
						}
						for (int j = 0; j * 3 < trianglesCount; ++j) {
							//CCRenderBufferSetTriangle(buffer, j, triangles[j * 3], triangles[j * 3 + 1], triangles[j * 3 + 2]);
						}
					} else {
            /*
						if (slot->darkColor) {
							dr = slot->darkColor->r;
							dg = slot->darkColor->g;
							db = slot->darkColor->b;
						} else {
							dr = dg = db = 0;
						}
            */
						
						spMeshPart meshPart;
						spMesh_allocatePart(mesh, &meshPart, verticesCount / 2, trianglesCount, self.texture.name, srcBlend, dstBlend);
						
						spVertex* verts = &meshPart.mesh->vertices[meshPart.startVertex];
						unsigned short* indices = &meshPart.mesh->indices[meshPart.startIndex];
					
            /*
						if (_effect) {
							spColor light;
							light.r = r;
							light.g = g;
							light.b = b;
							light.a = a;
							spColor dark;
							dark.r = dr;
							dark.g = dg;
							dark.b = db;
							dark.a = da;
							for (int i = 0; i * 2 < verticesCount; i++, verts++) {
								spColor lightCopy = light;
								spColor darkCopy = dark;
								
								CCVertex vertex;
								vertex.position = GLKVector4Make(vertices[i * 2], vertices[i * 2 + 1], 0.0, 1.0);
								verts->u = uvs[i * 2];
								verts->v = 1 - uvs[i * 2 + 1];
								_effect->transform(_effect, &vertex.position.x, &vertex.position.y, &verts->u, &verts->v, &lightCopy, &darkCopy);
								
								vertex = CCVertexApplyTransform(vertex, transform);
								verts->x = vertex.position.x;
								verts->y = vertex.position.y;
								verts->z = vertex.position.z;
								verts->w = vertex.position.w;
								verts->color = ((unsigned short)(lightCopy.r * 255))| ((unsigned short)(lightCopy.g * 255)) << 8 | ((unsigned short)(lightCopy.b * 255)) <<16 | ((unsigned short)(lightCopy.a * 255)) << 24;
								verts->color2 = ((unsigned short)(darkCopy.r * 255)) | ((unsigned short)(darkCopy.g * 255)) << 8 | ((unsigned short)(darkCopy.b * 255)) << 16 | ((unsigned short)(darkCopy.a * 255)) << 24;
								
							}
						} else {
							for (int i = 0; i * 2 < verticesCount; i++, verts++) {
								CCVertex vertex;
								vertex.position = GLKVector4Make(vertices[i * 2], vertices[i * 2 + 1], 0.0, 1.0);
								vertex = CCVertexApplyTransform(vertex, transform);
								verts->x = vertex.position.x;
								verts->y = vertex.position.y;
								verts->z = vertex.position.z;
								verts->w = vertex.position.w;
								verts->color = ((unsigned short)(r * 255))| ((unsigned short)(g * 255)) << 8 | ((unsigned short)(b * 255)) <<16 | ((unsigned short)(a * 255)) << 24;
								verts->color2 = ((unsigned short)(dr * 255)) | ((unsigned short)(dg * 255)) << 8 | ((unsigned short)(db * 255)) << 16 | ((unsigned short)(da * 255)) << 24;
								verts->u = uvs[i * 2];
								verts->v = 1 - uvs[i * 2 + 1];
							}
						}
						
						for (int j = 0; j < trianglesCount; j++, indices++) {
							*indices = triangles[j];
						}
					  
          */

					  spTwoColorBatcher_add(batcher, meshPart);
					}
				}
			}
		}
		spSkeletonClipping_clipEnd(_clipper, slot);
	}
	spSkeletonClipping_clipEnd2(_clipper);
	
	spTwoColorBatcher_flush(batcher);

  /*
	if (_debugSlots) {
		// Slots.
		CGPoint points[4];
		for (int i = 0, n = skeletonTop->slotsCount; i < n; i++) {
			spSlot* slot = skeletonTop->drawOrder[i];
			if (!slot->attachment || slot->attachment->type != SP_ATTACHMENT_REGION) continue;
			spRegionAttachment* attachment = (spRegionAttachment*)slot->attachment;
			spRegionAttachment_computeWorldVertices(attachment, slot->bone, _worldVertices, 0, 2);
			points[0] = ccp(_worldVertices[0], _worldVertices[1]);
			points[1] = ccp(_worldVertices[2], _worldVertices[3]);
			points[2] = ccp(_worldVertices[4], _worldVertices[5]);
			points[3] = ccp(_worldVertices[6], _worldVertices[7]);
			[_drawNode drawPolyWithVerts:points count:4 fillColor:[CCColor clearColor] borderWidth:1 borderColor:[CCColor blueColor]];
		}
	}
  */

  /*
	if (_debugBones) {
		// Bone lengths.
		for (int i = 0, n = skeletonTop->bonesCount; i < n; i++) {
			spBone *bone = skeletonTop->bones[i];
			float x = bone->data->length * bone->a + bone->worldX;
			float y = bone->data->length * bone->c + bone->worldY;
			[_drawNode drawSegmentFrom:ccp(bone->worldX, bone->worldY) to: ccp(x, y)radius:2 color:[CCColor redColor]];
		}
		
		// Bone origins.
		for (int i = 0, n = skeletonTop->bonesCount; i < n; i++) {
			spBone *bone = skeletonTop->bones[i];
			[_drawNode drawDot:ccp(bone->worldX, bone->worldY) radius:4 color:[CCColor greenColor]];
			if (i == 0) [_drawNode drawDot:ccp(bone->worldX, bone->worldY) radius:4 color:[CCColor blueColor]];
		}
	}
	
	if (_effect) _effect->end(_effect);

  */

	spMesh_clearParts(mesh);


  return 0;

}


int impl_resize(int width, int height, int ew, int eh, int u) {
  int resized = qwqz_resize(qwqz_engine, width, height, ew, eh, u);

  for (int i=0; i<qwqz_engine->m_LinkageCount; i++) {
    glUseProgram(qwqz_engine->m_Linkages[i].m_Program);
    glUniform2f(qwqz_engine->m_Linkages[i].g_ResolutionUniform, qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);
    qwqz_linkage_resize(qwqz_engine, &qwqz_engine->m_Linkages[i]);
  }

  spSkeleton_updateWorldTransform(skeletonTop);

  ChipmunkDebugDrawResizeRenderer(width, height);
  glUniform2f(ChipmunkDebugDrawPushRenderer(), qwqz_engine->m_ScreenWidth, qwqz_engine->m_ScreenHeight);
 
  return resized;
}


int impl_hit(int x, int y, int s) {
  //spAnimationState_addAnimationByName(state, 0, "jump", 0, 0); // trackIndex, name, loop, delay
  //if (rand() > (RAND_MAX / 2)) {
    //spAnimationState_setAnimationByName(state, 0, "walk", 1);
  //} else {

  if (2 == s) {
    //spAnimationState_clearTracks(state); //, 0, "shoot", 0, 0);
    //spAnimationState_clearTrack(state, 1); //, 0, "shoot", 0, 0);
    ////spAnimationState_addAnimationByName(state, 0, "walk", 0, 0.0);

    //spAnimationState_addAnimationByName(state, 1, "shoot", 0, 0.0);
    //spAnimationState_addAnimationByName(state, 1, "walk", 1, 0.0);

    //spAnimationState_addAnimationByName(state, 0, "shoot", 0);
    //spAnimationState_addAnimationByName(state, 1, "walk", 1);

    spAnimationState_setEmptyAnimation(state, 1, 0.66);
    spTrackEntry* te = spAnimationState_addAnimationByName(state, 1, "shoot", 0, 0.0);

    //te->mixDuration = 0.33;

    //spAnimationState_addEmptyAnimation(state, 1, 0.33, 0.0);

    te = spAnimationState_addAnimationByName(state, 1, "walk", 1, 0.33);
    te->mixDuration = 0.66;

    //spAnimationState_addAnimationByName(state, 1, "walk", 1, 0.0);
  }

  //}
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
    cpSpaceSetIterations(space, 4);
    cpSpaceSetCollisionSlop(space, 0.1);

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

    for(int i=0; i<5; i++) {
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

  spAtlas* atlas;

  if (doSpine) {
    {
      atlas = spAtlas_createFromFile("assets/spine/spineboy.atlas", GL_TEXTURE0 + 0);
      spSkeletonJson* json = spSkeletonJson_create(atlas);
      spSkeletonData *skeletonData = spSkeletonJson_readSkeletonDataFile(json, "assets/spine/spineboy.json");
      assert(skeletonData);
      skeletonTop = spSkeleton_create(skeletonData);
      stateData = spAnimationStateData_create(skeletonData);

      state = spAnimationState_create(stateData);

      spAnimationState_addAnimationByName(state, 0, "walk", 1, 0.0);
      spAnimationState_addAnimationByName(state, 1, "shoot", 1, 0.0);

	    spSkeletonJson_dispose(json);
    }

    spSkeleton* skeletonFoo = skeletonTop;

    //__asm__("int $3");

    qwqz_stack_shader_linkage(qwqz_engine,
      "assets/shaders/spine_bone_texture_quad.vsh",
      "assets/shaders/indexed_filled_quad.fsh");

    int totalVerts = 0;

    /*
    for (int i=0; i<skeletonTop->slotsCount; i++) {
      spSlot *slot = skeletonTop->drawOrder[i];

      spAttachment* attachment = slot->attachment;
      if (!attachment) continue;
      if (attachment->type == SP_ATTACHMENT_REGION) {
         spRegionAttachment* regionAttachment = (spRegionAttachment*)attachment;
         spRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, worldVerticesPositions, 0, 2);
         totalVerts += 1;
      } else if (attachment->type == SP_ATTACHMENT_MESH) {
         spMeshAttachment* mesh = (spMeshAttachment*)attachment;
         if (mesh->super.worldVerticesLength > MAX_VERTICES_PER_ATTACHMENT) continue;
         spVertexAttachment_computeWorldVertices(SUPER(mesh), slot, 0, mesh->super.worldVerticesLength, worldVerticesPositions, 0, 2);
         //totalVerts += mesh->trianglesCount;
         LOGV("%d \n", mesh->trianglesCount);
      }
    }
    */

    //LOGV("------ %d %d !!!!!\n",  skeletonTop->slotsCount, totalVerts);
    //qwqz_batch_init(&qwqz_engine->m_Batches[0], &qwqz_engine->m_Linkages[0], 11);

    //glUseProgram(qwqz_engine->m_Linkages[0].m_Program);
    //int roboRegionRenderObject = (int)atlas->rendererObject; //(int)((spAtlasRegion *)((spRegionAttachment *)skeleton->drawOrder[0]->attachment)->rendererObject)->page->rendererObject; //TODO: fix this, fuck yea C
    
    ////LOGV("wtf %d!!!!!!\n\n\n\n", roboRegionRenderObject);

    //glUniform1i(qwqz_engine->m_Linkages[0].g_TextureUniform, roboRegionRenderObject); //TODO: texture unit

    //g_TextureOffset = glGetUniformLocation(program, "iTextureOffset");
    //glUniform2f(g_TextureOffset, 0, 0);

    //qwqz_engine->g_lastFrameBuffer = b;
    //qwqz_engine->m_Zoom2 = 512.0 / 1.0;

    //glActiveTexture(GL_TEXTURE0);

    //spSkeleton_updateWorldTransform(skeletonTop);

    if (!batcher) {
      batcher = spTwoColorBatcher_create();
      mesh = spMesh_create(64000, 32000);
    }
	
	  _worldVertices = MALLOC(float, 1000); // Max number of vertices per mesh.
  }

  return 0;

}
