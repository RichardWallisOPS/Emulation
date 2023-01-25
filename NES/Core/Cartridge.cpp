//
//  Cartridge.cpp
//  NES
//
//  Created by richardwallis on 15/11/2022.
//

#include <string>

#include "Cartridge.h"
#include "Mappers/CartMapperFactory.h"

// Parse header
struct iNesheader
{
    uint8_t m_constant[4];
    uint8_t m_prg16KChunks;
    uint8_t m_chr8KChunks;
    uint8_t m_flags6;
    uint8_t m_flags7;       // last standard flag
    uint8_t m_flags8;       // iNes1 or iNes2 onwards
    uint8_t m_flags9;
    uint8_t m_flags10;
    uint8_t m_flags11;
    uint8_t m_flags12;
    uint8_t m_flags13;
    uint8_t m_flags14;
    uint8_t m_flags15;
};

void Cartridge::Initialise(SystemIOBus& bus, const char* pCartPath)
{
    {
        uint32_t pathLen = (uint32_t)strlen(pCartPath);
        m_pCartPath = new char[pathLen + 1];
        memcpy(m_pCartPath, pCartPath, pathLen);
        m_pCartPath[pathLen] = 0;
    }

    FileStack fileHandle(fopen(m_pCartPath, "r"));
    
    if(fileHandle.handle() == nullptr)
    {
        return;
    }
    
    if(fseek(fileHandle.handle(), 0, SEEK_END) != 0)
    {
        return;
    }
    
    m_fileDataSize = (uint32_t)ftell(fileHandle.handle());
    
    if(fseek(fileHandle.handle(), 0, SEEK_SET) != 0)
    {
        return;
    }
    
    if(m_fileDataSize < sizeof(iNesheader))
    {
        return;
    }

    m_pFileData = new uint8_t[m_fileDataSize];

    if(fread(m_pFileData, 1, m_fileDataSize, fileHandle.handle()) != m_fileDataSize)
    {
        return;
    }

    iNesheader const* pHeader = (iNesheader const*)m_pFileData;
    m_pPakData = m_pFileData + sizeof(iNesheader);
    
    char const magic[4] = {0x4E, 0x45, 0x53, 0x1A};
    if(memcmp(magic, m_pFileData, 4) != 0)
    {
        return;
    }
    
    if((pHeader->m_flags6 & (1 << 2)) != 0)
    {
        // 512 byte trainer not handled
        return;
    }
    
    // Cache a NV RAM save location based on cartridge path
    {
        const char* pNVSave = ".NVRAM";
        const size_t cartPathLen = strlen(m_pCartPath);
        const size_t nvSaveExensionLen = strlen(pNVSave);
        
        m_pNVRAMPath = new char[cartPathLen + nvSaveExensionLen + 1];
        
        memcpy(m_pNVRAMPath, m_pCartPath, cartPathLen);
        memcpy(m_pNVRAMPath + cartPathLen, pNVSave, nvSaveExensionLen);
        *(m_pNVRAMPath + cartPathLen + nvSaveExensionLen) = 0;
    }
    
    bool iNes2_0 = ((pHeader->m_flags7 >> 2) & 0x3) == 2;
    
    // VRAM Name table mirror mode
    MirrorMode vramMirror = (pHeader->m_flags6 & 1) == 0 ? VRAM_MIRROR_H : VRAM_MIRROR_V;
    vramMirror = (pHeader->m_flags6 & 1 << 3) != 0 ? VRAM_MIRROR_CART4 : vramMirror;
    
    uint8_t mapperID = (pHeader->m_flags7 & 0xF0) | ((pHeader->m_flags6 & 0xF0) >> 4);
    
    // iNes 1.0 size
    uint32_t nProgramSize = (16384 * (uint32_t)pHeader->m_prg16KChunks);
    uint32_t nCharacterSize = (8192 * (uint32_t)pHeader->m_chr8KChunks);
    
    if(iNes2_0)
    {
        // TODO: MSB of ROM sizes if required iNes 1.0 is just the low bytes
    }

    // Setup cartridge data and rom mapping
    memset(m_cartVRAM, 0x00, sizeof(m_cartVRAM));

    uint8_t* pPrg = m_pPakData + 0;
    uint8_t* pChr = m_pPakData + nProgramSize;
    
    // Setup other on cart [NV]RAM
    uint32_t nPrgRamSize = 0;
    uint32_t nNVPrgRamSize = 0;
    uint32_t nChrRamSize = 0;
    uint32_t nNVChrRamSize = 0;

    // Other ram bank sizes
    if(iNes2_0)
    {
        nPrgRamSize = 64 << (pHeader->m_flags10 & 0b00001111);
        if(nPrgRamSize <= 64) nPrgRamSize = 0;
        nNVPrgRamSize = 64 << ((pHeader->m_flags10 & 0b11110000) >> 4);
        if(nNVPrgRamSize <= 64) nNVPrgRamSize = 0;
        nChrRamSize = 64 << (pHeader->m_flags11 & 0b00001111);
        if(nChrRamSize <= 64) nChrRamSize = 0;
        nNVChrRamSize = 64 << ((pHeader->m_flags11 & 0b11110000) >> 4);
        if(nNVChrRamSize <= 64) nNVChrRamSize = 0;
    }
    
    // Homebrew support
    if(nCharacterSize == 0 && nChrRamSize == 0 && nNVChrRamSize == 0)
    {
        nChrRamSize = 8192;
    }
    
    // Currently assume Volatile RAM OR Non-Volatile RAM but not both
    {
        uint32_t allocPrgRamSize = nPrgRamSize > nNVPrgRamSize ? nPrgRamSize : nNVPrgRamSize;
        if(allocPrgRamSize > 0)
        {
            m_pCartPRGRAM = new uint8_t[allocPrgRamSize];
            memset(m_pCartPRGRAM, 0x00, allocPrgRamSize);
        }
        
        uint32_t allocChrRamSize = nChrRamSize > nNVChrRamSize ? nChrRamSize : nNVChrRamSize;
        if(allocChrRamSize > 0)
        {
            m_pCartCHRRAM = new uint8_t[allocChrRamSize];
            memset(m_pCartCHRRAM, 0x00, allocPrgRamSize);
        }
    }

    // Mirror mode for cart wiring - mapper can override - unless wired to 4-Screen
    bus.SetMirrorMode(vramMirror);

    // Create specific mapper for this cart
    m_pMapper = Mapper::CreateMapper(   bus, mapperID,
                                        pPrg, nProgramSize,
                                        pChr, nCharacterSize,
                                        m_pCartPRGRAM, nPrgRamSize, nNVPrgRamSize,
                                        m_pCartCHRRAM, nChrRamSize, nNVChrRamSize);
    
}

