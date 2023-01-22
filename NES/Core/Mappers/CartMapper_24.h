//
//  CartMapper_24.hp
//  NES
//
//  Created by Richard Wallis on 22/01/2023.
//

#ifndef CartMapper_24_h
#define CartMapper_24_h

#include "CartMapperFactory.h"

class CartMapper_24 : public Mapper
{
public:
    BUS_HEADER_DECL
    MAPPER_HEADER_DECL
    SERIALISABLE_DECL
    
    virtual void systemTick(uint64_t cycleCount) override;
    
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
    
    uint64_t m_systemCycleCount;
};

#endif /* CartMapper_24_h */
