/* Copyright (c) 2007 Scott Lembcke
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <limits.h>
#include <string.h>

/*
#include "GL/glew.h"
#include "GL/glfw.h"
*/

#include "chipmunk/chipmunk_private.h"
#include "ChipmunkDebugDraw.h"
#include "ChipmunkDemoShaderSupport.h"

float ChipmunkDebugDrawPointLineScale = 1.0f;
float ChipmunkDebugDrawOutlineWidth = 1.0f;

static GLuint ChipmunkDebugRenderProgram;

struct v2f {GLfloat x, y;};
static struct v2f v2f0 = {0.0f, 0.0f};

static GLfloat ProjectionMatrix2[16];
GLuint ModelViewProjectionMatrix_location2;
GLuint resolutionHackLocation;

static inline struct v2f
v2f(cpVect v)
{
	struct v2f v2 = {(GLfloat)v.x, (GLfloat)v.y};
	return v2;
}

struct ExtrudeVerts {cpVect offset, n;};
struct ExtrudeVerts *extrude;

typedef struct Vertex {struct v2f vertex, aa_coord; cpSpaceDebugColor fill_color, outline_color;} Vertex;
typedef struct Triangle {Vertex a, b, c;} Triangle;

//static GLuint vao = 0;
static GLuint vbo = 0;

void HACKidentity(GLfloat *m) {
  GLfloat t[16] = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0,
  };
  
  memcpy(m, t, sizeof(t));
}

void HACKtranslate(GLfloat *m, float tx, float ty, float tz) {
    ProjectionMatrix2[12] += (ProjectionMatrix2[0] * tx + ProjectionMatrix2[4] * ty + ProjectionMatrix2[8] * tz);
    ProjectionMatrix2[13] += (ProjectionMatrix2[1] * tx + ProjectionMatrix2[5] * ty + ProjectionMatrix2[9] * tz);
    ProjectionMatrix2[14] += (ProjectionMatrix2[2] * tx + ProjectionMatrix2[6] * ty + ProjectionMatrix2[10] * tz);
    ProjectionMatrix2[15] += (ProjectionMatrix2[3] * tx + ProjectionMatrix2[7] * ty + ProjectionMatrix2[11] * tz);
    glUniformMatrix4fv(ModelViewProjectionMatrix_location2, 1, GL_FALSE, ProjectionMatrix2);
}

void HACKortho(GLfloat *m, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat nearZ, GLfloat farZ) {
  
  GLfloat deltaX = right - left;
  GLfloat deltaY = top - bottom;
  GLfloat deltaZ = farZ - nearZ;
  
  GLfloat tmp[16];
  HACKidentity(tmp);
  
  if ((deltaX == 0) || (deltaY == 0) || (deltaZ == 0)) {
    LOGV("Invalid ortho\n");
    return;
  }
  
  tmp[0] = (2.0 / deltaX);
  tmp[12] = (-(right + left) / deltaX);
  tmp[5] = (2.0 / deltaY);
  tmp[13] = (-(top + bottom) / deltaY);
  tmp[10] = (-2.0 / deltaZ);
  tmp[14] = (-(nearZ + farZ) / deltaZ);
  
  memcpy(m, tmp, sizeof(tmp));
}

