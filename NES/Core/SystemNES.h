//
//  SystemNES.h
//  NES
//
//  Created by richardwallis on 15/11/2022.
//

#ifndef SystemNES_h
#define SystemNES_h

#include "IOBus.h"
#include "Serialise.h"
#include "CPU6502.h"
#include "PPUNES.h"
#include "APUNES.h"
#include "Cartridge.h"

class SystemNES : public SystemIOBus, public Serialisable
{
public:
    BUS_HEADER_DECL
    SERIALISABLE_DECL

    enum DMA_MODE : uint8_t
    {
        DMA_OFF = 0,
        DMA_READ,
        DMA_WRITE
    };

    SystemNES();
    virtual ~SystemNES();
    
    void Reset();
    void EjectCartridge();
    bool InsertCartridge(const char* pCartPath);
    void PowerOn();

    void Tick();
    
    virtual float AudioOut() override;
    virtual void SignalReset(bool bSignal) override;
    virtual void SignalNMI(bool bSignal) override;
    virtual void SignalIRQ(bool bSignal) override;
    virtual void SetMirrorMode(MirrorMode mode) override;

    // assumed space for a 32bit colour 256x240 image data
    void SetVideoOutputDataPtr(uint32_t* pVideoOutData);
    
    // Assumed space for 1 frame 1/60 worth of audio data
    void SetAudioOutputBuffer(APUAudioBuffer* pAudioBuffer);
    
    // port 0 = player 1
    void SetControllerBits(uint8_t port, uint8_t bits);
    
private:
    bool        m_bPowerOn;
    uint64_t    m_cycleCount;
    uint8_t     m_ram[2048];

    CPU6502     m_cpu;
    PPUNES      m_ppu;
    APUNES      m_apu;
    Cartridge*  m_pCart;
    
    // Controller instantious and latch
    uint8_t     m_controller1;
    uint8_t     m_controller2;
    uint8_t     m_controllerLatch1;
    uint8_t     m_controllerLatch2;
    
    // DMA
    uint16_t    m_dmaAddress;
    uint8_t     m_dmaData;
    DMA_MODE    m_dmaMode;
};

#endif /* SystemNES_h */
