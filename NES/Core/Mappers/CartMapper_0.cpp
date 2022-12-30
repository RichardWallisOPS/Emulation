//
//  CartMapper_2.cpp
//  NES
//
//  Created by Richard Wallis on 13/12/2022.
//  Copyright © 2022 openplanetsoftware. All rights reserved.
//

#include "CartMapper_0.h"

CartMapper_0::CartMapper_0( IOBus& bus,
                            uint8_t* pPrg, uint32_t nProgramSize,
                            uint8_t* pChr, uint32_t nCharacterSize,
                            uint8_t* pCartPRGRAM, uint32_t nPrgRamSize, uint32_t nNVPrgRamSize,
                            uint8_t* pCartCHRRAM, uint32_t nChrRamSize, uint32_t nChrNVRamSize)
: Mapper(bus, pPrg, nProgramSize, pChr, nCharacterSize, pCartPRGRAM, nPrgRamSize, nNVPrgRamSize, pCartCHRRAM, nChrRamSize, nChrNVRamSize)
{
    if(nCharacterSize == 0)
    {
        m_nCharacterSize = nChrRamSize;
        m_pChr = m_pCartCHRRAM;
    }
}

uint8_t CartMapper_0::cpuRead(uint16_t address)
{
    if(m_pPrg != nullptr)
    {
        if(address >= 0x6000 && address <= 0x7FFF)
        {
            return m_pCartPRGRAM[address - 0x6000];
        }
        else if(address >= 0x8000 && address <= 0xFFFF)
        {
            uint32_t cartAddress = (address - 0x8000) & (m_nProgramSize - 1);
            return m_pPrg[cartAddress];
        }
    }
    return 0x00;
}

void CartMapper_0::cpuWrite(uint16_t address, uint8_t byte)
{
    if(address >= 0x6000 && address <= 0x7FFF)
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
