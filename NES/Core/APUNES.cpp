//
//  APUNES.cpp
//  NES
//
//  Created by Richard Wallis on 04/01/2023.
//

#include "APUNES.h"

enum RegisterID : uint16_t
{
    SQ1_VOL     = 0x4000,
    SQ1_SWEEP   = 0x4001,
    SQ1_LO      = 0x4002,
    SQ1_HI      = 0x4003,
    
    SQ2_VOL     = 0x4004,
    SQ2_SWEEP   = 0x4005,
    SQ2_LO      = 0x4006,
    SQ2_HI      = 0x4007,
    
    TRI_LINEAR  = 0x4008,
    TRI_UNUSED  = 0x4009,
    TRI_LO      = 0x400A,
    TRI_HI      = 0x400B,
    
    NOISE_VOL   = 0x400C,
    NOISE_UNUSED= 0x400D,
    NOISE_LO    = 0x400E,
    NOISE_HI    = 0x400F,
    
    DMC_FREQ 	= 0x4010,
    DMC_RAW     = 0x4011,
    DMC_START   = 0x4012,
    DMC_LEN     = 0x4013,
    
    OAMDMA      = 0x4014,   // Not used here
    SND_CHN     = 0x4015,   // Channel enable
    JOY1        = 0x4016,   // Not used here
    JOY2        = 0x4017    // Write sets APU frame counter control
};

APUNES::APUNES()
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

}

uint8_t APUNES::cpuRead(uint16_t address)
{
    // Open bus
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
        case JOY2:
            break;
    }
}