void
ChipmunkDebugDrawInit(void)
{
	size_t bytes = sizeof(struct ExtrudeVerts) * 64; //TODO: fix this hack
  extrude = (struct ExtrudeVerts *)malloc(bytes);

	// Setup the AA shader.
	GLint vshader = CompileShader(GL_VERTEX_SHADER, GLSL(
#if defined(GL_ES) || defined(EMSCRIPTEN) || TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
precision mediump float;
#endif
		attribute vec2 vertex;
		attribute vec2 aa_coord;
		attribute vec4 fill_color;
		attribute vec4 outline_color;
		
		varying vec2 v_aa_coord;
		varying vec4 v_fill_color;
		varying vec4 v_outline_color;
    uniform mat4 ModelViewProjectionMatrix;
    uniform vec2 iResolution;
		
		void main(void) {
			// TODO: get rid of the GL 2.x matrix bit eventually?
			//gl_Position = gl_ModelViewProjectionMatrix*vec4(vertex, 0.0, 1.0);
      //vec4 p = ModelViewProjectionMatrix * vec4(vertex, 0.0, 1.0);
      //p.y -= 1.0;
			//gl_Position = p;

  //float zoom = (256.0 / 512.0);
  float zoom = (iResolution.y / 512.0);
  vec4 p = ModelViewProjectionMatrix * vec4(vertex * zoom, 1.0, 1.0);
  p.y -= 1.0;
  gl_Position = p;
			
			v_fill_color = fill_color;
			v_outline_color = outline_color;
			v_aa_coord = aa_coord;
		}
	));
	
	GLint fshader = CompileShader(GL_FRAGMENT_SHADER, GLSL(
#if defined(GL_ES) || defined(EMSCRIPTEN) || TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
precision mediump float;
#endif
		uniform float u_outline_coef;
		
		varying vec2 v_aa_coord;
		varying vec4 v_fill_color;
		//const vec4 v_fill_color = vec4(0.0, 0.0, 0.0, 1.0);
		varying vec4 v_outline_color;
		
		float aa_step(float t1, float t2, float f)
		{
			//return step(t2, f);
			return smoothstep(t1, t2, f);
		}
		
		void main(void)
		{
			float l = length(v_aa_coord);
			
			// Different pixel size estimations are handy.
			//float fw = fwidth(l);
			//float fw = length(vec2(dFdx(l), dFdy(l)));
			//float fw = length(fwidth(v_aa_coord));
			float fw = 0.1;
      
			// Outline width threshold.
			float ow = 1.0 - fw;//*u_outline_coef;
			
			// Fill/outline color.
			float fo_step = aa_step(max(ow - fw, 0.0), ow, l);
			vec4 fo_color = mix(v_fill_color, v_outline_color, fo_step);
			
			// Use pre-multiplied alpha.
			float alpha = 0.5; //1.0 - aa_step(1.0 - fw, 1.0, l);
			gl_FragColor = fo_color*(fo_color.a*alpha);
			//gl_FragColor = vec4(vec3(l), 1);
		}
	));
	
	ChipmunkDebugRenderProgram = LinkProgram(vshader, fshader);
	//CHECK_GL_ERRORS();
	
	// Setu VBO and VAO.
	//glGenVertexArraysOES(1, &vao);
	//glBindVertexArrayOES(vao);
	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
  glUseProgram(ChipmunkDebugRenderProgram);
  
  glUniform1f(glGetUniformLocation(ChipmunkDebugRenderProgram, "u_outline_coef"), ChipmunkDebugDrawPointLineScale);

  SET_ATTRIBUTE(ChipmunkDebugRenderProgram, struct Vertex, vertex, GL_FLOAT);
  SET_ATTRIBUTE(ChipmunkDebugRenderProgram, struct Vertex, aa_coord, GL_FLOAT);
  SET_ATTRIBUTE(ChipmunkDebugRenderProgram, struct Vertex, fill_color, GL_FLOAT);
  SET_ATTRIBUTE(ChipmunkDebugRenderProgram, struct Vertex, outline_color, GL_FLOAT);

  //ChipmunkDebugDrawResizeRenderer(128.0, 128.0);

  ModelViewProjectionMatrix_location2 = glGetUniformLocation(ChipmunkDebugRenderProgram, "ModelViewProjectionMatrix");
  resolutionHackLocation = glGetUniformLocation(ChipmunkDebugRenderProgram, "iResolution");

  /*
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArrayOES(0);

	UNSET_ATTRIBUTE(program, struct Vertex, vertex, GL_FLOAT);
	UNSET_ATTRIBUTE(program, struct Vertex, aa_coord, GL_FLOAT);
	UNSET_ATTRIBUTE(program, struct Vertex, fill_color, GL_FLOAT);
	UNSET_ATTRIBUTE(program, struct Vertex, outline_color, GL_FLOAT);
  */
	
	//CHECK_GL_ERRORS();
}

#undef MAX // Defined on some systems
#define MAX(__a__, __b__) (__a__ > __b__ ? __a__ : __b__)

static GLsizei triangle_capacity = 0;
static GLsizei triangle_count = 0;
static Triangle *triangle_buffer = NULL;

static Triangle *PushTriangles(size_t count)
{
	if(triangle_count + count > triangle_capacity){
		triangle_capacity += MAX(triangle_capacity, count);
		triangle_buffer = (Triangle *)realloc(triangle_buffer, triangle_capacity*sizeof(Triangle));
	}
	
	Triangle *buffer = triangle_buffer + triangle_count;
	triangle_count += count;
	return buffer;
}


