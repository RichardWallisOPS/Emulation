///
//  MetalRenderer.m
//
//  Created by richardwallis on 15/11/2022.
//

#import "MetalRenderer.h"
#import "Controller.h"
#import "RenderDefs.h"
#include "SystemNES.h"

// The console we are emulating
SystemNES g_NESConsole;

@implementation EmulationMetalView

uint8_t m_keyboardPort = 0;
uint8_t m_keyboardController[2] = {0 , 0};

- (BOOL) acceptsFirstResponder
{
    return YES;
}

- (void) keyDown:(NSEvent *)event
{
    if(event.keyCode == 26)
    {
        m_keyboardController[m_keyboardPort] |= 1 << SystemNES::Controller_Start;
    }
    else if(event.keyCode == 22)
    {
        m_keyboardController[m_keyboardPort] |= 1 << SystemNES::Controller_Select;
    }
    else if(event.keyCode == 31)
    {
        m_keyboardController[m_keyboardPort] |= 1 << SystemNES::Controller_B;
    }
    else if(event.keyCode == 35)
    {
        m_keyboardController[m_keyboardPort] |= 1 << SystemNES::Controller_A;
    }
    else if(event.keyCode == 2)
    {
        m_keyboardController[m_keyboardPort] |= 1 << SystemNES::Controller_Right;
    }
    else if(event.keyCode == 0)
    {
        m_keyboardController[m_keyboardPort] |= 1 << SystemNES::Controller_Left;
    }
    else if(event.keyCode == 1)
    {
        m_keyboardController[m_keyboardPort] |= 1 << SystemNES::Controller_Down;
    }
    else if(event.keyCode == 13)
    {
        m_keyboardController[m_keyboardPort] |= 1 << SystemNES::Controller_Up;
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
        m_keyboardController[m_keyboardPort] = 0;
        m_keyboardPort = (m_keyboardPort + 1) % 2;
        m_keyboardController[m_keyboardPort] = 0;
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
        m_keyboardController[m_keyboardPort] &= ~(1 << SystemNES::Controller_Start);
    }
    else if(event.keyCode == 22)
    {
        m_keyboardController[m_keyboardPort] &= ~(1 << SystemNES::Controller_Select);
    }
    else if(event.keyCode == 31)
    {
        m_keyboardController[m_keyboardPort] &= ~(1 << SystemNES::Controller_B);
    }
    else if(event.keyCode == 35)
    {
        m_keyboardController[m_keyboardPort] &= ~(1 << SystemNES::Controller_A);
    }
    else if(event.keyCode == 2)
    {
        m_keyboardController[m_keyboardPort] &= ~(1 << SystemNES::Controller_Right);
    }
    else if(event.keyCode == 0)
    {
        m_keyboardController[m_keyboardPort] &= ~(1 << SystemNES::Controller_Left);
    }
    else if(event.keyCode == 1)
    {
        m_keyboardController[m_keyboardPort] &= ~(1 << SystemNES::Controller_Down);
    }
    else if(event.keyCode == 13)
    {
        m_keyboardController[m_keyboardPort] &= ~(1 << SystemNES::Controller_Up);
    }
}

@end

@interface MetalRenderer()

@property (nonatomic,readwrite) id<MTLDevice>       device;
@property (nonatomic,readwrite) id<MTLCommandQueue> cmdQueue;
@property (nonatomic,readwrite) id<MTLRenderPipelineState>  pipelineEmulationOutputToFrameBufferTexture;
@end


@implementation MetalRenderer

Vertex const g_quadVerts[] = {  {{-1.f,-1.f,0.f,1.f},   {0.f,1.f}},
                                {{1.f,1.f,0.f,1.f},     {1.f,0.f}},
                                {{-1.f,1.f,0.f,1.f},    {0.f,0.f}},
                                {{-1.f,-1.f,0.f,1.f},   {0.f,1.f}},
                                {{1.f,-1.f,0.f,1.f},    {1.f,1.f}},
                                {{1.f,1.f,0.f,1.f},     {1.f,0.f}} };

