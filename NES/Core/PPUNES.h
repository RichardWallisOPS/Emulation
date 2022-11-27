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

class PPUNES
{
public:
    static const uint32_t nVRamSize = 0x0800;

    PPUNES(IOBus& bus);
    ~PPUNES();
    
    void PowerOn();
    void Reset();
    void Tick();
    
    uint8_t cpuRead(uint16_t address);
    void cpuWrite(uint16_t address, uint8_t byte);
    
    // Debug
    void WritePatternTables(uint32_t* pOutputData);

private:
    IOBus& m_bus;
    
    uint8_t m_vram[nVRamSize];                      // 2x 1024 byte name tables - last 64 bytes of each are the attribute tables`
    uint8_t m_portRegisters[PortRegister_Count];    // Communications with the CPU
    
    union                                           // object attribute ram
    {
        OAMEntry m_oamEntries[64];
        uint8_t m_oam[256];
    };
    union                                           // current scan line sprites
    {
        OAMEntry m_scanlineEntries[8];
        uint8_t m_oamScanline[32];
    };
};

#endif /* PPUNES_h */
