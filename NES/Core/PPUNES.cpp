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

enum FlagControl : uint8_t
{
    CTRL_NAMETABLE_ADDRESS_BIT0 = 1 << 0,
    CTRL_NAMETABLE_ADDRESS_BIT1 = 1 << 1,
    
    // [bit 1 | bit 0]  - 0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00 = 0x2000 + (0x0400 * (CTRL & mask))
    CTRL_NAMETABLE_ADDRESS_MASK = CTRL_NAMETABLE_ADDRESS_BIT0 | CTRL_NAMETABLE_ADDRESS_BIT1,
    
    CTRL_VRAM_ADDRESS_INC       = 1 << 2,   // 0 = add 1, 1 = add 32, per CPU read, write PPUDATA
    CTRL_SPRITE_TABLE_ADDR      = 1 << 3,   // 0 = 0x0000, 1 = 0x1000
    CTRL_BACKGROUND_TABLE_ADDR  = 1 << 4,   // 0 = 0x0000, 1 = 0x1000
    CTRL_SPRITE_SIZE            = 1 << 5,   // 0 = 8 X 8, 1 = 8 x 16
    CTRL_MASTER_SLAVE           = 1 << 6,
    CTRL_GEN_VBLANK_NMI         = 1 << 7
};

enum FlagMask : uint8_t
{
    // TODO
};

enum FlagStatus : uint8_t
{
    STATUS_BIT0_OPEN_BUS    = 1 << 0,
    STATUS_BIT1_OPEN_BUS    = 1 << 1,
    STATUS_BIT2_OPEN_BUS    = 1 << 2,
    STATUS_BIT3_OPEN_BUS    = 1 << 3,
    STATUS_BIT4_OPEN_BUS    = 1 << 4,
    STATUS_SPRITE_OVERFLOW  = 1 << 5,
    STATUS_SPRITE0_HIT      = 1 << 6,
    STATUS_VBLANK           = 1 << 7
};

PPUNES::PPUNES(IOBus& bus)
: m_bus(bus)
, m_mirrorMode(VRAM_MIRROR_H)
, m_portLatch(0)
, m_tickCount(0)
, m_scanline(0)
, m_scanlineDot(0)
, m_ppuAddress(0)
, m_ppuAddressLatch(0)
, m_ppuData(0)
, m_bgNextPattern0(0)
, m_bgNextPattern1(0)
, m_bgShift0(0)
, m_bgShift1(0)
, m_pVideoOutput(nullptr)
{
    memset(m_vram, 0x00, nVRamSize);
    memset(m_portRegisters, 0x00, PortRegister_Count);
    memset(m_primaryOAM, 0x00, sizeof(m_primaryOAM));
    memset(m_secondaryOAM, 0x00, sizeof(m_secondaryOAM));
}

PPUNES::~PPUNES()
{

}

void PPUNES::SetVideoOutputDataPtr(uint32_t* pVideoOutData)
{
    m_pVideoOutput = pVideoOutData;
}

void PPUNES::SetMirrorMode(MirrorMode mode)
{
    m_mirrorMode = mode;
}

void PPUNES::SetFlag(uint8_t flag, PortRegisterID ppuRegister)
{
    uint8_t& ppuReg = m_portRegisters[ppuRegister];
    ppuReg |= flag;
}

void PPUNES::ClearFlag(uint8_t flag, PortRegisterID ppuRegister)
{
    uint8_t& ppuReg = m_portRegisters[ppuRegister];
    ppuReg &= ~flag;
}

bool PPUNES::TestFlag(uint8_t flag, PortRegisterID ppuRegister)
{
    uint8_t& ppuReg = m_portRegisters[ppuRegister];
    return (ppuReg & flag) != 0;
}

void PPUNES::PowerOn()
{
    m_portLatch = 0;
    m_tickCount = 0;
    m_scanline = 0;
    m_scanlineDot = 0;
    
    m_ppuAddress = 0;
    m_ppuAddressLatch = 0;
    m_ppuData = 0;
    
    m_bgNextPattern0 = 0;
    m_bgNextPattern1 = 0;
    m_bgShift0 = 0;
    m_bgShift1 = 0;
    
    memset(m_vram, 0x00, nVRamSize);
    memset(m_portRegisters, 0x00, PortRegister_Count);
    memset(m_primaryOAM, 0x00, sizeof(m_primaryOAM));
    memset(m_secondaryOAM, 0x00, sizeof(m_secondaryOAM));
}

void PPUNES::Reset()
{
    m_portLatch = 0;
    m_tickCount = 0;
    m_scanline = 0;
    m_scanlineDot = 0;
    
    m_ppuAddress = 0;
    m_ppuAddressLatch = 0;
    m_ppuData = 0;
    
    m_bgNextPattern0 = 0;
    m_bgNextPattern1 = 0;
    m_bgShift0 = 0;
    m_bgShift1 = 0;
    
    memset(m_vram, 0x00, nVRamSize);
    memset(m_portRegisters, 0x00, PortRegister_Count);
    memset(m_primaryOAM, 0x00, sizeof(m_primaryOAM));
    memset(m_secondaryOAM, 0x00, sizeof(m_secondaryOAM));
}

