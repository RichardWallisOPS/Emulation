//
//  CartMapper_2.h
//  NES
//
//  Created by Richard Wallis on 13/12/2022.
//

#ifndef CartMapper_2_h
#define CartMapper_2_h

#include "Cartridge.h"

class CartMapper_2 : public Mapper
{
public:
    CartMapper_2(   IOBus& bus,
                    uint8_t* pPrg, uint32_t nProgramSize,
                    uint8_t* pChr, uint32_t nCharacterSize,
                    uint32_t nPrgRamSize, uint32_t nNVPrgRamSize,
                    uint32_t nChrRamSize, uint32_t nChrNVRamSize);
    
    ~CartMapper_2();
    
    BUS_HEADER_IMPL
private:
    uint8_t m_prgBankSelect;
    uint8_t* m_pCartCHRRAM;
};

#endif /* CartMapper_2_h */
