//
//  CartMapper_1.cpp
//  NES
//
//  Created by Richard Wallis on 15/12/2022.
//

#include "CartMapper_1.h"

CartMapper_1::CartMapper_1(IOBus& bus,uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize)
: Mapper(bus, pPrg, nProgramSize, pChr, nCharacterSize)
, m_shiftRegister(0)
, m_shiftCount(0)
, m_ctrl(0)
, m_chrBank0(0)
, m_chrBank1(0)
, m_prgBank(0)
{}

uint8_t CartMapper_1::cpuRead(uint16_t address)
{
    return 0x00;
}
    
void CartMapper_1::cpuWrite(uint16_t address, uint8_t byte)
{
    if(address >= 0x6000 && address <= 0x7FFF)
    {
        // current 8k ram bank of max 32k
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
                // once the last bit has been shifted in place
                // the address controls the target register
                if(address >= 0x8000 && address <= 0x9FFF)
                {
                    m_ctrl = m_shiftRegister;
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
   return 0;
}

void CartMapper_1::ppuWrite(uint16_t address, uint8_t byte)
{
    
}
