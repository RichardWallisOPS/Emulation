//
//  CartMapper_2.cpp
//  NES
//
//  Created by Richard Wallis on 13/12/2022.
//  Copyright © 2022 openplanetsoftware. All rights reserved.
//

#include "CartMapper_0.h"

CartMapper_0::CartMapper_0(IOBus& bus, uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize)
: Mapper(bus, pPrg, nProgramSize, pChr, nCharacterSize)
{}

uint8_t CartMapper_0::cpuRead(uint16_t address)
{
    if(m_pPrg != nullptr)
    {
        if(address >= 0x8000 && address <= 0xFFFF)
        {
            uint32_t cartAddress = (address - 0x8000) % m_nProgramROMSize;
            return m_pPrg[cartAddress];
        }
    }
    return 0x00;
}
    
void CartMapper_0::cpuWrite(uint16_t address, uint8_t byte)
{
    // no registers to write to or no ram
}

uint8_t CartMapper_0::ppuRead(uint16_t address)
{
    if(m_pChr != nullptr && m_nCharacterROMSize > 0)
    {
        uint32_t cartAddress = address % m_nCharacterROMSize;
        return m_pChr[cartAddress];
    }
    else
    {
        return m_cartCHRRAM[address % nChrRAMSize];
    }
    
    return address & 0xFF; // open bus low byte return
}

void CartMapper_0::ppuWrite(uint16_t address, uint8_t byte)
{
    if(m_pChr == nullptr)
    {
        m_cartCHRRAM[address % nChrRAMSize] = byte;
    }
}
