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

const uint8_t LENGTH_COUNTER_LUT[] = { 0,254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14, 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30 };
const uint8_t TRIANGLE_SEQUENCE[] = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
const uint16_t NOISE_PERIOD[] =  {4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068};
const uint16_t DMC_RATE[] = {428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106, 84, 72, 54};

APUPulseChannel::APUPulseChannel(uint16_t sweepNegateComplement)
: m_dutyCycle(0)
, m_lengthCounterHaltOrEnvelopeLoop(0)
, m_volume_ConstantOrEnvelope(0)
, m_volume_LevelOrEnvelopeDividerPeriod(0)
, m_sweepEnabled(0)
, m_sweepPeriod(0)
, m_sweepNegate(0)
, m_sweepShift(0)
, m_sweepNegateComplement(sweepNegateComplement)
, m_timer(0)
, m_timerValue(0)
, m_currDutySequence(0)
, m_lengthCounter(0)
, m_envelopeStartFlag(0)
, m_envelopeDivider(0)
, m_envelopeDecayLevelCounter(0)
, m_sweepReloadFlag(0)
, m_sweepDivider(0)
{}

void APUPulseChannel::Load(Archive& rArchive)
{

}

void APUPulseChannel::Save(Archive& rArchive)
{

}

uint8_t APUPulseChannel::IsEnabled() const
{
    return m_lengthCounter > 0;
}

void APUPulseChannel::SetEnabled(uint8_t bEnabled)
{
    if(!bEnabled)
    {
        m_lengthCounter = 0;
    }
}

void APUPulseChannel::SetRegister(uint16_t reg, uint8_t byte)
{
    if(reg == 0)
    {
        // DDLC VVVV
        m_dutyCycle = (byte >> 6) & 0b11;
        m_lengthCounterHaltOrEnvelopeLoop = (byte >> 5) & 0b1;
        m_volume_ConstantOrEnvelope = (byte >> 4) & 0b1;;
        m_volume_LevelOrEnvelopeDividerPeriod = byte & 0b1111;
    }
    else if(reg == 1)
    {
        // EPPP NSSS
        m_sweepEnabled = (byte >> 7) & 0b1;
        m_sweepPeriod = (byte >> 4) & 0b111;
        m_sweepNegate = (byte >> 3) & 0b1;
        m_sweepShift = byte & 0b111;
        m_sweepReloadFlag = 1;
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
        m_envelopeStartFlag = 1;
        SetDutySequence();
    }
}

void APUPulseChannel::SetDutySequence()
{
    if(m_dutyCycle == 0)        m_currDutySequence = 0b01000000;
    else if(m_dutyCycle == 1)   m_currDutySequence = 0b01100000;
    else if(m_dutyCycle == 2)   m_currDutySequence = 0b01111000;
    else if(m_dutyCycle == 3)   m_currDutySequence = 0b10011111;
}

void APUPulseChannel::RotateDutySequence()
{
    uint8_t highBit = m_currDutySequence & 0b10000000;
    m_currDutySequence <<= 1;
    m_currDutySequence |= highBit >> 7;
}

void APUPulseChannel::QuarterFrameTick()
{
    if(m_envelopeStartFlag == 1)
    {
        m_envelopeStartFlag = 0;
        m_envelopeDecayLevelCounter = 15;
        m_envelopeDivider = m_volume_LevelOrEnvelopeDividerPeriod + 1;
    }
    else
    {
        if(m_envelopeDivider > 0)
        {
            --m_envelopeDivider;
        }
        else
        {
            m_envelopeDivider = m_volume_LevelOrEnvelopeDividerPeriod + 1;

            if(m_envelopeDecayLevelCounter > 0)
            {
                --m_envelopeDecayLevelCounter;
            }
            else if(m_lengthCounterHaltOrEnvelopeLoop == 1)
            {
                m_envelopeDecayLevelCounter = 15;
            }
        }
    }
}

