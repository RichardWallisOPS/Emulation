//
//  CartMapper_66.cpp
//  NES
//
//  Created by Richard Wallis on 15/12/2022.
//  Copyright © 2022 openplanetsoftware. All rights reserved.
//

#include "CartMapper_66.h"

CartMapper_66::CartMapper_66(IOBus& bus, uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize)
: Mapper(bus, pPrg, nProgramSize, pChr, nCharacterSize)
, m_bankSelect(0)
{}

uint8_t CartMapper_66::cpuRead(uint16_t address)
{
    if(address >= 0x8000 && address <= 0xFFFF)
    {
        uint32_t prgBank = (m_bankSelect >> 4) & 0b11;
        uint32_t cartAddress = (prgBank * 32768) + address - 0x8000;
        return m_pPrg[cartAddress];
    }
    
    return 0x00;
}
    
void CartMapper_66::cpuWrite(uint16_t address, uint8_t byte)
{
    if(address >= 0x8000 && address <= 0xFFFF)
    {
        m_bankSelect = byte;
    }
}

uint8_t CartMapper_66::ppuRead(uint16_t address)
{
    if(address >= 0x0000 && address <= 0x1FFF)
    {
        uint32_t chrBank = m_bankSelect & 0b11;
        uint32_t cartAddress = chrBank * 8192 + address;
        return m_pChr[cartAddress];
    }
    
    return 0;
}

void CartMapper_66::ppuWrite(uint16_t address, uint8_t byte)
{

}
