//
//  MetalRenderer.h
//  MetalTest
//
//  Created by richardwallis on 15/11/2016.
//  Copyright © 2016 openplanetsoftware. All rights reserved.
//

#import "RenderDefs.h"
#import <Foundation/Foundation.h>

@interface MetalRenderer : NSObject <MTKViewDelegate>

// Device Access
@property (nonatomic,readonly) id<MTLDevice> CurrentDevice;

// Initialisation
- (instancetype) initWithView:(MTKView*)metalView;

// Global Access
+ (MetalRenderer*) GetRenderer;

@end
