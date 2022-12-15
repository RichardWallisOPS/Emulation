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
    CartMapper_2(IOBus& bus, uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize);
    
    BUS_HEADER_IMPL
private:
    uint8_t m_bankSelect;
    
    static const uint32_t nChrRAMSize = 8192;
    uint8_t m_cartCHRRAM[nChrRAMSize];
};

#endif /* CartMapper_2_h */
