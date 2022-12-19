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
{}

uint8_t CartMapper_4::cpuRead(uint16_t address)
{
    if(address >= 0x6000 && address <= 0x7FFF)
    {
        return m_cartCHRRAM[address - 0x6000];
    }
    else if(address >= 0x8000 && address <= 0x9FFF)
    {
        // CPU $8000-$9FFF (or $C000-$DFFF): 8 KB switchable PRG ROM bank
    }
    else if(address >= 0xA000 && address <= 0xBFFF)
    {
        // CPU $A000-$BFFF: 8 KB switchable PRG ROM bank
    }
    else if(address >= 0xC000 && address <= 0xDFFF)
    {
        //CPU $C000-$DFFF (or $8000-$9FFF): 8 KB PRG ROM bank, fixed to the second-last bank
    }
    else if(address >= 0xE000 && address <= 0xFFFF)
    {
        //CPU $E000-$FFFF: 8 KB PRG ROM bank, fixed to the last bank
    }
    return 0;
}

void CartMapper_4::cpuWrite(uint16_t address, uint8_t byte)
{
    if(address >= 0x6000 && address <= 0x7FFF)
    {
        m_cartCHRRAM[address - 0x6000] = byte;
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
            // TODO ram protect
        }
    }
    
    //scanline counting
}

uint8_t CartMapper_4::ppuRead(uint16_t address)
{
    // TODO
    return 0;
}

void CartMapper_4::ppuWrite(uint16_t address, uint8_t byte)
{
    // TODO
}