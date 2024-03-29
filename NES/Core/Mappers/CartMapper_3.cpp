//
//  CartMapper_3.cpp
//  NES
//
//  Created by Richard Wallis on 17/12/2022.
//

#include "CartMapper_3.h"

void CartMapper_3::Initialise()
{
    m_chrBankSelect = 0;
}

void CartMapper_3::Load(Archive& rArchive)
{
    rArchive >> m_chrBankSelect;
}

void CartMapper_3::Save(Archive& rArchive) const
{
    rArchive << m_chrBankSelect;
}

uint8_t CartMapper_3::cpuRead(uint16_t address)
{
     if(m_pPrg != nullptr)
    {
        if(address >= 0x8000 && address <= 0xFFFF)
        {
            uint32_t cartAddress = (address - 0x8000) & (m_nProgramSize - 1);
            return m_pPrg[cartAddress];
        }
    }
    return 0;
}

void CartMapper_3::cpuWrite(uint16_t address, uint8_t byte)
{
    if(address >= 0x8000 && address <= 0xFFFF)
    {
        // compute max number of chr rom bits this cart can handle in 8k chunkcs
        uint32_t maxSize = (m_nCharacterSize / 8192) - 1;
        m_chrBankSelect = (byte & 0b11) & maxSize;
    }
}

uint8_t CartMapper_3::ppuRead(uint16_t address)
{
    uint32_t chrAddress = m_chrBankSelect * 8192 + address;
    return m_pChr[chrAddress];
}

void CartMapper_3::ppuWrite(uint16_t address, uint8_t byte)
{
    uint32_t chrAddress = m_chrBankSelect * 8192 + address;
    m_pChr[chrAddress] = byte;
}
