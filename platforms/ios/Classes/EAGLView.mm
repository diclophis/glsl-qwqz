//  Jon Bardin on 7/12/10 GPL


#import "EAGLView.h"


extern "C" {
  #include "opengles_bridge.h"
  #include "libqwqz.h"
}


FILE *iosfopen(const char *filename, const char *mode) {
  NSString *fileString = [NSString stringWithCString:filename encoding:NSASCIIStringEncoding];
  NSString *documentsDirectory = [[NSBundle mainBundle] bundlePath];
  NSString *path = [documentsDirectory stringByAppendingPathComponent:fileString];
  
  const char *filePath = [path cStringUsingEncoding:NSASCIIStringEncoding];
  
  return fopen(filePath, mode);
}


static GLuint g_LastFrameBuffer = -1;
static GLuint g_LastRenderBuffer = -1;


@implementation EAGLView


@synthesize animating;
@dynamic animationFrameInterval;


// You must implement this method
+ (Class)layerClass {
  return [CAEAGLLayer class];
}



-(id)initWithCoder:(NSCoder *)aDecoder {
  if ((self = [super initWithCoder:aDecoder])) {

    rotated = FALSE;
    animating = FALSE;
    animationFrameInterval = 1;
    displayLink = nil;
    implMainStatus = -1;
    
    // Get the layer
    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;

    eaglLayer.opaque = TRUE; //kEAGLColorFormatRGBA8
    eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGB565, kEAGLDrawablePropertyColorFormat, nil];
    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup:glShareGroup];
    
    if (!context || ![EAGLContext setCurrentContext:context]) {
      [self release];
      return nil;
    }
    
    glShareGroup = context.sharegroup;
    if (!glShareGroup)
    {
      NSLog(@"Could not get sharegroup from the main context");
      return nil;
    }
    
    glWorkingContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup:glShareGroup];
    
    // Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
    glGenFramebuffersOES(1, &defaultFramebuffer);
    glGenRenderbuffersOES(1, &colorRenderbuffer);
    g_LastFrameBuffer = defaultFramebuffer;
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, colorRenderbuffer);
    
    // The pixel dimensions of the CAEAGLLayer
    GLint backingWidth;
    GLint backingHeight;
    
    //Bind framebuffers to the context and this layer
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:eaglLayer];
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
    if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
      NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
      return nil;
    }

    NSString *reqSysVer = @"3.1";
    NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
    if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending) {
    } else {
      NSLog(@"display link required");
      return nil;
    }
  }
  return self;
}


-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	if (animating) {
		CGRect bounds;
		CGPoint location;
		bounds = [self bounds];
		for (UITouch *touch in touches) {			
			location = [touch locationInView:self];
			location.y = location.y;
      impl_hit(location.x, location.y, 0);
		}
	}
}


-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	if (animating) {
		CGRect bounds;
		CGPoint location;
		bounds = [self bounds];
		for (UITouch *touch in touches) {			
			location = [touch locationInView:self];
			location.y = location.y;
      impl_hit(location.x, location.y, 1);
		}
	}
}


-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	if (animating) {
		CGRect bounds;
		CGPoint location;
		bounds = [self bounds];
		for (UITouch *touch in touches) {			
			location = [touch locationInView:self];
			location.y = location.y;
      impl_hit(location.x, location.y, 2);
		}
	}
}


-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	if (animating) {
		NSSet *allTouches = [event allTouches];
		CGRect bounds;
		UITouch* touch;
		bounds = [self bounds];
		touch = [[allTouches allObjects] objectAtIndex:0];
		CGPoint location;
		location = [touch locationInView:self];
		location.y = location.y;
    impl_hit(location.x, location.y, -1);
	}
}


-(void)drawView:(id)sender {
  if (animating) {
    [EAGLContext setCurrentContext:context];

    impl_draw(defaultFramebuffer);
    
    if (g_LastRenderBuffer != colorRenderbuffer) {
      g_LastRenderBuffer = colorRenderbuffer;
      glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    }
    
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
  }
}


-(NSInteger)animationFrameInterval {
    return animationFrameInterval;
}


- (void)setAnimationFrameInterval:(NSInteger)frameInterval {
  // Frame interval defines how many display frames must pass between each time the
  // display link fires. The display link will only fire 30 times a second when the
  // frame internal is two on a display that refreshes 60 times a second. The default
  // frame interval setting of one will fire 60 times a second when the display refreshes
  // at 60 times a second. A frame interval setting of less than one results in undefined
  // behavior.
  if (frameInterval >= 1) {
    animationFrameInterval = frameInterval;
    if (animating) {
      [self stopAnimation];
      [self startAnimation];
    }
  }
}


-(void)startAnimation {
  if (!animating) {
    animating = TRUE;
    displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
    [displayLink setFrameInterval:animationFrameInterval];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
  }
}


-(void)stopAnimation {
  if (animating) {
    [displayLink invalidate];
    displayLink = nil;
    animating = FALSE;
  }
}


-(void)startGame:(id)i {
  implMainStatus = impl_main(0, NULL, defaultFramebuffer);
  [self resize:self.layer.frame.size.width :self.layer.frame.size.height :self.layer.frame.size.width :self.layer.frame.size.height];
}

-(void)layoutSubviews {
  if ([self wasActive]) {
    [self resize:self.layer.frame.size.height :self.layer.frame.size.width :self.layer.frame.size.width :self.layer.frame.size.height];
  }
}


-(void)resize:(int)w :(int)h :(int)ew :(int)eh {
  // The pixel dimensions of the CAEAGLLayer
  GLint backingWidth;
  GLint backingHeight;
  
  CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
  
  //Bind framebuffers to the context and this layer
  [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:eaglLayer];
  glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
  glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);

  impl_resize(backingWidth, backingHeight, ew, eh, defaultFramebuffer);

  rotated = !rotated;
}


-(void)dealloc {	
  if (defaultFramebuffer) {
    glDeleteFramebuffersOES(1, &defaultFramebuffer);
    defaultFramebuffer = 0;
  }
	
  if (colorRenderbuffer) {
    glDeleteRenderbuffersOES(1, &colorRenderbuffer);
    colorRenderbuffer = 0;
  }
	
	if(depthRenderbuffer) {
    glDeleteRenderbuffersOES(1, &depthRenderbuffer);
    depthRenderbuffer = 0;
  }
	
  if ([EAGLContext currentContext] == context) {
    [EAGLContext setCurrentContext:nil];
  }	
	
  [context release];
  context = nil;
	
  [super dealloc];
}


-(BOOL)wasActive {
  return (0 == implMainStatus);
}


@end
