//
//  CartMapper_9.cpp
//  NES
//
//  Created by Richard Wallis on 13/01/2023.
//

#include "CartMapper_9.h"

void CartMapper_9::Initialise()
{
    m_prgBankSelect = 0;
    m_chrBankSelect0 = 0;
    m_chrBankSelect1 = 0;
    m_chrBankSelect2 = 0;
    m_chrBankSelect3 = 0;
    m_latch0 = 0xFD;
    m_latch1 = 0xFD;
}

void CartMapper_9::Load(Archive& rArchive)
{
    rArchive >> m_prgBankSelect;
    rArchive >> m_chrBankSelect0;
    rArchive >> m_chrBankSelect1;
    rArchive >> m_chrBankSelect2;
    rArchive >> m_chrBankSelect3;
    rArchive >> m_latch0;
    rArchive >> m_latch1;
}

void CartMapper_9::Save(Archive& rArchive) const
{
    rArchive << m_prgBankSelect;
    rArchive << m_chrBankSelect0;
    rArchive << m_chrBankSelect1;
    rArchive << m_chrBankSelect2;
    rArchive << m_chrBankSelect3;
    rArchive << m_latch0;
    rArchive << m_latch1;
}

uint8_t CartMapper_9::cpuRead(uint16_t address)
{
    // CPU $8000-$9FFF: 8 KB switchable PRG ROM bank
    // CPU $A000-$FFFF: Three 8 KB PRG ROM banks, fixed to the last three banks
    const uint32_t bankSize = 0x2000;
    if(address >= 0x8000 && address <= 0x9FFF)
    {
        return m_pPrg[(uint32_t(m_prgBankSelect) * bankSize) + (address - 0x8000)];
    }
    else if(address >= 0xA000 && address <= 0xFFFF)
    {
        return m_pPrg[(m_nProgramSize - bankSize * 3) + (address - 0xA000)];
    }
    return 0x00;
}

void CartMapper_9::cpuWrite(uint16_t address, uint8_t byte)
{
    if(address >= 0xA000 && address <= 0xAFFF)
    {
        m_prgBankSelect = byte & 0b1111;
    }
    else if(address >= 0xB000 && address <= 0xBFFF)
    {
        m_chrBankSelect0 = byte & 0b11111;
    }
    else if(address >= 0xC000 && address <= 0xCFFF)
    {
        m_chrBankSelect1 = byte & 0b11111;
    }
    else if(address >= 0xD000 && address <= 0xDFFF)
    {
        m_chrBankSelect2 = byte & 0b11111;
    }
    else if(address >= 0xE000 && address <= 0xEFFF)
    {
        m_chrBankSelect3 = byte & 0b11111;
    }
    else if(address >= 0xF000 && address <= 0xFFFF)
    {
        uint8_t mirror = byte & 0b1;
        if(mirror == 0)
        {
            m_bus.SetMirrorMode(VRAM_MIRROR_V);
        }
        else if(mirror == 1)
        {
            m_bus.SetMirrorMode(VRAM_MIRROR_H);
        }
    }
}

uint8_t CartMapper_9::ppuRead(uint16_t address)
{
    uint8_t byte = 0;
    
    // Current latch sets chr bank - dynamic switch during ppu rendering
    if(address >= 0x0000 && address <= 0x0FFF)
    {
        if(m_latch0 == 0xFD)
        {
            byte = m_pChr[(0x1000 * uint32_t(m_chrBankSelect0)) + (address - 0x0000)];
        }
        else if(m_latch0 == 0xFE)
        {
            byte = m_pChr[(0x1000 * uint32_t(m_chrBankSelect1)) + (address - 0x0000)];
        }
    }
    else if(address >= 0x1000 && address <= 0x1FFF)
    {
        if(m_latch1 == 0xFD)
        {
            byte = m_pChr[(0x1000 * uint32_t(m_chrBankSelect2)) + (address - 0x1000)];
        }
        else if(m_latch1 == 0xFE)
        {
            byte = m_pChr[(0x1000 * uint32_t(m_chrBankSelect3)) + (address - 0x1000)];
        }
    }
    // Latch updates
    if(address == 0x0FD8)
    {
        m_latch0 = 0xFD;
    }
    else if(address == 0x0FE8)
    {
        m_latch0 = 0xFE;
    }
    else if(address >= 0x1FD8 && address <= 0x1FDF)
    {
        m_latch1 = 0xFD;
    }
    else if(address >= 0x1FE8 && address <= 0x1FEF)
    {
        m_latch1 = 0xFE;
    }
    
    return byte;
}

void CartMapper_9::ppuWrite(uint16_t address, uint8_t byte)
{

}