size_t          m_textureId = 0;
const size_t    m_renderTextureCount = 2;
id<MTLTexture>  m_emulationOutput[m_renderTextureCount];

- (id<MTLTexture>) nextTextureOutput
{
    m_textureId = (m_textureId + 1) % m_renderTextureCount;
    return m_emulationOutput[m_textureId];
}

- (BOOL) isAppleSilicon
{
	return [self.device supportsFamily:MTLGPUFamilyApple1];
}

- (instancetype) initWithView:(EmulationMetalView*)metalView
{
    self = [super init];
    
    if(self != nil)
    {
        if(self.device == nil)
        {
            self.device = MTLCreateSystemDefaultDevice();
        }
        
        bool const bAppleSilicon = [self isAppleSilicon];

        MTLTextureDescriptor* outputTextureDesc = [MTLTextureDescriptor new];
        outputTextureDesc.width = 256;
        outputTextureDesc.height = 240;
        outputTextureDesc.pixelFormat = MTLPixelFormatBGRA8Unorm;
        outputTextureDesc.storageMode = bAppleSilicon ? MTLStorageModeShared : MTLStorageModeManaged;
        
        size_t bufferBytes = outputTextureDesc.width * outputTextureDesc.height * 4;
        
        for(size_t idx = 0; idx < m_renderTextureCount; ++idx)
        {
            id<MTLBuffer> backingBuffer = [self.device newBufferWithLength:bufferBytes options: bAppleSilicon ? MTLResourceStorageModeShared : MTLResourceStorageModeManaged];
            m_emulationOutput[idx] = [backingBuffer newTextureWithDescriptor:outputTextureDesc offset:0 bytesPerRow: outputTextureDesc.width * 4];
        }

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
            // TODO better file selection in UI
            NSProcessInfo* process = [NSProcessInfo processInfo];
            NSArray* arguments = [process arguments];
            
            if(arguments.count > 1)
            {
                if(g_NESConsole.InsertCartridge([arguments[1] cStringUsingEncoding:NSUTF8StringEncoding]))
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
    id<MTLTexture> currentTexture = [self nextTextureOutput];
    id<MTLBuffer> currentTextureBacking = currentTexture.buffer;
    
    // Input update
    {
        for(uint32_t port = 0;port < 2;++port)
        {
            // Get controller
            uint32_t controllerBits = [PlayerControllerManager controllerBitsForNESController:port];
            
            // Mask out info and merge with keyboard
            uint8_t portBits = (controllerBits & ControllerInfo_DataMask_08) | m_keyboardController[port];
            
            // Set current controller instantious - upto game when it latches internally
            g_NESConsole.SetControllerBits(port, portBits);
        }
    }
    
    // Emulation ticks
    {
        g_NESConsole.SetVideoOutputDataPtr((uint32_t*)currentTextureBacking.contents);
        const size_t nNumPPUTicksPerFrame = 89342;
        for(size_t i = 0;i < nNumPPUTicksPerFrame;++i)
        {
            g_NESConsole.Tick();
        }
    }
    
    // Render
    {
        if(currentTextureBacking.storageMode == MTLStorageModeManaged)
        {
            [currentTextureBacking didModifyRange:NSMakeRange(0, currentTextureBacking.length)];
        }
        
        id<MTLCommandBuffer> cmdBuffer = [self.cmdQueue commandBuffer];
        
        {
            id<MTLRenderCommandEncoder> renderEncoder = [cmdBuffer renderCommandEncoderWithDescriptor:view.currentRenderPassDescriptor];
            {
                [renderEncoder setCullMode:MTLCullModeBack];
                [renderEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
                [renderEncoder setRenderPipelineState:self.pipelineEmulationOutputToFrameBufferTexture];
                [renderEncoder setVertexBytes:g_quadVerts length:sizeof(Vertex) * 6 atIndex:0];
                [renderEncoder setFragmentTexture:currentTexture atIndex:0];
                [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:6];
            }
            [renderEncoder endEncoding];
        }
        [cmdBuffer presentDrawable:view.currentDrawable];
        [cmdBuffer commit];
    }
}

@end
