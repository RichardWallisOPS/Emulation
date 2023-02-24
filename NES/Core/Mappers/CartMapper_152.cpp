//
//  CartMapper_152.cpp
//  NES
//
//  Created by Richard Wallis on 24/02/2023.
//

#include "CartMapper_152.h"

void CartMapper_152::Initialise()
{
    m_prgBank = 0;
    m_chrBank = 0;
}

void CartMapper_152::Load(Archive& rArchive)
{
    rArchive >> m_prgBank;
    rArchive >> m_chrBank;
}

void CartMapper_152::Save(Archive& rArchive) const
{
    rArchive << m_prgBank;
    rArchive << m_chrBank;
}

uint8_t CartMapper_152::cpuRead(uint16_t address)
{
    const uint32_t bankSize = 16384;
    if(address >= 0x8000 && address <= 0xBFFF)
    {
        return m_pPrg[(uint32_t(m_prgBank) * bankSize) + address - 0x8000];
    }
    else if(address >= 0xC000 && address <= 0xFFFF)
    {
        return m_pPrg[(m_nProgramSize - bankSize) + address - 0xC000];
    }
    return 0;
}

void CartMapper_152::cpuWrite(uint16_t address, uint8_t byte)
{
    if(address >= 0x8000 && address <= 0xFFFF)
    {
        m_prgBank = ((byte >> 4) & 0b111) & ((m_nProgramSize / 16384) - 1);
        m_chrBank = ((byte >> 0) & 0b1111) & ((m_nCharacterSize / 8192) - 1);
        
        uint8_t mirror = (byte >> 7) & 0b1;
        if(mirror == 0)
        {
            m_bus.SetMirrorMode(VRAM_MIRROR_SINGLEA);
        }
        else
        {
            m_bus.SetMirrorMode(VRAM_MIRROR_SINGLEB);
        }
    }
}

uint8_t CartMapper_152::ppuRead(uint16_t address)
{
    return m_pChr[(uint32_t(m_chrBank) * 8192) + address];
}

void CartMapper_152::ppuWrite(uint16_t address, uint8_t byte)
{
    m_pChr[(uint32_t(m_chrBank) * 8192) + address] = byte;
}
