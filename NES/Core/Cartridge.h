//
//  Cartridge.h
//  NES
//
//  Created by richardwallis on 15/11/2022.
//

#ifndef Cartridge_h
#define Cartridge_h

#include "IOBus.h"

class Mapper;

class Cartridge : public IOBus
{
public:
    Cartridge(IOBus& bus, const char* pCartPath);
    ~Cartridge();
    
    bool IsValid() const;

    BUS_HEADER_IMPL
    
private:

    void LoadNVRAM();
    void SaveNVRAM();
    
private:

    // Source cart file location
    const char* m_pCartPath;
    
    // Cached NVRAM save location
    char*       m_pNVRAMPath;
    
    // Mapper logic
    Mapper*     m_pMapper;
    
    // ROM Data PRG + CHR in one block
    uint8_t*    m_pPakData;
    
    // Extra VRAM tables for certain carts
    uint8_t     m_cartVRAM[4096];
    
    // On cart RAM (optional)
    uint8_t*    m_pCartPRGRAM;
    uint8_t*    m_pCartCHRRAM;
};

#endif /* Cartridge_h */