void PPUNES::Tick()
{
    // vblank set
    if(m_scanline == 241 && m_scanlineDot == 1)
    {
        SetFlag(STATUS_VBLANK, PPUSTATUS);
        
        if(TestFlag(CTRL_GEN_VBLANK_NMI, PPUCTRL))
        {
            m_bus.SignalNMI(true);
        }
    }
    
    // clear vblank etc ready for next frame
    if(m_scanline == 261 && m_scanlineDot == 1)
    {
        ClearFlag(STATUS_VBLANK, PPUSTATUS);
        ClearFlag(STATUS_SPRITE0_HIT, PPUSTATUS);
        ClearFlag(STATUS_SPRITE_OVERFLOW, PPUSTATUS);
    }
    
    // Main update draw, 0-239 is the visible scan lines, 261 is the pre-render line
    if((m_scanline >= 0 && m_scanline <= 239) || m_scanline == 261)
    {
        if(m_scanlineDot >= 257 && m_scanlineDot <= 320)
        {
            m_portRegisters[OAMADDR] = 0;
        }
        
        if(m_scanlineDot == 65)
        {
            uint8_t spriteAddress = m_portRegisters[OAMADDR];

            uint8_t* pEvalStartAddress = &m_primaryOAM[spriteAddress];
            uint8_t* pEvalEndAddress = pEvalStartAddress + sizeof(m_primaryOAM);

            // Eval start might not be aligned - this is as per hardware - interpret as start
            OAMEntry* pStart = (OAMEntry*)pEvalStartAddress;
            uint8_t spriteEvalCount = (pEvalEndAddress - pEvalStartAddress) / sizeof(m_primaryOAM);
            
            // TODO sprite evaluation
        }
        
        // Output current pixel
        if(m_scanline >= 0 && m_scanline <= 239 && m_scanlineDot >= 1 && m_scanlineDot <= 256)
        {
            GenerateVideoPixel();
        }
    }

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

void PPUNES::GenerateVideoPixel()
{
    // Hack some test output
    uint32_t colourLUT[4] = {0x00000000,  0xff555555,  0xffAAAAAA,  0xffFFFFff};
    if(m_pVideoOutput != nullptr)
    {
        // nametable byte
        // attribute table byte
        // pattern table low
        // pattern table high
        uint16_t baseAddress = 0x0000;
        if(TestFlag(CTRL_BACKGROUND_TABLE_ADDR, PPUCTRL))
        {
            baseAddress = 0x1000;
        }
        
        uint16_t y = m_scanline;
        uint16_t x = m_scanlineDot - 1;
        uint16_t tileY = y / 8;
        uint16_t tileX = x / 8;
        uint16_t nametableIndex = tileY * 32 + tileX;
        uint8_t tileIndex = m_vram[nametableIndex];
        
        uint16_t tileAddress = baseAddress + (uint16_t(tileIndex) * 16);
        
        uint16_t pY = y % 8;
        uint16_t pX = x % 8;
        
        uint8_t plane0 = m_bus.ppuRead(tileAddress + pY);
        uint8_t plane1 = m_bus.ppuRead(tileAddress + pY + 8);
        
        uint8_t pixel0 = (plane0 >> (7 - pX)) & 1;
        uint8_t pixel1 = (plane1 >> (7 - pX)) & 1;

        uint8_t pixelColourLUT = pixel0 | (pixel1 << 1);
        uint32_t pixelIndex = y * 256 + x;

        m_pVideoOutput[pixelIndex] = colourLUT[pixelColourLUT];
    }
}

uint16_t PPUNES::memoryAddressToNameTableOffset(uint16_t address)
{
    if(address >= 0x2000 && address <= 0x2FFF)
    {
        if(m_mirrorMode == VRAM_MIRROR_H)
        {
            if(address >= 0x2400 && address <= 0x2BFF)
            {
                address -= 0x0400;
            }
            else if(address >= 0x2C00 && address <= 0x2FFF)
            {
                address -= 0x0800;
            }
            address -= 0x2000;
        }
        else if(m_mirrorMode == VRAM_MIRROR_V)
        {
            if(address >= 0x2800 && address <= 0x2FFF)
            {
                address -= 0x0800;
            }
            address -= 0x2000;
        }
    }
    return address;
}

uint8_t PPUNES::cpuRead(uint8_t port)
{
    uint8_t data = 0;
    if(port < PortRegister_Count)
    {
        switch(port)
        {
            case PPUCTRL:
                data = m_portLatch;
                break;
            case PPUMASK:
                data = m_portLatch;
                break;
            case PPUSTATUS:
            {
                data = m_portLatch = m_portRegisters[port];
                ClearFlag(STATUS_VBLANK, PPUSTATUS);
                m_ppuAddress = 0;
                m_ppuAddressLatch = 0;
                break;
            }
            case OAMADDR:
                data = m_portLatch;
                break;
            case OAMDATA:
            {
                uint8_t spriteAddress = m_portRegisters[OAMADDR];
                data = m_primaryOAM[spriteAddress];
                m_portRegisters[port] = m_portLatch = data;
                break;
            }
            case PPUSCROLL:
                data = m_portLatch;
                break;
            case PPUADDR:
                data = m_portLatch;
                break;
            case PPUDATA:
            {
                uint16_t memAddress = m_ppuAddress;
                
                // Mirror VRAM ranges
                memAddress = (memAddress % 0x3FFF);
                if(memAddress >= 0x3000 && memAddress <= 0x3EFF)
                {
                    memAddress -= 0x1000;
                }
                
                if(memAddress >= 0 && memAddress <= 0x1FFF)
                {
                    // cart pattern table
                    data = m_bus.ppuRead(memAddress);
                }
                else if(memAddress >= 0x2000 && memAddress <= 0x2FFF)
                {
                    // name table mirrors
                    uint16_t offset = memoryAddressToNameTableOffset(memAddress);
                    data = m_vram[offset];
                }
                else if(memAddress >= 0x3F00 && memAddress <= 0x3FFF)
                {
                    uint16_t palletteIndex = (memAddress - 0x3F00) % nPalletteSize;
                    data = m_pallette[palletteIndex];
                }
                
                if(TestFlag(CTRL_VRAM_ADDRESS_INC, PPUCTRL) == false)
                {
                    m_ppuAddress += 1;
                }
                else
                {
                    m_ppuAddress += 32;
                }
                
                m_portRegisters[port] = m_portLatch = data;
                break;
            }
        }
    }
    return data;
}

void PPUNES::cpuWrite(uint8_t port, uint8_t byte)
{
    if(port < PortRegister_Count)
    {
        uint8_t oldByte =  m_portRegisters[port];
        m_portLatch = byte;
        switch(port)
        {
            case PPUCTRL:
            {
                m_portRegisters[port] = byte;

                // if in vblank and NMI request toggled from 0 - 1 gen the NMI now
                if(TestFlag(STATUS_VBLANK, PPUSTATUS))
                {
                    if((oldByte & CTRL_GEN_VBLANK_NMI) == 0 && (byte & CTRL_GEN_VBLANK_NMI) != 0)
                    {
                        m_bus.SignalNMI(true);
                    }
                }
                break;
            }
            case PPUMASK:
                m_portRegisters[port] = byte;
                break;
            case PPUSTATUS:
                // read only
                break;
            case OAMADDR:
                m_portRegisters[port] = byte;
                break;
            case OAMDATA:
            {
                m_portRegisters[port] = byte;
                uint8_t spriteAddress = m_portRegisters[OAMADDR]++;
                m_primaryOAM[spriteAddress] = byte;
                break;
            }
            case PPUSCROLL:
                m_portRegisters[port] = byte;
                break;
            case PPUADDR:
            {
                m_portRegisters[port] = byte;
                if(m_ppuAddressLatch == 0)
                {
                    m_ppuAddress = byte;
                    m_ppuAddress <<= 8;
                }
                else
                {
                    m_ppuAddress = (m_ppuAddress & 0xFF00) | byte;
                }
                m_ppuAddressLatch = m_ppuAddressLatch == 0 ? 1 : 0;
                break;
            }
            case PPUDATA:
            {
                m_portRegisters[port] = byte;
                
                uint16_t memAddress = m_ppuAddress;
                
                // Mirror VRAM ranges
                memAddress = (memAddress % 0x3FFF);
                if(memAddress >= 0x3000 && memAddress <= 0x3EFF)
                {
                    memAddress -= 0x1000;
                }
                
                if(memAddress >= 0 && memAddress <= 0x1FFF)
                {
                    // cart pattern table
                    m_bus.ppuWrite(memAddress, byte);
                }
                else if(memAddress >= 0x2000 && memAddress <= 0x2FFF)
                {
                    // name table mirrors
                    uint16_t offset = memoryAddressToNameTableOffset(memAddress);
                    m_vram[offset] = byte;
                }
                else if(memAddress >= 0x3F00 && memAddress <= 0x3FFF)
                {
                    // pallette is not writable
                }
                
                if(TestFlag(CTRL_VRAM_ADDRESS_INC, PPUCTRL) == false)
                {
                    m_ppuAddress += 1;
                }
                else
                {
                    m_ppuAddress += 32;
                }
                break;
            }
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
