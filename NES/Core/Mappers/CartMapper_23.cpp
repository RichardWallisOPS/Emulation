//
//  CartMapper_23.cpp
//  NES
//
//  Created by Richard Wallis on 11/02/2023.
//

#include "CartMapper_23.h"

void CartMapper_23::Initialise()
{
    m_prgBank0 = &m_pPrg[0];
    m_prgBank1 = &m_pPrg[m_nProgramSize - 0x6000];
    m_prgBank2 = &m_pPrg[m_nProgramSize - 0x4000];
    
    if(m_nCharacterSize == 0)
    {
        m_pChr = m_pCartCHRRAM;
    }
    
    m_chrBank0 = &m_pChr[0x0400 * 0];
    m_chrBank1 = &m_pChr[0x0400 * 1];
    m_chrBank2 = &m_pChr[0x0400 * 2];
    m_chrBank3 = &m_pChr[0x0400 * 3];
    m_chrBank4 = &m_pChr[0x0400 * 4];
    m_chrBank5 = &m_pChr[0x0400 * 5];
    m_chrBank6 = &m_pChr[0x0400 * 6];
    m_chrBank7 = &m_pChr[0x0400 * 7];
    
    m_microWireLatch = 0;
    m_regChrBank0 = 0;
    m_regChrBank1 = 0;
    m_regChrBank2 = 0;
    m_regChrBank3 = 0;
    m_regChrBank4 = 0;
    m_regChrBank5 = 0;
    m_regChrBank6 = 0;
    m_regChrBank7 = 0;
}

void CartMapper_23::Load(Archive& rArchive)
{
    rArchive >> m_microWireLatch;
    
    rArchive >> m_regChrBank0;
    rArchive >> m_regChrBank1;
    rArchive >> m_regChrBank2;
    rArchive >> m_regChrBank3;
    rArchive >> m_regChrBank4;
    rArchive >> m_regChrBank5;
    rArchive >> m_regChrBank6;
    rArchive >> m_regChrBank7;
    
    {
        uint8_t* pBasePrgAddress = &m_pPrg[0];
        size_t offsetBank0 = 0;
        size_t offsetBank1 = 0;
        size_t offsetBank2 = 0;
        rArchive >> offsetBank0;
        rArchive >> offsetBank1;
        rArchive >> offsetBank2;
        m_prgBank0 = pBasePrgAddress + offsetBank0;
        m_prgBank1 = pBasePrgAddress + offsetBank1;
        m_prgBank2 = pBasePrgAddress + offsetBank2;
    }

    {
        uint8_t* pBaseChrAddress = &m_pChr[0];
        size_t offsetBank0 = 0;
        size_t offsetBank1 = 0;
        size_t offsetBank2 = 0;
        size_t offsetBank3 = 0;
        size_t offsetBank4 = 0;
        size_t offsetBank5 = 0;
        size_t offsetBank6 = 0;
        size_t offsetBank7 = 0;
        rArchive >> offsetBank0;
        rArchive >> offsetBank1;
        rArchive >> offsetBank2;
        rArchive >> offsetBank3;
        rArchive >> offsetBank4;
        rArchive >> offsetBank5;
        rArchive >> offsetBank6;
        rArchive >> offsetBank7;
        m_chrBank0 = pBaseChrAddress + offsetBank0;
        m_chrBank1 = pBaseChrAddress + offsetBank1;
        m_chrBank2 = pBaseChrAddress + offsetBank2;
        m_chrBank3 = pBaseChrAddress + offsetBank3;
        m_chrBank4 = pBaseChrAddress + offsetBank4;
        m_chrBank5 = pBaseChrAddress + offsetBank5;
        m_chrBank6 = pBaseChrAddress + offsetBank6;
        m_chrBank7 = pBaseChrAddress + offsetBank7;
    }
}

void CartMapper_23::Save(Archive& rArchive) const
{
    rArchive << m_microWireLatch;
    
    rArchive << m_regChrBank0;
    rArchive << m_regChrBank1;
    rArchive << m_regChrBank2;
    rArchive << m_regChrBank3;
    rArchive << m_regChrBank4;
    rArchive << m_regChrBank5;
    rArchive << m_regChrBank6;
    rArchive << m_regChrBank7;
    
    {
        uint8_t* pBasePrgAddress = &m_pPrg[0];
        size_t offsetBank0 = m_prgBank0 - pBasePrgAddress;
        size_t offsetBank1 = m_prgBank1 - pBasePrgAddress;
        size_t offsetBank2 = m_prgBank2 - pBasePrgAddress;
        rArchive << offsetBank0;
        rArchive << offsetBank1;
        rArchive << offsetBank2;
    }

    {
        uint8_t* pBaseChrAddress = &m_pChr[0];
        size_t offsetBank0 = m_chrBank0 - pBaseChrAddress;
        size_t offsetBank1 = m_chrBank1 - pBaseChrAddress;
        size_t offsetBank2 = m_chrBank2 - pBaseChrAddress;
        size_t offsetBank3 = m_chrBank3 - pBaseChrAddress;
        size_t offsetBank4 = m_chrBank4 - pBaseChrAddress;
        size_t offsetBank5 = m_chrBank5 - pBaseChrAddress;
        size_t offsetBank6 = m_chrBank6 - pBaseChrAddress;
        size_t offsetBank7 = m_chrBank7 - pBaseChrAddress;
        rArchive << offsetBank0;
        rArchive << offsetBank1;
        rArchive << offsetBank2;
        rArchive << offsetBank3;
        rArchive << offsetBank4;
        rArchive << offsetBank5;
        rArchive << offsetBank6;
        rArchive << offsetBank7;
    }
}

