///
//  MetalRenderer.m
//  MetalTest
//
//  Created by richardwallis on 15/11/2016.
//  Copyright © 2016 openplanetsoftware. All rights reserved.
//

#import "MetalRenderer.h"
#import "RenderDefs.h"
#include "SystemNES.h"

@interface MetalRenderer()

@property (nonatomic,readwrite) id<MTLDevice>       device;
@property (nonatomic,readwrite) id<MTLCommandQueue> cmdQueue;
@property (nonatomic,readwrite) id<MTLRenderPipelineState> pipelineEmulationOutputToFrameBufferTexture;

@property (nonatomic,readwrite) id<MTLBuffer> emulationOutputData;
@property (nonatomic,readwrite) id<MTLTexture> emulationOutput;

@end

Vertex g_quadVerts[] = {    {{-1.f,-1.f,0.f,1.f},   {0.f,1.f}},
                            {{1.f,1.f,0.f,1.f},     {1.f,0.f}},
                            {{-1.f,1.f,0.f,1.f},    {0.f,0.f}},

                            {{-1.f,-1.f,0.f,1.f},   {0.f,1.f}},
                            {{1.f,-1.f,0.f,1.f},    {1.f,1.f}},
                            {{1.f,1.f,0.f,1.f},     {1.f,0.f}} };
                            
static __weak MetalRenderer* theRenderer;

SystemNES NESConsole;

@implementation MetalRenderer

+ (MetalRenderer*) GetRenderer
{
    return theRenderer;
}

- (id<MTLDevice>) CurrentDevice
{
    return self.device;
}

- (instancetype) initWithView:(MTKView*)metalView gamePakData:(NSData*)pakData
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
        
        if(NESConsole.InsertCartridge(pakData.bytes, (uint32_t)pakData.length))
        {
            NESConsole.PowerOn();
        }
        else
        {
            NSLog(@"Insert Cartridge failed!!!");
        }
    }
    
    return self;
}

- (void) mtkView:(MTKView*)view drawableSizeWillChange:(CGSize)size
{
    // TODO
}

- (void) drawInMTKView:(MTKView*)view
{
    // TODO Better timing
    const size_t nNumTicksPerFrame = 89341;
    for(size_t i = 0;i < nNumTicksPerFrame;++i)
    {
        NESConsole.Tick();
    }
    
    // TODO Test data only
    {
        // Write some test data
        uint32_t width = (uint32_t)self.emulationOutput.width;
        uint32_t height = (uint32_t)self.emulationOutput.height;
        uint32_t* data = (uint32_t*)self.emulationOutputData.contents;
        
        for(uint32_t y = 0;y < height;++y)
        {
            for(uint32 x = 0;x < width;++x)
            {
                uint32 pixelIndex = y * width + x;
                if(x % 2)
                {
                    // AARRGGBB
                    data[pixelIndex] = 0xffff0000;
                }
                else
                {
                    data[pixelIndex] = 0x00;
                }
            }
        }
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
