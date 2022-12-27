//
//  PPUNES.h
//  NES
//
//  Created by richardwallis on 15/11/2022.
//

#ifndef PPUNES_h
#define PPUNES_h

#include <cstdint>
#include "IOBus.h"

enum PortRegisterID :uint8_t
{
    PPUCTRL = 0,        // 0x2000
    PPUMASK,            // 0x2001
    PPUSTATUS,          // 0x2002
    OAMADDR,            // 0x2003
    OAMDATA,            // 0x2004
    PPUSCROLL,          // 0x2005
    PPUADDR,            // 0x2006
    PPUDATA,            // 0x2007
    PortRegister_Count
};

class PPUNES
{
public:
    static const uint32_t nVRamSize = 0x0800;
    static const uint32_t nPalletteSize = 0x20;

    PPUNES(IOBus& bus);
    ~PPUNES();
    
    void SetMirrorMode(MirrorMode mode);
    
    void PowerOn();
    void Reset();
    void Tick();
    
    uint8_t cpuRead(uint8_t port);
    void cpuWrite(uint8_t port, uint8_t byte);
    
    // Expects a 256x240 RGBA8 pixel foramt data pointer
    void SetVideoOutputDataPtr(uint32_t* pVideoOutData);

private:

    void SetFlag(uint8_t flag, uint8_t& ppuRegister);
    void ClearFlag(uint8_t flag, uint8_t& ppuRegister);
    bool TestFlag(uint8_t flag, uint8_t& ppuRegister);
    
    uint16_t absoluteAddressToVRAMAddress(uint16_t address);
    uint32_t GetPixelColour(uint32_t palletteIndex);
    
    void UpdateShiftRegisters();
    void ClearSecondaryOAM();
    void SpriteEvaluation();
    void SpriteFetch();
    void GenerateVideoPixel();
    
    uint8_t ppuReadAddress(uint16_t address);
    void ppuWriteAddress(uint16_t address, uint8_t byte);
    
    void vramIncHorz();
    void vramIncVert();
    void vramHorzCopy();
    void vramVertCopy();
    
private:
    IOBus& m_bus;
    
    MirrorMode m_mirrorMode;
    
    // Nametable + Pallette RAM
    uint8_t m_vram[nVRamSize];                      // 2x 1024 byte name tables - last 64 bytes of each are the attribute tables
    uint8_t m_pallette[nPalletteSize];
    
    // OAM RAM
    uint8_t m_primaryOAM[256];                      // object attribute ram
    uint8_t m_secondaryOAM[32];                     // current scanline sprites
    uint8_t m_secondaryOAMWrite;
    uint8_t m_spriteZero;
    
    // Registers - CPU accessible
    uint8_t m_ctrl;
    uint8_t m_mask;
    uint8_t m_status;
    uint8_t m_oamAddress;
    uint8_t m_portLatch;                            // data bus between CPU and GPU
    uint8_t m_ppuDataBuffer;
    
    // Internal Registers
    uint16_t m_ppuAddress;
    uint16_t m_ppuTAddress;
    uint8_t m_ppuWriteToggle;
    uint8_t m_ppuData;
    uint8_t m_fineX;
    
    // Background pattern shift registers
    uint16_t m_bgPatternShift0;
    uint16_t m_bgPatternShift1;
    uint16_t m_bgPalletteShift0;
    uint16_t m_bgPalletteShift1;
    
    // Sprite shift registers
    struct ScanlineSprite
    {
        uint8_t m_patternLatch;
        uint8_t m_patternShift0;
        uint8_t m_patternShift1;
        uint8_t m_attribute;
        uint8_t m_counter;
        uint8_t m_spriteZero;
    } m_scanlineSprites[8];
    
    // Emulation
    uint64_t m_tickCount;
    uint16_t m_scanline;
    uint16_t m_scanlineDot;
    
    // Output
    uint32_t* m_pVideoOutput;
};

#endif /* PPUNES_h */
