//
//  CartMapper_24.cpp
//  NES
//
//  Created by Richard Wallis on 22/01/2023.
//

#include "CartMapper_24.h"

void CartMapper_24::Initialise()
{
    m_prgBank0 = &m_pPrg[0];
    m_prgBank1 = &m_pPrg[m_nProgramSize - 0x4000];
    m_prgBank2 = &m_pPrg[m_nProgramSize - 0x2000];
    
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
    
    m_irqLatch = 0;
    m_irqMode = 0;
    m_irqEnable = 0;
    m_irqEnableAfterAck = 0;
    m_irqCounter = 0;
    m_irqPrescaler = 0;
}

void CartMapper_24::Load(Archive& rArchive)
{
    /*
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
    */
}

void CartMapper_24::Save(Archive& rArchive) const
{
    /*
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
    */
}

uint8_t CartMapper_24::cpuRead(uint16_t address)
{
    if(address >= 0x6000 && address <= 0x7FFF && m_pCartPRGRAM != nullptr)
    {
        return m_pCartPRGRAM[address - 0x6000];
    }
    else if(address >= 0x8000 && address <= 0xBFFF)
    {
        return m_prgBank0[address - 0x8000];
    }
    else if(address >= 0xC000 && address <= 0xDFFF)
    {
        return m_prgBank1[address - 0xC000];
    }
    else if(address >= 0xE000 && address <= 0xFFFF)
    {
        return m_prgBank2[address - 0xE000];
    }
    return 0;
}

