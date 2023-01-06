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

class APUPulseChannel
{
public:
    APUPulseChannel();
    
    void SetRegister(uint16_t reg, uint8_t byte);
    
    void Tick();
    void QuarterFrameTick();
    void HalfFrameTick();
    
protected:

    uint8_t m_dutyCycle;
    uint8_t m_lengthCounterHalt;
    uint8_t m_constantVolumeEnvelope;
    uint8_t m_VolumeEnvelopeDividerPeriod;

    uint8_t m_sweepEnabled;
    uint8_t m_sweepPeriod;
    uint8_t m_sweepNegate;
    uint8_t m_sweepShift;

    uint16_t    m_timer;
    uint16_t    m_timerValue;
    
    uint8_t     m_lengthCounter;
};

class APUNES : public Serialisable
{
public:
    SERIALISABLE_DECL
    
    APUNES(SystemIOBus& bus);
    ~APUNES();
    
    void Tick();
    
    uint8_t cpuRead(uint16_t address);
    void cpuWrite(uint16_t address, uint8_t byte);
    
    void QuarterFrameTick();
    void HalfFrameTick();

private:
    SystemIOBus& m_bus;
    uint16_t m_frameCounter;
    uint8_t m_frameCountModeAndInterrupt;
    
    APUPulseChannel m_pulse1;
    APUPulseChannel m_pulse2;
};

#endif /* APUNES_h */
