//
//  CartMapper_4.cpp
//  NES
//
//  Created by Richard Wallis on 18/12/2022.
//

#include "CartMapper_4.h"

void CartMapper_4::Initialise()
{
    m_bankSelect = 0;
    m_bankData = 0;
    m_scanlineLatch = 0;
    m_scanlineCounter = 0;
    m_scanlineEnable = 0;
    m_scanlineReload = 0;
    m_lastA12 = 0;
    m_delay = 0;
    m_cycleCount = 0;
    m_systemCycleCount = 0;

    m_prgBank0 = &m_pPrg[m_nProgramSize - 0x4000];
    m_prgBank1 = &m_pPrg[m_nProgramSize - 0x4000];
    m_prgBank2 = &m_pPrg[m_nProgramSize - 0x4000];
    m_prgBank3 = &m_pPrg[m_nProgramSize - 0x2000];
    
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
    
    // There is a bug in the IRQ handling
    // Provide a per game workaround
    // TODO: Fix this so we don't need this game "detection"
    uint32_t signature = 0;
    if(m_pPrg != nullptr && m_nProgramSize > 0)
    {
        for(uint32_t byteIdx = 0;byteIdx < m_nProgramSize;++byteIdx)
        {
            uint8_t byte = m_pPrg[byteIdx];
            signature += byte;
        }
    }
    if(m_pChr != nullptr && m_nCharacterSize > 0)
    {
        for(uint32_t byteIdx = 0;byteIdx < m_nCharacterSize;++byteIdx)
        {
            uint8_t byte = m_pChr[byteIdx];
            signature += byte;
        }
    }
    m_bIRQWorkaround = signature == 23721245; // Buckie 'O Hare
}