void APUPulseChannel::HalfFrameTick()
{
    if(m_lengthCounter > 0 && m_lengthCounterHaltOrEnvelopeLoop == 0)
    {
        --m_lengthCounter;
    }
    
    if(m_sweepEnabled)
    {
        uint16_t changeAmount = m_timerValue >> m_sweepShift;
        if(m_sweepNegate)
        {
            changeAmount += m_sweepNegateComplement;
            if(changeAmount > m_timerValue)
            {
                m_timerValue = 0;
            }
            else
            {
                m_timerValue -= changeAmount;
            }
        }
        else
        {
            m_timerValue += changeAmount;
        }
    }
    
    if(m_sweepEnabled && m_sweepDivider == 0)
    {
        m_timer = m_timerValue;
    }
    
    if(m_sweepDivider == 0 || m_sweepReloadFlag == 1)
    {
        m_sweepDivider = m_sweepPeriod + 1;
        m_sweepReloadFlag = 0;
    }
    else
    {
        --m_sweepDivider;
    }
}

void APUPulseChannel::Tick()
{
    if(IsEnabled())
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
}

uint8_t APUPulseChannel::OutputValue() const
{
    uint8_t output = 0;
    
    if(m_lengthCounter > 0)
    {
        // Output is current MSB
        output = (m_currDutySequence >> 7) & 0b1;
        
        // Multipled by constant volume or envelope decay level
        if(m_volume_ConstantOrEnvelope == 1)
        {
            output *= m_volume_LevelOrEnvelopeDividerPeriod;
        }
        else
        {
            output *= m_envelopeDecayLevelCounter;
        }
    }
    
    return output;
}

APUTriangleChannel::APUTriangleChannel()
: m_timer(0)
, m_timerValue(0)
, m_lengthCounter(0)
, m_linearCounter(0)
, m_linearCounterReloadValue(0)
, m_linearCounterReloadFlag(0)
, m_controlFlag(0)
, m_sequenceIndex(0)
{}

void APUTriangleChannel::Load(Archive& rArchive)
{

}

void APUTriangleChannel::Save(Archive& rArchive)
{

}
    
uint8_t APUTriangleChannel::IsEnabled() const
{
    return m_lengthCounter > 0;
}

void APUTriangleChannel::SetEnabled(uint8_t bEnabled)
{
    if(!bEnabled)
    {
        m_lengthCounter = 0;
    }
}

uint8_t APUTriangleChannel::OutputValue() const
{
    return TRIANGLE_SEQUENCE[m_sequenceIndex];
}

void APUTriangleChannel::SetRegister(uint16_t reg, uint8_t byte)
{
    if(reg == 0)
    {
        // CRRR RRRR
        m_controlFlag = (byte >> 7) & 0b1;
        m_linearCounterReloadValue = byte & 0b01111111;
    }
    else if(reg == 1)
    {
        // Nothing
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
        m_linearCounterReloadFlag = 1;
    }
}

void APUTriangleChannel::QuarterFrameTick()
{
    if(m_linearCounterReloadFlag == 1)
    {
        m_linearCounter = m_linearCounterReloadValue;
    }
    else if(m_linearCounter > 0)
    {
        --m_linearCounter;
    }
    
    if(m_controlFlag == 0)
    {
        m_linearCounterReloadFlag = 0;
    }
}

void APUTriangleChannel::HalfFrameTick()
{
    if(m_lengthCounter > 0)
    {
        --m_lengthCounter;
    }
}

void APUTriangleChannel::Tick()
{
    if(m_lengthCounter > 0 && m_linearCounter > 0)
    {
        if(m_timer > 0)
        {
            m_timer = m_timer - 1;
        }
        else
        {
            m_timer = m_timerValue + 1;
            m_sequenceIndex = (m_sequenceIndex + 1) % sizeof(TRIANGLE_SEQUENCE);
        }
    }
}

