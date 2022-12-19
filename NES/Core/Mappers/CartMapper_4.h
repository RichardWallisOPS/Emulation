//
//  CartMapper_4.hpp
//  NES
//
//  Created by Richard Wallis on 18/12/2022.
//

#ifndef CartMapper_4_h
#define CartMapper_4_h

#include "Cartridge.h"

class CartMapper_4 : public Mapper
{
public:
    CartMapper_4(IOBus& bus, uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize);
    
    BUS_HEADER_IMPL
private:

    uint8_t m_bankSelect;
    uint8_t m_bankData;
    uint8_t m_mirror;
    uint8_t m_prgRamProtect;
    
    static const uint32_t nChrRAMSize = 8192;
    uint8_t m_cartCHRRAM[nChrRAMSize];
};

#endif /* CartMapper_4_h */
