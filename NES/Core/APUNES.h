//
//  APUNES.h
//  NES
//
//  Created by Richard Wallis on 04/01/2023.
//

#ifndef APUNES_h
#define APUNES_h

#include "IOBus.h"
#include "Serialise.h"
#include <atomic>

class APUAudioBuffer
{
public:
    APUAudioBuffer()
    : m_bufferSize(0)
    , m_pBuffer(nullptr)
    , m_samplesWritten(0)
    , m_bReverseFlag(false)
    , m_bReady(false)
    {}
    APUAudioBuffer(size_t size)
    : m_bufferSize(size)
    , m_pBuffer(nullptr)
    , m_samplesWritten(0)
    , m_bReverseFlag(false)
    , m_bReady(false)
    {
        m_pBuffer = new float[m_bufferSize];
    }
    ~APUAudioBuffer()
    {
        if(m_pBuffer != nullptr)
        {
            delete [] m_pBuffer;
            m_pBuffer = nullptr;
        }
    }
    size_t GetBufferSize() const
    {
        return m_bufferSize;
    }
    size_t GetSamplesWritten() const
    {
        return m_samplesWritten;
    }
    void Reset()
    {
        m_bReady = false;
        m_samplesWritten = 0;
        m_bReverseFlag = false;
    }
    bool IsReady() const
    {
        return m_bReady;
    }
    void AddSample(float fSample)
    {
        if(m_samplesWritten < m_bufferSize && m_pBuffer != nullptr)
        {
            if(m_bReverseFlag)
            {
                m_pBuffer[m_bufferSize - m_samplesWritten - 1] = fSample;
            }
            else
            {
                m_pBuffer[m_samplesWritten] = fSample;
            }
            ++m_samplesWritten;
        }
    }
    void Finialise()
    {
        if(m_samplesWritten > 0)
        {
            if(m_bReverseFlag)
            {
                while(m_samplesWritten < m_bufferSize)
                {
                    m_pBuffer[m_bufferSize - m_samplesWritten - 1] = m_pBuffer[m_bufferSize - m_samplesWritten];
                    ++m_samplesWritten;
                }
            }
            else
            {
                while(m_samplesWritten < m_bufferSize)
                {
                    m_pBuffer[m_samplesWritten] = m_pBuffer[m_samplesWritten - 1];
                    ++m_samplesWritten;
                }
            }
            
            m_bReady = true;
        }
    }
    void SetShouldReverseBuffer(bool bReverse)
    {
        m_bReverseFlag = bReverse;
    }
    float const* GetSampleBuffer() const
    {
        return m_pBuffer;
    }

private:
    float* m_pBuffer;
    size_t m_bufferSize;
    size_t m_samplesWritten;
    bool   m_bReverseFlag;
    std::atomic<bool> m_bReady;
};

class APUPulseChannel : public Serialisable
{
public:
    SERIALISABLE_DECL

    APUPulseChannel(uint16_t sweepNegateComplement);
    
    uint8_t IsEnabled() const;
    void SetEnabled(uint8_t bEnabled);

    uint8_t OutputValue() const;
    void SetRegister(uint16_t reg, uint8_t byte);
    
    void Tick();
    void QuarterFrameTick();
    void HalfFrameTick();
    
    void SetDutySequence();
    void RotateDutySequence();
    
protected:

    // Registers
    uint8_t m_dutyCycle;
    uint8_t m_lengthCounterHaltOrEnvelopeLoop;
    uint8_t m_volume_ConstantOrEnvelope;
    uint8_t m_volume_LevelOrEnvelopeDividerPeriod;

    uint8_t m_sweepEnabled;
    uint8_t m_sweepPeriod;
    uint8_t m_sweepNegate;
    uint8_t m_sweepShift;
    uint16_t m_sweepNegateComplement;

    // Timer for next duty cycle rotation
    uint16_t m_timer;
    uint16_t m_timerValue;
    
    // Current in progress rotated duty sequence
    uint8_t m_currDutySequence;
    
    // Main counter
    uint8_t m_lengthCounter;
        
    // Current Envelope state
    uint8_t m_envelopeStartFlag;
    uint8_t m_envelopeDivider;
    uint8_t m_envelopeDecayLevelCounter;
    
