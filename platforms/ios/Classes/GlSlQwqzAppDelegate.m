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
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(handleDidChangeStatusBarOrientationNotification:)
                                               name:UIApplicationDidChangeStatusBarOrientationNotification
                                             object:nil];
//
//  [glView setAutoresizingMask:UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight];
  
	if ([glView wasActive]) {
		[glView startAnimation];
	} else {
		[glView startGame:[NSNumber numberWithInt:0]];
		[glView startAnimation];
	}
}

- (void)handleDidChangeStatusBarOrientationNotification:(NSNotification *)notification;
{
  // Do something interesting
  //NSLog(@"The orientation is %f %f %d", [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height, [[notification.userInfo objectForKey: UIApplicationStatusBarOrientationUserInfoKey] intValue]);
  //[glView setCenter:[UIScreen mainScreen].bounds.origin];

  CGRect rotF;
  //  CGRect newF = [glView frame];
  if ([[notification.userInfo objectForKey: UIApplicationStatusBarOrientationUserInfoKey] intValue] == 1) {
//    NSLog(@"%f--", newF.size.width);
    rotF = CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height);
    //[glView setFrame:rotF];
  } else {
    rotF = CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height);
    //[glView setFrame:rotF];
  }
  
  //[glView resize:rotF.size.width :rotF.size.height];
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
