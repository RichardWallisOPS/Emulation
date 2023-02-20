//
//  EmulationController.h
//
//  Created by richardwallis on 15/11/2022.
//

#import "RenderDefs.h"
#import <Foundation/Foundation.h>

@interface EmulationMetalView : MTKView
@end

@interface EmulationController : NSObject <MTKViewDelegate, NSApplicationDelegate>
- (instancetype) initWithView:(EmulationMetalView*)metalView;
@end
