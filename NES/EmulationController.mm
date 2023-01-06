///
//  EmulationController.m
//
//  Created by richardwallis on 15/11/2022.
//

#import "EmulationController.h"
#import "Controller.h"
#import "RenderDefs.h"
#include "SystemNES.h"
#include "Serialise.h"
#import "AVFAudio/AVFAudio.h"

// The console we are emulating
SystemNES g_NESConsole;

// History and rewind support
int             m_emulationDirection = 1;
const int       m_kRewindFlashFrames = 8;
int             m_rewindCounter = 0;
size_t          m_archiveIndex = 0;
size_t          m_rewindStartIndex = 0;
const size_t    m_kArchiveCount = 5 * 60;
Archive         m_ArchiveBuffer[m_kArchiveCount];

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
    else if(event.keyCode == 125)   // down - save into file
    {
        NSProcessInfo* process = [NSProcessInfo processInfo];
        NSArray* arguments = [process arguments];
            
        if(arguments.count > 1)
        {
            Archive archive(ArchiveMode_Persistent);
            g_NESConsole.Save(archive);
            NSString* cartPath = arguments[1];
            NSString* savePath = [cartPath stringByAppendingString:@".SAVE"];
            archive.Save([savePath cStringUsingEncoding:NSUTF8StringEncoding]);
        }
    }
    else if(event.keyCode == 126) // up - load from file
    {
        NSProcessInfo* process = [NSProcessInfo processInfo];
        NSArray* arguments = [process arguments];
            
        if(arguments.count > 1)
        {
            Archive archive(ArchiveMode_Persistent);
            NSString* cartPath = arguments[1];
            NSString* loadPath = [cartPath stringByAppendingString:@".SAVE"];
            
            if(archive.Load([loadPath cStringUsingEncoding:NSUTF8StringEncoding]))
            {
                if(archive.ByteCount() > 0)
                {
                    g_NESConsole.Load(archive);
                    
                    // Clear history on load
                    m_emulationDirection = 1;
                    m_archiveIndex = 0;
                    m_rewindStartIndex = 0;
                    for(size_t i = 0;i < m_kArchiveCount;++i)
                    {
                        m_ArchiveBuffer[i].Reset();
                    }
                }
            }
        }
    }
    else if(event.keyCode == 124) // right
    {

    }
    else if(event.keyCode == 123) // left - go back in time
    {
        if(m_emulationDirection > 0)
        {
            m_emulationDirection = -1;
            m_rewindCounter = m_kRewindFlashFrames;
            m_rewindStartIndex = m_archiveIndex;
        }
    }
    else if(event.keyCode == 49) // space
    {

    }
    else if(event.keyCode == 36) // enter - swap keyboard for player 1<->2
    {
        m_keyboardController[m_keyboardPort] = 0;
        m_keyboardPort = (m_keyboardPort + 1) % 2;
        m_keyboardController[m_keyboardPort] = 0;
    }
    else if(event.keyCode == 53) // esc - console reset button
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
    else if(event.keyCode == 124)
    {
    
    }
    else if(event.keyCode == 123)
    {
        m_emulationDirection = 1;
    }
}

@end

@interface EmulationController()

// Graphics
@property (nonatomic,readwrite) id<MTLDevice>       device;
@property (nonatomic,readwrite) id<MTLCommandQueue> cmdQueue;
@property (nonatomic,readwrite) id<MTLRenderPipelineState>  pipelineEmulationOutputToFrameBufferTexture;

// Audio
@property (nonatomic,readwrite) AVAudioEngine*      audioEngine;
@property (nonatomic,readwrite) AVAudioSourceNode*  audioSourceNode;

@end