Cartridge::Cartridge(SystemIOBus& bus, const char* pCartPath)
: m_pCartPath(nullptr)
, m_pNVRAMPath(nullptr)
, m_pMapper(nullptr)
, m_fileDataSize(0)
, m_pFileData(nullptr)
, m_pPakData(nullptr)
, m_cartVRAMAccessed(0)
, m_pCartPRGRAM(nullptr)
, m_pCartCHRRAM(nullptr)
{
    // Setup for cart
    Initialise(bus, pCartPath);
    
    // All setup - load any saved NV RAM data
    LoadNVRAM();
}

Cartridge::Cartridge(SystemIOBus& bus, Archive& rArchive)
: m_pCartPath(nullptr)
, m_pMapper(nullptr)
, m_fileDataSize(0)
, m_pFileData(nullptr)
, m_pPakData(nullptr)
, m_cartVRAMAccessed(0)
, m_pCartPRGRAM(nullptr)
, m_pCartCHRRAM(nullptr)
{
    // Reload last cart
    // Could have just saved cart data instead... safer?
    char Buffer[512];
    uint32_t pathLen = 0;
    rArchive >> pathLen;
    rArchive.ReadBytes(Buffer, pathLen);
    Buffer[pathLen] = 0;
    
    // Initialise as usual
    Initialise(bus, Buffer);
    
    // Load saved state
    Load(rArchive);
    
    // SnapShot - don't load NVRAM
}