uint8_t CartMapper_23::cpuRead(uint16_t address)
{
    if(address >= 0x6000 && address <= 0x6FFF)
    {
        return 0x60 | m_microWireLatch;
    }
    else if(address >= 0x7000 && address <= 0x7FFF)
    {
        return 0x70;
    }
    else if(address >= 0x8000 && address <= 0x9FFF)
    {
        return m_prgBank0[address - 0x8000];
    }
    else if(address >= 0xA000 && address <= 0xBFFF)
    {
        return m_prgBank1[address - 0xA000];
    }
    else if(address >= 0xC000 && address <= 0xFFFF)
    {
        return m_prgBank2[address - 0xC000];
    }
    return 0;
}

void CartMapper_23::cpuWrite(uint16_t address, uint8_t byte)
{
    // Note: VRC2 submapper 3 + VRC4e submapper 2 := A0, A1 mapped as expected
    // Note VRC4f A0, A1 := wired to A2, A3.  i.e regAddress = (address & 0xF000) | ((address >> 2) & 0x3);
    uint16_t regAddress = address & 0xF003;
    
    if(address >= 0x6000 && address <= 0x6FFF)
    {
        m_microWireLatch = byte & 0b1;
    }
    else if(regAddress >= 0x8000 && regAddress <= 0x8003)
    {
        uint32_t bankIndex = (byte & 0b11111) & ((m_nProgramSize / 8192) - 1);
        uint32_t bankAddress = bankIndex * 8192;
        m_prgBank0 = &m_pPrg[bankAddress];
    }
    else if(regAddress >= 0xA000 && regAddress <= 0xA003)
    {
        uint32_t bankIndex = (byte & 0b11111) & ((m_nProgramSize / 8192) - 1);
        uint32_t bankAddress = bankIndex * 8192;
        m_prgBank1 = &m_pPrg[bankAddress];
    }
    else if(regAddress >= 0x9000 && regAddress <= 0x9003)
    {
        uint8_t mirror = (byte >> 1) & 0b1;
        if(mirror == 0)
        {
            m_bus.SetMirrorMode(VRAM_MIRROR_V);
        }
        else
        {
            m_bus.SetMirrorMode(VRAM_MIRROR_H);
        }
    }
    else if(regAddress == 0xB000)
    {
        m_regChrBank0 = (m_regChrBank0 & 0xF0) | (byte & 0x0F);
        SetChrBank(&m_chrBank0, m_regChrBank0);
    }
    else if(regAddress == 0xB001)
    {
        m_regChrBank0 = (m_regChrBank0 & 0x0F) | ((byte & 0x0F) << 4);
        SetChrBank(&m_chrBank0, m_regChrBank0);
    }
    else if(regAddress == 0xB002)
    {
        m_regChrBank1 = (m_regChrBank1 & 0xF0) | (byte & 0x0F);
        SetChrBank(&m_chrBank1, m_regChrBank1);
    }
    else if(regAddress == 0xB003)
    {
        m_regChrBank1 = (m_regChrBank1 & 0x0F) | ((byte & 0x0F) << 4);
        SetChrBank(&m_chrBank1, m_regChrBank1);
    }
    else if(regAddress == 0xC000)
    {
        m_regChrBank2 = (m_regChrBank2 & 0xF0) | (byte & 0x0F);
        SetChrBank(&m_chrBank2, m_regChrBank2);
    }
    else if(regAddress == 0xC001)
    {
        m_regChrBank2 = (m_regChrBank2 & 0x0F) | ((byte & 0x0F) << 4);
        SetChrBank(&m_chrBank2, m_regChrBank2);
    }
    else if(regAddress == 0xC002)
    {
        m_regChrBank3 = (m_regChrBank3 & 0xF0) | (byte & 0x0F);
        SetChrBank(&m_chrBank3, m_regChrBank3);
    }
    else if(regAddress == 0xC003)
    {
        m_regChrBank3 = (m_regChrBank3 & 0x0F) | ((byte & 0x0F) << 4);
        SetChrBank(&m_chrBank3, m_regChrBank3);
    }
    else if(regAddress == 0xD000)
    {
        m_regChrBank4 = (m_regChrBank4 & 0xF0) | (byte & 0x0F);
        SetChrBank(&m_chrBank4, m_regChrBank4);
    }
    else if(regAddress == 0xD001)
    {
        m_regChrBank4 = (m_regChrBank4 & 0x0F) | ((byte & 0x0F) << 4);
        SetChrBank(&m_chrBank4, m_regChrBank4);
    }
    else if(regAddress == 0xD002)
    {
        m_regChrBank5 = (m_regChrBank5 & 0xF0) | (byte & 0x0F);
        SetChrBank(&m_chrBank5, m_regChrBank5);
    }
    else if(regAddress == 0xD003)
    {
        m_regChrBank5 = (m_regChrBank5 & 0x0F) | ((byte & 0x0F) << 4);
        SetChrBank(&m_chrBank5, m_regChrBank5);
    }
    else if(regAddress == 0xE000)
    {
        m_regChrBank6 = (m_regChrBank6 & 0xF0) | (byte & 0x0F);
        SetChrBank(&m_chrBank6, m_regChrBank6);
    }
    else if(regAddress == 0xE001)
    {
        m_regChrBank6 = (m_regChrBank6 & 0x0F) | ((byte & 0x0F) << 4);
        SetChrBank(&m_chrBank6, m_regChrBank6);
    }
    else if(regAddress == 0xE002)
    {
        m_regChrBank7 = (m_regChrBank7 & 0xF0) | (byte & 0x0F);
        SetChrBank(&m_chrBank7, m_regChrBank7);
    }
    else if(regAddress == 0xE003)
    {
        m_regChrBank7 = (m_regChrBank7 & 0x0F) | ((byte & 0x0F) << 4);
        SetChrBank(&m_chrBank7, m_regChrBank7);
    }
}

