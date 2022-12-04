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
, m_secondaryOAMWrite(0)
, m_portLatch(0)
, m_ppuDataBuffer(0)
, m_tickCount(0)
, m_scanline(0)
, m_scanlineDot(0)
, m_ppuAddress(0)
, m_ppuTAddress(0)
, m_ppuWriteToggle(0)
, m_ppuData(0)
, m_scrollX(0)
, m_scrollY(0)
, m_bgNextPattern0(0)
, m_bgNextPattern1(0)
, m_bgShift0(0)
, m_bgShift1(0)
, m_pVideoOutput(nullptr)
{
    memset(m_vram, 0x00, nVRamSize);
    memset(m_portRegisters, 0x00, PortRegister_Count);
    memset(m_primaryOAM, 0xFF, sizeof(m_primaryOAM));
    memset(m_secondaryOAM, 0xFF, sizeof(m_secondaryOAM));
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
    m_secondaryOAMWrite = 0;
    m_portLatch = 0;
    m_ppuDataBuffer = 0;
    m_tickCount = 0;
    m_scanline = 0;
    m_scanlineDot = 0;
    
    m_ppuTAddress = 0;
    m_ppuAddress = 0;
    m_ppuWriteToggle = 0;
    m_ppuData = 0;
    m_scrollX = 0;
    m_scrollY = 0;
    
    m_bgNextPattern0 = 0;
    m_bgNextPattern1 = 0;
    m_bgShift0 = 0;
    m_bgShift1 = 0;
    
    memset(m_vram, 0x00, nVRamSize);
    memset(m_portRegisters, 0x00, PortRegister_Count);
    memset(m_primaryOAM, 0xFF, sizeof(m_primaryOAM));
    memset(m_secondaryOAM, 0xFF, sizeof(m_secondaryOAM));
}

