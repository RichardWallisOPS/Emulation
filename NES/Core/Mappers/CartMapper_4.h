//
//  CartMapper_4.hpp
//  NES
//
//  Created by Richard Wallis on 18/12/2022.
//

#ifndef CartMapper_4_h
#define CartMapper_4_h

#include "CartMapperFactory.h"

class CartMapper_4 : public Mapper
{
public:
    BUS_HEADER_DECL
    MAPPER_HEADER_DECL
    SERIALISABLE_DECL
    
    virtual void systemTick(uint64_t cycleCount) override;
    
private:
    void MM3Signal(uint16_t address);
    
private:

    uint8_t m_bankSelect;
    uint8_t m_bankData;
    
    // 4x 8KB Program banks
    uint8_t* m_prgBank0;
    uint8_t* m_prgBank1;
    uint8_t* m_prgBank2;
    uint8_t* m_prgBank3;
    
    // 8x 1KB Character banks
    // Some are conceptually swapped in 2K chunks so the next bank is sequential in memory
    uint8_t* m_chrBank0;
    uint8_t* m_chrBank1;
    uint8_t* m_chrBank2;
    uint8_t* m_chrBank3;
    uint8_t* m_chrBank4;
    uint8_t* m_chrBank5;
    uint8_t* m_chrBank6;
    uint8_t* m_chrBank7;
        
    uint8_t m_scanlineLatch;
    uint8_t m_scanlineCounter;
    uint8_t m_scanlineEnable;
    uint8_t m_scanlineReload;
    uint8_t m_lastA12;
    uint8_t m_delay;
    uint64_t m_cycleCount;
    uint64_t m_systemCycleCount;
};

#endif /* CartMapper_4_h */