void CartMapper_23::SetChrBank(uint8_t** pChrBank, uint8_t bank)
{
    uint32_t maxBanks = (m_nCharacterSize > 0 ? m_nCharacterSize : GetChrRamSize()) / 0x400;
    uint32_t bankIndex = uint32_t(bank) & (maxBanks - 1);
    uint32_t bankAddress = bankIndex * 0x400;
    *pChrBank = &m_pChr[bankAddress];
}

uint8_t CartMapper_23::ppuRead(uint16_t address)
{
    if(address >= 0x0000 && address <= 0x03FF)
    {
        return m_chrBank0[address - 0x0000];
    }
    else if(address >= 0x0400 && address <= 0x07FF)
    {
        return m_chrBank1[address - 0x0400];
    }
    else if(address >= 0x0800 && address <= 0x0BFF)
    {
        return m_chrBank2[address - 0x0800];
    }
    else if(address >= 0x0C00 && address <= 0x0FFF)
    {
        return m_chrBank3[address - 0x0C00];
    }
    else if(address >= 0x1000 && address <= 0x13FF)
    {
        return m_chrBank4[address - 0x1000];
    }
    else if(address >= 0x1400 && address <= 0x17FF)
    {
        return m_chrBank5[address - 0x1400];
    }
    else if(address >= 0x1800 && address <= 0x1BFF)
    {
        return m_chrBank6[address - 0x1800];
    }
    else if(address >= 0x1C00 && address <= 0x1FFF)
    {
        return m_chrBank7[address - 0x1C00];
    }
    return 0;
}

void CartMapper_23::ppuWrite(uint16_t address, uint8_t byte)
{
    if(address >= 0x0000 && address <= 0x03FF)
    {
        m_chrBank0[address - 0x0000] = byte;
    }
    else if(address >= 0x0400 && address <= 0x07FF)
    {
        m_chrBank1[address - 0x0400] = byte;
    }
    else if(address >= 0x0800 && address <= 0x0BFF)
    {
        m_chrBank2[address - 0x0800] = byte;
    }
    else if(address >= 0x0C00 && address <= 0x0FFF)
    {
        m_chrBank3[address - 0x0C00] = byte;
    }
    else if(address >= 0x1000 && address <= 0x13FF)
    {
        m_chrBank4[address - 0x1000] = byte;
    }
    else if(address >= 0x1400 && address <= 0x17FF)
    {
        m_chrBank5[address - 0x1400] = byte;
    }
    else if(address >= 0x1800 && address <= 0x1BFF)
    {
        m_chrBank6[address - 0x1800] = byte;
    }
    else if(address >= 0x1C00 && address <= 0x1FFF)
    {
        m_chrBank7[address - 0x1C00] = byte;
    }
}
