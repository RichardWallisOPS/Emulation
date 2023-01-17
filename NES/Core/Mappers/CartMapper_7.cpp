//
//  CartMapper_7.cpp
//  NES
//
//  Created by Richard Wallis on 15/01/2023.
//

#include "CartMapper_7.h"

void CartMapper_7::Initialise()
{
    uint16_t maxBanks = (m_nProgramSize / 0x8000);
    m_prgBankSelect = maxBanks - 1;
}

void CartMapper_7::Load(Archive& rArchive)
{

}

void CartMapper_7::Save(Archive& rArchive) const
{

}

uint8_t CartMapper_7::cpuRead(uint16_t address)
{
    if(address >= 0x8000 && address <= 0xFFFF)
    {
        uint32_t bankAddress = uint32_t(m_prgBankSelect) * 0x8000;
        return m_pPrg[bankAddress + address - 0x8000];
    }
    return 0x00;
}

void CartMapper_7::cpuWrite(uint16_t address, uint8_t byte)
{
    if(address >= 0x8000 && address <= 0xFFFF)
    {
        uint16_t maxBanks = (m_nProgramSize / 0x8000);
        m_prgBankSelect = (byte & 0b111) & maxBanks - 1;
        
        
        uint8_t nameTableVRAMSelect = (byte >> 4) & 0b1;
        if(nameTableVRAMSelect == 0)
        {
            m_bus.SetMirrorMode(VRAM_MIRROR_SINGLEA);
        }
        else
        {
            m_bus.SetMirrorMode(VRAM_MIRROR_SINGLEB);
        }
    }
}

uint8_t CartMapper_7::ppuRead(uint16_t address)
{
    return m_pChr[address & (m_nCharacterSize - 1)];
}

void CartMapper_7::ppuWrite(uint16_t address, uint8_t byte)
{
     m_pChr[address & (m_nCharacterSize - 1)] = byte;
}
