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
	if ([glView wasActive]) {
		[glView startAnimation];
	} else {
		[glView startGame:[NSNumber numberWithInt:0]];
		[glView startAnimation];
	}
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
