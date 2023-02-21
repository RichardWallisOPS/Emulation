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
#import <AudioToolbox/AudioToolbox.h>
#import "Foundation/Foundation.h"

// Global Constants
const size_t    kRenderTextureCount = 2;
const size_t    kArchiveCount       = 5 * 60;
const int32_t   kAudioBufferCount   = 8;
const float     kOutputMixerVolume  = 0.5;
const int       kRewindFlashFrames  = 8;
Vertex const    kQuadVerts[]        = {{{-1.f,-1.f,0.f,1.f},    {0.f,1.f}},
                                        {{1.f,1.f,0.f,1.f},     {1.f,0.f}},
                                        {{-1.f,1.f,0.f,1.f},    {0.f,0.f}},
                                        {{-1.f,-1.f,0.f,1.f},   {0.f,1.f}},
                                        {{1.f,-1.f,0.f,1.f},    {1.f,1.f}},
                                        {{1.f,1.f,0.f,1.f},     {1.f,0.f}}};

@interface EmulationController()
{
@private
    // Keyboard controller support
    uint8_t m_keyboardPort;
    uint8_t m_keyboardController[2];

    // Double buffering
    size_t          m_textureId;
    id<MTLTexture>  m_emulationVideoOut[kRenderTextureCount];
    
    // History and rewind support
    int             m_emulationDirection;
    int             m_rewindCounter;
    size_t          m_archiveIndex;
    size_t          m_rewindStartIndex;
    Archive         m_ArchiveBuffer[kArchiveCount];
    
    // Audio buffers
    bool                    m_allowAudio;
    std::atomic<bool>       m_audioSynced;
    std::atomic<int32_t>    m_readAudioBuffer;
    std::atomic<int32_t>    m_writeAudioBuffer;
    APUAudioBuffer          m_audioBuffers[kAudioBufferCount];
    
    // The console we are emulating
    SystemNES m_NESConsole;
}

// Graphics
@property (nonatomic,readwrite) id<MTLDevice>               device;
@property (nonatomic,readwrite) id<MTLCommandQueue>         cmdQueue;
@property (nonatomic,readwrite) id<MTLRenderPipelineState>  pipelineEmulationOutputToFrameBufferTexture;

// Audio
@property (nonatomic,readwrite) AVAudioEngine*      audioEngine;
@property (nonatomic,readwrite) AVAudioSourceNode*  audioSourceNode;

// Last loaded cart path
@property (nonatomic, readwrite) NSString*          cartLoadPath;

@end


@implementation EmulationController

- (void) gameControllerKeyboardButtonDown:(ControllerButton)button
{
    m_keyboardController[m_keyboardPort] |= 1 << button;
}

- (void) gameControllerKeyboardButtonUp:(ControllerButton)button
{
    m_keyboardController[m_keyboardPort] &= ~(1 << button);
}

- (void) gameControllerKeyboardSwapPort
{
    m_keyboardController[m_keyboardPort] = 0;
    m_keyboardPort = (m_keyboardPort + 1) % 2;
    m_keyboardController[m_keyboardPort] = 0;
}

- (void) clearHistory
{
    m_emulationDirection = 1;
    m_archiveIndex = 0;
    m_rewindStartIndex = 0;
    for(size_t i = 0;i < kArchiveCount;++i)
    {
        m_ArchiveBuffer[i].Reset();
    }
}

- (void) beginRewind
{
    if(m_emulationDirection > 0)
    {
        m_emulationDirection = -1;
        m_rewindCounter = kRewindFlashFrames;
        m_rewindStartIndex = m_archiveIndex;
    }
}

- (void) endRewind
{
    m_emulationDirection = 1;
}

- (void) loadConsoleStateFromArchive:(Archive*)pArchive
{
    m_NESConsole.Load(*pArchive);
}

- (void) saveConsoleStateToArchive:(Archive*)pArchive
{
    m_NESConsole.Save(*pArchive);
}

- (void) usrResetConsole
{
    m_NESConsole.Reset();
}

- (void) setCartLoadPath:(NSString*)cartLoadPath
{
    _cartLoadPath = cartLoadPath;
    [[[NSApplication sharedApplication] mainWindow] setTitle:cartLoadPath.lastPathComponent.stringByDeletingPathExtension];
}

- (id<MTLTexture>) nextTextureOutput
{
    m_textureId = (m_textureId + 1) % kRenderTextureCount;
    return m_emulationVideoOut[m_textureId];
}

