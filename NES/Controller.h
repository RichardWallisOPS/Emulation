//
//  Controller.h
//  NES
//
//  Created by Richard Wallis on 21/12/2022.
//

#ifndef Controller_h
#define Controller_h

#import <Foundation/Foundation.h>

enum ControllerInfo
{
    // upper byte info flags
    ControllerInfo_Valid        = 1 << 31,
    ControllerInfo_Reserved_6   = 1 << 30,
    ControllerInfo_Reserved_5   = 1 << 29,
    ControllerInfo_Reserved_4   = 1 << 28,
    ControllerInfo_Reserved_3   = 1 << 27,
    ControllerInfo_Reserved_2   = 1 << 26,
    ControllerInfo_Reserved_1   = 1 << 25,
    ControllerInfo_Reserved_0   = 1 << 24,
    
    // Mask low 24 bits of controller data
    ControllerInfo_DataMask_24  = 0x00FFFFFF,
    // Mask low 16 bits of controller data
    ControllerInfo_DataMask_16  = 0x0000FFFF,
    // Mask low  8 bits of controller data
    ControllerInfo_DataMask_08  = 0x000000FF,
};

// Controller bits in max low 24 bits, upper 8 bits for info
@interface PlayerControllerManager : NSObject

// Controller bits in low byte
+ (uint32_t) controllerBitsForNESController:(uint8_t)port;

@end

#endif /* Controller_h */
