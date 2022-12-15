//
//  Cartridge.h
//  NES
//
//  Created by richardwallis on 15/11/2022.
//

#ifndef Cartridge_h
#define Cartridge_h

#include "IOBus.h"

class Mapper : public IOBus
{
public:
    Mapper(IOBus& bus, uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize)
    : m_bus(bus)
    , m_pPrg(pPrg)
    , m_pChr(pChr)
    , m_nProgramROMSize(nProgramSize)
    , m_nCharacterROMSize(nCharacterSize)
    {}
    
    virtual ~Mapper() {}
        
protected:

    IOBus&      m_bus;
    uint8_t*    m_pPrg;
    uint8_t*    m_pChr;
    uint32_t    m_nProgramROMSize;
    uint32_t    m_nCharacterROMSize;
};

class Cartridge : public IOBus
{
public:
    Cartridge(IOBus& bus, uint8_t mapperID, MirrorMode mirrorMode, uint8_t const* pPakData, uint8_t nPakPrgCount, uint8_t nPakChrCount);
    ~Cartridge();
    
    bool IsValid() const;
    
    BUS_HEADER_IMPL
    
private:
    MirrorMode  m_mirrorMode;
    Mapper*     m_pMapper;
    uint8_t*    m_pPakData;
        
    uint8_t     m_cartVRAM[4096];
};

#endif /* Cartridge_h */
