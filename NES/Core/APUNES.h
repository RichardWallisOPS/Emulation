//
//  APUNES.h
//  NES
//
//  Created by Richard Wallis on 04/01/2023.
//

#ifndef APUNES_h
#define APUNES_h

#include <cstdint>
#include "IOBus.h"
#include "Serialise.h"

class APUAudioBuffer
{
public:
    // 48000 KHz each frame 1/60 second = x samples per video frame
    APUAudioBuffer(size_t size = 48000 / 60)
    : m_bufferSize(size)
    , m_samplesWritten(0)
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
    size_t GetSamplesToWrite() const
    {
        return m_bufferSize - m_samplesWritten;
    }
    void Reset()
    {
        m_samplesWritten = 0;
    }
    void AddSample(float fSample)
    {
        if(m_samplesWritten < m_bufferSize && m_pBuffer != nullptr)
        {
            m_pBuffer[m_samplesWritten++] = fSample;
        }
    }
    float* GetSampleBuffer()
    {
        return m_pBuffer;
    }

private:
    float* m_pBuffer;
    size_t m_bufferSize;
    size_t m_samplesWritten;
};

class APUPulseChannel
{
public:
    APUPulseChannel();
    
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

    uint8_t m_enabled;

    uint8_t m_dutyCycle;
    uint8_t m_lengthCounterHalt;
    uint8_t m_constantVolumeEnvelope;
    uint8_t m_VolumeEnvelopeDividerPeriod;

    uint8_t m_sweepEnabled;
    uint8_t m_sweepPeriod;
    uint8_t m_sweepNegate;
    uint8_t m_sweepShift;

    uint16_t m_timer;
    uint16_t m_timerValue;
    
    uint8_t m_lengthCounter;
    
    uint8_t m_sequence;
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
    
    uint16_t m_frameCounter;
    uint8_t m_frameCountModeAndInterrupt;
    
    APUPulseChannel m_pulse1;
    APUPulseChannel m_pulse2;
    
    APUAudioBuffer* m_pAudioBuffer;
    size_t m_audioOutDataCounter;
};

#endif /* APUNES_h */