- (BOOL) isAppleSilicon
{
	return [self.device supportsFamily:MTLGPUFamilyApple1];
}

- (void) stopAudio
{
    self.audioEngine.mainMixerNode.outputVolume = 0.f;
    [self.audioEngine pause];
    
    m_allowAudio = false;
    m_audioSynced = false;
    
    m_readAudioBuffer = 0;
    m_writeAudioBuffer = 0;
    
     for(size_t i = 0;i < kAudioBufferCount;++i)
    {
        m_audioBuffers[i].Reset();
    }
}

- (void) allowAudio
{
    m_allowAudio = true;
}

- (void) showOpenNewDialogue
{
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.25 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^
	{
		NSOpenPanel* panel = [NSOpenPanel openPanel];
		[panel beginWithCompletionHandler: ^(NSInteger result)
		{
			if(result == NSModalResponseOK)
			{
				NSURL* url = [[panel URLs] objectAtIndex:0];
				if(url != nil)
				{
					[self insertCartridge:url.path];
				}
			}
		}];
	});
}

- (BOOL) insertCartridge:(NSString*)cartPath
{
    [self stopAudio];
    [self clearHistory];
    
    BOOL bResult = NO;
	
	if([cartPath.pathExtension rangeOfString:@"nes" options:NSCaseInsensitiveSearch].length != 0)
	{
		if(m_NESConsole.InsertCartridge([cartPath cStringUsingEncoding:NSUTF8StringEncoding]))
		{
            bResult = YES;
            
			m_NESConsole.PowerOn();
            self.cartLoadPath = cartPath;
		}
	}
    else if([cartPath rangeOfString:@".nes.save" options:NSCaseInsensitiveSearch].length != 0)
    {
        Archive archive(ArchiveMode_Persistent);
        if(archive.Load([cartPath cStringUsingEncoding:NSUTF8StringEncoding]))
        {
            bResult = YES;
                
            m_NESConsole.Load(archive);
            self.cartLoadPath = [cartPath stringByDeletingPathExtension];
        }
    }
    
    [self allowAudio];
	
	return bResult;
}

- (void) applicationWillTerminate:(NSNotification*)NSNotification
{
    m_NESConsole.EjectCartridge();
}

