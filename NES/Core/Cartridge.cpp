//
//  Cartridge.cpp
//  NES
//
//  Created by richardwallis on 15/11/2022.
//  Copyright © 2022 openplanetsoftware. All rights reserved.
//

#include "Cartridge.h"
#include <string>

Cartridge::Cartridge(uint8_t const* pPakData, uint8_t nPakPrgCount, uint8_t nPakChrCount)
: m_pPakData(nullptr)
, m_pPrg(nullptr)
, m_pChr(nullptr)
{
    const uint32_t nProgramSize = (16384 * (uint32_t)nPakPrgCount);
    const uint32_t nCharacterSize = (8192 * (uint32_t)nPakChrCount);
    const uint32_t nCartDataSize = nProgramSize + nCharacterSize;
                                
    m_pPakData = new uint8_t[nCartDataSize];
    m_pPrg = m_pPakData + 0;
    m_pChr = m_pPakData + nProgramSize;
    
    memcpy(m_pPakData, pPakData, nCartDataSize);
}

Cartridge::~Cartridge()
{
    if(m_pPakData != nullptr)
    {
        delete [] m_pPakData;
        m_pPakData = nullptr;
    }
    
    m_pPrg = nullptr;
    m_pChr = nullptr;
}

uint8_t Cartridge::cpuRead(uint16_t address)
{
    if(m_pPrg != nullptr)
    {
        if(address >= 0x4020 && address <= 0x7FFF)
        {
            // 
        }
        else if(address >= 0x8000 && address <= 0xFFFF)
        {
            uint32_t cartAddress = address - 0x8000;
            return m_pPrg[cartAddress];
        }
        // mapper set interuppt vectors
//        else if(address >= 0xFFFA && address < 0xFFFB)
//        {
//            // NMI
//        }
//        else if(address >= 0xFFFA && address < 0xFFFB)
//        {
//            // Reset
//        }
//        else if(address >= 0xFFFA && address < 0xFFFB)
//        {
//            // IRQ/BRK vector
//        }
    }
    return 0;
}
    
void Cartridge::cpuWrite(uint16_t address, uint8_t byte)
{
    if(address >= 0x8000 && address <= 0xFFFF)
    {
        // NOP if rom
        //uint32_t cartAddress = address - 0x8000;
        //m_pPrg[cartAddress] = byte;
    }
    // mapper set interupt vectors
//    else if(address >= 0xFFFA && address < 0xFFFB)
//    {
//        // NMI
//    }
//    else if(address >= 0xFFFA && address < 0xFFFB)
//    {
//        // Reset
//    }
//    else if(address >= 0xFFFA && address < 0xFFFB)
//    {
//        // IRQ/BRK
//    }
}

uint8_t Cartridge::ppuRead(uint16_t address)
{
    if(m_pChr != nullptr)
    {
        // TODO absolute or local space address
        return m_pChr[address];
    }
    return 0;
}

void Cartridge::ppuWrite(uint16_t address, uint8_t byte)
{
    
}
