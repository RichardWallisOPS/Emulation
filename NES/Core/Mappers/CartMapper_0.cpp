//
//  CartMapper_0.cpp
//  NES
//
//  Created by Richard Wallis on 13/12/2022.
//

#include "CartMapper_0.h"

void CartMapper_0::Initialise()
{
    if(m_nCharacterSize == 0)
    {
        m_pChr = m_pCartCHRRAM;
    }
}

uint8_t CartMapper_0::cpuRead(uint16_t address)
{
    if(address >= 0x6000 && address <= 0x7FFF && m_pCartPRGRAM != nullptr)
    {
        return m_pCartPRGRAM[address - 0x6000];
    }
    else if(address >= 0x8000 && address <= 0xFFFF)
    {
        uint32_t cartAddress = (address - 0x8000) & (m_nProgramSize - 1);
        return m_pPrg[cartAddress];
    }
    return 0x00;
}

void CartMapper_0::cpuWrite(uint16_t address, uint8_t byte)
{
    if(address >= 0x6000 && address <= 0x7FFF && m_pCartPRGRAM != nullptr)
    {
        m_pCartPRGRAM[address - 0x6000] = byte;
    }
}

uint8_t CartMapper_0::ppuRead(uint16_t address)
{
    uint16_t addressRange = m_nCharacterSize > 0 ? m_nCharacterSize - 1 : GetChrRamSize() - 1;
    return m_pChr[address & addressRange];
}

void CartMapper_0::ppuWrite(uint16_t address, uint8_t byte)
{
    uint16_t addressRange = m_nCharacterSize > 0 ? m_nCharacterSize - 1 : GetChrRamSize() - 1;
    m_pChr[address & addressRange] = byte;
}