APUNoiseChannel::APUNoiseChannel()
: m_lengthCounterHaltOrEnvelopeLoop(0)
, m_volume_ConstantOrEnvelope(0)
, m_volume_LevelOrEnvelopeDividerPeriod(0)
, m_mode(0)
, m_period(0)
, m_periodValue(0)
, m_lengthCounter(0)
, m_envelopeStartFlag(0)
, m_envelopeDivider(0)
, m_envelopeDecayLevelCounter(0)
, m_linearFeedbackShift(1)
{}

void APUNoiseChannel::Load(Archive& rArchive)
{

}

void APUNoiseChannel::Save(Archive& rArchive)
{

}

uint8_t APUNoiseChannel::IsEnabled() const
{
    return m_lengthCounter > 0;
}

void APUNoiseChannel::SetEnabled(uint8_t bEnabled)
{
    if(!bEnabled)
    {
        m_lengthCounter = 0;
    }
}

void APUNoiseChannel::SetRegister(uint16_t reg, uint8_t byte)
{
    if(reg == 0)
    {
        // --LC VVVV
        m_lengthCounterHaltOrEnvelopeLoop = (byte >> 5) & 0b1;
        m_volume_ConstantOrEnvelope = (byte >> 4) & 0b1;;
        m_volume_LevelOrEnvelopeDividerPeriod = byte & 0b1111;
    }
    else if(reg == 1)
    {
        // Nothing
    }
    else if(reg == 2)
    {
        // M---- PPPP
        m_mode = (byte >> 7) & 0b1;
        m_periodValue = byte & 0b1111;
        m_periodValue = NOISE_PERIOD[m_periodValue];
    }
    else if(reg == 3)
    {
        // LLLL L---
        m_lengthCounter = (byte >> 3) & 0b11111;
        m_lengthCounter = LENGTH_COUNTER_LUT[m_lengthCounter];
        m_envelopeStartFlag = 1;
        //m_linearFeedbackShift = 1;
    }
}

void APUNoiseChannel::QuarterFrameTick()
{
    if(m_envelopeStartFlag == 1)
    {
        m_envelopeStartFlag = 0;
        m_envelopeDecayLevelCounter = 15;
        m_envelopeDivider = m_volume_LevelOrEnvelopeDividerPeriod + 1;
    }
    else
    {
        if(m_envelopeDivider > 0)
        {
            --m_envelopeDivider;
        }
        else
        {
            m_envelopeDivider = m_volume_LevelOrEnvelopeDividerPeriod + 1;

            if(m_envelopeDecayLevelCounter > 0)
            {
                --m_envelopeDecayLevelCounter;
            }
            else if(m_lengthCounterHaltOrEnvelopeLoop == 1)
            {
                m_envelopeDecayLevelCounter = 15;
            }
        }
    }
}

void APUNoiseChannel::HalfFrameTick()
{
    if(m_lengthCounter > 0 && m_lengthCounterHaltOrEnvelopeLoop == 0)
    {
        --m_lengthCounter;
    }
}

void APUNoiseChannel::Tick()
{
    if(IsEnabled())
    {
        if(m_period > 0)
        {
            m_period = m_period - 1;
        }
        else
        {
            m_period = m_periodValue + 1;
            
            uint16_t modeBit = m_mode ? 6 : 1;
            uint16_t feedback = (m_linearFeedbackShift & 0b1) ^ ((m_linearFeedbackShift >> modeBit) & 0b1);
            m_linearFeedbackShift >>= 1;
            m_linearFeedbackShift |= feedback << 14;
        }
    }
}

uint8_t APUNoiseChannel::OutputValue() const
{
    uint8_t output = 0;
    
    if(m_lengthCounter > 0 && (m_linearFeedbackShift & 0b1) != 0)
    {
        // Constant volume or decay
        if(m_volume_ConstantOrEnvelope == 1)
        {
            output = m_volume_LevelOrEnvelopeDividerPeriod;
        }
        else
        {
            output = m_envelopeDecayLevelCounter;
        }
    }
    
    return output;
}

