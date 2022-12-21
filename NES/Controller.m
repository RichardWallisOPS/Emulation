//
//  Controller.m
//  NES
//
//  Created by Richard Wallis on 21/12/2022.
//


#import "Controller.h"
#import "GameController/GameController.h"

@implementation PlayerControllerManager

// See SystemNES.h
+ (uint32_t) controllerBitsForNESController:(uint8_t)port
{
    uint32_t result = 0;
    
    NSArray<GCController*>* connectedControllers = [GCController controllers];
    if(connectedControllers != nil && port < connectedControllers.count)
    {
        GCController* controller = connectedControllers[port];
        if(controller != nil)
        {
            GCExtendedGamepad* gamePad = controller.extendedGamepad;
            if(gamePad != nil)
            {
                GCControllerDirectionPad* dpad = gamePad.dpad;
                if(dpad != nil)
                {
                    if(dpad.left.pressed)   result |= 1 << 6;
                    if(dpad.right.pressed)  result |= 1 << 7;
                    if(dpad.up.pressed)     result |= 1 << 4;
                    if(dpad.down.pressed)   result |= 1 << 5;
                }
                
                GCControllerDirectionPad* leftThumb = gamePad.leftThumbstick;
                if(leftThumb != nil)
                {
                    const float deadZone = 0.5f;
                    
                    if(leftThumb.left.value > deadZone)   result |= 1 << 6;
                    if(leftThumb.right.value > deadZone)  result |= 1 << 7;
                    if(leftThumb.up.value > deadZone)     result |= 1 << 4;
                    if(leftThumb.down.value > deadZone)   result |= 1 << 5;
                }
                
                if(gamePad.buttonMenu.pressed)      result |= 1 << 3;
                if(gamePad.buttonOptions.pressed)   result |= 1 << 2;
                
                if(gamePad.buttonB.pressed)         result |= 1 << 0;
                if(gamePad.buttonA.pressed)         result |= 1 << 0;
                
                if(gamePad.buttonY.pressed)         result |= 1 << 1;
                if(gamePad.buttonX.pressed)         result |= 1 << 1;
            }
            
            result |= ControllerInfo_Valid;
        }
    }
    
    return result;
}

@end
