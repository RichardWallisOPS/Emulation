//
//  CartMapper_2.cpp
//  NES
//
//  Created by Richard Wallis on 13/12/2022.
//  Copyright © 2022 openplanetsoftware. All rights reserved.
//

#include "CartMapper_0.h"

void CartMapper_0::Initialise()
{
    if(m_nCharacterSize == 0)
    {
        m_nCharacterSize = m_nChrRamSize;
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
    return m_pChr[address & (m_nCharacterSize - 1)];
}

void CartMapper_0::ppuWrite(uint16_t address, uint8_t byte)
{
    m_pChr[address & (m_nCharacterSize - 1)] = byte;
}