void Cartridge::Load(Archive& rArchive)
{
    // Path or cart data is handled in the Archive Constructor
    
    // TODO: When or if required
    // m_cartVRAM
    if(m_pMapper != nullptr)
    {
        {
            rArchive >> m_cartVRAMAccessed;
            if(m_cartVRAMAccessed)
            {
                rArchive.ReadBytes(m_cartVRAM, sizeof(m_cartVRAM));
            }
        }
        
        {
            uint32_t prgRamSize = 0;
            rArchive >> prgRamSize;
            
            if(m_pCartPRGRAM != nullptr && prgRamSize > 0 && prgRamSize == m_pMapper->GetPrgRamSize())
            {
                rArchive.ReadBytes(m_pCartPRGRAM, prgRamSize);
            }
    #if DEBUG
            else if(prgRamSize > 0)
            {
                // Something has gone wrong
                *(volatile char*)(0) = 'C' | 'A' | 'R' | 'T';
            }
    #endif
        }
        
        {
            uint32_t chrRamSize = 0;
            rArchive >> chrRamSize;
            
            if(m_pCartCHRRAM != nullptr && chrRamSize > 0 && chrRamSize == m_pMapper->GetChrRamSize())
            {
                rArchive.ReadBytes(m_pCartCHRRAM, chrRamSize);
            }
    #if DEBUG
            else if(chrRamSize > 0)
            {
                // Something has gone wrong
                *(volatile char*)(0) = 'C' | 'A' | 'R' | 'T';
            }
    #endif
        }

        {
            uint8_t mapperInfo = 0;
            rArchive >> mapperInfo;
            
            if(mapperInfo == kArchiveSentinelHasData)
            {
                // We should already have a mapper object - History or Saved Data
                // Mappers don't handle memory, just offsets into this carts memory
                if(m_pMapper != nullptr)
                {
                    m_pMapper->Load(rArchive);
                }
    #if DEBUG
                else
                {
                    *(volatile char*)(0) = 'C' | 'A' | 'R' | 'T';
                }
    #endif
            }
        }
    }
}

void Cartridge::Save(Archive& rArchive) const
{
    if(m_pMapper != nullptr)
    {
        if(rArchive.GetArchiveMode() == ArchiveMode_Persistent)
        {
            uint32_t pathLen = (uint32_t)strlen(m_pCartPath);
            rArchive << pathLen;
            rArchive.WriteBytes(m_pCartPath, pathLen);
        }
        
        {
            rArchive << m_cartVRAMAccessed;
            if(m_cartVRAMAccessed)
            {
                rArchive.WriteBytes(m_cartVRAM, sizeof(m_cartVRAM));
            }
        }
        
        {
            uint32_t prgRamSize = m_pMapper->GetPrgRamSize();
            rArchive << prgRamSize;
            if(m_pCartPRGRAM != nullptr && prgRamSize > 0)
            {
                rArchive.WriteBytes(m_pCartPRGRAM, prgRamSize);
            }
        }
        
        {
            uint32_t chrRamSize = m_pMapper->GetChrRamSize();
            rArchive << chrRamSize;
            if(m_pCartCHRRAM != nullptr && chrRamSize > 0)
            {
                rArchive.WriteBytes(m_pCartCHRRAM, chrRamSize);
            }
        }

        if(m_pMapper != nullptr)
        {
            rArchive << kArchiveSentinelHasData;
            m_pMapper->Save(rArchive);
        }
        else
        {
            rArchive << kArchiveSentinelNoData;
        }
    }
}

void Cartridge::LoadNVRAM()
{
    if(m_pMapper != nullptr)
    {
        const size_t nvPrgRamSize = m_pMapper->GetNVPrgRAMSize();
        const size_t nvChrRamSize = m_pMapper->GetNVChrRAMSize();
        
        if(nvPrgRamSize + nvChrRamSize > 0)
        {
            FileStack fileLoad(fopen(m_pNVRAMPath, "r"));
            if(fileLoad.handle() != nullptr)
            {
                if(nvPrgRamSize > 0)
                {
                    fread(m_pCartPRGRAM, 1, nvPrgRamSize, fileLoad.handle());
                }
                if(nvChrRamSize > 0)
                {
                    fread(m_pCartCHRRAM, 1, nvChrRamSize, fileLoad.handle());
                }
            }
        }
    }
}

