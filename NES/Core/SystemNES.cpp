//
//  SystemNES.cpp
//  NES
//
//  Created by richardwallis on 15/11/2022.
//
//

#include "SystemNES.h"
#include <string>

#include "Serialise.h"

SystemNES::SystemNES()
: m_bPowerOn(false)
, m_cycleCount(0)
, m_cpu(*this)
, m_ppu(*this)
, m_apu(*this)
, m_pCart(nullptr)
, m_controller1(0)
, m_controller2(0)
, m_controllerLatch1(0)
, m_controllerLatch2(0)
, m_dmaAddress(0xFFFF)
, m_DMAMode(DMA_OFF)
{
    memset(m_ram, 0x00, nRamSize);
}

SystemNES::~SystemNES()
{
    EjectCartridge();
}

void SystemNES::Load(Archive& rArchive)
{
    uint8_t cartInfo = 0;
    rArchive >> cartInfo;
    
    if(cartInfo == kArchiveSentinalHasData)
    {
        if(m_pCart != nullptr && rArchive.GetArchiveMode() == ArchiveMode_History)
        {
            m_pCart->Load(rArchive);
        }
        else
        {
            if(m_pCart != nullptr )
            {
                delete m_pCart;
                m_pCart = nullptr;
            }
            m_pCart = new Cartridge(*this, rArchive);
        }
    }
    
    rArchive >> m_bPowerOn;
    rArchive >> m_cycleCount;
    rArchive.ReadBytes(m_ram, nRamSize);
    rArchive >> m_controller1;
    rArchive >> m_controller2;
    rArchive >> m_controllerLatch1;
    rArchive >> m_controllerLatch2;
    rArchive >> m_dmaAddress;
    rArchive >> m_dmaData;
    rArchive >> m_DMAMode;
    
    m_cpu.Load(rArchive);
    m_ppu.Load(rArchive);
    m_apu.Load(rArchive);
}

void SystemNES::Save(Archive& rArchive) const
{
    if(m_pCart != nullptr)
    {
        rArchive << kArchiveSentinalHasData;
        m_pCart->Save(rArchive);
    }
    else
    {
        rArchive << kArchiveSentinalNoData;
    }
    
    rArchive << m_bPowerOn;
    rArchive << m_cycleCount;
    rArchive.WriteBytes(m_ram, nRamSize);
    rArchive << m_controller1;
    rArchive << m_controller2;
    rArchive << m_controllerLatch1;
    rArchive << m_controllerLatch2;
    rArchive << m_dmaAddress;
    rArchive << m_dmaData;
    rArchive << m_DMAMode;
    
    m_cpu.Save(rArchive);
    m_ppu.Save(rArchive);
    m_apu.Save(rArchive);
}

void SystemNES::PowerOn()
{
    m_cycleCount = 0;
    m_dmaAddress = 0xFFFF;
    m_DMAMode = DMA_OFF;
    
    m_ppu.PowerOn();
    m_cpu.PowerOn();
    
    m_controller1 = 0;
    m_controller2 = 0;
    m_controllerLatch1 = 0;
    m_controllerLatch2 = 0;
    
    memset(m_ram, 0x00, nRamSize);
    
    m_bPowerOn = true;
}

