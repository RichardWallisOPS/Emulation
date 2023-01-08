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

uint8_t LENGTH_COUNTER_LUT[] = { 0,254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14, 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30 };

APUPulseChannel::APUPulseChannel()
: m_dutyCycle(0)
, m_lengthCounterHalt(0)
, m_constantVolumeEnvelope(0)
, m_VolumeEnvelopeDividerPeriod(0)
, m_sweepEnabled(0)
, m_sweepPeriod(0)
, m_sweepNegate(0)
, m_sweepShift(0)
, m_timer(0)
, m_timerValue(0)
, m_lengthCounter(0)
{

}

void APUPulseChannel::SetRegister(uint16_t reg, uint8_t byte)
{
    if(reg == 0)
    {
        // DDLC VVVV
        m_dutyCycle = (byte >> 6) & 0b11;
        m_lengthCounterHalt = (byte >> 5) & 0b1;
        m_constantVolumeEnvelope = (byte >> 4) & 0b1;;
        m_VolumeEnvelopeDividerPeriod = byte & 0b1111;
    }
    else if(reg == 1)
    {
        // EPPP NSSS
        m_sweepEnabled = (byte >> 7) & 0b1;
        m_sweepPeriod = (byte >> 4) & 0b111;
        m_sweepNegate = (byte << 3) & 0b1;
        m_sweepShift = byte & 0b111;
    }
    else if(reg == 2)
    {
        // TTTT TTTT
        m_timerValue = (m_timerValue & 0xFF00) | uint16_t(byte);
    }
    else if(reg == 3)
    {
        // LLLL LTTT
        m_timerValue = (m_timerValue & 0x00FF) | (uint16_t(byte & 0b111) << 8);
        m_lengthCounter = (byte >> 3) & 0b11111;
        m_lengthCounter = LENGTH_COUNTER_LUT[m_lengthCounter];
        
        SetDutySequence();
    }
}

void APUPulseChannel::SetDutySequence()
{
    if(m_dutyCycle == 0)        m_sequence = 0b01000000;
    else if(m_dutyCycle == 1)   m_sequence = 0b01100000;
    else if(m_dutyCycle == 2)   m_sequence = 0b01111000;
    else if(m_dutyCycle == 3)   m_sequence = 0b10011111;
}

void APUPulseChannel::RotateDutySequence()
{
    uint8_t highBit = m_sequence & 0b10000000;
    m_sequence <<= 1;
    m_sequence |= highBit >> 7;
}

void APUPulseChannel::QuarterFrameTick()
{
    // TODO
}

void APUPulseChannel::HalfFrameTick()
{
    if(m_lengthCounter > 0 && m_lengthCounterHalt == 0)
    {
        --m_lengthCounter;
        // TODO silence
    }
}

void APUPulseChannel::Tick()
{
    if(m_timer > 0)
    {
        m_timer = m_timer - 1;
    }
    else
    {
        m_timer = m_timerValue + 1;
        RotateDutySequence();
    }
}

uint8_t APUPulseChannel::OutputValue() const
{
    return m_lengthCounter > 0 ? (m_sequence >> 7) * 15 : 0;
}

APUNES::APUNES(SystemIOBus& bus)
: m_bus(bus)
, m_frameCounter(0)
, m_frameCountModeAndInterrupt(0)
, m_pAudioBuffer(nullptr)
, m_audioOutDataCounter(0)
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

void APUNES::QuarterFrameTick()
{
    // Envelopes & triangle's linear counter
    m_pulse1.QuarterFrameTick();
    m_pulse2.QuarterFrameTick();
}

void APUNES::HalfFrameTick()
{
    // Length counters & sweep units
    m_pulse1.HalfFrameTick();
    m_pulse2.HalfFrameTick();
}

float APUNES::OutputValue()
{
    float pulse1 = m_pulse1.OutputValue();
    float pulse2 = m_pulse2.OutputValue();
    
    //float pulseOut = 95.88f / ((8128.f / (pulse1 + pulse2)) + 100.f);
    //float tndOut = 0.f; // TODO
    //return pulseOut + tndOut;
    
    float sample = 0.00752f * (pulse1 + pulse2);
    
    //return sample;
    return (sample - 0.5f) * 2.f;

//    48000 samples per second
//    60 FPS frame rate
//    48000 / 60 = 800 samples per frame
}

void APUNES::Tick()
{
    ++m_frameCounter;
    
    uint16_t frameCount = m_frameCounter >> 1;
    uint16_t halfFrame = m_frameCounter & 1;
    
    // Ticked every second CPU tick
    if(halfFrame == 0)
    {
        m_pulse1.Tick();
        m_pulse2.Tick();
    }
    
    // Ticked every CPU tick
    // m_triangle.Tick();
    // Noise/DMC ?
    
    if(m_pAudioBuffer != nullptr)
    {
        // Down sample hack - think of a better way!
        ++m_audioOutDataCounter;
        if(m_audioOutDataCounter == 37)
        {
            m_pAudioBuffer->AddSample(OutputValue());
            m_audioOutDataCounter = 0;
        }
    }
    
    bool bFrameModeStep4 = (m_frameCountModeAndInterrupt & (1 << 7)) == 0;
    bool bIRQInhibit = (m_frameCountModeAndInterrupt & (1 << 6)) != 0;
    
    if(frameCount == 3728 && halfFrame == 1)
    {
        QuarterFrameTick();
    }
    else if(frameCount == 7456 && halfFrame == 1)
    {
        QuarterFrameTick();
        HalfFrameTick();
    }
    else if(frameCount == 11185 && halfFrame == 1)
    {
        QuarterFrameTick();
    }
    else if(frameCount == 14914 && halfFrame == 0 && bFrameModeStep4 && !bIRQInhibit)
    {
        // TODO
        //m_bus.SignalIRQ(true);
    }
    else if(frameCount == 14914 && halfFrame == 1 && bFrameModeStep4)
    {
        QuarterFrameTick();
        HalfFrameTick();
    }
    else if(frameCount == 14915 && bFrameModeStep4)
    {
        m_frameCounter = 0;
    }
    else if(frameCount == 18640 && halfFrame == 1)
    {
        QuarterFrameTick();
        HalfFrameTick();
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
        uint8_t status = 0;

                
        // Clear frame interrupt flag on read
        status |= m_frameCountModeAndInterrupt & (1 << 6);
        m_frameCountModeAndInterrupt = m_frameCountModeAndInterrupt & (~(1 << 6));
        
        return status;
    }
    return 0;
}

void APUNES::cpuWrite(uint16_t address, uint8_t byte)
{
    switch(address)
    {
        case SQ1_VOL:
        case SQ1_SWEEP:
        case SQ1_LO:
        case SQ1_HI:
            m_pulse1.SetRegister(address - 0x4000, byte);
            break;
        case SQ2_VOL:
        case SQ2_SWEEP:
        case SQ2_LO:
        case SQ2_HI:
            m_pulse2.SetRegister(address - 0x4004, byte);
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
            // TODO set status and enabled flags - so we can start updating things
            break;
        case FRAME_COUNTER:
            m_frameCountModeAndInterrupt = byte;
            break;
    }
}

void APUNES::SetAudioOutputBuffer(APUAudioBuffer* pAudioBuffer)
{
    m_audioOutDataCounter = 0;
    m_pAudioBuffer = pAudioBuffer;
    
    if(m_pAudioBuffer != nullptr)
    {
        m_pAudioBuffer->Reset();
    }
}
