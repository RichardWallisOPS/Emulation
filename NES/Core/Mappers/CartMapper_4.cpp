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
, m_chrBank6(nullptr)
, m_chrBank7(nullptr)
, m_scanlineLatch(0)
, m_scanlineCounter(0)
, m_scanlineEnable(0)
, m_scanlineReload(0)
, m_lastA12(0)
{
    m_prgBank0 = &m_pPrg[0];
    m_prgBank1 = &m_pPrg[0x2000];
    m_prgBank2 = &m_pPrg[m_nProgramSize - 0x4000];
    m_prgBank3 = &m_pPrg[m_nProgramSize - 0x2000];
    
    if(nCharacterSize == 0)
    {
        *(volatile char*)(0) = 'M' | 'M' | 'C' | '3'; // CHRRAM is untested and there could be combinations/interleaving of both CHR ROM and CHR RAM
        
        m_pChr = m_cartCHRRAM;
        m_nCharacterSize = nChrRAMSize;
    }
    
    m_chrBank0 = &m_pChr[0x0400 * 0];
    m_chrBank1 = &m_pChr[0x0400 * 1];
    m_chrBank2 = &m_pChr[0x0400 * 2];
    m_chrBank3 = &m_pChr[0x0400 * 3];
    m_chrBank4 = &m_pChr[0x0400 * 4];
    m_chrBank5 = &m_pChr[0x0400 * 5];
    m_chrBank6 = &m_pChr[0x0400 * 6];
    m_chrBank7 = &m_pChr[0x0400 * 7];
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
        if((address & 1) == 0)  // even registers
        {
            m_bankSelect = byte;
        }
        else                    // odd registers
        {
            m_bankData = byte;
            uint8_t registerSelect = m_bankSelect & 0b111;
            
            // 0-5 chr select, 6-7 prg select
            if(registerSelect >= 0 && registerSelect <= 5)
            {
                uint16_t chrBankSize = 0x0400;
                uint8_t chrBankMode = (m_bankSelect >> 7) & 1;
                
                uint8_t maxBanks = m_nCharacterSize / 0x0400;
                uint16_t bankData = m_bankData % maxBanks;

                if(chrBankMode == 0)
                {
                    if(registerSelect == 0)
                    {
                        uint16_t bankData2K = bankData & 0b11111110;
                        m_chrBank0 = &m_pChr[bankData2K * chrBankSize];
                        m_chrBank1 = m_chrBank0 + 0x0400;
                    }
                    else if(registerSelect == 1)
                    {
                        uint16_t bankData2K = bankData & 0b11111110;
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
                        uint16_t bankData2K = bankData & 0b11111110;
                        m_chrBank4 = &m_pChr[bankData2K * 0x0400];
                        m_chrBank5 = m_chrBank4 + 0x0400;
                    }
                    else if(registerSelect == 1)
                    {
                        uint16_t bankData2K = bankData & 0b11111110;
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
                uint16_t prgBankSize = 0x2000;
                uint16_t bankData = m_bankData & 0b00111111;
                
                uint8_t maxBanks = m_nProgramSize / prgBankSize;
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
    else if(address >= 0xA000 && address <= 0xBFFF)
    {
        // mirroring
        if((address & 1) == 0)  // even registers
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
        else                    // odd registers
        {
            // RAM Protect
        }
    }
    else if(address >= 0xC000 && address <= 0xDFFF)
    {
        if((address & 1) == 0)  // even registers
        {
            // IRQ latch
            m_scanlineLatch = byte; // TODO: Fix me: Different games need different values so I'm missing some edge cases
        }
        else                    // odd registers
        {
            // IRQ reload
            m_scanlineReload = 1;
        }
    }
    else if(address >= 0xE000 && address <= 0xFFFF)
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

void CartMapper_4::MM3Signal(uint16_t address)
{
    uint8_t currentA12 = (address & (1 << 12)) >> 12;

    if(m_lastA12 == 0 && currentA12 == 1)
    {
        if(m_scanlineReload)
        {
            m_scanlineReload = 0;
            m_scanlineCounter = m_scanlineLatch;
        }
        else
        {
            --m_scanlineCounter;

            if(m_scanlineCounter == 0)
            {
                m_bus.SignalIRQ(true);
                m_scanlineReload = 1;
            }
        }
    }
    m_lastA12 = currentA12;
}
