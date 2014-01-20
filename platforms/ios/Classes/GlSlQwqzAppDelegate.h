//
//  GlSlQwqzAppDelegate.h
//  GlSlQwqz
//
//  Created by Jon Bardin on 9/7/09.
//  Copyright GPL
//

#import <UIKit/UIKit.h>


@class EAGLView;


@interface GlSlQwqzAppDelegate : NSObject <UIApplicationDelegate> {
  UIWindow *window;
  EAGLView *glView;
}


@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet EAGLView *glView;


@end