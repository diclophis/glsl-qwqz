//
//  sound.m
//  GlSlQwqz
//
//  Created by Cody Byrnes on 2/14/14.
//
//

#import "sound.h"

@implementation sound

-(void)applicationDidFinishLaunching:(UIApplication *)application{
    [player setVolume:1];
    NSURL *url = [NSURL fileURLWithPath:[NSString stringWithFormat:@"assets/music.mp3", [[NSBundle mainBundle] resourcePath]]];
    
    NSError *error;
    
    player = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error];
    player.numberOfLoops = 0;
    
    [player play];
    
}


@end