void PPUNES::Reset()
{
    m_secondaryOAMWrite = 0;
    m_portLatch = 0;
    m_ppuDataBuffer = 0;
    m_tickCount = 0;
    m_scanline = 0;
    m_scanlineDot = 0;
    
    m_ppuTAddress = 0;
    m_ppuAddress = 0;
    m_ppuWriteToggle = 0;
    m_ppuData = 0;
    m_scrollX = 0;
    m_scrollY = 0;
    
    m_bgNextPattern0 = 0;
    m_bgNextPattern1 = 0;
    m_bgShift0 = 0;
    m_bgShift1 = 0;
    
    memset(m_vram, 0x00, nVRamSize);
    memset(m_portRegisters, 0x00, PortRegister_Count);
    memset(m_primaryOAM, 0xFF, sizeof(m_primaryOAM));
    memset(m_secondaryOAM, 0xFF, sizeof(m_secondaryOAM));
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
    
    if(m_scanline == 261)
    {
        // TODO still need to fill the shift registers
    }
    
    // Main update draw, 0-239 is the visible scan lines, 261 is the pre-render line
    if(m_scanline >= 0 && m_scanline <= 239)
    {
        if(m_scanlineDot >= 1 && m_scanlineDot <= 64)
        {
            ClearSecondaryOEM();
        }
        else if(m_scanlineDot >= 65 && m_scanlineDot <= 256)
        {
            SpriteEvaluation();
        }
        else if(m_scanlineDot >= 257 && m_scanlineDot <= 320)
        {
            m_portRegisters[OAMADDR] = 0;
            m_secondaryOAMWrite = 0;
            // TODO sprie fetch
        }
        
        // Output current pixel
        if(m_scanlineDot >= 1 && m_scanlineDot <= 256)
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

void PPUNES::ClearSecondaryOEM()
{
    if(m_scanlineDot >= 1 && m_scanlineDot <= 64)
    {
        // clearing secondary OEM 32 bytes 32 reads / 32 writes 0xFF
        if(m_scanlineDot % 2 == 0)
        {
            m_secondaryOAM[(m_scanlineDot - 1) / 2] = 0xFF;
        }
    }
}

void PPUNES::SpriteEvaluation()
{
    // odd data read even data write
    if(m_scanlineDot >= 65 && m_scanlineDot <= 256)
    {
        if(m_scanlineDot % 2 == 0)
        {
            uint8_t& spriteIndex = m_portRegisters[OAMADDR];
            uint8_t spriteTop = m_primaryOAM[spriteIndex];
            uint8_t spriteBottom = spriteTop + 8;
            
            if(m_scanline >= spriteTop && m_scanline <= spriteBottom && m_secondaryOAMWrite < 32)
            {
                m_secondaryOAM[m_secondaryOAMWrite++] = m_primaryOAM[spriteIndex + 0];
                m_secondaryOAM[m_secondaryOAMWrite++] = m_primaryOAM[spriteIndex + 1];
                m_secondaryOAM[m_secondaryOAMWrite++] = m_primaryOAM[spriteIndex + 2];
                m_secondaryOAM[m_secondaryOAMWrite++] = m_primaryOAM[spriteIndex + 3];
            }

            spriteIndex += 4;
            
            // TODO other steps for overflow etc
        }
        
        // TODO remove this!!!
        if(m_scanlineDot == 256)
        {
            memcpy(m_renderOAM, m_secondaryOAM, 32);
        }
    }
}

void PPUNES::GenerateVideoPixel()
{
    // HACK some test output
    // TODO Implement the sprite and background properly with shift registers
    
    uint16_t y = m_scanline;
    uint16_t x = m_scanlineDot - 1;
    
    uint8_t tilePalletteSelect = 0x00;
    uint8_t tileAttributePalletteSelect = 0x00;
        
    uint8_t spritePalletteSelect = 0x00;
    uint8_t spriteAttributePalletteSelect = 0x00;
    
    uint8_t spritePriority = 0;
    
    bool bSpriteZero = false;
    
    // Background
    {
        // nametable byte
        // attribute table byte
        // pattern table low
        // pattern table high
        
        // 43210
        // |||||
        // |||++- Pixel value from tile data
        // |++--- Palette number from attribute table or OAM
        // +----- Background/Sprite select
        
        uint16_t baseAddress = 0x0000;
        if(TestFlag(CTRL_BACKGROUND_TABLE_ADDR, PPUCTRL))
        {
            baseAddress = 0x1000;
        }

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

        tilePalletteSelect = pixel0 | (pixel1 << 1);
        
        uint8_t attributeX = x / 32;
        uint8_t attributeY = y / 32;
        uint8_t attributeIndex = attributeY * 8 + attributeX;
        uint8_t attribute =  m_vram[0 + 0x3C0 + attributeIndex];
        
        uint8_t attribQuadX = (x / 16) % 2;
        uint8_t attribQuadY = (y / 16) % 2;
        
        // attrib
        // 7654 3210
        // |||| ||++- Color bits 3-2 for top left quadrant of this byte
        // |||| ++--- Color bits 3-2 for top right quadrant of this byte
        // ||++------ Color bits 3-2 for bottom left quadrant of this byte
        // ++-------- Color bits 3-2 for bottom right quadrant of this byte
                
        if(attribQuadX == 0 && attribQuadY == 0)
        {
            tileAttributePalletteSelect = attribute & 0x3;
        }
        else if(attribQuadX == 1 && attribQuadY == 0)
        {
            tileAttributePalletteSelect = (attribute >> 2) & 0x3;
        }
        else if(attribQuadX == 0 && attribQuadY == 1)
        {
            tileAttributePalletteSelect = (attribute >> 4) & 0x3;
        }
        else if(attribQuadX == 1 && attribQuadY == 1)
        {
            tileAttributePalletteSelect = (attribute >> 6) & 0x3;
        }
    }
    
    // Sprite
    {
        uint16_t spriteBaseAddress = 0x0000;
        if(TestFlag(CTRL_SPRITE_TABLE_ADDR, PPUCTRL))
        {
            spriteBaseAddress = 0x1000;
        }
        
        for(uint8_t idx = 0;idx < 32;idx += 4)
        {
            uint8_t yPos = m_renderOAM[idx + 0];
            uint8_t xPos = m_renderOAM[idx + 3];
            
            yPos += 1;  // Is this right?
            
            if(yPos != 0xFF && x >= xPos && x < xPos + 8 && y >= yPos && y < yPos + 8)
            {
                uint8_t spriteTileId = m_renderOAM[idx + 1];
                uint8_t spriteAttribute = m_renderOAM[idx + 2];
                
                spriteAttributePalletteSelect = spriteAttribute & 0x3;
                
                spritePriority = (spriteAttribute & (1 << 5)) != 0 ? 0 : 1;
                                    
                bool bFlipH = (spriteAttribute & (1 << 6)) != 0;
                bool bFlipV = (spriteAttribute & (1 << 7)) != 0;
            
                uint16_t spriteTileAddress = spriteBaseAddress + (uint16_t(spriteTileId) * 16);

                uint16_t spriteAddress = spriteTileAddress + m_scanline - yPos;
                if(bFlipV)
                {
                    spriteAddress = spriteTileAddress + (7 - (m_scanline - yPos));
                }
                
                uint8_t spritePlane0 = m_bus.ppuRead(spriteAddress);
                uint8_t spritePlane1 = m_bus.ppuRead(spriteAddress + 8);
                
                uint8_t spriteShift = 7 - (x - xPos);
                if(bFlipH)
                {
                    spriteShift = x - xPos;
                }
                
                uint8_t spritePixel0 = (spritePlane0 >> spriteShift) & 1;
                uint8_t spritePixel1 = (spritePlane1 >> spriteShift) & 1;
                
                spritePalletteSelect = spritePixel0 | (spritePixel1 << 1);
                
                if(spritePalletteSelect != 0)
                {
                    // TODO rework sprite zero
                    if(idx == 0)
                    {
                        uint32_t* infoRender = (uint32_t*)&m_renderOAM[0];
                        uint32_t* infoOEM = (uint32_t*)&m_primaryOAM[0];
                        if(*infoOEM == *infoRender)
                        {
                            bSpriteZero = true;
                        }
                    }
                    break;
                }
            }
        }
    }
    
    uint8_t backgroundSelect = (0 << 4) + (tileAttributePalletteSelect << 2) + (tilePalletteSelect << 0);
    uint8_t spriteSelect = (1 << 4) + (spriteAttributePalletteSelect << 2) + (spritePalletteSelect << 0);
    
    uint8_t finalPalletteSelect = 0x00;
    
    if(tilePalletteSelect == 0 && spritePalletteSelect != 0)
    {
        finalPalletteSelect = spriteSelect;
    }
    else if(tilePalletteSelect != 0 && spritePalletteSelect == 0)
    {
        finalPalletteSelect = backgroundSelect;
    }
    else if(tilePalletteSelect != 0 && spritePalletteSelect != 0)
    {
        if(bSpriteZero)
        {
            SetFlag(STATUS_SPRITE0_HIT, PPUSTATUS);
        }
        
        if(spritePriority)
        {
            finalPalletteSelect = spriteSelect;
        }
        else
        {
            finalPalletteSelect = backgroundSelect;
        }
    }
    
    uint8_t palletteIndex = m_pallette[finalPalletteSelect];
    if(m_pVideoOutput != nullptr)
    {
        m_pVideoOutput[y * 256 + x] = GetPixelColour(palletteIndex);
    }
}

uint16_t PPUNES::absoluteAddressToVRAMAddress(uint16_t address)
{
    if(address >= 0x2000 && address <= 0x2FFF)
    {
        if(m_mirrorMode == VRAM_MIRROR_H)
        {
            if(address >= 0x2400 && address <= 0x27FF)
            {
                address -= 0x0400;
            }
            else if(address >= 0x2C00 && address <= 0x2FFF)
            {
                address -= 0x0800;
            }
        }
        else if(m_mirrorMode == VRAM_MIRROR_V)
        {
            if(address >= 0x2800 && address <= 0x2FFF)
            {
                address -= 0x0800;
            }
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
                m_ppuWriteToggle = 0;
                break;
            }
            case OAMADDR:
                data = m_portLatch;
                break;
            case OAMDATA:
            {
                uint8_t spriteAddress = m_portRegisters[OAMADDR];
                if(m_scanlineDot >= 1 && m_scanlineDot <= 64)
                {
                    // during init of sprite evaluation
                    data = 0XFF;
                }
                else
                {
                    data = m_primaryOAM[spriteAddress];
                }
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
                // return value is last read
                uint8_t readBuffer = m_ppuDataBuffer;
                
                // Mirror VRAM ranges
                uint16_t memAddress = m_ppuAddress;
                memAddress = (memAddress % 0x3FFF);
                if(memAddress >= 0x3000 && memAddress <= 0x3EFF)
                {
                    memAddress -= 0x1000;
                }
                
                if(memAddress >= 0 && memAddress <= 0x1FFF)
                {
                    // cart pattern table
                    m_ppuDataBuffer = m_bus.ppuRead(memAddress);
                }
                else if(memAddress >= 0x2000 && memAddress <= 0x2FFF)
                {
                    // name table mirrors
                    if(m_mirrorMode == VRAM_MIRROR_CART4)
                    {
                        m_ppuDataBuffer = m_bus.ppuRead(memAddress);
                    }
                    else
                    {
                        uint16_t vramAddress = absoluteAddressToVRAMAddress(memAddress);
                        uint16_t vramOffset = vramAddress - 0x2000;
                        m_ppuDataBuffer = m_vram[vramOffset];
                    }
                }
                else if(memAddress >= 0x3F00 && memAddress <= 0x3FFF)
                {
                    uint16_t palletteIndex = (memAddress - 0x3F00) % nPalletteSize;
                    m_ppuDataBuffer = m_pallette[palletteIndex];
                    readBuffer = m_ppuDataBuffer;
                }
                
                if(TestFlag(CTRL_VRAM_ADDRESS_INC, PPUCTRL) == false)
                {
                    m_ppuAddress += 1;
                }
                else
                {
                    m_ppuAddress += 32;
                }
                
                m_portRegisters[port] = m_portLatch = m_ppuDataBuffer;
                data = readBuffer;
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
                if(m_ppuWriteToggle == 0)
                {
                    m_scrollX = byte;
                }
                else
                {
                    m_scrollY = byte;
                }
                m_ppuWriteToggle = m_ppuWriteToggle == 0 ? 1 : 0;
                break;
            case PPUADDR:
            {
                m_portRegisters[port] = byte;
                if(m_ppuWriteToggle == 0)
                {
                    m_ppuAddress = byte;
                    m_ppuAddress <<= 8;
                }
                else
                {
                    m_ppuAddress = (m_ppuAddress & 0xFF00) | byte;
                }
                m_ppuWriteToggle = m_ppuWriteToggle == 0 ? 1 : 0;
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
                    if(m_mirrorMode == VRAM_MIRROR_CART4)
                    {
                        m_bus.ppuWrite(memAddress, byte);
                    }
                    else
                    {
                        uint16_t vramAddress = absoluteAddressToVRAMAddress(memAddress);
                        uint16_t vramOffset = vramAddress - 0x2000;
                        m_vram[vramOffset] = byte;
                    }
                }
                else if(memAddress >= 0x3F00 && memAddress <= 0x3FFF)
                {
                    uint16_t palletteIndex = (memAddress - 0x3F00) % nPalletteSize;
                    m_pallette[palletteIndex] = byte;
                    
                    // $3F10/$3F14/$3F18/$3F1C <-Mirror-> $3F00/$3F04/$3F08/$3F0C - mirror writes to keep read simple
                    if(palletteIndex == 0x0 || palletteIndex == 0x4 || palletteIndex == 0x8 || palletteIndex == 0xC)
                    {
                        palletteIndex += 0x10;
                        m_pallette[palletteIndex] = byte;
                    }
                    else if(palletteIndex == 0x10 || palletteIndex == 0x14 || palletteIndex == 0x18 || palletteIndex == 0x1C)
                    {
                        palletteIndex -= 0x10;
                        m_pallette[palletteIndex] = byte;
                    }
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

uint32_t PPUNES::GetPixelColour(uint32_t palletteIndex)
{
    const uint8_t Pallette[] =
    {
         84,  84,  84,    0,  30, 116,    8,  16, 144,   48,   0, 136,   68,   0, 100,   92,   0,  48,   84,   4,   0,   60,  24,   0,   32,  42,   0,    8,  58,   0,    0,  64,   0,    0,  60,   0,    0,  50,  60,    0,   0,   0,    0, 0, 0,  0, 0, 0,
        152, 150, 152,    8,  76, 196,   48,  50, 236,   92,  30, 228,  136,  20, 176,  160,  20, 100,  152,  34,  32,  120,  60,   0,   84,  90,   0,   40, 114,   0,    8, 124,   0,    0, 118,  40,    0, 102, 120,    0,   0,   0,    0, 0, 0,  0, 0, 0,
        236, 238, 236,   76, 154, 236,  120, 124, 236,  176,  98, 236,  228,  84, 236,  236,  88, 180,  236, 106, 100,  212, 136,  32,  160, 170,   0,  116, 196,   0,   76, 208,  32,   56, 204, 108,   56, 180, 204,   60,  60,  60,    0, 0, 0,  0, 0, 0,
        236, 238, 236,  168, 204, 236,  188, 188, 236,  212, 178, 236,  236, 174, 236,  236, 174, 212,  236, 180, 176,  228, 196, 144,  204, 210, 120,  180, 222, 120,  168, 226, 144,  152, 226, 180,  160, 214, 228,  160, 162, 160,    0, 0, 0,  0, 0, 0,
    };
    
    uint32_t r = Pallette[palletteIndex * 3 + 0];
    uint32_t g = Pallette[palletteIndex * 3 + 1];
    uint32_t b = Pallette[palletteIndex * 3 + 2];
    
    return (0xFF << 24) + (r << 16) + (g << 8) + (b << 0);
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

                        pOutputData[pixelIndex] = GetPixelColour(pixelColourLUT);
                    }
                }
            }
        }
    };
    
    fnDrawPattenTable(pOutputData, 0, 0, 0x0000);
    fnDrawPattenTable(pOutputData, 128, 0, 0x1000);
}
