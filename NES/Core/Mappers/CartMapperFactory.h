//
//  CartMapperFactory.h
//  NES
//
//  Created by Richard Wallis on 14/12/2022.
//

#ifndef CartMapperFactory_h
#define CartMapperFactory_h

#include "IOBus.h"

class Mapper : public IOBus
{
public:
    Mapper( IOBus& bus,
            uint8_t* pPrg, uint32_t nProgramSize,
            uint8_t* pChr, uint32_t nCharacterSize,
            uint8_t* pCartPRGRAM, uint32_t nPrgRamSize, uint32_t nNVPrgRamSize,
            uint8_t* pCartCHRRAM, uint32_t nChrRamSize, uint32_t nChrNVRamSize)
    : m_bus(bus)
    , m_pPrg(pPrg)
    , m_pChr(pChr)
    , m_nProgramSize(nProgramSize)
    , m_nCharacterSize(nCharacterSize)
    , m_pCartPRGRAM(pCartPRGRAM)
    , m_nPrgRamSize(nPrgRamSize)
    , m_nNVPrgRamSize(nNVPrgRamSize)
    , m_pCartCHRRAM(pCartCHRRAM)
    , m_nChrRamSize(nChrRamSize)
    , m_nChrNVRamSize(nChrNVRamSize)
    {}
    
    virtual ~Mapper() {}
    
    
    static Mapper* CreateMapper(    IOBus& bus, uint8_t mapperID,
                                    uint8_t* pPrg, uint32_t nProgramSize,
                                    uint8_t* pChr, uint32_t nCharacterSize,
                                    uint8_t* pCartPRGRAM, uint32_t nPrgRamSize, uint32_t nNVPrgRamSize,
                                    uint8_t* pCartCHRRAM, uint32_t nChrRamSize, uint32_t nChrNVRamSize);
        
protected:

    IOBus&      m_bus;
    
    uint8_t*    m_pPrg;
    uint8_t*    m_pChr;
    
    uint32_t    m_nProgramSize;
    uint32_t    m_nCharacterSize;
    
    uint8_t*    m_pCartPRGRAM;
    uint32_t    m_nPrgRamSize;
    uint32_t    m_nNVPrgRamSize;
    
    uint8_t*    m_pCartCHRRAM;
    uint32_t    m_nChrRamSize;
    uint32_t    m_nChrNVRamSize;
};

#endif /* CartMapperFactory_h */
