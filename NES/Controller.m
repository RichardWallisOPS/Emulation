//
//  Controller.m
//  NES
//
//  Created by Richard Wallis on 21/12/2022.
//

#import "Controller.h"
#import "GameController/GameController.h"
#include "CoreDefines.h"

@implementation PlayerControllerManager


+ (uint8_t) controllerBitsForNESController:(uint8_t)port
{
    uint8_t result = 0;
    
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
                    if(dpad.left.pressed)   result |= 1 << Controller_Left;
                    if(dpad.right.pressed)  result |= 1 << Controller_Right;
                    if(dpad.up.pressed)     result |= 1 << Controller_Up;
                    if(dpad.down.pressed)   result |= 1 << Controller_Down;
                }
                
                GCControllerDirectionPad* leftThumb = gamePad.leftThumbstick;
                if(leftThumb != nil)
                {
                    const float deadZone = 0.5f;
                    
                    if(leftThumb.left.value > deadZone)   result |= 1 << Controller_Left;
                    if(leftThumb.right.value > deadZone)  result |= 1 << Controller_Right;
                    if(leftThumb.up.value > deadZone)     result |= 1 << Controller_Up;
                    if(leftThumb.down.value > deadZone)   result |= 1 << Controller_Down;
                }
                
                if(gamePad.buttonMenu.pressed)      result |= 1 << Controller_Start;
                if(gamePad.buttonOptions.pressed)   result |= 1 << Controller_Select;
                
                if(gamePad.buttonB.pressed)         result |= 1 << Controller_A;
                if(gamePad.buttonA.pressed)         result |= 1 << Controller_A;
                
                if(gamePad.buttonY.pressed)         result |= 1 << Controller_B;
                if(gamePad.buttonX.pressed)         result |= 1 << Controller_B;
            }
        }
    }
    
    return result;
}

@end
