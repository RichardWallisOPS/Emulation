//
//  Cartridge.cpp
//  NES
//
//  Created by richardwallis on 15/11/2022.
//

#include "Cartridge.h"
#include <string>

Cartridge::Cartridge(uint8_t mapperID, uint8_t const* pPakData, uint8_t nPakPrgCount, uint8_t nPakChrCount)
: m_nMapperID(mapperID)
, m_pPakData(nullptr)
, m_pPrg(nullptr)
, m_pChr(nullptr)
, m_nProgramSize(0)
, m_nCharacterSize(0)
{
    // TODO handle mapper ID - this is currently only for mapper 0
    if(m_nMapperID == 0)
    {
        m_nProgramSize = (16384 * (uint32_t)nPakPrgCount);
        m_nCharacterSize = (8192 * (uint32_t)nPakChrCount);
        
        const uint32_t nCartDataSize = m_nProgramSize + m_nCharacterSize;
        
        m_pPakData = new uint8_t[nCartDataSize];
        m_pPrg = m_pPakData + 0;
        m_pChr = m_pPakData + m_nProgramSize;
        
        memcpy(m_pPakData, pPakData, nCartDataSize);
    }
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
    m_nProgramSize = 0;
    m_nCharacterSize = 0;
}

bool Cartridge::IsValid() const
{
    // TODO
    return m_pPakData != nullptr;
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
            // mapper 0 logic
            uint32_t cartAddress = (address - 0x8000) % m_nProgramSize;
            return m_pPrg[cartAddress];
        }
    }
    return 0;
}
    
void Cartridge::cpuWrite(uint16_t address, uint8_t byte)
{
    if(m_pPrg != nullptr)
    {
        if(address >= 0x8000 && address <= 0xFFFF)
        {
            // mapper 0 logic - rom
            //uint32_t cartAddress = (address - 0x8000) % m_nProgramSize;
            //m_pPrg[cartAddress] = byte;
        }
    }
}

uint8_t Cartridge::ppuRead(uint16_t address)
{
    if(m_pChr != nullptr)
    {
        return m_pChr[address];
    }
    return 0;//address & 0xFF; // open bus low byte return
}

void Cartridge::ppuWrite(uint16_t address, uint8_t byte)
{
    if(m_pChr != nullptr)
    {
        // TODO
    }
}
