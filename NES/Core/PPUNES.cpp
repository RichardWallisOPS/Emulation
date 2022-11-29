//
//  PPUNES.cpp
//  NES
//
//

#include "PPUNES.h"
#include <stdio.h>
#include <string.h>

// PPU memory map
// Range        Size    Description
// $0000-$0FFF 	$1000 	Pattern table 0 - cart
// $1000-$1FFF 	$1000 	Pattern table 1 - cart
// $2000-$23FF 	$0400 	Nametable 0
// $2400-$27FF 	$0400 	Nametable 1
// $2800-$2BFF 	$0400 	Nametable 2
// $2C00-$2FFF 	$0400 	Nametable 3
// $3000-$3EFF 	$0F00 	Mirrors of $2000-$2EFF
// $3F00-$3F1F 	$0020 	Palette RAM indexes
// $3F20-$3FFF 	$00E0 	Mirrors of $3F00-$3F1F

PPUNES::PPUNES(IOBus& bus)
: m_bus(bus)
, m_portLatch(0)
, m_tickCount(0)
, m_scanline(0)
, m_scanlineDot(0)
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
    // TODO
}

void PPUNES::Reset()
{
    m_portLatch = 0;
    m_tickCount = 0;
    m_scanline = 0;
    m_scanlineDot = 0;
    // TODO
}

void PPUNES::Tick()
{
    //TODO scanline and dot updates

    // update next dot positon and scanline
    ++m_tickCount;
    ++m_scanlineDot;
    
    if(m_scanlineDot > 340)
    {
        m_scanlineDot = 0;
        ++m_scanline;
        
        if(m_scanline > 261)
        {
            m_scanline = 0;
        }
    }
}

uint8_t PPUNES::cpuRead(uint16_t address)
{
    uint8_t data = 0;
    if(address < PortRegister_Count)
    {
        switch(address)
        {
            case PPUCTRL:
                data = m_portLatch;
                break;
            case PPUMASK:
                data = m_portLatch;
                break;
            case PPUSTATUS:
                data = m_portLatch = m_portRegisters[address];
                break;
            case OAMADDR:
                data = m_portLatch;
                break;
            case OAMDATA:
                data = m_portLatch = m_portRegisters[address];
                break;
            case PPUSCROLL:
                data = m_portLatch;;
                break;
            case PPUADDR:
                data = m_portLatch;
                break;
            case PPUDATA:
                data = m_portLatch = m_portRegisters[address];
                break;
        }
    }
    return data;
}

void PPUNES::cpuWrite(uint16_t address, uint8_t byte)
{
    if(address < PortRegister_Count)
    {
        m_portLatch = byte;
        switch(address)
        {
            case PPUCTRL:
                m_portRegisters[address] = byte;
                break;
            case PPUMASK:
                m_portRegisters[address] = byte;
                break;
            case PPUSTATUS:
                // read only
                break;
            case OAMADDR:
                m_portRegisters[address] = byte;
                break;
            case OAMDATA:
                m_portRegisters[address] = byte;
                break;
            case PPUSCROLL:
                m_portRegisters[address] = byte;
                break;
            case PPUADDR:
                m_portRegisters[address] = byte;
                break;
            case PPUDATA:
                m_portRegisters[address] = byte;
                break;
        }
    }
}

void PPUNES::WritePatternTables(uint32_t* pOutputData)
{
//    DCBA98 76543210
//    ---------------
//    0HRRRR CCCCPTTT
//    |||||| |||||+++- T: Fine Y offset, the row number within a tile
//    |||||| ||||+---- P: Bit plane (0: "lower"; 1: "upper")
//    |||||| ++++----- C: Tile column
//    ||++++---------- R: Tile row
//    |+-------------- H: Half of pattern table (0: "left"; 1: "right")
//    +--------------- 0: Pattern table is at $0000-$1FFF

    // Assumes a 256px  source texture width
    // Grey scaleoutput
    auto fnDrawPattenTable = [&](uint32_t* pOutputData, uint32_t xOffset, uint32_t yOffset, uint16_t baseAddress)
    {
        uint32_t colourLUT[4] = {0x00000000,  0xff555555,  0xffAAAAAA,  0xffFFFFff};
        
        for(uint32_t tileX = 0;tileX < 16;++tileX)
        {
            for(uint32_t tileY = 0;tileY < 16;++tileY)
            {
                uint16_t tileAddress = baseAddress + (((tileY * 16) + tileX) * 16);
                
                for(uint32_t pX = 0;pX < 8;++pX)
                {
                    for(uint32_t pY = 0;pY < 8;++pY)
                    {
                        uint8_t plane0 = m_bus.ppuRead(tileAddress + pY);
                        uint8_t plane1 = m_bus.ppuRead(tileAddress + pY + 8);
                        
                        uint8_t pixel0 = (plane0 >> (7 - pX)) & 1;
                        uint8_t pixel1 = (plane1 >> (7 - pX)) & 1;

                        uint8_t pixelColourLUT = pixel0 | (pixel1 << 1);
                        uint32_t pixelIndex = ((yOffset * 256) + xOffset) + (((tileY * 8) + pY) * 256) + ((tileX * 8) + pX);

                        pOutputData[pixelIndex] = colourLUT[pixelColourLUT];
                    }
                }
            }
        }
    };
    
    fnDrawPattenTable(pOutputData, 0, 0, 0x0000);
    fnDrawPattenTable(pOutputData, 128, 0, 0x1000);
}
