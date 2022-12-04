//
//  SystemNES.h
//  NES
//
//  Created by richardwallis on 15/11/2022.
//

#ifndef SystemNES_h
#define SystemNES_h

#include "IOBus.h"
#include "CPU6502.h"
#include "PPUNES.h"
#include "Cartridge.h"

enum ControllerButton : uint8_t
{
    Controller_Right    = 7,
    Controller_Left     = 6,
    Controller_Down     = 5,
    Controller_Up       = 4,
    Controller_Start    = 3,
    Controller_Select   = 2,
    Controller_B        = 1,
    Controller_A        = 0
};

class SystemNES : public IOBus
{
public:
    static const uint32_t nRamSize = 0x0800;
    static const uint32_t nAPURegisterCount = 0x20;

    SystemNES();
    virtual ~SystemNES();
    
    void Reset();
    void EjectCartridge();
    bool InsertCartridge(void const* pData, uint32_t dataSize);
    void PowerOn();

    void Tick();
    
    virtual uint8_t cpuRead(uint16_t address) override;
    virtual void cpuWrite(uint16_t address, uint8_t byte) override;
    virtual uint8_t ppuRead(uint16_t address) override;
    virtual void ppuWrite(uint16_t address, uint8_t byte) override;
    
    virtual void SignalReset(bool bSignal) override;
    virtual void SignalNMI(bool bSignal) override;
    virtual void SignalIRQ(bool bSignal) override;

    // assumed space for a 32bit colour 256x240 image data
    void SetVideoOutputDataPtr(uint32_t* pVideoOutData);
    
    void SetControllerButtonState(uint8_t port, ControllerButton Button, bool bSet);
    
    // Debug
    void SetCPUProgramCounter(uint16_t pc);
    void WritePPUMetaData(uint32_t* pData);

private:
    bool        m_bPowerOn;
    uint64_t    m_cycleCount;
    uint8_t     m_ram[nRamSize];
    uint8_t     m_apuRegisters[nAPURegisterCount];  // just cover them all we will probably break them out as required

    CPU6502     m_cpu;
    PPUNES      m_ppu;
    Cartridge*  m_pCart;
    
    // Controller instantious and latch
    uint8_t     m_controller1;
    uint8_t     m_controller2;
    uint8_t     m_controllerLatch1;
    uint8_t     m_controllerLatch2;
    
    // DMA
    uint16_t    m_dmaAddress;
    uint8_t     m_dmaData;
    bool        m_bDMARead;
};

#endif /* SystemNES_h */