void SystemNES::Reset()
{
    SignalReset(true);
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

bool SystemNES::InsertCartridge(const char* pCartPath)
{
    EjectCartridge();
    
    m_pCart = new Cartridge(*this, pCartPath);
    
    if(m_pCart != nullptr)
    {
        // Workarounds for hard to emulate games - mapper 7 detection good enough for now
        // Anything for a specific game will need something like cart data crc
        {
            uint8_t compatabilityFlag = 0;
            uint16_t mapperID = m_pCart->GetMapperID();
            if(mapperID == 7)
            {
                compatabilityFlag = CompatabilityModeFlag_NMI | CompatabilityModeFlag_SPRITE0;
            }
            m_ppu.SetCompatabilityMode(compatabilityFlag);
        }
        
        return m_pCart->IsValid();
    }
    
    return false;
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

void SystemNES::SetMirrorMode(MirrorMode mode)
{
    m_ppu.SetMirrorMode(mode);
}

uint64_t SystemNES::CycleCount() const
{
    return m_cycleCount;
}

void SystemNES::SetControllerBits(uint8_t port, uint8_t bits)
{
    if(port == 0)
    {
        m_controller1 = bits;
    }
    else if(port == 1)
    {
        m_controller2 = bits;
    }
}

void SystemNES::Tick()
{
    // NTSC:
    // Master clock = 21.477272 MHz
    // PPU clock    = 21.477272 /  4 = 5.369318 MHz
    // CPU clock    = 21.477272 / 12 = 1.789773 MHz
    // 3 PPU clock ticks to 1 CPU clock tick
    // 2 CPU ticks to 1 APU tick - but ticking at same rate due to its internal requirements
    if(m_bPowerOn)
    {
        ++m_cycleCount;
        {
            m_ppu.Tick();
        }
        
        if((m_cycleCount % 3) == 0 && m_DMAMode == DMA_OFF)
        {
            m_cpu.Tick();
        }
        
        if((m_cycleCount % 3) == 0)
        {
            m_apu.Tick();
        }

        // DMA handling
        if(m_DMAMode != DMA_OFF)
        {
            if(m_DMAMode == DMA_READ)
            {
                m_dmaData = this->cpuRead(m_dmaAddress);
                m_DMAMode = DMA_WRITE;
            }
            else if(m_DMAMode == DMA_WRITE)
            {
                m_ppu.cpuWrite(0x2004, m_dmaData);
                
                if((m_dmaAddress & 0xFF) != 0xFF)
                {
                    m_DMAMode = DMA_READ;
                    ++m_dmaAddress;
                }
                else
                {
                    m_DMAMode = DMA_OFF;
                }
            }
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
        return m_ppu.cpuRead(address);
    }
    else if(address >= 0x4000 && address <= 0x401F)
    {
        // controller latches
        if(address == 0x4016)
        {
            uint8_t result = m_controllerLatch1 & 1;
            m_controllerLatch1 >>= 1;
            m_controllerLatch1 |= 1 << 7;
            return result;
        }
        else if(address == 0x4017)
        {
            uint8_t result = m_controllerLatch2 & 1;
            m_controllerLatch2 >>= 1;
            m_controllerLatch2 |= 1 << 7;
            return result;
        }
        else
        {
            // APU registers
            return m_apu.cpuRead(address);
        }
    }
    else if(address >= 0x4020 && address <= 0xFFFF && m_pCart != nullptr)
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
        m_ppu.cpuWrite(address, byte);
    }
    else if(address >= 0x4000 && address <= 0x401F)
    {
        if(address == 0x4014)
        {
            // Writing value XX (high byte) to 0x4014 will upload 256 bytes of data from
            // 0xXX00 - 0xXXFF at PPU address OAMADDR
            m_dmaAddress = uint16_t(byte) << 8;
            m_DMAMode = DMA_READ;
        }
        else if(address == 0x4016)
        {
            if((byte & 1) == 0)
            {
                m_controllerLatch1 = m_controller1;
                m_controllerLatch2 = m_controller2;
            }
        }
        else
        {
            m_apu.cpuWrite(address, byte);
        }
    }
    else if(address >= 0x4020 && address <= 0xFFFF && m_pCart != nullptr)
    {
        m_pCart->cpuWrite(address, byte);
    }
}

uint8_t SystemNES::ppuRead(uint16_t address)
{
    if(m_pCart != nullptr)
    {
        return m_pCart->ppuRead(address);
    }
    
    return 0;
}

void SystemNES::ppuWrite(uint16_t address, uint8_t byte)
{
    if(m_pCart != nullptr)
    {
        m_pCart->ppuWrite(address, byte);
    }
}

void SystemNES::SetVideoOutputDataPtr(uint32_t* pVideoOutData)
{
    m_ppu.SetVideoOutputDataPtr(pVideoOutData);
}

void SystemNES::SetAudioOutputBuffer(APUAudioBuffer* pAudioBuffer)
{
    m_apu.SetAudioOutputBuffer(pAudioBuffer);
}
