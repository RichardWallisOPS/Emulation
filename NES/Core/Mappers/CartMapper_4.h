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
    
    static const uint32_t nPrgRAMSize = 8192;
    uint8_t m_cartPRGRAM[nPrgRAMSize];
    
    static const uint32_t nChrRAMSize = 8192;
    uint8_t m_cartCHRRAM[nChrRAMSize];
};

#endif /* CartMapper_4_h */
