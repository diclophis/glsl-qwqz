//
//  EAGLView.h
//
//  Created by Jon Bardin on 7/12/10.
//  Copyright GPL 2010. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>


// This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
// The view content is basically an EAGL surface you render your OpenGL scene into.
// Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
@interface EAGLView : UIView {    


@private

  // animating determines if the displaylink should fire off a draw call or not
  BOOL animating;
  
  // see :setAnimationFrameInterval for description
  NSInteger animationFrameInterval;
  
  // A CADisplayLink object is a timer object that allows your application to synchronize its drawing to the refresh rate of the display.
  CADisplayLink *displayLink;
  
  // An EAGLContext object manages the state information, commands, and resources needed to draw using OpenGL ES. All OpenGL ES commands are executed in relation to an EAGL context.
	EAGLContext *context;
  
  // glWorkingContext is the second context that is shared with the first context
  EAGLContext *glWorkingContext;

  // An EAGLSharegroup object manages OpenGL ES resources associated with one or more EAGLContext objects. It is created when an EAGLContext object is initialized and disposed of when the last EAGLContext object that references it is released. As an opaque object, there is no developer accessible API.
  EAGLSharegroup *glShareGroup;
    
  // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view
  // OpenGL renderbuffers are simple interfaces for drawing to destinations other than the buffers provided to the GL by the window-system.
  GLuint defaultFramebuffer, colorRenderbuffer, depthRenderbuffer;

}


@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;


-(void)startAnimation;
-(void)stopAnimation;
-(void)drawView:(id)sender;
-(void)startGame:(id)i;
-(BOOL)wasActive;


@end