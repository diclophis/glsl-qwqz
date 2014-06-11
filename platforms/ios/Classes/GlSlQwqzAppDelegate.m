//
//  GlSlQwqzAppDelegate.m
//  GlSlQwqz
//
//  Created by Jon Bardin on 9/7/09.
//  Copyright GPL
//


#import "GlSlQwqzAppDelegate.h"
#import "EAGLView.h"


@implementation GlSlQwqzAppDelegate


@synthesize window;
@synthesize glView;


-(BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
  return YES;
}


//- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
//	return YES;
//}


-(void)applicationWillResignActive:(UIApplication *)application {
    [glView stopAnimation];
}


-(void)applicationDidBecomeActive:(UIApplication *)application {
//  [[NSNotificationCenter defaultCenter] addObserver:self
//                                           selector:@selector(handleDidChangeStatusBarOrientationNotification:)
//                                               name:UIApplicationDidChangeStatusBarOrientationNotification
//                                             object:nil];
//
//  [glView setAutoresizingMask:UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight];
  
	if ([glView wasActive]) {
		[glView startAnimation];
	} else {
		//[glView startGame:[NSNumber numberWithInt:0]];
		[glView startAnimation];
	}
}

- (void)handleDidChangeStatusBarOrientationNotification:(NSNotification *)notification;
{
  // Do something interesting
  NSLog(@"The orientation is %f %f %d", [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height, [[notification.userInfo objectForKey: UIApplicationStatusBarOrientationUserInfoKey] intValue]);
  //[glView setCenter:[UIScreen mainScreen].bounds.origin];

  CGSize screenSize;
  CGSize screenSize2;
  
  if ([[UIApplication sharedApplication] statusBarOrientation] == UIDeviceOrientationPortrait || [[UIApplication sharedApplication] statusBarOrientation] == UIDeviceOrientationPortraitUpsideDown){
    screenSize = CGSizeMake ([UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height); //[UIScreen mainScreen].applicationFrame.size.width, [UIScreen mainScreen].applicationFrame.size.height);
    screenSize2 = CGSizeMake ([UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height); //[UIScreen mainScreen].applicationFrame.size.width, [UIScreen mainScreen].applicationFrame.size.height);

    //screenSize = glView.layer.frame.size;
  } else{
    screenSize = CGSizeMake ([UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height); //[UIScreen mainScreen].applicationFrame.size.width, [UIScreen mainScreen].applicationFrame.size.height);
    screenSize2 = CGSizeMake ([UIScreen mainScreen].bounds.size.height, [UIScreen mainScreen].bounds.size.width); //[UIScreen mainScreen].applicationFrame.size.height, [UIScreen mainScreen].applicationFrame.size.width);
    //CGSizeMake(20, 20);
    //screenSize = CGSizeMake (glView.layer.frame.size.height, glView.layer.frame.size.width); //[UIScreen mainScreen].applicationFrame.size.height, [UIScreen mainScreen].applicationFrame.size.width);
  }
  NSLog(@"%f, %f, %f, %f", screenSize.width, screenSize.height, screenSize.width, screenSize.height);

  //[glView resize:screenSize.width :screenSize.height :screenSize2.width :screenSize2.height];
  //glViewport(0, 0, screenSize.width, screenSize.height);
}


-(void)applicationWillTerminate:(UIApplication *)application {
	[glView stopAnimation];
}


-(void)dealloc {
    [window release];
    [glView release];	
    [super dealloc];
}


@end
