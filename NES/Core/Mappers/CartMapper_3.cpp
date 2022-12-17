//
//  CartMapper_3.cpp
//  NES
//
//  Created by Richard Wallis on 17/12/2022.
//

#include "CartMapper_3.h"

CartMapper_3::CartMapper_3(IOBus& bus, uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize)
: Mapper(bus, pPrg, nProgramSize, pChr, nCharacterSize)
, m_chrBankSelect(0)
{}

uint8_t CartMapper_3::cpuRead(uint16_t address)
{
     if(m_pPrg != nullptr)
    {
        if(address >= 0x8000 && address <= 0xFFFF)
        {
            uint32_t cartAddress = (address - 0x8000) & (m_nProgramROMSize - 1);
            return m_pPrg[cartAddress];
        }
    }
    return 0;
}

void CartMapper_3::cpuWrite(uint16_t address, uint8_t byte)
{
    // compute max number of chr rom bits this cart can handle in 8k chunkcs
    uint32_t maxSize = (m_nCharacterROMSize / 8192) - 1;
    m_chrBankSelect = byte & maxSize;
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