void CartMapper_4::Load(Archive& rArchive)
{
    rArchive >> m_bankSelect;
    rArchive >> m_bankData;
    
    {
        uint8_t* pBasePrgAddress = &m_pPrg[0];
        size_t offsetBank0 = 0;
        size_t offsetBank1 = 0;
        size_t offsetBank2 = 0;
        size_t offsetBank3 = 0;
        rArchive >> offsetBank0;
        rArchive >> offsetBank1;
        rArchive >> offsetBank2;
        rArchive >> offsetBank3;
        m_prgBank0 = pBasePrgAddress + offsetBank0;
        m_prgBank1 = pBasePrgAddress + offsetBank1;
        m_prgBank2 = pBasePrgAddress + offsetBank2;
        m_prgBank3 = pBasePrgAddress + offsetBank3;
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
        
    rArchive >> m_scanlineLatch;
    rArchive >> m_scanlineCounter;
    rArchive >> m_scanlineEnable;
    rArchive >> m_scanlineReload;
    rArchive >> m_lastA12;
    rArchive >> m_delay;
    rArchive >> m_cycleCount;
    rArchive >> m_systemCycleCount;
}

void CartMapper_4::Save(Archive& rArchive) const
{
    rArchive << m_bankSelect;
    rArchive << m_bankData;
    
    {
        uint8_t* pBasePrgAddress = &m_pPrg[0];
        size_t offsetBank0 = m_prgBank0 - pBasePrgAddress;
        size_t offsetBank1 = m_prgBank1 - pBasePrgAddress;
        size_t offsetBank2 = m_prgBank2 - pBasePrgAddress;
        size_t offsetBank3 = m_prgBank3 - pBasePrgAddress;
        rArchive << offsetBank0;
        rArchive << offsetBank1;
        rArchive << offsetBank2;
        rArchive << offsetBank3;
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

    rArchive << m_scanlineLatch;
    rArchive << m_scanlineCounter;
    rArchive << m_scanlineEnable;
    rArchive << m_scanlineReload;
    rArchive << m_lastA12;
    rArchive << m_delay;
    rArchive << m_cycleCount;
    rArchive << m_systemCycleCount;
}

uint8_t CartMapper_4::cpuRead(uint16_t address)
{
    if(address >= 0x6000 && address <= 0x7FFF && m_pCartPRGRAM != nullptr)
    {
        return m_pCartPRGRAM[address - 0x6000];
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
    if(address >= 0x6000 && address <= 0x7FFF && m_pCartPRGRAM != nullptr)
    {
        m_pCartPRGRAM[address - 0x6000] = byte;
    }
    else if(address >= 0x8000 && address <= 0x9FFF)
    {
        // memory mapping
        if((address & 1) == 0)  // even registers
        {
            if(m_mapperID == 206)
            {
                m_bankSelect = byte & 0b111;
            }
            else // Standard MMC3
            {
                m_bankSelect = byte;
            }
        }
        else                    // odd registers
        {
            if(m_mapperID == 206)
            {
                m_bankData = byte & 0b00111111;
            }
            else // Standard MMC3
            {
                m_bankData = byte;
            }
            
            uint8_t registerSelect = m_bankSelect & 0b111;
            
            // 0-5 chr select, 6-7 prg select
            if(registerSelect >= 0 && registerSelect <= 5)
            {
                uint32_t chrBankSize = 0x0400;
                uint8_t chrBankMode = (m_bankSelect >> 7) & 1;
                
                uint32_t maxBanks = (m_nCharacterSize > 0 ? m_nCharacterSize : GetChrRamSize()) / 0x400;
                uint32_t bankData = m_bankData % maxBanks;

                if(chrBankMode == 0)
                {
                    if(registerSelect == 0)
                    {
                        uint32_t bankData2K = bankData & 0b11111110;
                        m_chrBank0 = &m_pChr[bankData2K * chrBankSize];
                        m_chrBank1 = m_chrBank0 + 0x0400;
                    }
                    else if(registerSelect == 1)
                    {
                        uint32_t bankData2K = bankData & 0b11111110;
                        m_chrBank2 = &m_pChr[bankData2K * chrBankSize];
                        m_chrBank3 = m_chrBank2 + 0x0400;
                    }
                    else if(registerSelect == 2)
                    {
                        m_chrBank4 = &m_pChr[bankData * chrBankSize];
                    }
                    else if(registerSelect == 3)
                    {
                        m_chrBank5 = &m_pChr[bankData * chrBankSize];
                    }
                    else if(registerSelect == 4)
                    {
                        m_chrBank6 = &m_pChr[bankData * chrBankSize];
                    }
                    else if(registerSelect == 5)
                    {
                        m_chrBank7 = &m_pChr[bankData * chrBankSize];
                    }
                }
                else
                {
                    if(registerSelect == 0)
                    {
                        uint32_t bankData2K = bankData & 0b11111110;
                        m_chrBank4 = &m_pChr[bankData2K * chrBankSize];
                        m_chrBank5 = m_chrBank4 + 0x0400;
                    }
                    else if(registerSelect == 1)
                    {
                        uint32_t bankData2K = bankData & 0b11111110;
                        m_chrBank6 = &m_pChr[bankData2K * chrBankSize];
                        m_chrBank7 = m_chrBank6 + 0x0400;
                    }
                    else if(registerSelect == 2)
                    {
                        m_chrBank0 = &m_pChr[bankData * chrBankSize];
                    }
                    else if(registerSelect == 3)
                    {
                        m_chrBank1 = &m_pChr[bankData * chrBankSize];
                    }
                    else if(registerSelect == 4)
                    {
                        m_chrBank2 = &m_pChr[bankData * chrBankSize];
                    }
                    else if(registerSelect == 5)
                    {
                        m_chrBank3 = &m_pChr[bankData * chrBankSize];
                    }
                }
            }
            else if(registerSelect >= 6 && registerSelect <= 7)
            {
                uint32_t prgBankSize = 0x2000;
                uint32_t bankData = m_bankData & 0b00111111;
                
                if(m_mapperID == 206)
                {
                    bankData = bankData & 0b1111;
                }
                
                uint32_t maxBanks = m_nProgramSize / prgBankSize;
                bankData = bankData % maxBanks;

                uint8_t prgBankMode = (m_bankSelect >> 6) & 1;
                if(prgBankMode == 0)
                {
                    if(registerSelect == 6)
                    {
                        m_prgBank0 = &m_pPrg[bankData * prgBankSize];
                    }
                    else if(registerSelect == 7)
                    {
                        m_prgBank1 = &m_pPrg[bankData * prgBankSize];
                    }
                    m_prgBank2 = &m_pPrg[m_nProgramSize - (prgBankSize * 2)];
                    m_prgBank3 = &m_pPrg[m_nProgramSize - (prgBankSize * 1)];
                }
                else
                {
                    m_prgBank0 = &m_pPrg[m_nProgramSize - (prgBankSize * 2)];
                    if(registerSelect == 6)
                    {
                        m_prgBank2 = &m_pPrg[bankData * prgBankSize];
                    }
                    else if(registerSelect == 7)
                    {
                        m_prgBank1 = &m_pPrg[bankData * prgBankSize];
                    }
                    m_prgBank3 = &m_pPrg[m_nProgramSize - (prgBankSize * 1)];
                }
            }
        }
    }
    else if(address >= 0xA000 && address <= 0xBFFF && m_mapperID == 4)
    {
        // mirroring
        if((address & 1) == 0)  // even registers
        {
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
        else                    // odd registers
        {
            // RAM Protect
        }
    }
    else if(address >= 0xC000 && address <= 0xDFFF && m_mapperID == 4)
    {
        if((address & 1) == 0)  // even registers
        {
            // IRQ latch
            m_scanlineLatch = byte - (m_bIRQWorkaround ? 0 : 1);
        }
        else                    // odd registers
        {
            // IRQ reload
            m_scanlineReload = 1;
        }
    }
    else if(address >= 0xE000 && address <= 0xFFFF && m_mapperID == 4)
    {
        if((address & 1) == 0)  // even registers
        {
            m_scanlineEnable = 0;
        }
        else                    // odd registers
        {
            m_scanlineEnable = 1;
        }
    }
}

uint8_t CartMapper_4::ppuRead(uint16_t address)
{
    MM3Signal(address);
    
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

void CartMapper_4::ppuWrite(uint16_t address, uint8_t byte)
{
    MM3Signal(address);
    
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

void CartMapper_4::SystemTick(uint64_t cycleCount)
{
    m_systemCycleCount = cycleCount;
}

void CartMapper_4::MM3Signal(uint16_t address)
{
    if(m_mapperID == 4)
    {
        uint8_t currentA12 = (address & (1 << 12)) >> 12;
        
        // We need to know how many cycles have passes since A12 went low
        if(m_lastA12 == 1 && currentA12 == 0)
        {
            m_cycleCount = m_systemCycleCount;
        }
        
        if(m_delay > 0)
        {
            --m_delay;
            if(m_delay == 0)
            {
                if(m_scanlineEnable)
                {
                    m_bus.SignalIRQ(true);
                }
            }
        }

        // Its gone high but also have enough cycles passed
        if(m_lastA12 == 0 && currentA12 == 1 && m_systemCycleCount - m_cycleCount > 16)
        {
            if(m_scanlineReload || m_scanlineCounter == 0)
            {
                m_scanlineReload = 0;
                m_scanlineCounter = m_scanlineLatch;
            }
            else
            {
                --m_scanlineCounter;
                if(m_scanlineCounter == 0)
                {
                    m_scanlineReload = 1;
                    m_delay = 4;
                }
            }
        }
        m_lastA12 = currentA12;
    }
}
