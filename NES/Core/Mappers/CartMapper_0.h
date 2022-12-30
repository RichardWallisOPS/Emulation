//
//  CartMapper_2.h
//  NES
//
//  Created by Richard Wallis on 13/12/2022.
//  Copyright © 2022 openplanetsoftware. All rights reserved.
//

#ifndef CartMapper_0_h
#define CartMapper_0_h

#include "CartMapperFactory.h"

class CartMapper_0 : public Mapper
{
public:
    CartMapper_0(   IOBus& bus,
                    uint8_t* pPrg, uint32_t nProgramSize,
                    uint8_t* pChr, uint32_t nCharacterSize,
                    uint8_t* pCartPRGRAM, uint32_t nPrgRamSize, uint32_t nNVPrgRamSize,
                    uint8_t* pCartCHRRAM, uint32_t nChrRamSize, uint32_t nChrNVRamSize);
    
    BUS_HEADER_IMPL
private:
};

#endif /* CartMapper_2_h */
