//
//  CartMapper_2.h
//  NES
//
//  Created by Richard Wallis on 13/12/2022.
//  Copyright © 2022 openplanetsoftware. All rights reserved.
//

#ifndef CartMapper_0_h
#define CartMapper_0_h

#include "Cartridge.h"

class CartMapper_0 : public Mapper
{
public:
    CartMapper_0(   IOBus& bus,
                    uint8_t* pPrg, uint32_t nProgramSize,
                    uint8_t* pChr, uint32_t nCharacterSize,
                    uint32_t nPrgRamSize, uint32_t nNVPrgRamSize,
                    uint32_t nChrRamSize, uint32_t nChrNVRamSize);
    
    BUS_HEADER_IMPL
private:

    uint8_t m_cartPRGRAM[8192];

    // HOMEBREW ONLY
    uint8_t m_cartCHRRAM[8192];
};

#endif /* CartMapper_2_h */
