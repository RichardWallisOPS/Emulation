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
    const char* m_pCartPath;
    Mapper*     m_pMapper;
    uint8_t*    m_pPakData;
        
    uint8_t     m_cartVRAM[4096];
};

#endif /* Cartridge_h */
