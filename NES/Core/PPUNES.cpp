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
// $0000-$0FFF 	$1000 	Pattern table 0
// $1000-$1FFF 	$1000 	Pattern table 1
// $2000-$23FF 	$0400 	Nametable 0
// $2400-$27FF 	$0400 	Nametable 1
// $2800-$2BFF 	$0400 	Nametable 2
// $2C00-$2FFF 	$0400 	Nametable 3
// $3000-$3EFF 	$0F00 	Mirrors of $2000-$2EFF
// $3F00-$3F1F 	$0020 	Palette RAM indexes
// $3F20-$3FFF 	$00E0 	Mirrors of $3F00-$3F1F

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

void PPUNES::WritePatternTables(uint32_t* pOutputData)
{
    uint32_t colourTable[4] = {0x00000000,  0xff555555,  0xffAAAAAA,  0xffFFFFff};

    auto drawPattenTable = [&](uint32_t* pOutputData, uint32_t xOffset, uint32_t yOffset, uint16_t baseAddress)
    {
        for(uint32_t tileX = 0;tileX < 16;++tileX)
        {
            for(uint32_t tileY = 0;tileY < 16;++tileY)
            {
                uint16_t addressPlane0 = baseAddress + (((tileY * 16) + tileX) * 16);
                
                for(uint32_t pX = 0;pX < 8;++pX)
                {
                    for(uint32_t pY = 0;pY < 8;++pY)
                    {
                        uint8_t plane0 = m_bus.ppuRead(addressPlane0 + pY);
                        uint8_t plane1 = m_bus.ppuRead(addressPlane0 + pY + 8);
                        
                        uint8_t pixel0 = (plane0 >> (8 - pX)) & 1;
                        uint8_t pixel1 = (plane1 >> (8 - pX)) & 1;
                
                        uint8_t pixel = pixel0 | (pixel1 << 1);
                        // 256 is the source texture width
                        uint32_t pixelIndex = (yOffset * 256 + xOffset) + (((tileY * 8) + pY) * 256) + ((tileX * 8) + pX);
                        uint32_t colour = colourTable[pixel];
                        pOutputData[pixelIndex] = colour;
                    }
                }
            }
        }
    };
    
    drawPattenTable(pOutputData, 0, 0, 0x0000);
    drawPattenTable(pOutputData, 128, 0, 0x1000);
}
