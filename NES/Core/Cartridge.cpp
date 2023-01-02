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

class FileStack
{
public:
    FileStack(FILE* pFile)
    : m_pHandle(pFile)
    {}
    ~FileStack()
    {
        if(m_pHandle != nullptr)
        {
            fclose(m_pHandle);
            m_pHandle = nullptr;
        }
    }
    
    FILE* handle() {return m_pHandle;}

private:
    FILE* m_pHandle;
};

class MemStack
{
public:
    MemStack(uint32_t size)
    : m_pData(nullptr)
    {
        m_pData = new uint8_t[size];
    }
    ~MemStack()
    {
        if(m_pData != nullptr)
        {
            delete m_pData;
            m_pData = nullptr;
        }
    }
    
    uint8_t* mem() {return m_pData;}

private:
    uint8_t* m_pData;
};

Cartridge::Cartridge(SystemIOBus& bus, const char* pCartPath)
: m_pCartPath(pCartPath)
, m_pMapper(nullptr)
, m_pPakData(nullptr)
, m_pCartPRGRAM(nullptr)
, m_pCartCHRRAM(nullptr)
{
    FileStack fileHandle(fopen(pCartPath, "r"));
    
    if(fileHandle.handle() == nullptr)
    {
        return;
    }
    
    if(fseek(fileHandle.handle(), 0, SEEK_END) != 0)
    {
        return;
    }
    
    uint32_t dataSize = (uint32_t)ftell(fileHandle.handle());
    
    if(fseek(fileHandle.handle(), 0, SEEK_SET) != 0)
    {
        return;
    }
    
    if(dataSize < sizeof(iNesheader))
    {
        return;
    }
    
    MemStack fileBytes(dataSize);

    if(fread(fileBytes.mem(), 1, dataSize, fileHandle.handle()) != dataSize)
    {
        return;
    }

    iNesheader const* pHeader = (iNesheader const*)fileBytes.mem();
    uint8_t const* pCartData = fileBytes.mem() + sizeof(iNesheader);
    
    char const magic[4] = {0x4E, 0x45, 0x53, 0x1A};
    if(memcmp(magic, fileBytes.mem(), 4) != 0)
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
        const char* pNVSave = ".NVPRGRAM";
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

    const uint32_t nCartDataSize = nProgramSize + nCharacterSize;

    m_pPakData = new uint8_t[nCartDataSize];
    memcpy(m_pPakData, pCartData, nCartDataSize);

    uint8_t* pPrg = m_pPakData + 0;
    uint8_t* pChr = m_pPakData + nProgramSize;
    
    // Setup other on cart [NV]RAM
    uint32_t nPrgRamSize = 0;
    uint32_t nNVPrgRamSize = 0;
    uint32_t nChrRamSize = 0;
    uint32_t nChrNVRamSize = 0;

    // Other ram bank sizes
    if(iNes2_0)
    {
        nPrgRamSize = 64 << (pHeader->m_flags10 & 0b00001111);
        if(nPrgRamSize <= 64) nPrgRamSize = 0;
        nNVPrgRamSize = 64 << ((pHeader->m_flags10 & 0b11110000) >> 4);
        if(nNVPrgRamSize <= 64) nNVPrgRamSize = 0;
        nChrRamSize = 64 << (pHeader->m_flags11 & 0b00001111);
        if(nChrRamSize <= 64) nChrRamSize = 0;
        nChrNVRamSize = 64 << ((pHeader->m_flags11 & 0b11110000) >> 4);
        if(nChrNVRamSize <= 64) nChrNVRamSize = 0;
    }
    
    // Currently assume Volatile RAM OR Non-Volatile RAM but not both
    {
        uint32_t allocPrgRamSize = nPrgRamSize > nNVPrgRamSize ? nPrgRamSize : nNVPrgRamSize;
        if(allocPrgRamSize > 0)
        {
            m_pCartPRGRAM = new uint8_t[allocPrgRamSize];
            memset(m_pCartPRGRAM, 0x00, allocPrgRamSize);
        }
        
        uint32_t allocChrRamSize = nChrRamSize > nChrNVRamSize ? nChrRamSize : nChrNVRamSize;
        if(allocChrRamSize > 0)
        {
            m_pCartCHRRAM = new uint8_t[allocChrRamSize];
            memset(m_pCartCHRRAM, 0x00, allocPrgRamSize);
        }
    }

    // Mirror mode for cart wiring - mapper can override
    bus.SetMirrorMode(vramMirror);

    // Create specific mapper for this cart
    m_pMapper = Mapper::CreateMapper(   bus, mapperID,
                                        pPrg, nProgramSize,
                                        pChr, nCharacterSize,
                                        m_pCartPRGRAM, nPrgRamSize, nNVPrgRamSize,
                                        m_pCartCHRRAM, nChrRamSize, nChrNVRamSize);
    
    // All setup - load any saved NV RAM data
    LoadNVRAM();
}

void Cartridge::LoadNVRAM()
{
    if(m_pMapper != nullptr)
    {
        FileStack fileLoad(fopen(m_pNVRAMPath, "r"));
        if(fileLoad.handle() != nullptr)
        {
            fread(m_pCartPRGRAM, 1, m_pMapper->GetNVPrgRAMSize(), fileLoad.handle());
        }
    }
}

void Cartridge::SaveNVRAM()
{
    if(m_pMapper != nullptr)
    {
        FileStack fileSave(fopen(m_pNVRAMPath, "w"));
        if(fileSave.handle() != nullptr)
        {
            fwrite(m_pCartPRGRAM, 1, m_pMapper->GetNVPrgRAMSize(), fileSave.handle());
        }
    }
}

Cartridge::~Cartridge()
{
    SaveNVRAM();    // TODO: more often than stutdown?
    
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
    
    if(m_pPakData != nullptr)
    {
        delete [] m_pPakData;
        m_pPakData = nullptr;
    }
}

bool Cartridge::IsValid() const
{
    return m_pMapper != nullptr;
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
        return m_pMapper->cpuWrite(address, byte);
    }
}

uint8_t Cartridge::ppuRead(uint16_t address)
{
    if(address>= 0 && address <= 0x1FFF)
    {
        if(m_pMapper != nullptr)
        {
            return m_pMapper->ppuRead(address);
        }
    }
    else if(address >= 0x2000 && address <= 0x3EFF)
    {
        uint32_t cartAddress = (address - 0x2000) % 4096;
        return m_cartVRAM[cartAddress];
    }
    return address & 0xFF; // open bus low byte return
}

void Cartridge::ppuWrite(uint16_t address, uint8_t byte)
{
    if(address>= 0 && address <= 0x1FFF)
    {
        if(m_pMapper != nullptr)
        {
            m_pMapper->ppuWrite(address, byte);
        }
    }
    else if(address >= 0x2000 && address <= 0x3EFF)
    {
        uint32_t cartAddress = (address - 0x2000) % 4096;
        m_cartVRAM[cartAddress] = byte;
    }
}
