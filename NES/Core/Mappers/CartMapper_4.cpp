//
//  CartMapper_4.cpp
//  NES
//
//  Created by Richard Wallis on 18/12/2022.
//

#include "CartMapper_4.h"

CartMapper_4::CartMapper_4(IOBus& bus, uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize)
: Mapper(bus, pPrg, nProgramSize, pChr, nCharacterSize)
, m_bankSelect(0)
, m_bankData(0)
, m_mirror(0)
, m_prgRamProtect(0)
, m_prgBank0(nullptr)
, m_prgBank1(nullptr)
, m_prgBank2(nullptr)
, m_prgBank3(nullptr)
, m_chrBank0(nullptr)
, m_chrBank1(nullptr)
, m_chrBank2(nullptr)
, m_chrBank3(nullptr)
, m_chrBank4(nullptr)
, m_chrBank5(nullptr)
{
    // Point the bank pointers somewhere useful for startup - especially the last prg bank for reset vectors
    uint32_t lastBankAddress = m_nProgramSize - 0x2000;
    m_prgBank0 = m_prgBank1 = m_prgBank2 = m_prgBank3 = &m_pPrg[lastBankAddress];
    
    if(nCharacterSize == 0)
    {
        *(volatile char*)(0) = 'M' | 'M' | 'C'; // CHRRAM is untested and there could be combinations of both CHR ROM and CHR RAM
        m_pChr = m_cartCHRRAM;
        m_nCharacterSize = nChrRAMSize;
    }
    
    m_chrBank0 = m_chrBank1 = m_chrBank2 = m_chrBank3 = m_chrBank4 = m_chrBank5 = m_pChr;
}

uint8_t CartMapper_4::cpuRead(uint16_t address)
{
    if(address >= 0x6000 && address <= 0x7FFF)
    {
        return m_cartPRGRAM[address - 0x6000];
    }
    else if(address >= 0x8000 && address <= 0x9FFF)
    {
        return m_prgBank0[address - 0x8000];
    }
    else if(address >= 0xA000 && address <= 0xBFFF)
    {
        return m_prgBank1[address - 0xA000];
    }
    else if(address >= 0xC000 && address <= 0xDFFF)
    {
        return m_prgBank2[address - 0xC000];
    }
    else if(address >= 0xE000 && address <= 0xFFFF)
    {
        return m_prgBank3[address - 0xE000];
    }
    return 0;
}

void CartMapper_4::cpuWrite(uint16_t address, uint8_t byte)
{
    if(address >= 0x6000 && address <= 0x7FFF)
    {
        m_cartPRGRAM[address - 0x6000] = byte;
    }
    else if(address >= 0x8000 && address <= 0x9FFF)
    {
        // memory mapping
        if((address & 1) == 0)
        {
            // TODO
            m_bankSelect = byte;
        }
        else
        {
            // TODO
            m_bankData = byte;
        }
    }
    else if(address >= 0xA000 && address <= 0xBFFF)
    {
        // mirroring
        if((address & 1) == 0)
        {
            m_mirror = byte;
            uint8_t mirrorMode = byte & 1;
            if(mirrorMode == 0)
            {
                m_bus.SetMirrorMode(VRAM_MIRROR_V);
            }
            else if(mirrorMode == 1)
            {
                m_bus.SetMirrorMode(VRAM_MIRROR_H);
            }
        }
        else
        {
            // TODO: ram protect
        }
    }
    
    //TODO: scanline counting
}

uint8_t CartMapper_4::ppuRead(uint16_t address)
{
    if(address >= 0x0000 && address <= 0x07FF)
    {
        return m_chrBank0[address - 0x0000];
    }
    else if(address >= 0x0800 && address <= 0x0FFF)
    {
        return m_chrBank1[address - 0x0800];
    }
    else if(address >= 0x1000 && address <= 0x13FF)
    {
        return m_chrBank2[address - 0x1000];
    }
    else if(address >= 0x1400 && address <= 0x17FF)
    {
        return m_chrBank3[address - 0x1400];
    }
    else if(address >= 0x1800 && address <= 0x1BFF)
    {
        return m_chrBank4[address - 0x1800];
    }
    else if(address >= 0x1C00 && address <= 0x1FFF)
    {
        return m_chrBank5[address - 0x1C00];
    }
    return 0;
}

void CartMapper_4::ppuWrite(uint16_t address, uint8_t byte)
{
    if(address >= 0x0000 && address <= 0x07FF)
    {
        m_chrBank0[address - 0x0000] = byte;
    }
    else if(address >= 0x0800 && address <= 0x0FFF)
    {
        m_chrBank1[address - 0x0800] = byte;
    }
    else if(address >= 0x1000 && address <= 0x13FF)
    {
        m_chrBank2[address - 0x1000] = byte;
    }
    else if(address >= 0x1400 && address <= 0x17FF)
    {
        m_chrBank3[address - 0x1400] = byte;
    }
    else if(address >= 0x1800 && address <= 0x1BFF)
    {
        m_chrBank4[address - 0x1800] = byte;
    }
    else if(address >= 0x1C00 && address <= 0x1FFF)
    {
        m_chrBank5[address - 0x1C00] = byte;
    }
}
