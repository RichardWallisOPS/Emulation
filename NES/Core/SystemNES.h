//
//  SystemNES.h
//  NES
//
//  Created by richardwallis on 15/11/2022.
//
// Thanks to https://www.nesdev.org/ and all the people who contributed to the information contained there.
// All internal NES hardware info for this implementation taken from that source.  Again many thanks.
// This implementation is my own take on that information.  The goal was to make it fairly accurate.
// There are however some edge cases not accounted for. And there will be bugs!

// Goal: Decently accurate emulation - don't sweat the tiny edge cases - but try to have most "Top 50" games "working" if possible.
// Extra: Rather than virtual functions for my bus implementation it would be nice to emulate the bus and cartridge pins
//        as is directly and send clock pulses to each system to update what goes on the pins.  Better for mappers/open bus/etc
//        Only spotted this info on nesdev later on.  If I feel the need later, I'll make this change - might be fun....
// Extra2: Looking like NMI and IRQ signals should be held back some amount of ticks.  Must be as the H/W take time to pull the lines low
//          Look at moving any wait logic into the System::SignalXXX() functiions before signaling the CPU
// Extra3: Some features of more complex mappers are not implemented. Mainly extra audio, but especially MMC5 which is missing many features.

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

    enum ControllerButton : uint8_t
    {
        Controller_Right    = 7,
        Controller_Left     = 6,
        Controller_Down     = 5,
        Controller_Up       = 4,
        Controller_Start    = 3,
        Controller_Select   = 2,
        Controller_B        = 1,
        Controller_A        = 0
    };

    enum DMAMODE : uint8_t
    {
        DMA_OFF = 0,
        DMA_READ,
        DMA_WRITE
    };

    static const uint32_t nRamSize = 0x0800;

    SystemNES();
    virtual ~SystemNES();
    
    void Reset();
    void EjectCartridge();
    bool InsertCartridge(const char* pCartPath);
    void PowerOn();

    void Tick();
        
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
    uint8_t     m_ram[nRamSize];

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
    DMAMODE     m_DMAMode;
};

#endif /* SystemNES_h */
