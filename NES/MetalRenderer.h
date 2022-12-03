//
//  MetalRenderer.h
//
//  Created by richardwallis on 15/11/2022.
//

#import "RenderDefs.h"
#import <Foundation/Foundation.h>

@interface EmulationMetalView : MTKView
@end

@interface MetalRenderer : NSObject <MTKViewDelegate>

// Device Access
@property (nonatomic,readonly) id<MTLDevice> CurrentDevice;

// Initialisation
- (instancetype) initWithView:(EmulationMetalView*)metalView;

// Global Access
+ (MetalRenderer*) GetRenderer;

@end
