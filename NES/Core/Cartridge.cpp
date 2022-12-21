//
//  Cartridge.cpp
//  NES
//
//  Created by richardwallis on 15/11/2022.
//

#include <string>

#include "Cartridge.h"
#include "Mappers/CartMapperFactory.h"

Cartridge::Cartridge(IOBus& bus, uint8_t mapperID, MirrorMode mirrorMode, uint8_t const* pPakData, uint8_t nPakPrgCount, uint8_t nPakChrCount)
: m_pMapper(nullptr)
, m_pPakData(nullptr)
{
    memset(m_cartVRAM, 0x00, sizeof(m_cartVRAM));
    
    uint32_t nProgramSize = (16384 * (uint32_t)nPakPrgCount);
    uint32_t nCharacterSize = (8192 * (uint32_t)nPakChrCount);
    
    const uint32_t nCartDataSize = nProgramSize + nCharacterSize;
        
    m_pPakData = new uint8_t[nCartDataSize];
    memcpy(m_pPakData, pPakData, nCartDataSize);
    
    uint8_t* pPrg = m_pPakData + 0;
    uint8_t* pChr = m_pPakData + nProgramSize;

    // Mirror mode for cart wiring - mapper can override
    bus.SetMirrorMode(mirrorMode);
    
    // Mappers can do their own mirror modes
    m_pMapper = CartMapper::CreateMapper(bus, mapperID, pPrg, nProgramSize, pChr, nCharacterSize);
}

Cartridge::~Cartridge()
{
    if(m_pMapper != nullptr)
    {
        delete m_pMapper;
        m_pMapper = nullptr;
    }
    
    if(m_pPakData != nullptr)
    {
        delete [] m_pPakData;
        m_pPakData = nullptr;
    }
}

bool Cartridge::IsValid() const
{
    return m_pMapper != nullptr;
}

uint8_t Cartridge::cpuRead(uint16_t address)
{
    if(m_pMapper != nullptr)
    {
        return m_pMapper->cpuRead(address);
    }
    return 0x00;
}
    
void Cartridge::cpuWrite(uint16_t address, uint8_t byte)
{
    if(m_pMapper != nullptr)
    {
        return m_pMapper->cpuWrite(address, byte);
    }
}

uint8_t Cartridge::ppuRead(uint16_t address)
{
    if(address>= 0 && address <= 0x1FFF)
    {
        if(m_pMapper != nullptr)
        {
            return m_pMapper->ppuRead(address);
        }
    }
    else if(address >= 0x2000 && address <= 0x3EFF)
    {
        uint32_t cartAddress = (address - 0x2000) % 4096;
        return m_cartVRAM[cartAddress];
    }
    return address & 0xFF; // open bus low byte return
}

void Cartridge::ppuWrite(uint16_t address, uint8_t byte)
{
    if(address>= 0 && address <= 0x1FFF)
    {
        if(m_pMapper != nullptr)
        {
            m_pMapper->ppuWrite(address, byte);
        }
    }
    else if(address >= 0x2000 && address <= 0x3EFF)
    {
        uint32_t cartAddress = (address - 0x2000) % 4096;
        m_cartVRAM[cartAddress] = byte;
    }
}
