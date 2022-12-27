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
    Cartridge(IOBus& bus, void const* pData, uint32_t dataSize);
    ~Cartridge();
    
    bool IsValid() const;

    BUS_HEADER_IMPL
    
private:
    Mapper*     m_pMapper;
    uint8_t*    m_pPakData;
        
    uint8_t     m_cartVRAM[4096];
};

#endif /* Cartridge_h */
