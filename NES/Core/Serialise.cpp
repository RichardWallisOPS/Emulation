//
//  Serialise.cpp
//  NES
//
//  Created by Richard Wallis on 02/01/2023.
//  Copyright © 2023 openplanetsoftware. All rights reserved.
//

#include <cstdint>
#include <string>
#include "Serialise.h"
#include "SystemNES.h"

Archive::Archive()
: m_pMem(nullptr)
, m_memSize(0)
, m_readHead(0)
, m_writeHead(0)
{
    m_memSize = 1024 * 1024;
    m_pMem = new uint8_t[m_memSize];
}

Archive::~Archive()
{
    if(m_pMem != nullptr)
    {
        delete [] m_pMem;
        m_pMem = nullptr;
    }
}

bool Archive::Load(const char* pPath)
{
    FileStack fileLoad(fopen(pPath, "r"));
    if(fileLoad.handle() != nullptr)
    {
        if(fseek(fileLoad.handle(), 0, SEEK_END) == 0)
        {
            m_memSize = (uint32_t)ftell(fileLoad.handle());
        
            if(fseek(fileLoad.handle(), 0, SEEK_SET) == 0)
            {
                m_writeHead = fread(m_pMem, 1, m_memSize, fileLoad.handle());
            }
        }
    }
     
    return m_writeHead > 0 && m_writeHead == m_memSize;
}

bool Archive::Save(const char* pPath)
{
    size_t bytesSaved = 0;
    
    FileStack fileSave(fopen(pPath, "w"));
    if(fileSave.handle() != nullptr)
    {
        bytesSaved = fwrite(m_pMem, 1, m_writeHead, fileSave.handle());
    }
     
    return bytesSaved == m_writeHead;
}

void SystemNES::Load(Archive& rArchive)
{
    rArchive >> m_bPowerOn;
    rArchive >> m_cycleCount;
    rArchive.ReadBytes(m_ram, nRamSize);
    rArchive.ReadBytes(m_apuRegisters, nAPURegisterCount);
    rArchive >> m_controller1;
    rArchive >> m_controller2;
    rArchive >> m_controllerLatch1;
    rArchive >> m_controllerLatch2;
    rArchive >> m_dmaAddress;
    rArchive >> m_dmaData;
    rArchive >> m_DMAMode;
    
    m_cpu.Load(rArchive);
    m_ppu.Load(rArchive);
    
    if(m_pCart != nullptr)
    {
        m_pCart->Load(rArchive);
    }
}

void SystemNES::Save(Archive& rArchive)
{
    rArchive << m_bPowerOn;
    rArchive << m_cycleCount;
    rArchive.WriteBytes(m_ram, nRamSize);
    rArchive.WriteBytes(m_apuRegisters, nAPURegisterCount);
    rArchive << m_controller1;
    rArchive << m_controller2;
    rArchive << m_controllerLatch1;
    rArchive << m_controllerLatch2;
    rArchive << m_dmaAddress;
    rArchive << m_dmaData;
    rArchive << m_DMAMode;
    
    m_cpu.Save(rArchive);
    m_ppu.Save(rArchive);
    
    if(m_pCart != nullptr)
    {
        m_pCart->Save(rArchive);
    }
}

void CPU6502::Load(Archive& rArchive)
{
    rArchive >> m_a;
    rArchive >> m_x;
    rArchive >> m_y;
    rArchive >> m_stack;
    rArchive >> m_flags;
    rArchive >> m_pc;
    rArchive >> m_tickCount;
    rArchive >> m_instructionCycle;
    rArchive >> m_opCode;
    rArchive >> m_dataBus;
    rArchive >> m_addressBusH;
    rArchive >> m_addressBusL;
    rArchive >> m_baseAddressH;
    rArchive >> m_baseAddressL;
    rArchive >> m_indirectAddressH;
    rArchive >> m_indirectAddressL;
    rArchive >> m_effectiveAddressH;
    rArchive >> m_effectiveAddressL;
    rArchive >> m_bSignalReset;
    rArchive >> m_bSignalIRQ;
    rArchive >> m_bSignalNMI;
    rArchive >> m_bBranch;
}

void CPU6502::Save(Archive& rArchive)
{
    rArchive << m_a;
    rArchive << m_x;
    rArchive << m_y;
    rArchive << m_stack;
    rArchive << m_flags;
    rArchive << m_pc;
    rArchive << m_tickCount;
    rArchive << m_instructionCycle;
    rArchive << m_opCode;
    rArchive << m_dataBus;
    rArchive << m_addressBusH;
    rArchive << m_addressBusL;
    rArchive << m_baseAddressH;
    rArchive << m_baseAddressL;
    rArchive << m_indirectAddressH;
    rArchive << m_indirectAddressL;
    rArchive << m_effectiveAddressH;
    rArchive << m_effectiveAddressL;
    rArchive << m_bSignalReset;
    rArchive << m_bSignalIRQ;
    rArchive << m_bSignalNMI;
    rArchive << m_bBranch;
}

void PPUNES::Load(Archive& rArchive)
{
    rArchive.ReadBytes(m_vram, nVRamSize);
    rArchive.ReadBytes(m_pallette, nPalletteSize);
    rArchive.ReadBytes(m_primaryOAM, 256);
    rArchive.ReadBytes(m_secondaryOAM, 32);
    rArchive >> m_secondaryOAMWrite;
    rArchive >> m_spriteZero;
    rArchive >> m_ctrl;
    rArchive >> m_mask;
    rArchive >> m_status;
    rArchive >> m_oamAddress;
    rArchive >> m_portLatch;
    rArchive >> m_ppuDataBuffer;
    rArchive >> m_ppuAddress;
    rArchive >> m_ppuTAddress;
    rArchive >> m_ppuWriteToggle;
    rArchive >> m_ppuData;
    rArchive >> m_fineX;
    rArchive >> m_bgPatternShift0;
    rArchive >> m_bgPatternShift1;
    rArchive >> m_bgPalletteShift0;
    rArchive >> m_bgPalletteShift1;
    rArchive.ReadBytes(m_scanlineSprites, sizeof(ScanlineSprite) * 8);
    rArchive >> m_scanline;
    rArchive >> m_scanlineDot;
}

void PPUNES::Save(Archive& rArchive)
{
    rArchive.WriteBytes(m_vram, nVRamSize);
    rArchive.WriteBytes(m_pallette, nPalletteSize);
    rArchive.WriteBytes(m_primaryOAM, 256);
    rArchive.WriteBytes(m_secondaryOAM, 32);
    rArchive << m_secondaryOAMWrite;
    rArchive << m_spriteZero;
    rArchive << m_ctrl;
    rArchive << m_mask;
    rArchive << m_status;
    rArchive << m_oamAddress;
    rArchive << m_portLatch;
    rArchive << m_ppuDataBuffer;
    rArchive << m_ppuAddress;
    rArchive << m_ppuTAddress;
    rArchive << m_ppuWriteToggle;
    rArchive << m_ppuData;
    rArchive << m_fineX;
    rArchive << m_bgPatternShift0;
    rArchive << m_bgPatternShift1;
    rArchive << m_bgPalletteShift0;
    rArchive << m_bgPalletteShift1;
    rArchive.WriteBytes(m_scanlineSprites, sizeof(ScanlineSprite) * 8);
    rArchive << m_scanline;
    rArchive << m_scanlineDot;
}

void Cartridge::Load(Archive& rArchive)
{

}

void Cartridge::Save(Archive& rArchive)
{

}
