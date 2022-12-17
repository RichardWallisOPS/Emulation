///
//  MetalRenderer.m
//
//  Created by richardwallis on 15/11/2022.
//

#import "MetalRenderer.h"
#import "RenderDefs.h"
#include "SystemNES.h"

// The console we are emulating
SystemNES g_NESConsole;
BOOL g_showDebugInfo = NO;

@implementation EmulationMetalView

- (BOOL) acceptsFirstResponder
{
    return YES;
}

- (void) keyDown:(NSEvent *)event
{
    if(event.keyCode == 26)
    {
        g_NESConsole.SetControllerButtonState(1, Controller_Start, true);
    }
    else if(event.keyCode == 22)
    {
        g_NESConsole.SetControllerButtonState(1, Controller_Select, true);
    }
    else if(event.keyCode == 31)
    {
        g_NESConsole.SetControllerButtonState(1, Controller_B, true);
    }
    else if(event.keyCode == 35)
    {
        g_NESConsole.SetControllerButtonState(1, Controller_A, true);
    }
    else if(event.keyCode == 2)
    {
        g_NESConsole.SetControllerButtonState(1, Controller_Right, true);
    }
    else if(event.keyCode == 0)
    {
        g_NESConsole.SetControllerButtonState(1, Controller_Left, true);
    }
        else if(event.keyCode == 1)
    {
    g_NESConsole.SetControllerButtonState(1, Controller_Down, true);
    }
    else if(event.keyCode == 13)
    {
        g_NESConsole.SetControllerButtonState(1, Controller_Up, true);
    }
    else if(event.keyCode == 126)   // up
    {

    }
    else if(event.keyCode == 125) // down
    {

    }
    else if(event.keyCode == 124) // right
    {

    }
    else if(event.keyCode == 123) // left
    {

    }
    else if(event.keyCode == 49) // space
    {

    }
    else if(event.keyCode == 36) // enter
    {
        g_showDebugInfo = !g_showDebugInfo;
    }
    else if(event.keyCode == 53) // esc
    {
         g_NESConsole.Reset();
    }
}

- (void) keyUp:(NSEvent *)event
{
    if(event.keyCode == 26)
    {
        g_NESConsole.SetControllerButtonState(1, Controller_Start, false);
    }
    else if(event.keyCode == 22)
    {
        g_NESConsole.SetControllerButtonState(1, Controller_Select, false);
    }
    else if(event.keyCode == 31)
    {
        g_NESConsole.SetControllerButtonState(1, Controller_B, false);
    }
    else if(event.keyCode == 35)
    {
        g_NESConsole.SetControllerButtonState(1, Controller_A, false);
    }
    else if(event.keyCode == 2)
    {
        g_NESConsole.SetControllerButtonState(1, Controller_Right, false);
    }
    else if(event.keyCode == 0)
    {
        g_NESConsole.SetControllerButtonState(1, Controller_Left, false);
    }
        else if(event.keyCode == 1)
    {
    g_NESConsole.SetControllerButtonState(1, Controller_Down, false);
    }
    else if(event.keyCode == 13)
    {
        g_NESConsole.SetControllerButtonState(1, Controller_Up, false);
    }
}

@end

@interface MetalRenderer()

@property (nonatomic,readwrite) id<MTLDevice>       device;
@property (nonatomic,readwrite) id<MTLCommandQueue> cmdQueue;
@property (nonatomic,readwrite) id<MTLRenderPipelineState> pipelineEmulationOutputToFrameBufferTexture;

@property (nonatomic,readwrite) id<MTLBuffer> emulationOutputData;
@property (nonatomic,readwrite) id<MTLTexture> emulationOutput;

@end
                            
static __weak MetalRenderer* theRenderer;

@implementation MetalRenderer

Vertex const g_quadVerts[] = {  {{-1.f,-1.f,0.f,1.f},   {0.f,1.f}},
                                {{1.f,1.f,0.f,1.f},     {1.f,0.f}},
                                {{-1.f,1.f,0.f,1.f},    {0.f,0.f}},
                                {{-1.f,-1.f,0.f,1.f},   {0.f,1.f}},
                                {{1.f,-1.f,0.f,1.f},    {1.f,1.f}},
                                {{1.f,1.f,0.f,1.f},     {1.f,0.f}} };

+ (MetalRenderer*) GetRenderer
{
    return theRenderer;
}

- (id<MTLDevice>) CurrentDevice
{
    return self.device;
}

