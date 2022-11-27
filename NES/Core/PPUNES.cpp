//
//  PPUNES.cpp
//  NES
//
//

#include "PPUNES.h"
#include <stdio.h>
#include <string.h>

PPUNES::PPUNES(IOBus& bus)
: m_bus(bus)
{
    memset(m_vram, 0x00, nVRamSize);
    memset(m_portRegisters, 0x00, PortRegister_Count);
    memset(m_oam, 0x00, sizeof(m_oam));
    memset(m_oamScanline, 0x00, sizeof(m_oamScanline));
}

PPUNES::~PPUNES()
{

}

void PPUNES::PowerOn()
{

}

void PPUNES::Reset()
{

}

void PPUNES::Tick()
{

}

uint8_t PPUNES::cpuRead(uint16_t address)
{
    uint8_t data = 0;
    
    if(address < PortRegister_Count)
    {
        data = m_portRegisters[address];
        switch(address)
        {
            case PPUCTRL:
                break;
            case PPUMASK:
                break;
            case PPUSTATUS:
                break;
            case OAMADDR:
                break;
            case OAMDATA:
                break;
            case PPUSCROLL:
                break;
            case PPUADDR:
                break;
            case PPUDATA:
                break;
        }
    }
    
    return data;
}

void PPUNES::cpuWrite(uint16_t address, uint8_t byte)
{
    if(address < PortRegister_Count)
    {
        m_portRegisters[address] = byte;
        switch(address)
        {
            case PPUCTRL:
                break;
            case PPUMASK:
                break;
            case PPUSTATUS:
                break;
            case OAMADDR:
                break;
            case OAMDATA:
                break;
            case PPUSCROLL:
                break;
            case PPUADDR:
                break;
            case PPUDATA:
                break;
        }
    }
}
