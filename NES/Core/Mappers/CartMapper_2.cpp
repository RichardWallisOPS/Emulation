//
//  CartMapper_0.cpp
//  NES
//
//  Created by Richard Wallis on 13/12/2022.
//

#include "CartMapper_2.h"

void CartMapper_2::Initialise()
{
    m_prgBankSelect = 0;
}

void CartMapper_2::Load(Archive& rArchive)
{
    rArchive >> m_prgBankSelect;
}

void CartMapper_2::Save(Archive& rArchive)
{
    rArchive << m_prgBankSelect;
}

uint8_t CartMapper_2::cpuRead(uint16_t address)
{
    if(m_pPrg != nullptr && m_nProgramSize > 0)
    {
        if(address >= 0x8000 && address <= 0xBFFF)
        {
            uint32_t cartBaseAddress = uint32_t(m_prgBankSelect) * 0x4000;
            uint32_t cartOffset = cartBaseAddress + address - 0x8000;
            return m_pPrg[cartOffset];
        }
        else if(address >= 0xC000 && address <= 0xFFFF)
        {
            uint32_t cartAddress = m_nProgramSize - 0x4000;
            uint32_t cartOffset = cartAddress + address - 0xC000;
            return m_pPrg[cartOffset];
        }
    }
    return 0x00;
}

void CartMapper_2::cpuWrite(uint16_t address, uint8_t byte)
{
    uint16_t maxBanks = (m_nProgramSize / 0x4000);
    m_prgBankSelect = byte & (maxBanks - 1);
}

uint8_t CartMapper_2::ppuRead(uint16_t address)
{
    return m_pCartCHRRAM[address % m_nChrRamSize];
}

void CartMapper_2::ppuWrite(uint16_t address, uint8_t byte)
{
    m_pCartCHRRAM[address % m_nChrRamSize] = byte;
}
