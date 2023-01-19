//
//  CartMapper_69.cpp
//  NES
//
//  Created by Richard Wallis on 19/01/2023.
//

#include "CartMapper_69.h"

void CartMapper_69::Initialise()
{
    m_prgBank0RAM = 0;
    m_prgBank0RAMEnabled = 0;
    
    m_prgBank0 = &m_pPrg[0];    //TODO can be RAM or ROM
    m_prgBank1 = &m_pPrg[m_nProgramSize - 0x8000];
    m_prgBank2 = &m_pPrg[m_nProgramSize - 0x6000];
    m_prgBank3 = &m_pPrg[m_nProgramSize - 0x4000];
    m_prgBank4 = &m_pPrg[m_nProgramSize - 0x2000];
    
    m_chrBank0 = &m_pChr[0x0400 * 0];
    m_chrBank1 = &m_pChr[0x0400 * 1];
    m_chrBank2 = &m_pChr[0x0400 * 2];
    m_chrBank3 = &m_pChr[0x0400 * 3];
    m_chrBank4 = &m_pChr[0x0400 * 4];
    m_chrBank5 = &m_pChr[0x0400 * 5];
    m_chrBank6 = &m_pChr[0x0400 * 6];
    m_chrBank7 = &m_pChr[0x0400 * 7];
    
    m_irqGenerate = 0;
    m_irqCounterDecrement = 0;
    m_irqCounter = 0;
}

