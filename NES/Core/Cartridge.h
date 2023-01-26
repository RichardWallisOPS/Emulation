//
//  Cartridge.h
//  NES
//
//  Created by richardwallis on 15/11/2022.
//

#ifndef Cartridge_h
#define Cartridge_h

#include "IOBus.h"
#include "Serialise.h"

class Mapper;

class Cartridge : public IOBus, public Serialisable
{
public:
    BUS_HEADER_DECL
    SERIALISABLE_DECL;

    Cartridge(SystemIOBus& bus, const char* pCartPath);
    Cartridge(SystemIOBus& bus, Archive& rArchive);
    ~Cartridge();
    
    bool IsValid() const;
    
    uint16_t GetMapperID() const;
    
    virtual void SystemTick(uint64_t cycleCount) override;
    virtual float AudioOut() override;
    
private:

    void Initialise(SystemIOBus& bus, const char* pCartPath);

    void LoadNVRAM();
    void SaveNVRAM();
    
private:

    // Source cart file location
    char*       m_pCartPath;
    
    // Cached NVRAM save location
    char*       m_pNVRAMPath;
    
    // Mapper logic
    Mapper*     m_pMapper;
    
    // The whole data - inc header
    uint8_t*    m_pFileData;
    uint32_t    m_fileDataSize;
    
    // ROM Data PRG + CHR in one block
    uint8_t*    m_pPakData;
    
    // Extra VRAM tables for certain carts
    uint8_t*    m_pCartVRAM;
    
    // On cart RAM (optional)
    uint8_t*    m_pCartPRGRAM;
    uint8_t*    m_pCartCHRRAM;
};

#endif /* Cartridge_h */
