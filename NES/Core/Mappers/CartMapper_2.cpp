//
//  CartMapper_0.cpp
//  NES
//
//  Created by Richard Wallis on 13/12/2022.
//

#include <stdio.h>
#include "CartMapper_2.h"

CartMapper_2::CartMapper_2(uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize)
: Mapper(pPrg, nProgramSize, pChr, nCharacterSize)
, m_bankSelect(0)
{
    
}

uint8_t CartMapper_2::cpuRead(uint16_t address)
{
    if(m_pPrg != nullptr&& m_nProgramSize > 0)
    {
        const uint32_t bankSize = 0x4000;
        
        if(address >= 0x8000 && address <= 0xBFFF)
        {
            uint32_t cartAddress = uint32_t(m_bankSelect) * bankSize;
            return m_pPrg[cartAddress];
        }
        else if(address >= 0xC000 && address <= 0xFFFF)
        {
            uint32_t cartAddress = m_nProgramSize - bankSize;
             return m_pPrg[cartAddress];
        }
    }
    return 0x00;
}

void CartMapper_2::cpuWrite(uint16_t address, uint8_t byte)
{
    m_bankSelect = byte & 0b00001111;
}

uint8_t CartMapper_2::ppuRead(uint16_t address)
{
    if(m_pChr != nullptr && m_nCharacterSize > 0)
    {
        uint32_t cartAddress = address % m_nCharacterSize;
        return m_pChr[cartAddress];
    }
    return address & 0xFF; // open bus low byte return
}

void CartMapper_2::ppuWrite(uint16_t address, uint8_t byte)
{
    
}
