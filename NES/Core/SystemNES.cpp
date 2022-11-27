//
//  SystemNES.cpp
//  NES
//
//  Created by richardwallis on 15/11/2022.
//

// Implementation Notes:

// All implementation info taken from https://www.nesdev.org/ with thanks.
// 2A03 [CPU+APU+IO]; This implementation has CPU saparate with APU+IO inside the System
// Cart is currently only mapper 0, todo make generic to allow other mapper implementations

// PPU:
// registers
// memory map - including cartridge mapping
// OAM DMA

#include "SystemNES.h"
#include <string>

SystemNES::SystemNES()
: m_bPowerOn(false)
, m_cycleCount(0)
, m_cpu(*this)
, m_ppu(*this)
, m_pCart(nullptr)
{
    memset(m_ram, 0x00, nRamSize);
    memset(m_apuRegisters, 0x00, nAPURegisterCount);
}

SystemNES::~SystemNES()
{
    EjectCartridge();
}

void SystemNES::PowerOn()
{
    m_cycleCount = 0;
    
    m_ppu.PowerOn();
    m_cpu.PowerOn();
    
    memset(m_ram, 0x00, nRamSize);
    
    // TODO apu registers on power on
    memset(m_apuRegisters, 0x00, nAPURegisterCount);
    
    m_bPowerOn = true;
}

void SystemNES::SetCPUProgramCounter(uint16_t pc)
{
    m_cpu.SetPC(pc);
}

void SystemNES::Reset()
{
    m_cycleCount = 0;
    
    m_ppu.Reset();
    m_cpu.Reset();
    
    // ram does not change on reset
    
    // TODO apu registers on reset
    memset(m_apuRegisters, 0x00, nAPURegisterCount);
}

void SystemNES::EjectCartridge()
{
    m_bPowerOn = false;
    
     if(m_pCart != nullptr)
    {
        delete m_pCart;
        m_pCart = nullptr;
    }
}

bool SystemNES::InsertCartridge(void const* pData, uint32_t dataSize)
{
    EjectCartridge();
    
    struct iNesheader
    {
        uint8_t m_constant[4];
        uint8_t m_prg16KChunks;
        uint8_t m_chr8KChunks;
        uint8_t m_flags6;
        uint8_t m_flags7;
        uint8_t m_flags8;
        uint8_t m_flags9;
        uint8_t m_flags10;
        uint8_t m_flagsUnused[5];
    };
    
    if(pData == nullptr || dataSize < sizeof(iNesheader))
    {
        return false;
    }
        
    uint8_t const* pRawBytes = (uint8_t const*)pData;
    iNesheader const* pHeader = (iNesheader const*)pRawBytes;
    uint8_t const* pCartData = pRawBytes + sizeof(iNesheader);
    
    char const magic[4] = {0x4E, 0x45, 0x53, 0x1A};
    if(memcmp(magic, pData, 4) != 0)
    {
        return false;
    }
    
    if((pHeader->m_flags6 & (1 << 2)) != 0)
    {
        // 512 byte trainer not handled
        return false;
    }
    
    uint8_t mapperID = (pHeader->m_flags7 & 0xF0) | ((pHeader->m_flags6 & 0xF0) >> 4);
    m_pCart = new Cartridge(mapperID, pCartData, pHeader->m_prg16KChunks, pHeader->m_chr8KChunks);
    
    return m_pCart != nullptr && m_pCart->IsValid();
}

void SystemNES::SignalReset(bool bSignal)
{
    m_cpu.SignalReset(bSignal);
}

void SystemNES::SignalNMI(bool bSignal)
{
    m_cpu.SignalNMI(bSignal);
}

void SystemNES::SignalIRQ(bool bSignal)
{
    m_cpu.SignalIRQ(bSignal);
}

void SystemNES::Tick()
{
    // NTSC:
    // Master clock = 21.477272 MHz
    // PPU clock    = 21.477272 /  4 = 5.369318 MHz
    // CPU clock    = 21.477272 / 12 = 1.789773 MHz
    // 3 PPU clock ticks to 1 CPU clock tick
    if(m_bPowerOn)
    {
        ++m_cycleCount;
        m_ppu.Tick();
        
        if(m_cycleCount % 3 == 0)
        {
            m_cpu.Tick();
        }
    }
}

uint8_t SystemNES::cpuRead(uint16_t address)
{
    if(address >= 0x0000 && address <= 0x1FFF)
    {
        // ram with mirrors every 2KB (0x0800 bytes)
        uint16_t memAddress = address % 0x0800;
        return m_ram[memAddress];
    }
    else if(address >= 0x2000 && address <= 0x3FFF)
    {
        uint16_t memAddress = (address - 0x2000) % 8;
        m_ppu.cpuRead(memAddress);
    }
    else if(address >= 0x4000 && address <= 0x401F)
    {
        // APU and IO registers
        uint16_t memAddress = address - 0x4000;
        return m_apuRegisters[memAddress];
    }
    else if(address >= 0x4020 && address < 0xFFFF && m_pCart != nullptr)
    {
        // Cart with mapper+prg+chr data + interrupt vectors
        return m_pCart->cpuRead(address);
    }
    
    // Open bus read
    return 0x00;
}

void SystemNES::cpuWrite(uint16_t address, uint8_t byte)
{
    if(address >= 0x0000 && address <= 0x1FFF)
    {
        uint16_t memAddress = address % 0x0800;
        m_ram[memAddress] = byte;
    }
    else if(address >= 0x2000 && address <= 0x3FFF)
    {
        uint16_t memAddress = (address - 0x2000) % 8;
        m_ppu.cpuWrite(memAddress, byte);
    }
    else if(address >= 0x4000 && address <= 0x401F)
    {
        uint16_t memAddress = address - 0x4000;
        m_apuRegisters[memAddress] = byte;
    }
    else if(address >= 0x4020 && address < 0xFFFF && m_pCart != nullptr)
    {
        m_pCart->cpuWrite(address, byte);
    }
}

uint8_t SystemNES::ppuRead(uint16_t address)
{
    return m_pCart->ppuRead(address);
}

void SystemNES::ppuWrite(uint16_t address, uint8_t Byte)
{
    // !
}
