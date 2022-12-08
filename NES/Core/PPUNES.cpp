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
, m_fineX(0)
, m_bgPatternShift0(0)
, m_bgPatternShift1(0)
, m_bgPalletteShift0(0)
, m_bgPalletteShift1(0)
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
    m_fineX = 0;
    
    m_bgPatternShift0 = 0;
    m_bgPatternShift1 = 0;
    m_bgPalletteShift0 = 0;
    m_bgPalletteShift1 = 0;
    
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
    m_fineX = 0;
    
    m_bgPatternShift0 = 0;
    m_bgPatternShift1 = 0;
    m_bgPalletteShift0 = 0;
    m_bgPalletteShift1 = 0;
    
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
    
    if( ((m_scanline >= 0 && m_scanline <= 239) || m_scanline == 261) &&
        (m_scanlineDot > 0))
    {
        UpdateShiftRegisters();
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
            memcpy(m_renderOAM, m_secondaryOAM, sizeof(m_secondaryOAM));
        }
    }
}

uint8_t PPUNES::ppuReadAddress(uint16_t address)
{
    uint8_t data = 0;
    
    address = (address % 0x3FFF);
    if(address >= 0x3000 && address <= 0x3EFF)
    {
        address -= 0x1000;
    }
    
    if(address >= 0 && address <= 0x1FFF)
    {
        // cart pattern table
        data = m_bus.ppuRead(address);
    }
    else if(address >= 0x2000 && address <= 0x2FFF)
    {
        // name table mirrors
        if(m_mirrorMode == VRAM_MIRROR_CART4)
        {
            data = m_bus.ppuRead(address);
        }
        else
        {
            uint16_t vramAddress = absoluteAddressToVRAMAddress(address);
            uint16_t vramOffset = vramAddress - 0x2000;
            if(vramOffset <= nVRamSize)
            {
                data = m_vram[vramOffset];
            }
        }
    }
    else if(address >= 0x3F00 && address <= 0x3FFF)
    {
        uint16_t palletteIndex = (address - 0x3F00) % nPalletteSize;
        data = m_pallette[palletteIndex];
    }
    
    return data;
}

