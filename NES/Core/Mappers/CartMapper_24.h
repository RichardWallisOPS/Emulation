//
//  CartMapper_24.h
//  NES
//
//  Created by Richard Wallis on 22/01/2023.
//

#ifndef CartMapper_24_h
#define CartMapper_24_h

#include "CartMapperFactory.h"

class VRC6AudioPulseChannel : public Serialisable
{
public:
    SERIALISABLE_DECL
    
    VRC6AudioPulseChannel();
    uint8_t OutputValue();
    
    void Tick();
    void SetRegister(uint16_t reg, uint8_t byte);
private:

    uint8_t m_enabled;
    uint8_t m_mode;
    uint8_t m_volume;
    uint8_t m_duty;
    uint16_t m_period;
    
    uint16_t m_divider;
    uint8_t m_dutyCycle;
};

class VRC6AudioSawChannel : public Serialisable
{
public:
    SERIALISABLE_DECL
    
    VRC6AudioSawChannel();
    uint8_t OutputValue();
    
    void Tick();
    void SetRegister(uint16_t reg, uint8_t byte);
private:

    uint8_t m_enabled;
    uint8_t m_accumRate;
    uint16_t m_period;
    
    uint16_t m_divider;
    uint8_t m_accumTick;
    uint8_t m_accumulator;
};

class CartMapper_24 : public Mapper
{
public:
    BUS_HEADER_DECL
    MAPPER_HEADER_DECL
    SERIALISABLE_DECL
    
    virtual float AudioOut() override;
    virtual void SystemTick(uint64_t cycleCount) override;
    
private:

    void ClockIRQCounter();
    void SetChrBank(uint8_t** pChrBank, uint8_t bank);
    
private:

    // 16+8+8KB
    uint8_t* m_prgBank0;
    uint8_t* m_prgBank1;
    uint8_t* m_prgBank2;
    
    uint8_t* m_chrBank0;
    uint8_t* m_chrBank1;
    uint8_t* m_chrBank2;
    uint8_t* m_chrBank3;
    uint8_t* m_chrBank4;
    uint8_t* m_chrBank5;
    uint8_t* m_chrBank6;
    uint8_t* m_chrBank7;

    uint8_t m_irqLatch;
    uint8_t m_irqMode;
    uint8_t m_irqEnable;
    uint8_t m_irqEnableAfterAck;
    uint8_t m_irqCounter;
    uint16_t m_irqPrescaler;
    
    VRC6AudioPulseChannel   m_pulse1;
    VRC6AudioPulseChannel   m_pulse2;
    VRC6AudioSawChannel     m_saw;
};

#endif /* CartMapper_24_h */