@implementation EmulationController

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
        
        {
            // https://developer.apple.com/documentation/avfaudio/avaudioengine?language=objc
            // https://developer.apple.com/documentation/avfaudio/audio_engine/building_a_signal_generator
            // https://developer.apple.com/documentation/avfaudio/avaudiosourcenode?language=objc
            self.audioEngine = [AVAudioEngine new];
            
            AVAudioNode* outputNode = self.audioEngine.outputNode;
            AVAudioFormat* outputFormat = [outputNode inputFormatForBus:0];
            AVAudioFormat* inputFormat = [[AVAudioFormat alloc] initWithCommonFormat:outputFormat.commonFormat // CHECK: We want AVAudioPCMFormatFloat32
                                                                          sampleRate:outputFormat.sampleRate
                                                                            channels:1
                                                                         interleaved:outputFormat.isInterleaved];
                                                                         
                                                                         

            // TODO: Set or get buffer size so all systems know the amount of samples required and its not assumed.
            static float s_fPhase = 0.f;
            static float s_fFrequency = 440.f;
            static float s_fPhaseIncrement = ((2.0 * M_PI) / outputFormat.sampleRate) * s_fFrequency;
            self.audioSourceNode = [[AVAudioSourceNode alloc] initWithRenderBlock:^OSStatus(BOOL* pIsSilence, const AudioTimeStamp* pTimestamp, AVAudioFrameCount frameCount, AudioBufferList* pOutputData)
            {
                // TODO: linkup with APU
                if(pOutputData->mNumberBuffers > 0)
                {
                    AudioBuffer* pAudioBuffer = &pOutputData->mBuffers[0];
                    
                    // Buffer size and element count
                    //pAudioBuffer->mDataByteSize / sizeof(float);
                    
                    {
                        float* pFloatBuffer = (float*)pAudioBuffer->mData;
                        for(size_t frameIdx = 0;frameIdx < frameCount;++frameIdx)
                        {
                            pFloatBuffer[frameIdx] = sin(s_fPhase);
                            
                            s_fPhase += s_fPhaseIncrement;
                            if(s_fPhase > 2.0 * M_PI)
                            {
                                s_fPhase -= 2.0 * M_PI;
                            }
                        }
                    }
                }
                
                return 0;
            }];
            

            [self.audioEngine attachNode:self.audioSourceNode];
            [self.audioEngine connect:self.audioSourceNode to:self.audioEngine.mainMixerNode format:inputFormat];
            [self.audioEngine connect:self.audioEngine.mainMixerNode to:outputNode format:outputFormat];
            self.audioEngine.mainMixerNode.outputVolume = 0.5f;
            
            //BOOL bStart = [self.audioEngine startAndReturnError:nil];
            //NSLog(@"Starting AudioEngine = %d", bStart ? 1 : 0);
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

    // Rewind into archive history
    if(m_emulationDirection <= 0)
    {
        ssize_t tmpArchiveIndex = m_archiveIndex == 0 ? m_kArchiveCount - 1 : m_archiveIndex - 1;

        if(tmpArchiveIndex != m_rewindStartIndex)
        {
            m_archiveIndex = tmpArchiveIndex;
        }
        else
        {
            m_emulationDirection = 0;
        }

        Archive& current = m_ArchiveBuffer[m_archiveIndex];
        if(current.ByteCount() > 0)
        {
            current.ResetRead();
            g_NESConsole.Load(current);
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
    
    // HAck to show visual rewind feedback - write it directly into the output texture for now
    if(m_emulationDirection <= 0)
    {
         --m_rewindCounter;
        if(m_rewindCounter < -m_kRewindFlashFrames)
        {
            m_rewindCounter = m_kRewindFlashFrames;
        }
        
        if(m_rewindCounter > 0)
        {
            uint32_t hudColour = 0xFFFF0000;
            uint32_t* pFinalOutput = (uint32_t*)currentTextureBacking.contents;
            
            if(m_emulationDirection == 0)
            {
                // End of history - pause symbol ||
                pFinalOutput[ 10 * 256 + 10] = hudColour; pFinalOutput[ 10 * 256 + 15] = hudColour;
                pFinalOutput[ 10 * 256 + 11] = hudColour; pFinalOutput[ 10 * 256 + 16] = hudColour;
                pFinalOutput[ 11 * 256 + 10] = hudColour; pFinalOutput[ 11 * 256 + 15] = hudColour;
                pFinalOutput[ 11 * 256 + 11] = hudColour; pFinalOutput[ 11 * 256 + 16] = hudColour;
                pFinalOutput[ 12 * 256 + 10] = hudColour; pFinalOutput[ 12 * 256 + 15] = hudColour;
                pFinalOutput[ 12 * 256 + 11] = hudColour; pFinalOutput[ 12 * 256 + 16] = hudColour;
                pFinalOutput[ 13 * 256 + 10] = hudColour; pFinalOutput[ 13 * 256 + 15] = hudColour;
                pFinalOutput[ 13 * 256 + 11] = hudColour; pFinalOutput[ 13 * 256 + 16] = hudColour;
                pFinalOutput[ 14 * 256 + 10] = hudColour; pFinalOutput[ 14 * 256 + 15] = hudColour;
                pFinalOutput[ 14 * 256 + 11] = hudColour; pFinalOutput[ 14 * 256 + 16] = hudColour;
                pFinalOutput[ 15 * 256 + 10] = hudColour; pFinalOutput[ 15 * 256 + 15] = hudColour;
                pFinalOutput[ 15 * 256 + 11] = hudColour; pFinalOutput[ 15 * 256 + 16] = hudColour;
                pFinalOutput[ 16 * 256 + 10] = hudColour; pFinalOutput[ 16 * 256 + 15] = hudColour;
                pFinalOutput[ 16 * 256 + 11] = hudColour; pFinalOutput[ 16 * 256 + 16] = hudColour;
                pFinalOutput[ 17 * 256 + 10] = hudColour; pFinalOutput[ 17 * 256 + 15] = hudColour;
                pFinalOutput[ 17 * 256 + 11] = hudColour; pFinalOutput[ 17 * 256 + 16] = hudColour;
                pFinalOutput[ 18 * 256 + 10] = hudColour; pFinalOutput[ 18 * 256 + 15] = hudColour;
                pFinalOutput[ 18 * 256 + 11] = hudColour; pFinalOutput[ 18 * 256 + 16] = hudColour;
                pFinalOutput[ 19 * 256 + 10] = hudColour; pFinalOutput[ 19 * 256 + 15] = hudColour;
                pFinalOutput[ 19 * 256 + 11] = hudColour; pFinalOutput[ 19 * 256 + 16] = hudColour;
            }
            else
            {
                // Scanning through history - rewind symbol <<
                pFinalOutput[ 10 * 256 + 12] = hudColour; pFinalOutput[ 10 * 256 + 18] = hudColour;
                pFinalOutput[ 11 * 256 + 11] = hudColour; pFinalOutput[ 11 * 256 + 17] = hudColour;
                pFinalOutput[ 12 * 256 + 10] = hudColour; pFinalOutput[ 12 * 256 + 16] = hudColour;
                pFinalOutput[ 13 * 256 +  9] = hudColour; pFinalOutput[ 13 * 256 + 15] = hudColour;
                pFinalOutput[ 14 * 256 +  8] = hudColour; pFinalOutput[ 14 * 256 + 14] = hudColour;
                pFinalOutput[ 15 * 256 +  8] = hudColour; pFinalOutput[ 15 * 256 + 14] = hudColour;
                pFinalOutput[ 16 * 256 +  9] = hudColour; pFinalOutput[ 16 * 256 + 15] = hudColour;
                pFinalOutput[ 17 * 256 + 10] = hudColour; pFinalOutput[ 17 * 256 + 16] = hudColour;
                pFinalOutput[ 18 * 256 + 11] = hudColour; pFinalOutput[ 18 * 256 + 17] = hudColour;
                pFinalOutput[ 19 * 256 + 12] = hudColour; pFinalOutput[ 19 * 256 + 18] = hudColour;
                pFinalOutput[ 10 * 256 + 11] = hudColour; pFinalOutput[ 10 * 256 + 17] = hudColour;
                pFinalOutput[ 11 * 256 + 10] = hudColour; pFinalOutput[ 11 * 256 + 16] = hudColour;
                pFinalOutput[ 12 * 256 +  9] = hudColour; pFinalOutput[ 12 * 256 + 15] = hudColour;
                pFinalOutput[ 13 * 256 +  8] = hudColour; pFinalOutput[ 13 * 256 + 14] = hudColour;
                pFinalOutput[ 14 * 256 +  7] = hudColour; pFinalOutput[ 14 * 256 + 13] = hudColour;
                pFinalOutput[ 15 * 256 +  7] = hudColour; pFinalOutput[ 15 * 256 + 13] = hudColour;
                pFinalOutput[ 16 * 256 +  8] = hudColour; pFinalOutput[ 16 * 256 + 14] = hudColour;
                pFinalOutput[ 17 * 256 +  9] = hudColour; pFinalOutput[ 17 * 256 + 15] = hudColour;
                pFinalOutput[ 18 * 256 + 10] = hudColour; pFinalOutput[ 18 * 256 + 16] = hudColour;
                pFinalOutput[ 19 * 256 + 11] = hudColour; pFinalOutput[ 19 * 256 + 17] = hudColour;
            }
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
    
    if(m_emulationDirection > 0)
    {
        m_ArchiveBuffer[m_archiveIndex].Reset();
        g_NESConsole.Save(m_ArchiveBuffer[m_archiveIndex]);
        m_archiveIndex = (m_archiveIndex + 1) % m_kArchiveCount;
    }
}

@end
