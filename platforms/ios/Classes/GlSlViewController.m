//
//  GlSl.m
//  GlSlQwqz
//
//  Created by Jon Bardin on 2/25/14.
//
//

#import "GlSlViewController.h"

#include "libqwqz.h"

FILE *iosfopen(const char *filename, const char *mode) {
  NSString *fileString = [NSString stringWithCString:filename encoding:NSASCIIStringEncoding];
  NSString *documentsDirectory = [[NSBundle mainBundle] bundlePath];
  NSString *path = [documentsDirectory stringByAppendingPathComponent:fileString];
  
  const char *filePath = [path cStringUsingEncoding:NSASCIIStringEncoding];
  
  return fopen(filePath, mode);
}


@interface GlSlViewController () {
  int defaultFrameBuffer;
}

@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) GLKBaseEffect *effect;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation GlSlViewController

- (void)viewDidLoad
{
  [super viewDidLoad];
  
  self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
  
  if (!self.context) {
    NSLog(@"Failed to create ES context");
  }
  
  GLKView *view = (GLKView *)self.view;
  view.context = self.context;
  view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
  
  [self setupGL];
  
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFrameBuffer);
  
  impl_main(0, NULL, defaultFrameBuffer);
}

- (void)dealloc
{
  [self tearDownGL];
  
  if ([EAGLContext currentContext] == self.context) {
    [EAGLContext setCurrentContext:nil];
  }
  
  [super dealloc];
}

- (void)setupGL
{
  [EAGLContext setCurrentContext:self.context];

  
  //glEnable(GL_DEPTH_TEST);
  
  //glGenVertexArraysOES(1, &_vertexArray);
  //glBindVertexArrayOES(_vertexArray);
  
  //glGenBuffers(1, &_vertexBuffer);
  //glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
  //glBufferData(GL_ARRAY_BUFFER, sizeof(gCubeVertexData), gCubeVertexData, GL_STATIC_DRAW);
  
  //glEnableVertexAttribArray(GLKVertexAttribPosition);
  //glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(0));
  //glEnableVertexAttribArray(GLKVertexAttribNormal);
  //glVertexAttribPointer(GLKVertexAttribNormal, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(12));
  
  //glBindVertexArrayOES(0);
}

- (void)tearDownGL
{
  [EAGLContext setCurrentContext:self.context];
  
  //glDeleteBuffers(1, &_vertexBuffer);
  //glDeleteVertexArraysOES(1, &_vertexArray);
  
  //self.effect = nil;
  
  //if (_program) {
  //  glDeleteProgram(_program);
  //  _program = 0;
  //}
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
  //float aspect = fabsf(self.view.bounds.size.width / self.view.bounds.size.height);
  //GLKMatrix4 projectionMatrix = GLKMatrix4MakePerspective(GLKMathDegreesToRadians(65.0f), aspect, 0.1f, 100.0f);
  
  
  //self.effect.transform.projectionMatrix = projectionMatrix;
  
//  GLKMatrix4 baseModelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, -4.0f);
//  baseModelViewMatrix = GLKMatrix4Rotate(baseModelViewMatrix, _rotation, 0.0f, 1.0f, 0.0f);
//  
//  // Compute the model view matrix for the object rendered with GLKit
//  GLKMatrix4 modelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, -1.5f);
//  modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, _rotation, 1.0f, 1.0f, 1.0f);
//  modelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, modelViewMatrix);
//  
//  self.effect.transform.modelviewMatrix = modelViewMatrix;
//  
//  // Compute the model view matrix for the object rendered with ES2
//  modelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, 1.5f);
//  modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, _rotation, 1.0f, 1.0f, 1.0f);
//  modelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, modelViewMatrix);
//  
//  _normalMatrix = GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelViewMatrix), NULL);
//  
//  _modelViewProjectionMatrix = GLKMatrix4Multiply(projectionMatrix, modelViewMatrix);
//  
//  _rotation += self.timeSinceLastUpdate * 0.5f;
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
  //glClearColor(0.65f, 0.65f, 0.65f, 1.0f);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  //glBindVertexArrayOES(_vertexArray);
  
  // Render the object with GLKit
  //[self.effect prepareToDraw];
  
  //glDrawArrays(GL_TRIANGLES, 0, 36);
  
  // Render the object again with ES2
  //glUseProgram(_program);
  
  //glUniformMatrix4fv(uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX], 1, 0, _modelViewProjectionMatrix.m);
  //glUniformMatrix3fv(uniforms[UNIFORM_NORMAL_MATRIX], 1, 0, _normalMatrix.m);
  
  //glDrawArrays(GL_TRIANGLES, 0, 36);
  
  impl_resize(rect.size.width, rect.size.height);

  impl_draw(defaultFrameBuffer);

}

@end