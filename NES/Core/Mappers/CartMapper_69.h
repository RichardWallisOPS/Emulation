//
//  CartMapper_69.h
//  NES
//
//  Created by Richard Wallis on 19/01/2023.
//
//  NOTE: Not all features of this mapper are implemented (missing extra audio)

#ifndef CartMapper_69_h
#define CartMapper_69_h

#include "CartMapperFactory.h"

class CartMapper_69 : public Mapper
{
public:
    BUS_HEADER_DECL
    MAPPER_HEADER_DECL
    SERIALISABLE_DECL

    virtual float AudioOut() override;
    virtual void SystemTick(uint64_t cycleCount) override;
    
private:

    uint8_t m_cmdRegister;
    uint8_t m_paramRegister;
    
    // 1 = ram 0 = rom
    uint8_t m_prgBank0RAM;
    uint8_t m_prgBank0RAMEnabled;

    // 5x 8KB Program banks
    uint8_t* m_prgBank0;
    uint8_t* m_prgBank1;
    uint8_t* m_prgBank2;
    uint8_t* m_prgBank3;
    uint8_t* m_prgBank4;
    
    // 8x 1KB Character banks
    uint8_t* m_chrBank0;
    uint8_t* m_chrBank1;
    uint8_t* m_chrBank2;
    uint8_t* m_chrBank3;
    uint8_t* m_chrBank4;
    uint8_t* m_chrBank5;
    uint8_t* m_chrBank6;
    uint8_t* m_chrBank7;
    
    uint8_t m_irqGenerate;
    uint8_t m_irqCounterDecrement;
    uint16_t m_irqCounter;

};

#endif /* CartMapper_69_h */
