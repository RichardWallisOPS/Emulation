//
//  APUNES.cpp
//  NES
//
//  Created by Richard Wallis on 04/01/2023.
//

#include "APUNES.h"

enum RegisterID : uint16_t
{
    SQ1_VOL         = 0x4000,
    SQ1_SWEEP       = 0x4001,
    SQ1_LO          = 0x4002,
    SQ1_HI          = 0x4003,
        
    SQ2_VOL         = 0x4004,
    SQ2_SWEEP       = 0x4005,
    SQ2_LO          = 0x4006,
    SQ2_HI          = 0x4007,
        
    TRI_LINEAR      = 0x4008,
    UNUSED_0        = 0x4009,
    TRI_LO          = 0x400A,
    TRI_HI          = 0x400B,
        
    NOISE_VOL       = 0x400C,
    UNUSED_1        = 0x400D,
    NOISE_LO        = 0x400E,
    NOISE_HI        = 0x400F,
        
    DMC_FREQ 	    = 0x4010,
    DMC_RAW         = 0x4011,
    DMC_START       = 0x4012,
    DMC_LEN         = 0x4013,
        
    UNUSED_3        = 0x4014,
    SND_CHN         = 0x4015,
    UNUSED_4        = 0x4016,
    FRAME_COUNTER   = 0x4017,
};

APUNES::APUNES(SystemIOBus& bus)
: m_bus(bus)
, m_frameCounter(0)
, m_frameCountModeAndInterrupt(0)
{

}

APUNES::~APUNES()
{

}

void APUNES::Load(Archive& rArchive)
{

}

void APUNES::Save(Archive& rArchive)
{

}

void APUNES::Tick()
{
    ++m_frameCounter;
    
    uint16_t frameCount = m_frameCounter >> 1;
    uint16_t halfFrame = m_frameCounter & 1;
    
    bool bFrameModeStep4 = (m_frameCountModeAndInterrupt & (1 << 7)) == 0;
    bool bIRQInhibit = (m_frameCountModeAndInterrupt & (1 << 6)) != 0;
    
    if(frameCount == 3728 && halfFrame == 1)
    {
        // 1/4 frame clock
    }
    else if(frameCount == 7456 && halfFrame == 1)
    {
        // 1/4 and 1/2 frame clock
    }
    else if(frameCount == 11185 && halfFrame == 1)
    {
        // 1/4 frame clock
    }
    else if(frameCount == 14914 && halfFrame == 0 && bFrameModeStep4 && !bIRQInhibit)
    {
        m_bus.SignalIRQ(true);
    }
    else if(frameCount == 14914 && halfFrame == 1 && bFrameModeStep4)
    {
        // 1/4 and 1/2 frame clock
    }
    else if(frameCount == 14915 && bFrameModeStep4)
    {
        m_frameCounter = 0;
    }
    else if(frameCount == 18640 && halfFrame == 1)
    {
        // 1/4 and 1/2 frame clock
    }
    else if(frameCount == 18641)
    {
        m_frameCounter = 0;
    }
}

uint8_t APUNES::cpuRead(uint16_t address)
{
    if(address == SND_CHN)
    {
        // TODO: Sound channel and IRQ Status
    }
    return 0;
}

void APUNES::cpuWrite(uint16_t address, uint8_t byte)
{
    switch(address)
    {
        case SQ1_VOL:
            break;
        case SQ1_SWEEP:
            break;
        case SQ1_LO:
            break;
        case SQ1_HI:
            break;
        case SQ2_VOL:
            break;
        case SQ2_SWEEP:
            break;
        case SQ2_LO:
            break;
        case SQ2_HI:
            break;
        case TRI_LINEAR:
            break;
        case TRI_LO:
            break;
        case TRI_HI:
            break;
        case NOISE_VOL:
            break;
        case NOISE_LO:
            break;
        case NOISE_HI:
            break;
        case DMC_FREQ:
            break;
        case DMC_RAW:
            break;
        case DMC_START:
            break;
        case DMC_LEN:
            break;
        case SND_CHN:
            break;
        case FRAME_COUNTER:
            m_frameCountModeAndInterrupt = byte;
            break;
    }
}
