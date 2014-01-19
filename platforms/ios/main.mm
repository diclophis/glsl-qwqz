//
//  main.m
//  GlSlQwqz
//
//  Created by Jon Bardin on 9/7/09.
//  Copyright GPL
//

#import <UIKit/UIKit.h>
#import "GlSlQwqzAppDelegate.h"

int main(int argc, char *argv[]) {		
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int retVal = UIApplicationMain(argc, argv, nil, NSStringFromClass([GlSlQwqzAppDelegate class]));
    [pool release];
    return retVal;
}
