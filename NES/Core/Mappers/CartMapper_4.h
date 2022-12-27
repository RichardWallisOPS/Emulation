//
//  CartMapper_4.hpp
//  NES
//
//  Created by Richard Wallis on 18/12/2022.
//

#ifndef CartMapper_4_h
#define CartMapper_4_h

#include "CartMapperFactory.h"

class CartMapper_4 : public Mapper
{
public:
    CartMapper_4(   IOBus& bus,
                    uint8_t* pPrg, uint32_t nProgramSize,
                    uint8_t* pChr, uint32_t nCharacterSize,
                    uint32_t nPrgRamSize, uint32_t nNVPrgRamSize,
                    uint32_t nChrRamSize, uint32_t nChrNVRamSize);
    
    BUS_HEADER_IMPL
    
private:
    void MM3Signal(uint16_t address);
    
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
    
    uint8_t m_cartPRGRAM[8192];
        
    uint8_t m_scanlineLatch;
    uint8_t m_scanlineCounter;
    uint8_t m_scanlineEnable;
    uint8_t m_scanlineReload;
    uint8_t m_lastA12;
    uint8_t m_delay;
};

#endif /* CartMapper_4_h */