void PPUNES::ppuWriteAddress(uint16_t address, uint8_t byte)
{
    // Mirror VRAM ranges
    address = (address % 0x3FFF);
    if(address >= 0x3000 && address <= 0x3EFF)
    {
        address -= 0x1000;
    }
    
    if(address >= 0 && address <= 0x1FFF)
    {
        // cart pattern table
        m_bus.ppuWrite(address, byte);
    }
    else if(address >= 0x2000 && address <= 0x2FFF)
    {
        // name table mirrors
        if(m_mirrorMode == VRAM_MIRROR_CART4)
        {
            m_bus.ppuWrite(address, byte);
        }
        else
        {
            uint16_t vramAddress = absoluteAddressToVRAMAddress(address);
            uint16_t vramOffset = vramAddress - 0x2000;
            if(vramOffset <= nVRamSize)
            {
                m_vram[vramOffset] = byte;
            }
        }
    }
    else if(address >= 0x3F00 && address <= 0x3FFF)
    {
        uint16_t palletteIndex = (address - 0x3F00) % nPalletteSize;
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
}

// VRAM address
//     VH
// yyy NN YYYYY XXXXX
// ||| || ||||| +++++-- coarse X scroll
// ||| || +++++-------- coarse Y scroll
// ||| ++-------------- nametable select
// +++----------------- fine Y scroll

void PPUNES::vramIncHorz()
{
    // break apart
    uint16_t coarseX =      (m_ppuRenderAddress >> 0) & 31;
    uint16_t coarseY =      (m_ppuRenderAddress >> 5) & 31;
    uint16_t nametable =    (m_ppuRenderAddress >> 10) & 3;
    uint16_t fineY =        (m_ppuRenderAddress >> 12) & 7;
        
    // update coarse for next tile fetch
    if(coarseX < 31)
    {
        ++coarseX;
    }
    else
    {
        coarseX = 0;
        nametable ^= 1;
    }

    // put back together
    m_ppuRenderAddress =  (coarseX & 31) << 0;
    m_ppuRenderAddress |= (coarseY & 31) << 5;
    m_ppuRenderAddress |= (nametable & 3) << 10;
    m_ppuRenderAddress |= (fineY & 7) << 12;
}

void PPUNES::vramIncVert()
{
    // break apart
    uint16_t coarseX =      (m_ppuRenderAddress >> 0) & 31;
    uint16_t coarseY =      (m_ppuRenderAddress >> 5) & 31;
    uint16_t nametable =    (m_ppuRenderAddress >> 10) & 3;
    uint16_t fineY =        (m_ppuRenderAddress >> 12) & 7;
    
    // update coarse for next tile fetch
    if(fineY < 7)
    {
        ++fineY;
    }
    else
    {
        fineY = 0;
        if(coarseY == 29)
        {
            coarseY = 0;
            nametable ^= 2;
        }
        else if(coarseY == 31)
        {
            coarseY = 0;
        }
        else
        {
            ++coarseY;
        }
    }
    
    // put back together
    m_ppuRenderAddress =  (coarseX & 31) << 0;
    m_ppuRenderAddress |= (coarseY & 31) << 5;
    m_ppuRenderAddress |= (nametable & 3) << 10;
    m_ppuRenderAddress |= (fineY & 7) << 12;
}

void PPUNES::vramHorzCopy()
{
    // break apart - some parts from Temp Address
    uint16_t coarseX =      (m_ppuTAddress >> 0) & 31;
    uint16_t coarseY =      (m_ppuRenderAddress >> 5) & 31;
    uint16_t nametableX =   (m_ppuTAddress >> 10) & 1;
    uint16_t nametableY =   (m_ppuRenderAddress >> 11) & 1;
    uint16_t fineY =        (m_ppuRenderAddress >> 12) & 7;
    
    // put back together
    m_ppuRenderAddress =  (coarseX & 31) << 0;
    m_ppuRenderAddress |= (coarseY & 31) << 5;
    m_ppuRenderAddress |= (nametableX & 1) << 10;
    m_ppuRenderAddress |= (nametableY & 1) << 11;
    m_ppuRenderAddress |= (fineY & 7) << 12;
}

void PPUNES::vramVertCopy()
{
    // break apart - some parts from Temp Address
    uint16_t coarseX =      (m_ppuRenderAddress >> 0) & 31;
    uint16_t coarseY =      (m_ppuTAddress >> 5) & 31;
    uint16_t nametableX =   (m_ppuRenderAddress >> 10) & 1;
    uint16_t nametableY =   (m_ppuTAddress >> 11) & 1;
    uint16_t fineY =        (m_ppuTAddress >> 12) & 7;
    
    // put back together
    m_ppuRenderAddress =  (coarseX & 31) << 0;
    m_ppuRenderAddress |= (coarseY & 31) << 5;
    m_ppuRenderAddress |= (nametableX & 1) << 10;
    m_ppuRenderAddress |= (nametableY & 1) << 11;
    m_ppuRenderAddress |= (fineY & 7) << 12;
}

void PPUNES::UpdateShiftRegisters()
{
    // TODO sprites
    // background - scanline 261 is prefetch only - norender
    if( ((m_scanline >= 0 && m_scanline <= 239) || m_scanline == 261) &&
        (m_scanlineDot > 0))
    {
        if(m_scanlineDot <= 256 || (m_scanlineDot >= 321 && m_scanlineDot <= 336))
        {
            uint8_t vramFetchCycle = m_scanlineDot % 8;

            // Try doing all this at once, if it doesn't work then follow the vram fetch cycles
            if(vramFetchCycle == 0)
            {
                uint16_t coarseX =      (m_ppuRenderAddress >> 0) & 31;(void)coarseX;
                uint16_t coarseY =      (m_ppuRenderAddress >> 5) & 31;(void)coarseY;
                uint16_t nametable =    (m_ppuRenderAddress >> 10) & 3;(void)nametable;
                uint16_t fineY =        (m_ppuRenderAddress >> 12) & 7;(void)fineY;

                // load data into latches
                // Pattern
                {
                    uint16_t nametableAddress = 0x2000 + (m_ppuRenderAddress & 0x0FFF);
                    uint8_t tileIndex = ppuReadAddress(nametableAddress);
                    
                    // Flag per frame or scanline, this is per tile line fetch?
                    uint16_t baseAddress = TestFlag(CTRL_BACKGROUND_TABLE_ADDR, PPUCTRL) ? 0x1000 : 0x0000;
                    uint16_t tileAddress = baseAddress + (uint16_t(tileIndex) * 16);
                                        
                    uint16_t pattern0 = ppuReadAddress(tileAddress + fineY + 0);
                    uint16_t pattern1 = ppuReadAddress(tileAddress + fineY + 8);
                    
                    // clear low bits and set next 8 bit pattern
                    m_bgPatternShift0 &= 0xFF00;
                    m_bgPatternShift1 &= 0xFF00;
                    m_bgPatternShift0 |= pattern0;
                    m_bgPatternShift1 |= pattern1;
                }
                
                // Attribute
                {
                    uint16_t attributeAddress = 0x23C0 | (m_ppuRenderAddress & 0x0C00) | ((m_ppuRenderAddress >> 4) & 0x38) | ((m_ppuRenderAddress >> 2) & 0x07);
                    uint8_t tileAttribute = ppuReadAddress(attributeAddress);
                    
                    uint8_t attribQuadX = (coarseX / 2) % 2;
                    uint8_t attribQuadY = (coarseY / 2) % 2;
                    
                    uint8_t attributeBits = 0;

                    if(attribQuadX == 0 && attribQuadY == 0)
                        attributeBits = tileAttribute & 0x3;
                    else if(attribQuadX == 1 && attribQuadY == 0)
                        attributeBits = (tileAttribute >> 2) & 0x3;
                    else if(attribQuadX == 0 && attribQuadY == 1)
                        attributeBits = (tileAttribute >> 4) & 0x3;
                    else if(attribQuadX == 1 && attribQuadY == 1)
                        attributeBits = (tileAttribute >> 6) & 0x3;

                    for(int i = 0;i < 8;++i)    // TODO fix this - got to be wrong
                    {
                        m_bgPalletteShift0 <<= 1;
                        m_bgPalletteShift0 |= attributeBits & 1;
                        m_bgPalletteShift1 <<= 1;
                        m_bgPalletteShift1 |= (attributeBits & 2) >> 1;
                    }
                }
                
                vramIncHorz();
            }
        }

        if(m_scanlineDot == 256)
        {
            vramIncVert();
        }
        else if(m_scanlineDot == 257)
        {
            vramHorzCopy();
        }
        
        if(m_scanline == 261 && (m_scanlineDot >= 280 && m_scanlineDot <= 304))
        {
            vramVertCopy();
        }
    }
}

void PPUNES::GenerateVideoPixel()
{
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
        uint8_t pixel0 = (m_bgPatternShift0 & (1 << (15 - m_fineX))) ? 1 : 0;
        uint8_t pixel1 = (m_bgPatternShift1 & (1 << (15 - m_fineX))) ? 1 : 0;
        
        tilePalletteSelect = (pixel1 << 1) | pixel0;
        
        m_bgPatternShift0 <<= 1;
        m_bgPatternShift1 <<= 1;

        uint8_t attrib0 = (m_bgPalletteShift0 & (1 << (8 - m_fineX))) ? 1 : 0;
        uint8_t attrib1 = (m_bgPalletteShift1 & (1 << (8 - m_fineX))) ? 1 : 0;

        tileAttributePalletteSelect = (attrib1 << 1) | attrib0;
    }

    // Sprite - TODO change to shift registers
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
                    bSpriteZero = idx == 0;
                    break;
                }
            }
        }
    }
    
    // Multiplexer logic
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
                m_ppuDataBuffer = ppuReadAddress(m_ppuAddress);
                
                // pallette info is returned right away
                if(m_ppuAddress >= 0x3F00 && m_ppuAddress <= 0x3FFF)
                {
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
                // 7  bit  0
                // ---- ----
                // VPHB SINN
                // |||| ||||
                // |||| ||++- Base nametable address
                // |||| ||    (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
                // |||| |+--- VRAM address increment per CPU read/write of PPUDATA
                // |||| |     (0: add 1, going across; 1: add 32, going down)
                // |||| +---- Sprite pattern table address for 8x8 sprites
                // ||||       (0: $0000; 1: $1000; ignored in 8x16 mode)
                // |||+------ Background pattern table address (0: $0000; 1: $1000)
                // ||+------- Sprite size (0: 8x8 pixels; 1: 8x16 pixels – see PPU OAM#Byte 1)
                // |+-------- PPU master/slave select
                // |          (0: read backdrop from EXT pins; 1: output color on EXT pins)
                // +--------- Generate an NMI at the start of the
                //            vertical blanking interval (0: off; 1: on)
                m_portRegisters[port] = byte;

                // if in vblank and NMI request toggled from 0 - 1 gen the NMI now
                if(TestFlag(STATUS_VBLANK, PPUSTATUS))
                {
                    if((oldByte & CTRL_GEN_VBLANK_NMI) == 0 && (byte & CTRL_GEN_VBLANK_NMI) != 0)
                    {
                        m_bus.SignalNMI(true);
                    }
                }

                // VRAM address
                // yyy NN YYYYY XXXXX
                // ||| || ||||| +++++-- coarse X scroll
                // ||| || +++++-------- coarse Y scroll
                // ||| ++-------------- nametable select
                // +++----------------- fine Y scroll
                
                // add nametable select to ppu address
                m_ppuTAddress &= ~(0x3 << 10);
                m_ppuTAddress |= uint16_t((byte & 0x3)) << 10;

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
            case PPUSCROLL: // 2005
            {
                m_portRegisters[port] = byte;
                if(m_ppuWriteToggle == 0)
                {
                    //ScrollX
                    m_ppuTAddress &= ~(uint16_t(0b11111));
                    m_ppuTAddress |= uint16_t(byte) >> 3;     // courseX;
                    m_fineX = byte & 0b00000111;                // fineX
                }
                else
                {
                    //ScrollY
                    m_ppuTAddress &= ~(uint16_t(0b11111000) << 2);
                    m_ppuTAddress &= ~(uint16_t(0b00000111) << 12);
                    m_ppuTAddress |= (uint16_t(byte) & 0b11111000) << 2;     // courseY
                    m_ppuTAddress |= (uint16_t(byte) & 0b00000111) << 12;    // fineY
                }
                m_ppuWriteToggle = m_ppuWriteToggle == 0 ? 1 : 0;
                break;
            }
            case PPUADDR: // 2006
            {
                // VRAM address
                //     VH
                // yyy NN YYYYY XXXXX
                // ||| || ||||| +++++-- coarse X scroll
                // ||| || +++++-------- coarse Y scroll
                // ||| ++-------------- nametable select
                // +++----------------- fine Y scroll
                m_portRegisters[port] = byte;
                if(m_ppuWriteToggle == 0)
                {
                    m_ppuTAddress = uint16_t(byte) & 0b00111111;
                    m_ppuTAddress <<= 8;
                }
                else
                {
                    m_ppuTAddress = (m_ppuTAddress & 0xFF00) | uint16_t(byte);
                    m_ppuAddress = m_ppuTAddress;
                    m_ppuRenderAddress = m_ppuAddress;
                }
                m_ppuWriteToggle = m_ppuWriteToggle == 0 ? 1 : 0;
                break;
            }
            case PPUDATA:
            {
                m_portRegisters[port] = byte;
                
                ppuWriteAddress(m_ppuAddress, byte);
                
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