void CartMapper_24::cpuWrite(uint16_t address, uint8_t byte)
{
    // Only Address lines 0-1 and 12-15 are used
    // Mask out irrelevant bits for register selecting
    uint16_t addressMask = 0b1111000000000011;
    uint16_t registerAddress = address & addressMask;

    if(address >= 0x6000 && address <= 0x7FFF && m_pCartPRGRAM != nullptr)
    {
        m_pCartPRGRAM[address - 0x6000] = byte;
    }
    else if(registerAddress >= 0x8000 && registerAddress <= 0x8003)
    {
        //16 KB switchable
        uint32_t bank = byte & 0b00001111;
        m_prgBank0 = &m_pPrg[bank * 0x4000];
    }
    else if(registerAddress >= 0xC000 && registerAddress <= 0xC003)
    {
        // 8 KB switchable
        uint32_t bank = byte & 0b00011111;
        m_prgBank1 = &m_pPrg[bank * 0x2000];
    }
    else if(registerAddress == 0xB003)
    {
        uint8_t chrBankingMode = byte & 0b11;

        if(chrBankingMode == 0)
        {
            uint8_t mirror = (byte >> 2) & 0b11;
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
#if DEBUG
        else
        {
            // Only mode 0 is supported
            *(volatile char*)(0) = 'V' | 'R' | 'C' | '6';
        }
        
        // Test any features in the upper bits that are also not supported e.g Name tables from NON-CIRAM (console nametable)
        if((byte >> 4) != 2)
        {
            *(volatile char*)(0) = 'V' | 'R' | 'C' | '6';
        }
#endif
    }
    else if(registerAddress == 0xD000)
    {
        SetChrBank(&m_chrBank0, byte);
    }
    else if(registerAddress == 0xD001)
    {
        SetChrBank(&m_chrBank1, byte);
    }
    else if(registerAddress == 0xD002)
    {
        SetChrBank(&m_chrBank2, byte);
    }
    else if(registerAddress == 0xD003)
    {
        SetChrBank(&m_chrBank3, byte);
    }
    else if(registerAddress == 0xE000)
    {
        SetChrBank(&m_chrBank4, byte);
    }
    else if(registerAddress == 0xE001)
    {
        SetChrBank(&m_chrBank5, byte);
    }
    else if(registerAddress == 0xE002)
    {
        SetChrBank(&m_chrBank6, byte);
    }
    else if(registerAddress == 0xE003)
    {
        SetChrBank(&m_chrBank7, byte);
    }
    else if(registerAddress == 0xF000)
    {
        m_irqLatch = byte;
    }
    else if(registerAddress == 0xF001)
    {
        m_irqEnableAfterAck = byte & 0b1;
        m_irqEnable = (byte >> 1) & 0b1;
        m_irqMode = (byte >> 2) & 0b1;      // 1 = M cycle (CPU tick), 0 = scanline
        m_irqPrescaler = 341;
        
        m_bus.SignalIRQ(false);
        if(m_irqEnable)
        {
            m_irqCounter = m_irqLatch;
        }
    }
    else if(registerAddress == 0xF002)
    {
        m_bus.SignalIRQ(false);
        m_irqEnable = m_irqEnableAfterAck;
    }
    else if(registerAddress >= 0x9000 && registerAddress <= 0x9002)
    {
        m_pulse1.SetRegister(registerAddress, byte);
    }
#if DEBUG
    else if(registerAddress == 0x9003)
    {
        // Frequency scaling - only 0 supported
        if(byte != 0)
        {
            *(volatile char*)(0) = 'V' | 'R' | 'C' | '6';
        }
    }
#endif
    else if(registerAddress >= 0xA000 && registerAddress <= 0xA002)
    {
        m_pulse2.SetRegister(registerAddress, byte);
    }
    else if(registerAddress >= 0xB000 && registerAddress <= 0xB002)
    {
        m_saw.SetRegister(registerAddress, byte);
    }
}

void CartMapper_24::ClockIRQCounter()
{
    if(m_irqEnable)
    {
        if(m_irqCounter == 0x00)
        {
            m_irqCounter = m_irqLatch;
            m_bus.SignalIRQ(true);
        }
        else
        {
            ++m_irqCounter;
        }
    }
}

void CartMapper_24::SystemTick(uint64_t cycleCount)
{
    const bool bCPUTick = (cycleCount % 3) == 0;
    
    if(bCPUTick)
    {
        m_pulse1.Tick();
        m_pulse2.Tick();
        m_saw.Tick();
    }
    
    if(m_irqMode == 1)
    {
        if(bCPUTick)
        {
            ClockIRQCounter();
        }
    }
    else if(m_irqEnable)
    {
        --m_irqPrescaler;
        if(m_irqPrescaler == 0)
        {
            m_irqPrescaler = 341;
            ClockIRQCounter();
        }
    }
}

float CartMapper_24::AudioOut()
{
    return m_pulse1.OutputValue() + m_pulse2.OutputValue() + m_saw.OutputValue();
}

void CartMapper_24::SetChrBank(uint8_t** pChrBank, uint8_t bank)
{
    bank = bank & ((m_nCharacterSize / 0x400) - 1);
    uint32_t bankAddress = (uint32_t(bank) * 0x400);
    *pChrBank = &m_pChr[bankAddress];
}

uint8_t CartMapper_24::ppuRead(uint16_t address)
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

void CartMapper_24::ppuWrite(uint16_t address, uint8_t byte)
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


VRC6AudioPulseChannel::VRC6AudioPulseChannel()
: m_mode(0)
, m_volume(0)
, m_duty(0)
, m_period(0)
{}

void VRC6AudioPulseChannel::Load(Archive& rArchive)
{

}

void VRC6AudioPulseChannel::Save(Archive& rArchive) const
{

}

void VRC6AudioPulseChannel::Tick()
{

}

uint8_t VRC6AudioPulseChannel::OutputValue()
{
    // 4 bit value 0 - 15 same as PPU Pulse
    return 0;
}

void VRC6AudioPulseChannel::SetRegister(uint16_t reg, uint8_t byte)
{

}


VRC6AudioSawChannel::VRC6AudioSawChannel()
{}

void VRC6AudioSawChannel::Load(Archive& rArchive)
{

}

void VRC6AudioSawChannel::Save(Archive& rArchive) const
{

}

void VRC6AudioSawChannel::Tick()
{

}

uint8_t VRC6AudioSawChannel::OutputValue()
{
    // High order 5 bits
    return 0 & 0b11111000;
}

void VRC6AudioSawChannel::SetRegister(uint16_t reg, uint8_t byte)
{

}
