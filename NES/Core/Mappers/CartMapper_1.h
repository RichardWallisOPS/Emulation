//
//  CartMapper_1.h
//  NES
//
//  Created by Richard Wallis on 15/12/2022.
//

#ifndef CartMapper_1_h
#define CartMapper_1_h

#include "CartMapperFactory.h"

class CartMapper_1 : public Mapper
{
public:
    CartMapper_1(   IOBus& bus,
                    uint8_t* pPrg, uint32_t nProgramSize,
                    uint8_t* pChr, uint32_t nCharacterSize,
                    uint8_t* pCartPRGRAM, uint32_t nPrgRamSize, uint32_t nNVPrgRamSize,
                    uint8_t* pCartCHRRAM, uint32_t nChrRamSize, uint32_t nChrNVRamSize);
    
    BUS_HEADER_IMPL
private:

    uint8_t m_shiftRegister;
    uint8_t m_shiftCount;
    uint8_t m_ctrl;
    uint8_t m_chrBank0;
    uint8_t m_chrBank1;
    uint8_t m_prgBank;
};

#endif /* CartMapper_1_h */
