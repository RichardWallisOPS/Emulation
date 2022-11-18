//
//  SystemNES.h
//  NES
//
//  Created by richardwallis on 15/11/2022.
//  Copyright © 2022 openplanetsoftware. All rights reserved.
//

#ifndef SystemNES_h
#define SystemNES_h

#include "IOBus.h"
#include "CPU6502.h"
#include "PPUNES.h"
#include "Cartridge.h"

class SystemNES : public IOBus
{
public:
    static const uint32_t nRamSize = 0x0800;

    SystemNES();
    virtual ~SystemNES();
    
    void Reset();
    void EjectCartridge();
    bool InsertCartridge(void const* pData, uint32_t dataSize);

    void Tick();
    
    virtual uint8_t cpuRead(uint16_t address) override;
    virtual void cpuWrite(uint16_t address, uint8_t byte) override;
    virtual uint8_t ppuRead(uint16_t address) override;
    virtual void ppuWrite(uint16_t address, uint8_t byte) override;

private:
    uint64_t    m_cycleCount;
    uint8_t     m_ram[nRamSize];
    CPU6502     m_cpu;
    PPUNES      m_ppu;
    Cartridge*  m_pCart;
};

#endif /* SystemNES_h */
