//
//  CartMapper_2.h
//  NES
//
//  Created by Richard Wallis on 13/12/2022.
//

#ifndef CartMapper_2_h
#define CartMapper_2_h

#include "CartMapperFactory.h"

class CartMapper_2 : public Mapper
{
public:
    CartMapper_2(   IOBus& bus,
                    uint8_t* pPrg, uint32_t nProgramSize,
                    uint8_t* pChr, uint32_t nCharacterSize,
                    uint8_t* pCartPRGRAM, uint32_t nPrgRamSize, uint32_t nNVPrgRamSize,
                    uint8_t* pCartCHRRAM, uint32_t nChrRamSize, uint32_t nChrNVRamSize);
    
    BUS_HEADER_IMPL
private:
    uint8_t m_prgBankSelect;
};

#endif /* CartMapper_2_h */
