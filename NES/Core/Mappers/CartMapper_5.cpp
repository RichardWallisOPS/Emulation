//
//  CartMapper_5.cpp
//  NES
//
//  Created by Richard Wallis on 20/01/2023.
//

#include "CartMapper_5.h"

void CartMapper_5::Initialise()
{

}

void CartMapper_5::Load(Archive& rArchive)
{

}

void CartMapper_5::Save(Archive& rArchive) const
{

}

uint8_t CartMapper_5::cpuRead(uint16_t address)
{
    if(address == 0x5204)
    {
        // Scanline IRQ Status read
    }
    else if(address == 0x5205)
    {
        // Unsigned 8x8 to 16 Multiplier read
    }
    else if(address == 0x5206)
    {
        // Unsigned 8x8 to 16 Multiplier read
    }
    else if(address >= 0x6000 && address <= 0x7FFF && m_pCartPRGRAM != nullptr)
    {
        return m_prgBank0[address - 0x6000]; // TODO RAM ranges
    }
    else if(address >= 0x8000 && address <= 0x9FFF)
    {
        return m_prgBank1[address - 0x8000];
    }
    else if(address >= 0xA000 && address <= 0xBFFF)
    {
        return m_prgBank2[address - 0xA000];
    }
    else if(address >= 0xC000 && address <= 0xDFFF)
    {
        return m_prgBank3[address - 0xC000];
    }
    else if(address >= 0xE000 && address <= 0xFFFF)
    {
        return m_prgBank4[address - 0xE000];
    }
    return 0x00;
}

void CartMapper_5::cpuWrite(uint16_t address, uint8_t byte)
{
    if(address >= 0x5000 && address <= 0x5015)
    {
        // Audio
    }
    if(address == 0x5100)
    {
        // prg mode
    }
    else if(address == 0x5101)
    {
        // chr mode
    }
    else if(address == 0x5102)
    {
        // prg ram protect 1
    }
    else if(address == 0x5103)
    {
        // prg ram protect 2
    }
    else if(address == 0x5104)
    {
        // Internal Extended RAM mode 1024bytes
    }
    else if(address == 0x5105)
    {
        // Nametable mapping
    }
    else if(address == 0x5106)
    {
        // Fill mode tile
    }
    else if(address == 0x5107)
    {
        // Fill mode colour
    }
    else if(address >= 0x5113 && address <= 0x5117)
    {
        // Prg bank switching
    }
    else if(address >= 0x5120 && address <= 0x5130)
    {
        // Prg bank switching
    }
    else if(address == 0x5200)
    {
        // vertical split mode
    }
    else if(address == 0x5201)
    {
        // vertical split scroll
    }
    else if(address == 0x5202)
    {
        // vertical split bank
    }
    else if(address == 0x5203)
    {
        // IRQ Scanline compare
    }
    else if(address == 0x5204)
    {
        // Scanline IRQ Status write
    }
    else if(address == 0x5205)
    {
        // Unsigned 8x8 to 16 Multiplier write
    }
    else if(address == 0x5206)
    {
        // Unsigned 8x8 to 16 Multiplier write
    }
    else  if(address >= 0x6000 && address <= 0x7FFF && m_pCartPRGRAM != nullptr)
    {
        m_pCartPRGRAM[address - 0x6000] = byte; // TODO RAM ranges
    }
}

uint8_t CartMapper_5::ppuRead(uint16_t address)
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
    return 0x00;
}

void CartMapper_5::ppuWrite(uint16_t address, uint8_t byte)
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