void ChipmunkDebugDrawCircle(cpVect pos, cpFloat angle, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor)
{
	Triangle *triangles = PushTriangles(2);
	
	cpFloat r = radius + 1.0f/ChipmunkDebugDrawPointLineScale;
	Vertex a = {{pos.x - r, pos.y - r}, {-1.0, -1.0}, fillColor, outlineColor};
	Vertex b = {{pos.x - r, pos.y + r}, {-1.0,  1.0}, fillColor, outlineColor};
	Vertex c = {{pos.x + r, pos.y + r}, { 1.0,  1.0}, fillColor, outlineColor};
	Vertex d = {{pos.x + r, pos.y - r}, { 1.0, -1.0}, fillColor, outlineColor};
	
	Triangle t0 = {a, b, c}; triangles[0] = t0;
	Triangle t1 = {a, c, d}; triangles[1] = t1;
	
	ChipmunkDebugDrawSegment(pos, cpvadd(pos, cpvmult(cpvforangle(angle), radius - ChipmunkDebugDrawPointLineScale*0.5f)), outlineColor);
}

void ChipmunkDebugDrawSegment(cpVect a, cpVect b, cpSpaceDebugColor color)
{
	ChipmunkDebugDrawFatSegment(a, b, 0.0f, color, color);
}

void ChipmunkDebugDrawFatSegment(cpVect a, cpVect b, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor)
{
	Triangle *triangles = PushTriangles(6);
	
	cpVect n = cpvnormalize(cpvrperp(cpvsub(b, a)));
	cpVect t = cpvrperp(n);
	
	cpFloat half = 1.0f/ChipmunkDebugDrawPointLineScale;
	cpFloat r = radius + half;
	if(r <= half){
		r = half;
		fillColor = outlineColor;
	}
	
	cpVect nw = (cpvmult(n, r));
	cpVect tw = (cpvmult(t, r));
	struct v2f v0 = v2f(cpvsub(b, cpvadd(nw, tw))); // { 1.0, -1.0}
	struct v2f v1 = v2f(cpvadd(b, cpvsub(nw, tw))); // { 1.0,  1.0}
	struct v2f v2 = v2f(cpvsub(b, nw)); // { 0.0, -1.0}
	struct v2f v3 = v2f(cpvadd(b, nw)); // { 0.0,  1.0}
	struct v2f v4 = v2f(cpvsub(a, nw)); // { 0.0, -1.0}
	struct v2f v5 = v2f(cpvadd(a, nw)); // { 0.0,  1.0}
	struct v2f v6 = v2f(cpvsub(a, cpvsub(nw, tw))); // {-1.0, -1.0}
	struct v2f v7 = v2f(cpvadd(a, cpvadd(nw, tw))); // {-1.0,  1.0}
	
	Triangle t0 = {{v0, { 1.0f, -1.0f}, fillColor, outlineColor}, {v1, { 1.0f,  1.0f}, fillColor, outlineColor}, {v2, { 0.0f, -1.0f}, fillColor, outlineColor}}; triangles[0] = t0;
	Triangle t1 = {{v3, { 0.0f,  1.0f}, fillColor, outlineColor}, {v1, { 1.0f,  1.0f}, fillColor, outlineColor}, {v2, { 0.0f, -1.0f}, fillColor, outlineColor}}; triangles[1] = t1;
	Triangle t2 = {{v3, { 0.0f,  1.0f}, fillColor, outlineColor}, {v4, { 0.0f, -1.0f}, fillColor, outlineColor}, {v2, { 0.0f, -1.0f}, fillColor, outlineColor}}; triangles[2] = t2;
	Triangle t3 = {{v3, { 0.0f,  1.0f}, fillColor, outlineColor}, {v4, { 0.0f, -1.0f}, fillColor, outlineColor}, {v5, { 0.0f,  1.0f}, fillColor, outlineColor}}; triangles[3] = t3;
	Triangle t4 = {{v6, {-1.0f, -1.0f}, fillColor, outlineColor}, {v4, { 0.0f, -1.0f}, fillColor, outlineColor}, {v5, { 0.0f,  1.0f}, fillColor, outlineColor}}; triangles[4] = t4;
	Triangle t5 = {{v6, {-1.0f, -1.0f}, fillColor, outlineColor}, {v7, {-1.0f,  1.0f}, fillColor, outlineColor}, {v5, { 0.0f,  1.0f}, fillColor, outlineColor}}; triangles[5] = t5;
}

