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
    PPUCTRL = 0,
    PPUMASK,
    PPUSTATUS,
    OAMADDR,
    OAMDATA,
    PPUSCROLL,
    PPUADDR,
    PPUDATA,
    PortRegister_Count
};

struct OAMEntry
{
    uint8_t m_y;
    uint8_t m_tileIdx;
    uint8_t m_attrib;
    uint8_t m_x;
};

enum MirrorMode : uint8_t
{
    VRAM_MIRROR_H = 0,
    VRAM_MIRROR_V,
    VRAM_MIRROR_CART4
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
    
    // Debug
    // Expects a 256x240 RGBA data pointer
    void WritePatternTables(uint32_t* pOutputData);
    
private:

    void SetFlag(uint8_t flag, PortRegisterID ppuRegister);
    void ClearFlag(uint8_t flag, PortRegisterID ppuRegister);
    bool TestFlag(uint8_t flag, PortRegisterID ppuRegister);
    
    uint16_t memoryAddressToNameTableOffset(uint16_t address);
    
private:
    IOBus& m_bus;
    
    MirrorMode m_mirrorMode;
    
    // Nametable + Pallette RAM
    uint8_t m_vram[nVRamSize];                      // 2x 1024 byte name tables - last 64 bytes of each are the attribute tables
    uint8_t m_pallette[nPalletteSize];
    
    // OAM RAM
    union                                           // object attribute ram
    {
        OAMEntry m_primaryOAMEntries[64];
        uint8_t m_primaryOAM[256];
    };
    union                                           // current scan line sprites
    {
        OAMEntry m_secondaryOAMEntries[8];
        uint8_t m_secondaryOAM[32];
    };
    
    // Registers - CPU accessible
    uint8_t m_portRegisters[PortRegister_Count];    // Communications with the CPU
    uint8_t m_portLatch;                            // data bus between CPU and GPU
    
    // PPUADDR + PPUDATA
    uint16_t m_ppuAddress;
    uint8_t m_ppuAddressLatch;
    uint8_t m_ppuData;
    
    // Background pattern shift registers
    uint8_t m_bgNextPattern0;
    uint8_t m_bgNextPattern1;
    uint16_t m_bgShift0;
    uint16_t m_bgShift1;
    
    // Emulation
    uint64_t m_tickCount;
    uint16_t m_scanline;
    uint16_t m_scanlineDot;
};

#endif /* PPUNES_h */