- (instancetype) initWithView:(EmulationMetalView*)metalView
{
    self = [super init];
    
    if(self != nil)
    {
        // App delegate callbacks - want to know if there is a shutdown for saving Cartridge NVRAM
        [NSApplication sharedApplication].delegate = self;
    
        // IVar C++ core object setup
        {
            m_textureId = 0;
            m_emulationDirection = 1;
            m_rewindCounter = 0;
            m_archiveIndex = 0;
            m_rewindStartIndex = 0;
            m_allowAudio = false;
            m_audioSynced = false;
            m_readAudioBuffer = 0;
            m_writeAudioBuffer = 0;
            m_keyboardPort = 0;
            m_keyboardController[0] = m_keyboardController[1] = 0;
        }
    
        self.device = MTLCreateSystemDefaultDevice();
        self.cmdQueue = [self.device newCommandQueue];
        
		// Setup view
		{
			metalView.device = self.device;
			metalView.clearColor = MTLClearColorMake(1.f,1.f,1.f,1.f);
			metalView.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
		}
        
        // Setup render resources
        {
			bool const bAppleSilicon = [self isAppleSilicon];

			MTLTextureDescriptor* outputTextureDesc = [MTLTextureDescriptor new];
			outputTextureDesc.width = 256;
			outputTextureDesc.height = 240;
			outputTextureDesc.pixelFormat = MTLPixelFormatBGRA8Unorm;
			outputTextureDesc.storageMode = bAppleSilicon ? MTLStorageModeShared : MTLStorageModeManaged;
			
			size_t bufferBytes = outputTextureDesc.width * outputTextureDesc.height * 4;
			
			for(size_t idx = 0; idx < kRenderTextureCount; ++idx)
			{
				id<MTLBuffer> backingBuffer = [self.device newBufferWithLength:bufferBytes options: bAppleSilicon ? MTLResourceStorageModeShared : MTLResourceStorageModeManaged];
				m_emulationVideoOut[idx] = [backingBuffer newTextureWithDescriptor:outputTextureDesc offset:0 bytesPerRow: outputTextureDesc.width * 4];
			}

			id <MTLLibrary> library = [self.device newDefaultLibrary];
			
			MTLRenderPipelineDescriptor* pipelineEmulationOutputToFrameBufferTextureDesc = [MTLRenderPipelineDescriptor new];

			pipelineEmulationOutputToFrameBufferTextureDesc.vertexFunction = [library newFunctionWithName: @"vertPassThrough"];
			pipelineEmulationOutputToFrameBufferTextureDesc.fragmentFunction = [library newFunctionWithName:@"fragCopy_Monitor"];
			pipelineEmulationOutputToFrameBufferTextureDesc.colorAttachments[0].pixelFormat = metalView.colorPixelFormat;
			pipelineEmulationOutputToFrameBufferTextureDesc.depthAttachmentPixelFormat = metalView.depthStencilPixelFormat;
			pipelineEmulationOutputToFrameBufferTextureDesc.stencilAttachmentPixelFormat = metalView.depthStencilPixelFormat;
		
			self.pipelineEmulationOutputToFrameBufferTexture = [self.device newRenderPipelineStateWithDescriptor:pipelineEmulationOutputToFrameBufferTextureDesc error:nil];
		}
		
        // Setup audio
        {
            // https://developer.apple.com/documentation/avfaudio/avaudioengine?language=objc
            // https://developer.apple.com/documentation/avfaudio/audio_engine/building_a_signal_generator
            // https://developer.apple.com/documentation/avfaudio/avaudiosourcenode?language=objc
            
            self.audioEngine = [AVAudioEngine new];
            
            uint32_t buffserSize = 48000 / 60;
            AudioUnitSetProperty(self.audioEngine.inputNode.audioUnit, kAudioDevicePropertyBufferFrameSize, kAudioUnitScope_Global, 0, &buffserSize, sizeof(buffserSize));

            AVAudioNode* outputNode = self.audioEngine.outputNode;
            AVAudioFormat* outputFormat = [outputNode inputFormatForBus:0];
            AVAudioFormat* inputFormat = [[AVAudioFormat alloc] initWithCommonFormat:outputFormat.commonFormat  // TODO-CHECK: We want AVAudioPCMFormatFloat32
                                                                          sampleRate:outputFormat.sampleRate    // TODO-CHECK: We have 48000Hz
                                                                            channels:1
                                                                         interleaved:outputFormat.isInterleaved];
                                                                         
            self.audioSourceNode = [[AVAudioSourceNode alloc] initWithRenderBlock:^OSStatus(BOOL* pIsSilence, const AudioTimeStamp* pTimestamp, AVAudioFrameCount frameCount, AudioBufferList* pOutputData)
            {
                if(pOutputData->mNumberBuffers > 0)
                {
                    bool bOutputBufferWritten = false;

                    AudioBuffer* pOutputAudioBuffer = &pOutputData->mBuffers[0];
                    float* pOutputFloatBuffer = (float*)pOutputAudioBuffer->mData;
                                        
                    const bool bAudioSynced = self->m_audioSynced;
                    int32_t bufferIndexDiff = abs(self->m_writeAudioBuffer - self->m_readAudioBuffer);
                    
                    // Audio not currently synced - try to use optimal readbuffer vs writebuffer index before syncing
                    // Audio currently synced - try to keep going - as long as we have valid data
                    if (    (!bAudioSynced && bufferIndexDiff > ((kAudioBufferCount / 2) - 1)) ||
                            ( bAudioSynced && bufferIndexDiff > 0))
                    {
                        APUAudioBuffer const* pInputAudioBuffer = &self->m_audioBuffers[self->m_readAudioBuffer];
                        const size_t samplesWritten = pInputAudioBuffer->GetSamplesWritten();
                        
                        if(pInputAudioBuffer->IsReady() && samplesWritten == frameCount)
                        {
                            float const* pInputFloatBuffer = pInputAudioBuffer->GetSampleBuffer();
                            
                            if(pInputAudioBuffer->ShouldReverseBuffer())
                            {
                                for(size_t sampleIdx = 0;sampleIdx < samplesWritten;++sampleIdx)
                                {
                                    pOutputFloatBuffer[sampleIdx] = pInputFloatBuffer[samplesWritten - sampleIdx - 1];
                                }
                            }
                            else
                            {
                                memcpy(pOutputFloatBuffer, pInputFloatBuffer, samplesWritten * sizeof(float));
                            }

                            self->m_readAudioBuffer = (self->m_readAudioBuffer + 1) % kAudioBufferCount;
                            bOutputBufferWritten = true;
                        }
                    }

                    self->m_audioSynced = bOutputBufferWritten;
                    *pIsSilence = !bOutputBufferWritten;
                }
                return 0;
            }];

            [self.audioEngine attachNode:self.audioSourceNode];
            [self.audioEngine connect:self.audioSourceNode to:self.audioEngine.mainMixerNode format:inputFormat];
            [self.audioEngine connect:self.audioEngine.mainMixerNode to:outputNode format:outputFormat];
            self.audioEngine.mainMixerNode.outputVolume = 0.f;
        }
        
        // Try load cart data from the command line otherwise throw up a file picker automatically
        {
            BOOL bGameLoaded = false;
            
            // Command line check
            NSProcessInfo* process = [NSProcessInfo processInfo];
            NSArray* arguments = [process arguments];
            
            if(arguments.count > 1)
            {
                bGameLoaded = [self insertCartridge:arguments[1]];
            }
            
            // No game - show file picker
            if(!bGameLoaded)
            {
                [self showOpenNewDialogue];
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
            uint8_t controllerBits = [PlayerControllerManager controllerBitsForNESController:port];
            
            // Get keyboard controller emulation
            uint8_t keyboardBits = m_keyboardController[port];
            
            // Merge controller and keyboard
            uint8_t portBits = controllerBits | keyboardBits;
            
            // Set current controller instantious - upto game when it latches internally
            m_NESConsole.SetControllerBits(port, portBits);
        }
    }

    // Rewind into archive history
    if(m_emulationDirection <= 0)
    {
        ssize_t tmpArchiveIndex = m_archiveIndex == 0 ? kArchiveCount - 1 : m_archiveIndex - 1;

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
            m_NESConsole.Load(current);
        }
    }
    
    //Update Audio level - silence when paused - otherwise normal
    if(m_emulationDirection == 0 || !m_audioSynced)
    {
        if(self.audioEngine.mainMixerNode.outputVolume != 0.f)
        {
            self.audioEngine.mainMixerNode.outputVolume = 0.f;
        }
    }
    else if(m_allowAudio && m_audioSynced)
    {
        if(self.audioEngine.running && self.audioEngine.mainMixerNode.outputVolume <= 0.f)
        {
            self.audioEngine.mainMixerNode.outputVolume = kOutputMixerVolume;
        }
    }
    
    // Emulation ticks
    {
        // Set ouput buffers
        {
            // Video
            {
                m_NESConsole.SetVideoOutputDataPtr((uint32_t*)currentTextureBacking.contents);
            }
            
            // Audio
            {
                APUAudioBuffer* pCurrentAudioBuffer = &m_audioBuffers[m_writeAudioBuffer];
                                
                pCurrentAudioBuffer->SetShouldReverseBuffer(m_emulationDirection < 0);
                
                m_NESConsole.SetAudioOutputBuffer(pCurrentAudioBuffer);
                m_writeAudioBuffer = (m_writeAudioBuffer + 1) % kAudioBufferCount;
            }
        }
        
        // Tick emulation
        // 341 x 262 = [scanline time + hBlank time in scanline dots] X [scanline count + vBlank time in scanlines]
        const size_t kNumPPUTicksPerFrame = 89342;
        for(size_t i = 0;i < kNumPPUTicksPerFrame;++i)
        {
            m_NESConsole.Tick();
        }
        
        if(m_allowAudio && !self.audioEngine.isRunning)
        {
            self.audioEngine.mainMixerNode.outputVolume = 0.f;
            if(![self.audioEngine startAndReturnError:nil])
            {
                NSLog(@"Failed to start audio engine");
            }
        }
    }
    
    //Show visual rewind feedback - write it directly into the output texture for now
    if(m_emulationDirection <= 0)
    {
         --m_rewindCounter;
        if(m_rewindCounter < -kRewindFlashFrames)
        {
            m_rewindCounter = kRewindFlashFrames;
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
                [renderEncoder setVertexBytes:kQuadVerts length:sizeof(kQuadVerts) atIndex:0];
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
        m_NESConsole.Save(m_ArchiveBuffer[m_archiveIndex]);
        m_archiveIndex = (m_archiveIndex + 1) % kArchiveCount;
    }
}

@end

@implementation EmulationMetalView

- (BOOL) acceptsFirstResponder
{
    return YES;
}

- (void) keyDown:(NSEvent *)event
{
    EmulationController* emuController = (EmulationController*)self.delegate;
    
    if(!event.isARepeat)
    {
        if(event.keyCode == 26)
        {
            [emuController gameControllerKeyboardButtonDown:Controller_Start];
        }
        else if(event.keyCode == 22)
        {
            [emuController gameControllerKeyboardButtonDown:Controller_Select];
        }
        else if(event.keyCode == 31)
        {
            [emuController gameControllerKeyboardButtonDown:Controller_B];
        }
        else if(event.keyCode == 35)
        {
            [emuController gameControllerKeyboardButtonDown:Controller_A];
        }
        else if(event.keyCode == 2)
        {
            [emuController gameControllerKeyboardButtonDown:Controller_Right];
        }
        else if(event.keyCode == 0)
        {
            [emuController gameControllerKeyboardButtonDown:Controller_Left];
        }
        else if(event.keyCode == 1)
        {
            [emuController gameControllerKeyboardButtonDown:Controller_Down];
        }
        else if(event.keyCode == 13)
        {
            [emuController gameControllerKeyboardButtonDown:Controller_Up];
        }
        else if(event.keyCode == 125)   // down - save into file
        {
            if(emuController.cartLoadPath != nil)
            {
                [emuController stopAudio];
                 
                Archive archive(ArchiveMode_Persistent);
                [emuController saveConsoleStateToArchive:&archive];
                
                NSString* savePath = [emuController.cartLoadPath stringByAppendingString:@".SAVE"];
                archive.Save([savePath cStringUsingEncoding:NSUTF8StringEncoding]);
                
                [emuController allowAudio];
            }
        }
        else if(event.keyCode == 126) // up - load from file
        {
            if(emuController.cartLoadPath != nil)
            {
                [emuController stopAudio];
                 
                Archive archive(ArchiveMode_Persistent);
                NSString* loadPath = [emuController.cartLoadPath stringByAppendingString:@".SAVE"];
                
                if(archive.Load([loadPath cStringUsingEncoding:NSUTF8StringEncoding]))
                {
                    if(archive.ByteCount() > 0)
                    {
                        [emuController clearHistory];
                        [emuController loadConsoleStateFromArchive:&archive];
                    }
                }
                
                [emuController allowAudio];
            }
        }
        else if(event.keyCode == 123) // left - go back in time
        {
            [emuController beginRewind];
        }
        else if(event.keyCode == 36) // enter - swap keyboard for player 1<->2
        {
            [emuController gameControllerKeyboardSwapPort];
        }
        else if(event.keyCode == 53) // esc - console reset button
        {
             [emuController usrResetConsole];
        }
        if(event.keyCode == 12 && (event.modifierFlags & NSEventModifierFlagCommand) != 0)
        {
            [[NSApplication sharedApplication] terminate:self];
        }
    }
}

- (void) keyUp:(NSEvent *)event
{
    EmulationController* emuController = (EmulationController*)self.delegate;
     
    if(event.keyCode == 26)
    {
        [emuController gameControllerKeyboardButtonUp:Controller_Start];
    }
    else if(event.keyCode == 22)
    {
        [emuController gameControllerKeyboardButtonUp:Controller_Select];
    }
    else if(event.keyCode == 31)
    {
        [emuController gameControllerKeyboardButtonUp:Controller_B];
    }
    else if(event.keyCode == 35)
    {
        [emuController gameControllerKeyboardButtonUp:Controller_A];
    }
    else if(event.keyCode == 2)
    {
        [emuController gameControllerKeyboardButtonUp:Controller_Right];
    }
    else if(event.keyCode == 0)
    {
        [emuController gameControllerKeyboardButtonUp:Controller_Left];
    }
    else if(event.keyCode == 1)
    {
        [emuController gameControllerKeyboardButtonUp:Controller_Down];
    }
    else if(event.keyCode == 13)
    {
        [emuController gameControllerKeyboardButtonUp:Controller_Up];
    }
    else if(event.keyCode == 45)
    {
        [emuController showOpenNewDialogue];
    }
    else if(event.keyCode == 123)
    {
        [emuController endRewind];
    }
}

@end