extern cpVect ChipmunkDemoMouse;

void ChipmunkDebugDrawPolygon(int count, const cpVect *verts, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor)
{
	size_t bytes = sizeof(struct ExtrudeVerts)*count;
	//struct ExtrudeVerts *extrude = (struct ExtrudeVerts *)alloca(bytes);
	memset(extrude, 0, bytes);

  int i=0;
	for(i=0; i<count; i++){
		cpVect v0 = verts[(i-1+count)%count];
		cpVect v1 = verts[i];
		cpVect v2 = verts[(i+1)%count];
		
		cpVect n1 = cpvnormalize(cpvrperp(cpvsub(v1, v0)));
		cpVect n2 = cpvnormalize(cpvrperp(cpvsub(v2, v1)));
		
		cpVect offset = cpvmult(cpvadd(n1, n2), 1.0/(cpvdot(n1, n2) + 1.0f));
		struct ExtrudeVerts v = {offset, n2}; extrude[i] = v;
	}
	
//	Triangle *triangles = PushTriangles(6*count);
	Triangle *triangles = PushTriangles(5*count - 2);
	Triangle *cursor = triangles;
	
	cpFloat inset = -cpfmax(0.0f, 1.0f/ChipmunkDebugDrawPointLineScale - radius);
	for(i=0; i<count-2; i++){
		struct v2f v0 = v2f(cpvadd(verts[  0], cpvmult(extrude[  0].offset, inset)));
		struct v2f v1 = v2f(cpvadd(verts[i+1], cpvmult(extrude[i+1].offset, inset)));
		struct v2f v2 = v2f(cpvadd(verts[i+2], cpvmult(extrude[i+2].offset, inset)));
		
		Triangle t = {{v0, v2f0, fillColor, fillColor}, {v1, v2f0, fillColor, fillColor}, {v2, v2f0, fillColor, fillColor}}; *cursor++ = t;
	}
	
	cpFloat outset = 1.0f/ChipmunkDebugDrawPointLineScale + radius - inset;
  int j=0;
	for(i=0, j=count-1; i<count; j=i, i++){
		cpVect vA = verts[i];
		cpVect vB = verts[j];
		
		cpVect nA = extrude[i].n;
		cpVect nB = extrude[j].n;
		
		cpVect offsetA = extrude[i].offset;
		cpVect offsetB = extrude[j].offset;
		
		cpVect innerA = cpvadd(vA, cpvmult(offsetA, inset));
		cpVect innerB = cpvadd(vB, cpvmult(offsetB, inset));
		
		// Admittedly my variable naming sucks here...
		struct v2f inner0 = v2f(innerA);
		struct v2f inner1 = v2f(innerB);
		struct v2f outer0 = v2f(cpvadd(innerA, cpvmult(nB, outset)));
		struct v2f outer1 = v2f(cpvadd(innerB, cpvmult(nB, outset)));
		struct v2f outer2 = v2f(cpvadd(innerA, cpvmult(offsetA, outset)));
		struct v2f outer3 = v2f(cpvadd(innerA, cpvmult(nA, outset)));
		
		struct v2f n0 = v2f(nA);
		struct v2f n1 = v2f(nB);
		struct v2f offset0 = v2f(offsetA);
		
		Triangle t0 = {{inner0, v2f0, fillColor, outlineColor}, {inner1,    v2f0, fillColor, outlineColor}, {outer1,      n1, fillColor, outlineColor}}; *cursor++ = t0;
		Triangle t1 = {{inner0, v2f0, fillColor, outlineColor}, {outer0,      n1, fillColor, outlineColor}, {outer1,      n1, fillColor, outlineColor}}; *cursor++ = t1;
		Triangle t2 = {{inner0, v2f0, fillColor, outlineColor}, {outer0,      n1, fillColor, outlineColor}, {outer2, offset0, fillColor, outlineColor}}; *cursor++ = t2;
		Triangle t3 = {{inner0, v2f0, fillColor, outlineColor}, {outer2, offset0, fillColor, outlineColor}, {outer3,      n0, fillColor, outlineColor}}; *cursor++ = t3;
	}
}

