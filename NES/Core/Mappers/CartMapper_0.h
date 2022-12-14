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
    CartMapper_0(uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize);
    
    BUS_HEADER_IMPL
private:

    // HOMEBREW ONLY
    static const uint32_t nChrRAMSize = 8292;
    uint8_t m_cartCHRRAM[nChrRAMSize];
};

#endif /* CartMapper_2_h */