    uint8_t m_sweepReloadFlag;
    uint8_t m_sweepDivider;
};

class APUTriangleChannel : public Serialisable
{
public:
    SERIALISABLE_DECL
    
    APUTriangleChannel();
    
    uint8_t IsEnabled() const;
    void SetEnabled(uint8_t bEnabled);

    uint8_t OutputValue() const;
    void SetRegister(uint16_t reg, uint8_t byte);
    
    void Tick();
    void QuarterFrameTick();
    void HalfFrameTick();
    
private:
    // Main Timer
    uint16_t m_timer;
    uint16_t m_timerValue;
    
    // Main counter
    uint8_t m_lengthCounter;
    
    // Linear counter
    uint8_t m_linearCounter;
    uint8_t m_linearCounterReloadValue;
    uint8_t m_linearCounterReloadFlag;
    uint8_t m_controlFlag;
    
    uint8_t m_sequenceIndex;
};

class APUNoiseChannel : public Serialisable
{
public:
    SERIALISABLE_DECL
    
    APUNoiseChannel();
    
    uint8_t IsEnabled() const;
    void SetEnabled(uint8_t bEnabled);

    uint8_t OutputValue() const;
    void SetRegister(uint16_t reg, uint8_t byte);
    
    void Tick();
    void QuarterFrameTick();
    void HalfFrameTick();

private:

    // Registers
    uint8_t m_lengthCounterHaltOrEnvelopeLoop;
    uint8_t m_volume_ConstantOrEnvelope;
    uint8_t m_volume_LevelOrEnvelopeDividerPeriod;
    
    // Main Timer
    uint8_t m_mode;
    uint16_t m_period;
    uint16_t m_periodValue;
    
    // Main counter
    uint8_t m_lengthCounter;
    
    // Current Envelope state
    uint8_t m_envelopeStartFlag;
    uint8_t m_envelopeDivider;
    uint8_t m_envelopeDecayLevelCounter;
    
    // Linear shift register
    uint16_t m_linearFeedbackShift;
};

class APUDMC : public Serialisable
{
public:
    SERIALISABLE_DECL
    
    APUDMC(SystemIOBus& bus);
    
    uint8_t IsEnabled() const;
    void SetEnabled(uint8_t bEnabled);
    uint8_t IsIRQEnabled() const;

    uint8_t OutputValue() const;
    void SetRegister(uint16_t reg, uint8_t byte);
    
    void Tick();
private:

    SystemIOBus& m_bus;

    // Registers
    uint8_t m_enabled;
    uint8_t m_IRQEnabled;
    uint8_t m_loop;
    
    uint16_t m_rate;
    uint16_t m_rateValue;
    
    uint16_t m_sampleAddress;
    uint16_t m_sampleLength;
    
    // Channle Output - auto or directly set
    uint8_t m_outputLevel;
    uint8_t m_silence;
    
    // Emulation
    uint8_t m_sampleBuffer;
    uint8_t m_sampleBufferLoaded;
    uint8_t m_sampleShiftBits;
    uint8_t m_sampleBitsRemaining;
    
    uint16_t m_currentSampleAddress;
    uint16_t m_sampleLengthRemaining;
};

class APUNES : public Serialisable
{
public:
    SERIALISABLE_DECL
    
    APUNES(SystemIOBus& bus);
    ~APUNES();
    
    float OutputValue();
    void Tick();
    
    uint8_t cpuRead(uint16_t address);
    void cpuWrite(uint16_t address, uint8_t byte);
    
    void QuarterFrameTick();
    void HalfFrameTick();
    
    void SetAudioOutputBuffer(APUAudioBuffer* pAudioBuffer);

private:
    SystemIOBus& m_bus;
    
    // Main system
    uint16_t m_frameCounter;
    uint8_t m_frameCountMode;
    uint8_t m_frameInhibitIRQ;
    
    // Channels
    APUPulseChannel     m_pulse1;
    APUPulseChannel     m_pulse2;
    APUTriangleChannel  m_triangle;
    APUNoiseChannel     m_noise;
    APUDMC              m_dmc;
    
    // Output
    APUAudioBuffer* m_pAudioBuffer;
    size_t m_audioOutDataCounter;
};

#endif /* APUNES_h */