void ChipmunkDebugDrawDot(cpFloat size, cpVect pos, cpSpaceDebugColor fillColor)
{
	Triangle *triangles = PushTriangles(2);
	
	float r = size*0.5f/ChipmunkDebugDrawPointLineScale;
	Vertex a = {{pos.x - r, pos.y - r}, {-1.0f, -1.0f}, fillColor, fillColor};
	Vertex b = {{pos.x - r, pos.y + r}, {-1.0f,  1.0f}, fillColor, fillColor};
	Vertex c = {{pos.x + r, pos.y + r}, { 1.0f,  1.0f}, fillColor, fillColor};
	Vertex d = {{pos.x + r, pos.y - r}, { 1.0f, -1.0f}, fillColor, fillColor};
	
	Triangle t0 = {a, b, c}; triangles[0] = t0;
	Triangle t1 = {a, c, d}; triangles[1] = t1;
}

void ChipmunkDebugDrawBB(cpBB bb, cpSpaceDebugColor color)
{
	cpVect verts[] = {
		cpv(bb.r, bb.b),
		cpv(bb.r, bb.t),
		cpv(bb.l, bb.t),
		cpv(bb.l, bb.b),
	};
	ChipmunkDebugDrawPolygon(4, verts, 0.0f, color, LAColor(0, 0));
}

void
ChipmunkDebugDrawFlushRenderer(void)
{
  //CHECK_GL_ERRORS();

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle)*triangle_count, triangle_buffer, GL_DYNAMIC_DRAW);

  glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle)*triangle_count, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Triangle)*triangle_count, triangle_buffer);


  SET_ATTRIBUTE(ChipmunkDebugRenderProgram, struct Vertex, vertex, GL_FLOAT);
  SET_ATTRIBUTE(ChipmunkDebugRenderProgram, struct Vertex, aa_coord, GL_FLOAT);
  SET_ATTRIBUTE(ChipmunkDebugRenderProgram, struct Vertex, fill_color, GL_FLOAT);
  SET_ATTRIBUTE(ChipmunkDebugRenderProgram, struct Vertex, outline_color, GL_FLOAT);
	
	//glBindVertexArrayOES(vao);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glDrawArrays(GL_TRIANGLES, 0, triangle_count*3);
	//glBindVertexArrayOES(0);
		
//	UNSET_ATTRIBUTE(program, struct Vertex, vertex, GL_FLOAT);
//	UNSET_ATTRIBUTE(program, struct Vertex, aa_coord, GL_FLOAT);
//	UNSET_ATTRIBUTE(program, struct Vertex, fill_color, GL_FLOAT);
//	UNSET_ATTRIBUTE(program, struct Vertex, outline_color, GL_FLOAT);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//CHECK_GL_ERRORS();
}

void
ChipmunkDebugDrawClearRenderer(void)
{
  //CHECK_GL_ERRORS();

	triangle_count = 0;
}

static int pushed_triangle_count = 0;
int
ChipmunkDebugDrawPushRenderer(void)
{
	pushed_triangle_count = triangle_count;

	glUseProgram(ChipmunkDebugRenderProgram);
  glUniformMatrix4fv(ModelViewProjectionMatrix_location2, 1, GL_FALSE, ProjectionMatrix2);

  return resolutionHackLocation;
}

void
ChipmunkDebugDrawPopRenderer(void)
{
	triangle_count = pushed_triangle_count;
}

void ChipmunkDebugDrawResizeRenderer(float w, float h) {
  float m_ScreenHalfHeight = h / 2.0;
  //float m_ScreenHalfWidth = w / 2.0;
  float m_ScreenAspect = w / h;

  float m_Zoom2 = 1.0;
  float a = (-m_ScreenHalfHeight * m_ScreenAspect) * m_Zoom2;
  float b = (m_ScreenHalfHeight * m_ScreenAspect) * m_Zoom2;
  float c = (-m_ScreenHalfHeight) * m_Zoom2;
  float d = m_ScreenHalfHeight * m_Zoom2;
  float ee = 10.0;
  float ff = -10.0;

  HACKidentity(ProjectionMatrix2);
  HACKortho(ProjectionMatrix2, (a), (b), (c), (d), (ee), (ff));
}