void Cartridge::SaveNVRAM()
{
    if(m_pMapper != nullptr)
    {
        const size_t nvPrgRamSize = m_pMapper->GetNVPrgRAMSize();
        const size_t nvChrRamSize = m_pMapper->GetNVChrRAMSize();
        
        if(nvPrgRamSize + nvChrRamSize > 0)
        {
            FileStack fileSave(fopen(m_pNVRAMPath, "w"));
            if(fileSave.handle() != nullptr)
            {
                if(nvPrgRamSize > 0)
                {
                    fwrite(m_pCartPRGRAM, 1, nvPrgRamSize, fileSave.handle());
                }
                if(nvChrRamSize > 0)
                {
                    fwrite(m_pCartCHRRAM, 1, nvChrRamSize, fileSave.handle());
                }
            }
        }
    }
}

Cartridge::~Cartridge()
{
    // TODO: more often than stutdown?  Every update is too often - maybe the end of a frame or each second?
    SaveNVRAM();
    
    if(m_pCartPath != nullptr)
    {
        delete [] m_pCartPath;
        m_pCartPath = nullptr;
    }
    
    if(m_pNVRAMPath != nullptr)
    {
        delete [] m_pNVRAMPath;
        m_pNVRAMPath = nullptr;
    }
    
    if(m_pMapper != nullptr)
    {
        delete m_pMapper;
        m_pMapper = nullptr;
    }
    
    if(m_pCartPRGRAM != nullptr)
    {
        delete [] m_pCartPRGRAM;
        m_pCartPRGRAM = nullptr;
    }
    
    if(m_pCartCHRRAM != nullptr)
    {
        delete [] m_pCartCHRRAM;
        m_pCartCHRRAM = nullptr;
    }
    
    if(m_pFileData != nullptr)
    {
        delete [] m_pFileData;
        m_pFileData = nullptr;
    }
}

bool Cartridge::IsValid() const
{
    return m_pMapper != nullptr;
}

uint16_t Cartridge::GetMapperID() const
{
    if(m_pMapper != nullptr)
    {
        return m_pMapper->GetMapperID();
    }
    return 0xFFFF;
}

void Cartridge::SystemTick(uint64_t cycleCount)
{
    if(m_pMapper != nullptr)
    {
        m_pMapper->SystemTick(cycleCount);
    }
}

float Cartridge::AudioOut()
{
    if(m_pMapper != nullptr)
    {
        return m_pMapper->AudioOut();
    }
    return 0.f;
}

uint8_t Cartridge::cpuRead(uint16_t address)
{
    if(m_pMapper != nullptr)
    {
        return m_pMapper->cpuRead(address);
    }
    return 0x00;
}

void Cartridge::cpuWrite(uint16_t address, uint8_t byte)
{
    if(m_pMapper != nullptr)
    {
        m_pMapper->cpuWrite(address, byte);
    }
}

uint8_t Cartridge::ppuRead(uint16_t address)
{
    if(address >= 0 && address <= 0x1FFF)
    {
        if(m_pMapper != nullptr)
        {
            return m_pMapper->ppuRead(address);
        }
    }
    else if(address >= 0x2000 && address <= 0x3EFF)
    {
        m_cartVRAMAccessed = 1;
        uint32_t cartAddress = (address - 0x2000) % 4096;
        return m_cartVRAM[cartAddress];
    }
    return address & 0xFF; // open bus low byte return
}

void Cartridge::ppuWrite(uint16_t address, uint8_t byte)
{
    if(address >= 0 && address <= 0x1FFF)
    {
        if(m_pMapper != nullptr)
        {
            m_pMapper->ppuWrite(address, byte);
        }
    }
    else if(address >= 0x2000 && address <= 0x3EFF)
    {
        m_cartVRAMAccessed = 1;
        uint32_t cartAddress = (address - 0x2000) % 4096;
        m_cartVRAM[cartAddress] = byte;
    }
}