- (instancetype) initWithView:(EmulationMetalView*)metalView
{
    self = [super init];
    
    if(self != nil)
    {
        theRenderer = self;
        
        if(self.device == nil)
        {
            self.device = MTLCreateSystemDefaultDevice();
        }

        MTLTextureDescriptor* outputTextureDesc = [MTLTextureDescriptor new];
        outputTextureDesc.width = 256;
        outputTextureDesc.height = 240;
        outputTextureDesc.pixelFormat = MTLPixelFormatBGRA8Unorm;
        outputTextureDesc.storageMode = MTLStorageModeManaged; //TODO change this for Apple Silicon
        
        size_t bufferBytes = outputTextureDesc.width * outputTextureDesc.height * 4;
        
        self.emulationOutputData = [self.device newBufferWithLength:bufferBytes options:MTLResourceStorageModeManaged];
        self.emulationOutput = [self.emulationOutputData newTextureWithDescriptor:outputTextureDesc offset:0 bytesPerRow: outputTextureDesc.width * 4];

        self.cmdQueue = [self.device newCommandQueue];
        
        //setup view
        metalView.device = self.device;
        metalView.clearColor = MTLClearColorMake(1.f,1.f,1.f,1.f);
        metalView.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
        metalView.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
        metalView.clearStencil = 0;
        
        id <MTLLibrary> library = [self.device newDefaultLibrary];
        
         MTLRenderPipelineDescriptor* pipelineEmulationOutputToFrameBufferTextureDesc = [MTLRenderPipelineDescriptor new];

        pipelineEmulationOutputToFrameBufferTextureDesc.vertexFunction = [library newFunctionWithName: @"vertPassThrough"];
        pipelineEmulationOutputToFrameBufferTextureDesc.fragmentFunction = [library newFunctionWithName:@"fragCopy"];
        pipelineEmulationOutputToFrameBufferTextureDesc.colorAttachments[0].pixelFormat = metalView.colorPixelFormat;
        pipelineEmulationOutputToFrameBufferTextureDesc.depthAttachmentPixelFormat = metalView.depthStencilPixelFormat;
        pipelineEmulationOutputToFrameBufferTextureDesc.stencilAttachmentPixelFormat = metalView.depthStencilPixelFormat;
    
        self.pipelineEmulationOutputToFrameBufferTexture = [self.device newRenderPipelineStateWithDescriptor:pipelineEmulationOutputToFrameBufferTextureDesc error:nil];
        
        {
            // TMP use passed in file for cart
            // TODO better file selection
            NSProcessInfo* process = [NSProcessInfo processInfo];
            NSArray* arguments = [process arguments];
            
            if(arguments.count > 1)
            {
                NSString* path = arguments[1];
                
                printf("Inserting cart: %s\n", [path cStringUsingEncoding:NSUTF8StringEncoding]);
                
                NSURL* gamePakURL = [[NSURL alloc] initFileURLWithPath:path isDirectory:NO];
                
                NSError* pError = nil;
                NSData* pakData = [NSData dataWithContentsOfURL:gamePakURL options:0 error:&pError];
                
                g_NESConsole.SetVideoOutputDataPtr((uint32_t*)self.emulationOutputData.contents);
                if(g_NESConsole.InsertCartridge(pakData.bytes, (uint32_t)pakData.length))
                {
                    g_NESConsole.PowerOn();
                }
            }
        }
    }
    
    return self;
}

- (void) mtkView:(MTKView*)view drawableSizeWillChange:(CGSize)size
{
    
}

- (void) drawInMTKView:(MTKView*)view
{
    // 1) Not a good way of doing this - move this tick logic into a different thread
    //  1.1) Maybe timed or maybe do the ticks as fast as possible?
    // 2) After the set number of ticks, waits on a conditional variables until the frame has been consumed
    //  2.1) This function signals the conditional variable for that thread to kick off again
    //  2.2) This function just then copies the frame data into this texture separately
    
    // 0) on drawInMTKView: called
    // 1) Make sure PPU frame generated from other thread
    // 2) Copy PPU frame data into this texture/buffer
    // 3) Signal SystemNES.Tick() thread to continue executation
    // 4) SystemNES.Tick() will generate next frame then wait for this consumer
    // 5) SystemNES.Tick() - maybe call all the ticks as fast as possible or time them
    //  5.1) Make this an option
    const size_t nNumPPUTicksPerFrame = 89342;
    for(size_t i = 0;i < nNumPPUTicksPerFrame;++i)
    {
        g_NESConsole.Tick();
    }
    
    if(g_showDebugInfo)
    {
        g_NESConsole.WritePPUMetaData();
    }
    
    if(self.emulationOutputData.storageMode == MTLStorageModeManaged)
    {
        [self.emulationOutputData didModifyRange:NSMakeRange(0, self.emulationOutputData.length)];
    }
    
    id<MTLCommandBuffer> cmdBuffer = [self.cmdQueue commandBuffer];
    
    {
        id<MTLRenderCommandEncoder> renderEncoder = [cmdBuffer renderCommandEncoderWithDescriptor:view.currentRenderPassDescriptor];
        {
            [renderEncoder setCullMode:MTLCullModeBack];
            [renderEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
            [renderEncoder setRenderPipelineState:self.pipelineEmulationOutputToFrameBufferTexture];
            [renderEncoder setVertexBytes:g_quadVerts length:sizeof(Vertex) * 6 atIndex:0];
            [renderEncoder setFragmentTexture:self.emulationOutput atIndex:0];
            [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:6];
        }
        [renderEncoder endEncoding];
    }
    [cmdBuffer presentDrawable:view.currentDrawable];
    [cmdBuffer commit];
}

@end
