//
//  Controller.h
//  NES
//
//  Created by Richard Wallis on 21/12/2022.
//

#ifndef Controller_h
#define Controller_h

#import <Foundation/Foundation.h>

@interface PlayerControllerManager : NSObject

+ (uint8_t) controllerBitsForNESController:(uint8_t)port;

@end

#endif /* Controller_h */
