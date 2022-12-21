//
//  PPUNES.cpp
//  NES
//
//

#include "PPUNES.h"
#include <stdio.h>
#include <string.h>

enum FlagControl : uint8_t
{
    // [bit 1 | bit 0]  - 0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00 = 0x2000 + (0x0400 * (CTRL & mask))
    CTRL_NAMETABLE_ADDRESS_BIT0 = 1 << 0,
    CTRL_NAMETABLE_ADDRESS_BIT1 = 1 << 1,
    CTRL_VRAM_ADDRESS_INC       = 1 << 2,   // 0 = add 1, 1 = add 32, per CPU read, write PPUDATA
    CTRL_SPRITE_TABLE_ADDR      = 1 << 3,   // 0 = 0x0000, 1 = 0x1000
    CTRL_BACKGROUND_TABLE_ADDR  = 1 << 4,   // 0 = 0x0000, 1 = 0x1000
    CTRL_SPRITE_SIZE            = 1 << 5,   // 0 = 8 X 8, 1 = 8 x 16
    CTRL_MASTER_SLAVE           = 1 << 6,
    CTRL_GEN_VBLANK_NMI         = 1 << 7
};

enum FlagMask : uint8_t
{
    MASK_GREYSCALE          = 1 << 0,
    MASK_BACKGROUND_L8      = 1 << 1,
    MASK_SPRITE_L8          = 1 << 2,
    MASK_BACKGROUND_SHOW    = 1 << 3,
    MASK_SPRITE_SHOW        = 1 << 4,
    MASK_EMPHASIZE_RED      = 1 << 5,
    MASK_EMPHASIZE_GREEN    = 1 << 6,
    MASK_EMPHASIZE_BLUE     = 1 << 7
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
, m_spriteZero(0xFF)
, m_ctrl(0)
, m_mask(0)
, m_status(0)
, m_oamAddress(0)
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

void PPUNES::SetFlag(uint8_t flag, uint8_t& ppuRegister)
{
    ppuRegister |= flag;
}

void PPUNES::ClearFlag(uint8_t flag, uint8_t& ppuRegister)
{
    ppuRegister &= ~flag;
}

bool PPUNES::TestFlag(uint8_t flag, uint8_t& ppuRegister)
{
    return (ppuRegister & flag) != 0;
}

void PPUNES::PowerOn()
{
    m_secondaryOAMWrite = 0;
    m_spriteZero = 0xFF;
    m_ctrl = 0;
    m_mask = 0;
    m_status = 0;
    m_oamAddress = 0;
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
    memset(m_primaryOAM, 0xFF, sizeof(m_primaryOAM));
    memset(m_secondaryOAM, 0xFF, sizeof(m_secondaryOAM));
}

void PPUNES::Reset()
{
    m_secondaryOAMWrite = 0;
    m_spriteZero = 0xFF;
    m_ctrl = 0;
    m_mask = 0;
    m_status = 0;
    m_oamAddress = 0;
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
    memset(m_primaryOAM, 0xFF, sizeof(m_primaryOAM));
    memset(m_secondaryOAM, 0xFF, sizeof(m_secondaryOAM));
}

void PPUNES::Tick()
{
    // VBlank set
    if(m_scanline == 241 && m_scanlineDot == 1)
    {
        SetFlag(STATUS_VBLANK, m_status);
        
        if(TestFlag(CTRL_GEN_VBLANK_NMI, m_ctrl))
        {
            m_bus.SignalNMI(true);
        }
    }
    
    // clear vblank etc ready for next frame
    if(m_scanline == 261 && m_scanlineDot == 1)
    {
        ClearFlag(STATUS_VBLANK, m_status);
        ClearFlag(STATUS_SPRITE0_HIT, m_status);
        ClearFlag(STATUS_SPRITE_OVERFLOW, m_status);

        m_bus.SignalNMI(false);
    }
    
    // Conflicting info - is this required
//    if(m_scanline == 0 && m_scanlineDot == 0 && TestFlag(MASK_BACKGROUND_SHOW, m_mask))
//    {
//        m_ppuAddress = m_ppuTAddress;
//    }
    
    // Main update draw, 0-239 is the visible scan lines, 261 is the pre-render line
    if(m_scanline >= 0 && m_scanline <= 239)
    {
        if(m_scanlineDot >= 1 && m_scanlineDot <= 64)
        {
            ClearSecondaryOAM();
        }
        else if(m_scanlineDot >= 65 && m_scanlineDot <= 256)
        {
            if(TestFlag(MASK_SPRITE_SHOW, m_mask))
            {
                SpriteEvaluation();
            }
        }
        else if(m_scanlineDot >= 257 && m_scanlineDot <= 320)
        {
            m_oamAddress = 0;
            SpriteFetch();

            // TODO check these
            if(m_scanlineDot == 320)
            {
                m_secondaryOAMWrite = 0;
                m_spriteZero = 0xFF;
            }
        }
        
        // Output current pixel
        if(m_scanlineDot >= 1 && m_scanlineDot <= 256)
        {
            GenerateVideoPixel();
        }
    }
    
    if(TestFlag(MASK_BACKGROUND_SHOW, m_mask))
    {
        if( ((m_scanline >= 0 && m_scanline <= 239) || m_scanline == 261) &&
            (m_scanlineDot > 0))
        {
            UpdateShiftRegisters();
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

void PPUNES::ClearSecondaryOAM()
{
    if(m_scanlineDot >= 1 && m_scanlineDot <= 64)
    {
        if(m_scanlineDot == 64)
        {
            m_spriteZero = 0xFF;
            memset(m_secondaryOAM, 0xFF, sizeof(m_secondaryOAM));
        }
    }
}

void PPUNES::SpriteEvaluation()
{
    // Odd data read even data write
    if(m_scanlineDot >= 65 && m_scanlineDot <= 256)
    {
        if(m_scanlineDot % 2 == 0)
        {
            uint8_t spriteTop = m_primaryOAM[m_oamAddress];
            uint8_t spriteBottom = spriteTop + (TestFlag(CTRL_SPRITE_SIZE, m_ctrl) ? 15 : 7);
            
            if(m_scanline >= spriteTop && m_scanline <= spriteBottom)
            {
                if(m_secondaryOAMWrite < 32)
                {
                    m_secondaryOAM[m_secondaryOAMWrite + 0] = m_primaryOAM[m_oamAddress + 0];
                    m_secondaryOAM[m_secondaryOAMWrite + 1] = m_primaryOAM[m_oamAddress + 1];
                    m_secondaryOAM[m_secondaryOAMWrite + 2] = m_primaryOAM[m_oamAddress + 2];
                    m_secondaryOAM[m_secondaryOAMWrite + 3] = m_primaryOAM[m_oamAddress + 3];
                    
                    m_secondaryOAMWrite += 4;
                    
                    if(m_oamAddress == 0)
                    {
                        m_spriteZero = 1;
                    }
                }
                else
                {
                    SetFlag(STATUS_SPRITE_OVERFLOW, m_status);
                }
            }
            
            m_oamAddress += 4;
        }
    }
}

void PPUNES::SpriteFetch()
{
    if(m_scanlineDot >= 257 && m_scanlineDot <= 320)
    {
        if((m_scanlineDot - 257) % 8 == 0)
        {
            uint8_t spriteIndex = ((m_scanlineDot - 257) / 8) % 8;
            ScanlineSprite& sprite = m_scanlineSprites[spriteIndex];
            
            sprite.m_patternLatch = 0;
            sprite.m_patternShift0 = 0;
            sprite.m_patternShift1 = 0;
            sprite.m_attribute = 0;
            sprite.m_counter = 0;
            sprite.m_spriteZero = 0;
            
            if(spriteIndex < m_secondaryOAMWrite / 4)
            {
                uint8_t yPos            = m_secondaryOAM[spriteIndex * 4 + 0];
                uint8_t spriteTileId    = m_secondaryOAM[spriteIndex * 4 + 1];
                uint8_t spriteAttribute = m_secondaryOAM[spriteIndex * 4 + 2];
                uint8_t xPos            = m_secondaryOAM[spriteIndex * 4 + 3];
                
                bool bFlipH = (spriteAttribute & (1 << 6)) != 0;
                bool bFlipV = (spriteAttribute & (1 << 7)) != 0;
                
                uint16_t spriteTileAddress = 0;
                if(TestFlag(CTRL_SPRITE_SIZE, m_ctrl))
                {
                    // 8 x 16 sprite size
                    
                    // LSB (i.e. odd/even) gives pattern table
                    uint16_t spriteBaseAddress = 0x0000;
                    if((spriteTileId & 1) != 0)
                    {
                        spriteBaseAddress = 0x1000;
                    }
                    
                    // Bottom is next tile, add 16 bytes
                    uint16_t top = spriteBaseAddress + (uint16_t(spriteTileId & 0b11111110) * 16);
                    uint16_t bottom = top + 16;
                    
                    // If its flipped then bottom becomes top
                    // Bit flipping is generic below for both 8 or 16 tall sprites
                    if(bFlipV)
                    {
                        uint16_t swap = bottom;
                        bottom = top;
                        top = swap;
                    }
                    
                    spriteTileAddress = top;
                    
                    // scanline is into the bottom part of the sprite
                    if(m_scanline - yPos > 7)
                    {
                        spriteTileAddress = bottom;
                        yPos += 8;
                    }
                }
                else
                {
                    // 8 x 8 sprite size
                    uint16_t spriteBaseAddress = 0x0000;
                    if(TestFlag(CTRL_SPRITE_TABLE_ADDR, m_ctrl))
                    {
                        spriteBaseAddress = 0x1000;
                    }
                    spriteTileAddress = spriteBaseAddress + (uint16_t(spriteTileId) * 16);
                }
                
                uint16_t spriteLineAddress = spriteTileAddress + m_scanline - yPos;
                
                if(bFlipV)
                {
                    spriteLineAddress = spriteTileAddress + (7 - (m_scanline - yPos));
                }
                
                uint8_t spritePlane0 = m_bus.ppuRead(spriteLineAddress);
                uint8_t spritePlane1 = m_bus.ppuRead(spriteLineAddress + 8);
                
                if(bFlipH)
                {
                    auto flipBits = [&](uint8_t& byte)
                    {
                        uint8_t src = byte;
                        byte = 0;
                        for(uint8_t b = 0;b < 8;++b)
                        {
                            byte |= (((src & 1 << (7 - b))) ? 1 : 0) << b;
                        }
                    };
                    flipBits(spritePlane0);
                    flipBits(spritePlane1);
                }
                
                sprite.m_patternLatch = 0;
                sprite.m_patternShift0 = spritePlane0;
                sprite.m_patternShift1 = spritePlane1;
                sprite.m_attribute = spriteAttribute;
                sprite.m_counter = xPos;
                sprite.m_spriteZero = spriteIndex == 0 && m_spriteZero == 1 ? 1 : 0;
            }
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
    else if(address >= 0x2000 && address <= 0x3EFF)
    {
        // name table mirrors
        uint16_t vRamAddress = absoluteAddressToVRAMAddress(address);
        if(m_mirrorMode == VRAM_MIRROR_CART4)
        {
            data = m_bus.ppuRead(vRamAddress);
        }
        else
        {
            uint16_t vRamOffset = vRamAddress - 0x2000;
            if(vRamOffset < nVRamSize)
            {
                data = m_vram[vRamOffset];
            }
#if DEBUG
            else
            {
                 *(volatile char*)(0) = 'V' | 'R' | 'A' | 'M';
            }
#endif
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
    else if(address >= 0x2000 && address <= 0x3EFF)
    {
        // name table mirrors
        uint16_t vRamAddress = absoluteAddressToVRAMAddress(address);
        if(m_mirrorMode == VRAM_MIRROR_CART4)
        {
            m_bus.ppuWrite(vRamAddress, byte);
        }
        else
        {
            uint16_t vRamOffset = vRamAddress - 0x2000;
            if(vRamOffset < nVRamSize)
            {
                m_vram[vRamOffset] = byte;
            }
#if DEBUG
            else
            {
                 *(volatile char*)(0) = 'V' | 'R' | 'A' | 'M';
            }
#endif
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
    uint16_t coarseX =      (m_ppuAddress >> 0) & 31;
    uint16_t coarseY =      (m_ppuAddress >> 5) & 31;
    uint16_t nametable =    (m_ppuAddress >> 10) & 3;
    uint16_t fineY =        (m_ppuAddress >> 12) & 7;
        
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
    m_ppuAddress =  (coarseX & 31) << 0;
    m_ppuAddress |= (coarseY & 31) << 5;
    m_ppuAddress |= (nametable & 3) << 10;
    m_ppuAddress |= (fineY & 7) << 12;
}

void PPUNES::vramIncVert()
{
    // break apart
    uint16_t coarseX =      (m_ppuAddress >> 0) & 31;
    uint16_t coarseY =      (m_ppuAddress >> 5) & 31;
    uint16_t nametable =    (m_ppuAddress >> 10) & 3;
    uint16_t fineY =        (m_ppuAddress >> 12) & 7;
    
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
    m_ppuAddress =  (coarseX & 31) << 0;
    m_ppuAddress |= (coarseY & 31) << 5;
    m_ppuAddress |= (nametable & 3) << 10;
    m_ppuAddress |= (fineY & 7) << 12;
}

void PPUNES::vramHorzCopy()
{
    // break apart - some parts from Temp Address
    uint16_t coarseX =      (m_ppuTAddress >> 0) & 31;
    uint16_t coarseY =      (m_ppuAddress >> 5) & 31;
    uint16_t nametableX =   (m_ppuTAddress >> 10) & 1;
    uint16_t nametableY =   (m_ppuAddress >> 11) & 1;
    uint16_t fineY =        (m_ppuAddress >> 12) & 7;
    
    // put back together
    m_ppuAddress =  (coarseX & 31) << 0;
    m_ppuAddress |= (coarseY & 31) << 5;
    m_ppuAddress |= (nametableX & 1) << 10;
    m_ppuAddress |= (nametableY & 1) << 11;
    m_ppuAddress |= (fineY & 7) << 12;
}

void PPUNES::vramVertCopy()
{
    // break apart - some parts from Temp Address
    uint16_t coarseX =      (m_ppuAddress >> 0) & 31;
    uint16_t coarseY =      (m_ppuTAddress >> 5) & 31;
    uint16_t nametableX =   (m_ppuAddress >> 10) & 1;
    uint16_t nametableY =   (m_ppuTAddress >> 11) & 1;
    uint16_t fineY =        (m_ppuTAddress >> 12) & 7;
    
    // put back together
    m_ppuAddress =  (coarseX & 31) << 0;
    m_ppuAddress |= (coarseY & 31) << 5;
    m_ppuAddress |= (nametableX & 1) << 10;
    m_ppuAddress |= (nametableY & 1) << 11;
    m_ppuAddress |= (fineY & 7) << 12;
}

void PPUNES::UpdateShiftRegisters()
{
    // sprites
    if( (m_scanline >= 0 && m_scanline <= 239)  && (m_scanlineDot > 0 && m_scanlineDot <= 256))
    {
        for(uint8_t spriteIndex = 0;spriteIndex < 8;++spriteIndex)
        {
            ScanlineSprite& sprite = m_scanlineSprites[spriteIndex];
            
            if(sprite.m_counter > 0)
            {
                --sprite.m_counter;
            }
            
            if(sprite.m_counter == 0)
            {
                uint8_t pixel0 = (sprite.m_patternShift0 & (1 << 7)) >> 7;
                uint8_t pixel1 = (sprite.m_patternShift1 & (1 << 7)) >> 7;
                
                sprite.m_patternLatch = (pixel1 << 1) | pixel0;
                
                sprite.m_patternShift0 <<= 1;
                sprite.m_patternShift1 <<= 1;
            }
        }
    }
    
    
    // background - scanline 261 is prefetch only - norender
    if( ((m_scanline >= 0 && m_scanline <= 239) || m_scanline == 261) &&
        (m_scanlineDot > 0))
    {
        if(m_scanlineDot <= 256 || (m_scanlineDot >= 321 && m_scanlineDot <= 336))
        {
            uint8_t vramFetchCycle = m_scanlineDot % 8;
            
            // Shift for a dot tick during tile pattern and attribute fetch phases
            m_bgPatternShift0 <<= 1;
            m_bgPatternShift1 <<= 1;
            m_bgPalletteShift0 <<= 1;
            m_bgPalletteShift1 <<= 1;

            // Try doing all this at once, if it doesn't work then follow the vram fetch cycles
            if(vramFetchCycle == 0)
            {
                uint16_t coarseX =      (m_ppuAddress >> 0) & 31;
                uint16_t coarseY =      (m_ppuAddress >> 5) & 31;
                //uint16_t nametable =    (m_ppuAddress >> 10) & 3;
                uint16_t fineY =        (m_ppuAddress >> 12) & 7;

                // load data into latches
                // Pattern
                {
                    uint16_t nametableAddress = 0x2000 + (m_ppuAddress & 0x0FFF);
                    uint8_t tileIndex = ppuReadAddress(nametableAddress);
                    
                    // Flag per frame or scanline, this is per tile line fetch?
                    uint16_t baseAddress = TestFlag(CTRL_BACKGROUND_TABLE_ADDR, m_ctrl) ? 0x1000 : 0x0000;
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
                    uint16_t attributeAddress = 0x23C0 | (m_ppuAddress & 0x0C00) | ((m_ppuAddress >> 4) & 0x38) | ((m_ppuAddress >> 2) & 0x07);
                    uint8_t tileAttribute = ppuReadAddress(attributeAddress);
                    
                    uint8_t attribQuadX = (coarseX / 2) % 2;
                    uint8_t attribQuadY = (coarseY / 2) % 2;
                    
                    uint8_t attributeBits = 0;

                    if(attribQuadX == 0 && attribQuadY == 0)
                        attributeBits = (tileAttribute >> 0 ) & 0x3;
                    else if(attribQuadX == 1 && attribQuadY == 0)
                        attributeBits = (tileAttribute >> 2) & 0x3;
                    else if(attribQuadX == 0 && attribQuadY == 1)
                        attributeBits = (tileAttribute >> 4) & 0x3;
                    else if(attribQuadX == 1 && attribQuadY == 1)
                        attributeBits = (tileAttribute >> 6) & 0x3;
#if DEBUG
                    else
                        *(volatile char*)(0) = 'P' | 'P' | 'U';
#endif

                    for(uint8_t i = 0;i < 8;++i)
                    {
                        m_bgPalletteShift0 |= ((attributeBits & 1) >> 0) << i;
                        m_bgPalletteShift1 |= ((attributeBits & 2) >> 1) << i;
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
    uint8_t tilePalletteSelect = 0x00;
    uint8_t tileAttributePalletteSelect = 0x00;
        
    uint8_t spritePalletteSelect = 0x00;
    uint8_t spriteAttributePalletteSelect = 0x00;
    
    uint8_t spritePriority = 0;
    
    bool bSpriteZero = false;
    
    // Background
    if(TestFlag(MASK_BACKGROUND_L8, m_mask) || (m_scanlineDot - 1) > 7)
    {
        uint8_t pixel0 = (m_bgPatternShift0 & (1 << (15 - m_fineX))) ? 1 : 0;
        uint8_t pixel1 = (m_bgPatternShift1 & (1 << (15 - m_fineX))) ? 1 : 0;
        
        tilePalletteSelect = (pixel1 << 1) | pixel0;

        uint8_t attrib0 = (m_bgPalletteShift0 & (1 << (15 - m_fineX))) ? 1 : 0;
        uint8_t attrib1 = (m_bgPalletteShift1 & (1 << (15 - m_fineX))) ? 1 : 0;
        
        tileAttributePalletteSelect = (attrib1 << 1) | attrib0;
    }
    
    // Sprite
    if(TestFlag(MASK_SPRITE_L8, m_mask) || (m_scanlineDot - 1) > 7)
    {
        for(uint8_t spriteIndex = 0;spriteIndex < 8;++spriteIndex)
        {
            ScanlineSprite& sprite = m_scanlineSprites[spriteIndex];
            
            if(sprite.m_patternLatch != 0)
            {
                spritePriority = (sprite.m_attribute & (1 << 5)) != 0 ? 0 : 1;
                spriteAttributePalletteSelect = sprite.m_attribute & 0x3;
                spritePalletteSelect = sprite.m_patternLatch;
                bSpriteZero = sprite.m_spriteZero;
                
                break;
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
            SetFlag(STATUS_SPRITE0_HIT, m_status);
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
        uint16_t y = m_scanline;
        uint16_t x = m_scanlineDot - 1;
  
        m_pVideoOutput[y * 256 + x] = GetPixelColour(palletteIndex);
    }
}

uint16_t PPUNES::absoluteAddressToVRAMAddress(uint16_t address)
{
    if(address >= 0x3000 && address <= 0x3EFF)
    {
        address -= 0x1000;
    }
    
    if(address >= 0x2000 && address <= 0x2FFF)
    {
        if(m_mirrorMode == VRAM_MIRROR_H)
        {
            if(address >= 0x2400 && address <= 0x27FF)
            {
                address -= 0x0400;
            }
            else if(address >= 0x2800 && address <= 0x2BFF)
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
        else if(m_mirrorMode == VRAM_MIRROR_CART4)
        {
            // no mirroring all exist on cart ram
            // does the cart handle the 0x3000 to 0x3EFF mirror?
        }
    }
    return address;
}

uint8_t PPUNES::cpuRead(uint8_t port)
{
    uint8_t data = m_portLatch;
    if(port < PortRegister_Count)
    {
        switch(port)
        {
            case PPUCTRL:   // 2000
                break;
            case PPUMASK:   // 2001
                break;
            case PPUSTATUS: // 2002
            {
                data = m_portLatch = (m_status & 0b11100000) | (m_portLatch & 0b00011111);
                ClearFlag(STATUS_VBLANK, m_status);
                m_ppuWriteToggle = 0;
                break;
            }
            case OAMADDR:   // 2003
                break;
            case OAMDATA:   // 2004
            {
                if(m_scanlineDot >= 1 && m_scanlineDot <= 64)
                {
                    // during init of sprite evaluation
                    data = m_portLatch = 0XFF;
                }
                else
                {
                    data = m_portLatch = m_primaryOAM[m_oamAddress];
                }
                break;
            }
            case PPUSCROLL: // 2005
                break;
            case PPUADDR:   // 2006
                break;
            case PPUDATA:   // 2007
            {
                // return value is last read
                data = m_portLatch = m_ppuDataBuffer;
                
                //next read will get this
                m_ppuDataBuffer = ppuReadAddress(m_ppuAddress);
                
                // pallette info is returned right away
                if(m_ppuAddress >= 0x3F00 && m_ppuAddress <= 0x3FFF)
                {
                    data = m_portLatch = m_ppuDataBuffer;
                    m_ppuDataBuffer = ppuReadAddress(m_ppuAddress - 0x1000);    // Weird
                }
                
                m_ppuAddress += TestFlag(CTRL_VRAM_ADDRESS_INC, m_ctrl) ? 32 : 1;
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
        m_portLatch = byte;
        
        switch(port)
        {
            case PPUCTRL:   // 2000
            {
                bool bWasGenVblank = TestFlag(CTRL_GEN_VBLANK_NMI, m_ctrl);
                
                m_ctrl = byte;

                // if in vblank and NMI request toggled from 0 - 1 gen the NMI now
                if(TestFlag(STATUS_VBLANK, m_status))
                {
                    if(bWasGenVblank == false && TestFlag(CTRL_GEN_VBLANK_NMI, m_ctrl))
                    {
                        m_bus.SignalNMI(true);
                    }
                }
                
                // add nametable select to ppu address
                m_ppuTAddress &= ~(0x3 << 10);
                m_ppuTAddress |= uint16_t((byte & 0x3)) << 10;

                break;
            }
            case PPUMASK:   // 2001
                m_mask = byte;
                break;
            case PPUSTATUS: // 2002
                // read only
                break;
            case OAMADDR:   // 2003
                m_oamAddress = byte;
                break;
            case OAMDATA:   // 2004
            {
                m_primaryOAM[m_oamAddress++] = byte;
                break;
            }
            case PPUSCROLL: // 2005
            {
                if(m_ppuWriteToggle == 0)
                {
                    //ScrollX
                    m_ppuTAddress &= ~(uint16_t(0b11111));
                    m_ppuTAddress |= uint16_t(byte) >> 3;       // courseX;
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
                if(m_ppuWriteToggle == 0)
                {
                    m_ppuTAddress &= ~(uint16_t(0xFF00));
                    m_ppuTAddress |= (uint16_t(byte) << 8) & 0x3F00;
                }
                else
                {
                    m_ppuTAddress &= (uint16_t(0xFF00));
                    m_ppuTAddress |= uint16_t(byte);
                    m_ppuAddress = m_ppuTAddress;
                }
                m_ppuWriteToggle = m_ppuWriteToggle == 0 ? 1 : 0;
                break;
            }
            case PPUDATA:   // 2007
            {
                ppuWriteAddress(m_ppuAddress, byte);
                m_ppuAddress += TestFlag(CTRL_VRAM_ADDRESS_INC, m_ctrl) ? 32 : 1;
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