APUDMC::APUDMC(SystemIOBus& bus)
: m_bus(bus)
, m_enabled(0)
, m_IRQEnabled(0)
, m_loop(0)
, m_rate(0)
, m_rateValue(0)
, m_sampleAddress(0)
, m_sampleLength(0)
, m_outputLevel(0)
, m_sampleBuffer(0)
, m_sampleBufferLoaded(0)
, m_sampleShiftBits(0)
, m_sampleBitsRemaining(0)
, m_currentSampleAddress(0)
, m_sampleLengthRemaining(0)
{}

void APUDMC::Load(Archive& rArchive)
{

}

void APUDMC::Save(Archive& rArchive)
{

}

uint8_t APUDMC::IsEnabled() const
{
    return m_enabled;
}

void APUDMC::SetEnabled(uint8_t bEnabled)
{
    m_enabled = bEnabled;
    if(bEnabled)
    {
        if(m_sampleLengthRemaining == 0)
        {
            m_currentSampleAddress = m_sampleAddress;
            m_sampleLengthRemaining = m_sampleLength;
        }
    }
    else
    {
        m_sampleLengthRemaining = 0;
    }
}

void APUDMC::SetRegister(uint16_t reg, uint8_t byte)
{
    if(reg == 0)
    {
        // IL-- RRRR
        m_IRQEnabled = (byte >> 7) & 0b1;
        m_loop = (byte >> 6) & 0b1;
        m_rateValue = byte & 0b1111;
        m_rateValue = DMC_RATE[m_rateValue];
    }
    else if(reg == 1)
    {
        // -DDD DDDD
        // TODO fix popping
        //m_outputLevel = byte & 0b01111111;
    }
    else if(reg == 2)
    {
        // AAAA AAAA
        m_sampleAddress = 0xC000 + (uint16_t(byte) << 6);
    }
    else if(reg == 3)
    {
        // LLLL LLLL
        m_sampleLength = (uint16_t(byte) << 4) + 1;
    }
}

void APUDMC::Tick()
{
    if(m_rate > 0)
    {
        m_rate = m_rate - 1;
    }
    else
    {
        m_rate = m_rateValue + 1;
        
        if(m_sampleBitsRemaining > 0)
        {
            uint8_t sampleModulation = m_sampleShiftBits & 0b1;
            
            if(sampleModulation == 1 && m_outputLevel <= 125)
            {
                m_outputLevel += 2;
            }
            else if(sampleModulation == 0 && m_outputLevel >= 2)
            {
                m_outputLevel -= 2;
            }
            
            --m_sampleBitsRemaining;
            m_sampleShiftBits >>= 1;
        }
        else
        {
            if(m_sampleBufferLoaded)
            {
                m_sampleBufferLoaded = 0;
                m_sampleBitsRemaining = 8;
                m_sampleShiftBits = m_sampleBuffer;
            }
            else
            {
                m_enabled = 0;
            }
            
            if(m_sampleLengthRemaining > 0)
            {
                m_sampleBuffer = m_bus.cpuRead(m_currentSampleAddress);
                m_sampleBufferLoaded = 1;
                
                --m_sampleLengthRemaining;
                ++m_currentSampleAddress;
                
                if(m_currentSampleAddress == 0x0000)
                {
                    m_currentSampleAddress = 0x8000;
                }
            }
            else
            {
                if(m_loop == 1)
                {
                    m_currentSampleAddress = m_sampleAddress;
                    m_sampleLengthRemaining = m_sampleLength;
                }
                else if(m_IRQEnabled)
                {
                    m_bus.SignalIRQ(true);
                }
            }
        }
    }
}

uint8_t APUDMC::OutputValue() const
{
    return m_outputLevel;
}

APUNES::APUNES(SystemIOBus& bus)
: m_bus(bus)
, m_frameCounter(0)
, m_pulse1(1)
, m_pulse2(0)
, m_triangle()
, m_noise()
, m_dmc(bus)
, m_frameCountModeAndInterrupt(1 << 6)
, m_pAudioBuffer(nullptr)
, m_audioOutDataCounter(0)
{}

APUNES::~APUNES()
{}

void APUNES::Load(Archive& rArchive)
{

}