void CartMapper_69::Load(Archive& rArchive)
{
    rArchive >> m_cmdRegister;
    rArchive >> m_paramRegister;
    rArchive >> m_prgBank0RAM;
    rArchive >> m_prgBank0RAMEnabled;
    rArchive >> m_irqGenerate;
    rArchive >> m_irqCounterDecrement;
    rArchive >> m_irqCounter;
    
    {
        uint8_t* pBasePrgAddress = &m_pPrg[0];
        size_t offsetBank0 = 0;
        size_t offsetBank1 = 0;
        size_t offsetBank2 = 0;
        size_t offsetBank3 = 0;
        size_t offsetBank4 = 0;
        rArchive >> offsetBank0;
        rArchive >> offsetBank1;
        rArchive >> offsetBank2;
        rArchive >> offsetBank3;
        rArchive >> offsetBank4;
        m_prgBank0 = pBasePrgAddress + offsetBank0;
        m_prgBank1 = pBasePrgAddress + offsetBank1;
        m_prgBank2 = pBasePrgAddress + offsetBank2;
        m_prgBank3 = pBasePrgAddress + offsetBank3;
        m_prgBank4 = pBasePrgAddress + offsetBank4;
        
        if(m_prgBank0RAM)
        {
            m_prgBank0 = m_pCartPRGRAM;
        }
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

void CartMapper_69::Save(Archive& rArchive) const
{
    rArchive << m_cmdRegister;
    rArchive << m_paramRegister;
    rArchive << m_prgBank0RAM;
    rArchive << m_prgBank0RAMEnabled;
    rArchive << m_irqGenerate;
    rArchive << m_irqCounterDecrement;
    rArchive << m_irqCounter;
    
    {
        uint8_t* pBasePrgAddress = &m_pPrg[0];
        size_t offsetBank0 = m_prgBank0 - pBasePrgAddress;  // TODO
        size_t offsetBank1 = m_prgBank1 - pBasePrgAddress;
        size_t offsetBank2 = m_prgBank2 - pBasePrgAddress;
        size_t offsetBank3 = m_prgBank3 - pBasePrgAddress;
        size_t offsetBank4 = m_prgBank4 - pBasePrgAddress;
        rArchive << offsetBank0;
        rArchive << offsetBank1;
        rArchive << offsetBank2;
        rArchive << offsetBank3;
        rArchive << offsetBank4;
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

uint8_t CartMapper_69::cpuRead(uint16_t address)
{
    if(address >= 0x6000 && address <= 0x7FFF && m_prgBank0RAM && m_prgBank0RAMEnabled && m_pCartPRGRAM == m_prgBank0)
    {
        // if ram is not enabled but bank 0 is ram then return open bus - not the ram bank data
        return m_prgBank0[address - 0x6000];
    }
    else if(address >= 0x6000 && address <= 0x7FFF && !m_prgBank0RAM)
    {
         return m_prgBank0[address - 0x6000];
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
    return 0;
}

void CartMapper_69::cpuWrite(uint16_t address, uint8_t byte)
{
    // Command $8000-$9FFF
    // Parameter A000-$BFFF
    if(address >= 0x6000 && address <= 0x7FFF && m_prgBank0RAM && m_prgBank0RAMEnabled && m_pCartPRGRAM == m_prgBank0)
    {
        m_prgBank0[address - 0x6000] = byte;
    }
    else if(address >= 0x8000 && address <= 0x9FFF)
    {
        m_cmdRegister = byte & 0b1111;
    }
    else if(address >= 0xA000 && address <= 0xBFFF)
    {
        m_paramRegister = byte;
        
        if(m_cmdRegister >= 0x0 && m_cmdRegister <= 0x7)
        {
            // $0-7 control CHR banking
            uint32_t chrBankSize = 0x0400;
            uint32_t maxBanks = m_nCharacterSize / chrBankSize;
            uint32_t bank = m_paramRegister & (maxBanks - 1);
            if(m_cmdRegister == 0x0)
            {
                m_chrBank0 = &m_pChr[chrBankSize * bank];
            }
            else if(m_cmdRegister == 0x1)
            {
                m_chrBank1 = &m_pChr[chrBankSize * bank];
            }
            else if(m_cmdRegister == 0x2)
            {
                m_chrBank2 = &m_pChr[chrBankSize * bank];
            }
            else if(m_cmdRegister == 0x3)
            {
                m_chrBank3 = &m_pChr[chrBankSize * bank];
            }
            else if(m_cmdRegister == 0x4)
            {
                m_chrBank4 = &m_pChr[chrBankSize * bank];
            }
            else if(m_cmdRegister == 0x5)
            {
                m_chrBank5 = &m_pChr[chrBankSize * bank];
            }
            else if(m_cmdRegister == 0x6)
            {
                m_chrBank6 = &m_pChr[chrBankSize * bank];
            }
            else if(m_cmdRegister == 0x7)
            {
                m_chrBank7 = &m_pChr[chrBankSize * bank];
            }
        }
        else if(m_cmdRegister >= 0x8 && m_cmdRegister <= 0xB)
        {
            // $8-B control PRG banking
            uint32_t bankSize = 0x2000;
            uint32_t maxBanks = m_nProgramSize / bankSize;
            uint32_t bank = (m_paramRegister & 0b0011111111) & (maxBanks - 1);
            if(m_cmdRegister == 0x8)
            {
                m_prgBank0RAM = (m_paramRegister >> 6) & 0b1;
                m_prgBank0RAMEnabled = (m_paramRegister >> 7) & 0b1;

                if(m_prgBank0RAM)
                {
                    m_prgBank0 = m_pCartPRGRAM;
                }
                else
                {
                    m_prgBank0 = &m_pPrg[bank * bankSize];
                }
            }
            else if(m_cmdRegister == 0x9)
            {
                m_prgBank1 = &m_pPrg[bank * bankSize];
            }
            else if(m_cmdRegister == 0xA)
            {
                m_prgBank2 = &m_pPrg[bank * bankSize];
            }
            else if(m_cmdRegister == 0xB)
            {
                m_prgBank3 = &m_pPrg[bank * bankSize];
            }
        }
        else if(m_cmdRegister == 0xC)
        {
            // $C controls nametable mirroring
            uint8_t mirror = m_paramRegister & 0b111;
            if(mirror == 0)
            {
                m_bus.SetMirrorMode(VRAM_MIRROR_V);
            }
            else if(mirror == 1)
            {
                m_bus.SetMirrorMode(VRAM_MIRROR_H);
            }
            else if(mirror == 2)
            {
                m_bus.SetMirrorMode(VRAM_MIRROR_SINGLEA);
            }
            else if(mirror == 3)
            {
                m_bus.SetMirrorMode(VRAM_MIRROR_SINGLEB);
            }
        }
        else if(m_cmdRegister >= 0xD && m_cmdRegister <= 0xF)
        {
            // $D-F controls IRQ
            if(m_cmdRegister == 0xD)
            {
                m_irqGenerate = byte & 0b1;
                m_irqCounterDecrement = (byte >> 7) & 0b1;
                m_bus.SignalIRQ(false);
            }
            else if(m_cmdRegister == 0xE)
            {
                m_irqCounter = (m_irqCounter & 0xFF00) | uint16_t(byte);
            }
            else if(m_cmdRegister == 0xF)
            {
                m_irqCounter = (uint16_t(byte) << 8) | (m_irqCounter & 0x00FF);
            }
        }
    }
    else if(address >= 0xC000 && address <= 0xDFFF)
    {
        // audio
    }
    else if(address >= 0xE000 && address <= 0xFFFF)
    {
        // audio
    }
}

void CartMapper_69::systemTick(uint64_t cycleCount)
{
    if(m_irqCounterDecrement && (cycleCount % 3) == 0)
    {
        --m_irqCounter;
        // Should be 0xFFFF - but hold it back a bit
        if(m_irqCounter == 0xFFF8 && m_irqGenerate)
        {
            m_bus.SignalIRQ(true);
        }
    }
}

uint8_t CartMapper_69::ppuRead(uint16_t address)
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

void CartMapper_69::ppuWrite(uint16_t address, uint8_t byte)
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
