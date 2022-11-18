//
//  SystemNES.cpp
//  NES
//
//  Created by richardwallis on 15/11/2022.
//  Copyright © 2022 openplanetsoftware. All rights reserved.
//

// Implementation Notes:

// All implementation info taken from https://www.nesdev.org/ with thanks.
// 2A03 [CPU+APU+IO]; This implementation has CPU saparate with APU+IO inside the System
// Cart is currently only mapper 0, todo make generic to allow other mapper implementations

// NEXT TASKs:
// 1) Set startup values for CPU and SYSTEM, PPU later
// 2) Load a cartridge and see the errors fly in the instruction error trap
// 3) Implement instructions: finish inc's, then simple 2 byte ones. 3 byte, 4 byte etc
// 4) Allow a cart to execute
// 5) Implement any missing ones that trap


#include "SystemNES.h"
#include <string>

SystemNES::SystemNES()
: cycleCount(0)
, m_cpu(*this)
, m_ppu(*this)
, m_pCart(nullptr)
{
    // Note: Cold and Reset states maybe different for the system, cpu and ppu
    memset(m_ram, 0x00, nRamSize);
}

SystemNES::~SystemNES()
{
    EjectCartridge();
}

void SystemNES::Reset()
{
    m_ppu.Reset();
    m_cpu.Reset();
    
    memset(m_ram, 0xE6, nRamSize);    // NOTE TEST instruction put back to zero
}

void SystemNES::EjectCartridge()
{
     if(m_pCart != nullptr)
    {
        delete m_pCart;
        m_pCart = nullptr;
    }
}

bool SystemNES::InsertCartridge(void const* pData, uint32_t dataSize)
{
    Reset();
    EjectCartridge();
    
    uint8_t const* pPakBytes = (uint8_t const*)pData;
    
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
    
    iNesheader const* pHeader = (iNesheader const*)pPakBytes;
    uint8_t const* pCartData = pPakBytes + sizeof(iNesheader);
    
    if((pHeader->m_flags6 & (1 << 2)) != 0)
    {
        // 512 byte trainer not handled
        return false;
    }
    
    m_pCart = new Cartridge(pCartData, pHeader->m_prg16KChunks, pHeader->m_chr8KChunks);
    
    return true;
}

void SystemNES::Tick()
{
    // NTSC:
    // Master clock = 21.477272 MHz
    // PPU clock    = 21.477272 /  4 = 5.369318 MHz
    // CPU clock    = 21.477272 / 12 = 1.789773 MHz
    // 3 PPU clock ticks to 1 CPU clock tick
    {
        ++cycleCount;
        m_ppu.Tick();
        
        if(cycleCount % 3 == 0)
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
    else if(address >= 0x4000 && address <= 0x401F)
    {
        // APU and IO registers
    }
    else if(address >= 0x4020 && address < 0xFFFF && m_pCart != nullptr)
    {
        // Cart with interrupt vectors (NMI=0xFFFA-0xFFFB, Reset=0xFFFC-0xFFFD, IRQ/BRK=0xFFFE-0xFFFF
        return m_pCart->cpuRead(address);
    }
            
    return 0;
}

void SystemNES::cpuWrite(uint16_t address, uint8_t Byte)
{
    if(address >= 0x0000 && address <= 0x1FFF)
    {
        uint16_t memAddress = address % 0x0800;
        m_ram[memAddress] = Byte;
    }
    else if(address >= 0x4000 && address <= 0x401F)
    {
    
    }
    else if(address >= 0x4020 && address < 0xFFFF && m_pCart != nullptr)
    {
        return m_pCart->cpuWrite(address, Byte);
    }
}

uint8_t SystemNES::ppuRead(uint16_t address)
{
    return 0;
}

void SystemNES::ppuWrite(uint16_t address, uint8_t Byte)
{

}