void APUNES::Save(Archive& rArchive)
{

}

float APUNES::OutputValue()
{
    float fPulse1 = m_pulse1.OutputValue();
    float fPulse2 = m_pulse2.OutputValue();
    float fTriangle = m_triangle.OutputValue();
    float fNoise = m_noise.OutputValue();
    float fDMC = m_dmc.OutputValue();
    
//    fPulse1 = 0.f;
//    fPulse2 = 0.f;
//    fTriangle = 0.f;
//    fNoise = 0.f;
//    fDMC = 0.f;
    
    float fPulse = 0.00752f * (fPulse1 + fPulse2);
    float fTND = 0.00851f * fTriangle + 0.00494f * fNoise + 0.00335f * fDMC;

    return fPulse + fTND;
}

void APUNES::QuarterFrameTick()
{
    // Envelopes & triangle's linear counter
    m_pulse1.QuarterFrameTick();
    m_pulse2.QuarterFrameTick();
    m_triangle.QuarterFrameTick();
    m_noise.QuarterFrameTick();
}

void APUNES::HalfFrameTick()
{
    // Length counters & sweep units
    m_pulse1.HalfFrameTick();
    m_pulse2.HalfFrameTick();
    m_triangle.HalfFrameTick();
    m_noise.HalfFrameTick();
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
        m_noise.Tick();
        m_dmc.Tick();
    }

    // Ticked every CPU tick
    m_triangle.Tick();
    
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
        m_bus.SignalIRQ(true);
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
    
    if(m_pAudioBuffer != nullptr)
    {
        // TODO: This is a bit of a down sample hack - think of a better way!
        // 89342 frame ticks, this is ticked every 3
        // 89342 / 3 = 29780.6667
        // We want 800 samples per 1/60 sec (48000 KHz / 60 fps = 800 samples)
        // 29780.6667 / 800 = 37.225
        // So we can sample ever 37.225 ticks
        ++m_audioOutDataCounter;
        // If we have hit 37 (or sometimes 38 for the .225 part - every 5 samples) create a sample for the buffer
        size_t sampleCouner = 37;
        if(m_pAudioBuffer->GetSamplesWritten() % 5 == 0)
        {
            sampleCouner = sampleCouner + 1;
        }
        if(m_audioOutDataCounter >= sampleCouner)
        {
            m_audioOutDataCounter = 0;
            m_pAudioBuffer->AddSample(OutputValue());
        }
    }
}

uint8_t APUNES::cpuRead(uint16_t address)
{
    if(address == SND_CHN)
    {
        // IF-D NT21 	DMC interrupt (I), frame interrupt (F), DMC active (D), length counter > 0 (N/T/2/1)
        uint8_t status = 0;
        
        status |= m_pulse1.IsEnabled() << 0;
        status |= m_pulse2.IsEnabled() << 1;
        status |= m_triangle.IsEnabled() << 2;
        status |= m_noise.IsEnabled() << 3;
        status |= m_dmc.IsEnabled() << 4;

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
        case TRI_LO:
        case TRI_HI:
            m_triangle.SetRegister(address - 0x4008, byte);
            break;
        case NOISE_VOL:
        case NOISE_LO:
        case NOISE_HI:
            m_noise.SetRegister(address - 0x400C, byte);
            break;
        case DMC_FREQ:
        case DMC_RAW:
        case DMC_START:
        case DMC_LEN:
            m_dmc.SetRegister(address - 0x4010, byte);
            break;
        case SND_CHN:
            // Sset status and enabled flags
            // ---D NT21 	Enable DMC (D), noise (N), triangle (T), and pulse channels (2/1)
            m_pulse1.SetEnabled((byte >> 0) & 0b1);
            m_pulse2.SetEnabled((byte >> 1) & 0b1);
            m_triangle.SetEnabled((byte >> 2) & 0b1);
            m_noise.SetEnabled((byte >> 3) & 0b1);
            m_dmc.SetEnabled((byte >> 4) & 0b1);
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
