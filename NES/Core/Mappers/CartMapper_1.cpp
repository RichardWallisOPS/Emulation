//
//  CartMapper_1.cpp
//  NES
//
//  Created by Richard Wallis on 15/12/2022.
//

#include "CartMapper_1.h"

CartMapper_1::CartMapper_1(IOBus& bus,
                            uint8_t* pPrg, uint32_t nProgramSize,
                            uint8_t* pChr, uint32_t nCharacterSize,
                            uint32_t nPrgRamSize, uint32_t nNVPrgRamSize,
                            uint32_t nChrRamSize, uint32_t nChrNVRamSize)
: Mapper(bus, pPrg, nProgramSize, pChr, nCharacterSize, nPrgRamSize, nNVPrgRamSize, nChrRamSize, nChrNVRamSize)
, m_shiftRegister(0)
, m_shiftCount(0)
, m_ctrl(0)
, m_chrBank0(0)
, m_chrBank1(0)
, m_prgBank(0)
{
    m_prgBank = 0x06;
    m_chrBank0 = 0x00;
    m_ctrl = 0b11111;
    
    // Using 8k CHR RAM
    if(nCharacterSize == 0)
    {
        m_pChr = m_cartCHRRAM;
    }
}

uint8_t CartMapper_1::cpuRead(uint16_t address)
{
    if(address >= 0x6000 && address <= 0x7fff)
    {
        // current 8k ram bank of max 32k
        // TODO: handle PrgRam size detection and bank switching
        return m_cartPRGRAM[address - 0x6000];
    }
    else
    {
        uint8_t progBankMode = (m_ctrl >> 2) & 0b11;
        
        // TODO: check masking out of high bit
        uint8_t progBank = m_prgBank & 0b01111;
        
        if(progBankMode == 0 || progBankMode == 1)
        {
            // 32k at 0x8000 - ignore low bit of control
            if(address >= 0x8000 && address <= 0xFFFF)
            {
                progBank = progBank >> 1; // TODO: Untested
                uint32_t bankAddress = (progBank * 0x8000) + (address - 0x8000);
            
                return m_pPrg[bankAddress];
            }
        }
        else if(progBankMode == 2)
        {
            // fix first bank at $8000 and switch 16 KB bank at $C000
            if(address >= 0x8000 && address <= 0xBFFF)
            {
                uint32_t bankAddress = address - 0x8000;
                return m_pPrg[bankAddress];
            }
            else if(address >= 0xC000 && address <= 0xFFFF)
            {
                 uint32_t bankAddress = (progBank * 0x4000) + (address - 0xC000);
                 return m_pPrg[bankAddress];
            }
        }
        else if(progBankMode == 3)
        {
            // fix last bank at $C000 and switch 16 KB bank at $8000
            if(address >= 0x8000 && address <= 0xBFFF)
            {
                uint32_t bankAddress = (progBank * 0x4000) + address - 0x8000;
                return m_pPrg[bankAddress];
            }
            else if(address >= 0xC000 && address <= 0xFFFF)
            {
                uint32_t bankAddress = (m_nProgramSize - 0x4000) + address - 0xC000;
                return m_pPrg[bankAddress];
            }
        }
    }
    
    return 0x00;
}
    
void CartMapper_1::cpuWrite(uint16_t address, uint8_t byte)
{
    if(address >= 0x6000 && address <= 0x7FFF)
    {
        // current 8k ram bank of max 32k
        m_cartPRGRAM[address - 0x6000] = byte;
    }
    else if(address >= 0x8000 && address <= 0xFFFF)
    {
        // shift register to control internal registers
        if((byte & (1 << 7)) != 0)
        {
            m_shiftCount = 0;
            m_shiftRegister = 0;
        }
        else
        {
            ++m_shiftCount;
            
            m_shiftRegister >>= 1;
            m_shiftRegister |= (byte & 1) << 4;
            
            if(m_shiftCount >= 5)
            {
                m_shiftRegister &= 0b11111;
                
                // once the last bit has been shifted in place
                // the address controls the target register
                if(address >= 0x8000 && address <= 0x9FFF)
                {
                    m_ctrl = m_shiftRegister;
                    
                    // 0 = one screen - lower, 1 = one screen upper
                    // 2 = vertical, 3 = horizontal
                    uint8_t mirrorMode = m_ctrl & 0b11;
                    if(mirrorMode == 0)
                    {
                        //TODO
                         m_bus.SetMirrorMode(VRAM_MIRROR_V);
                    }
                    else if(mirrorMode == 1)
                    {
                        // TODO
                         m_bus.SetMirrorMode(VRAM_MIRROR_V);
                    }
                    else if(mirrorMode == 2)
                    {
                        m_bus.SetMirrorMode(VRAM_MIRROR_V);
                    }
                    else if(mirrorMode == 3)
                    {
                        m_bus.SetMirrorMode(VRAM_MIRROR_H);
                    }
                }
                else if(address >= 0xA000 && address <= 0xBFFF)
                {
                    m_chrBank0 = m_shiftRegister;
                }
                else if(address >= 0xC000 && address <= 0xDFFF)
                {
                    m_chrBank1 = m_shiftRegister;
                }
                else if(address >= 0xE000 && address <= 0xFFFF)
                {
                    m_prgBank = m_shiftRegister;
                }
                
                m_shiftCount = 0;
                m_shiftRegister = 0;
            }
        }
    }
}

uint8_t CartMapper_1::ppuRead(uint16_t address)
{
    uint8_t chrBankMode = (m_ctrl >> 4) & 1;
    
    if(chrBankMode == 1)
    {
        // 2x 4k banks
        if(address >= 0x0000 && address <= 0x0FFF)
        {
            uint32_t bankAddress = (m_chrBank0 * 0x1000) + address;
            return m_pChr[bankAddress];
        }
        else if(address >= 0x1000 && address <= 0x1FFF)
        {
            uint32_t bankAddress = (m_chrBank1 * 0x1000) + (address - 0x1000);
            return m_pChr[bankAddress];
        }
    }
    else if(chrBankMode == 0)
    {
        // 1x 8k chunk
        if(address >= 0x0000 && address <= 0x1FFF)
        {
            uint32_t bankAddress = ((m_chrBank0 >> 1) * 0x2000) + address;
            return m_pChr[bankAddress];
        }
    }
    
    return 0;
}

void CartMapper_1::ppuWrite(uint16_t address, uint8_t byte)
{
    uint8_t chrBankMode = (m_ctrl >> 4) & 1;
    
    if(chrBankMode == 1)
    {
        // 2x 4k banks
        if(address >= 0x0000 && address <= 0x0FFF)
        {
            uint32_t bankAddress = (m_chrBank0 * 0x1000) + address;
            m_pChr[bankAddress] = byte;
        }
        else if(address >= 0x1000 && address <= 0x1FFF)
        {
            uint32_t bankAddress = (m_chrBank1 * 0x1000) + (address - 0x1000);
            m_pChr[bankAddress] = byte;
        }
    }
    else if(chrBankMode == 0)
    {
        // 1x 8k chunk
        if(address >= 0x0000 && address <= 0x1FFF)
        {
            uint32_t bankAddress = ((m_chrBank0 >> 1) * 0x2000) + address;
            m_pChr[bankAddress] = byte;
        }
    }
}
