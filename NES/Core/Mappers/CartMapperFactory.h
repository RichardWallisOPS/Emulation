//
//  CartMapperFactory.h
//  NES
//
//  Created by Richard Wallis on 14/12/2022.
//

#ifndef CartMapperFactory_h
#define CartMapperFactory_h

#include "IOBus.h"
#include "Serialise.h"

class Mapper : public IOBus, public Serialisable
{
public:
    Mapper( SystemIOBus& bus,
            uint8_t* pPrg, uint32_t nProgramSize,
            uint8_t* pChr, uint32_t nCharacterSize,
            uint8_t* pCartPRGRAM, uint32_t nPrgRamSize, uint32_t nNVPrgRamSize,
            uint8_t* pCartCHRRAM, uint32_t nChrRamSize, uint32_t nNVChrRamSize)
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
    , m_nNVChrRamSize(nNVChrRamSize)
    {}
    
    virtual ~Mapper() {}
    virtual void Initialise() {}
    
    // Return larger of the two
    uint32_t GetPrgRamSize() const   { return m_nPrgRamSize > m_nNVPrgRamSize ? m_nPrgRamSize : m_nNVPrgRamSize;}
    
    // Just the Non volatile prg ram
    uint32_t GetNVPrgRAMSize() const {return m_nNVPrgRamSize;}
    
    // Return larger of the two
    uint32_t GetChrRamSize() const   { return m_nChrRamSize > m_nNVChrRamSize ? m_nChrRamSize : m_nNVChrRamSize;}
    
    // Just the Non volatile chr ram
    uint32_t GetNVChrRAMSize() const {return m_nNVChrRamSize;}
    
    static Mapper* CreateMapper(SystemIOBus& bus, uint8_t mapperID,
                                    uint8_t* pPrg, uint32_t nProgramSize,
                                    uint8_t* pChr, uint32_t nCharacterSize,
                                    uint8_t* pCartPRGRAM, uint32_t nPrgRamSize, uint32_t nNVPrgRamSize,
                                    uint8_t* pCartCHRRAM, uint32_t nChrRamSize, uint32_t nNVChrRamSize);
        
protected:

    SystemIOBus& m_bus;
    
    uint8_t*    m_pPrg;
    uint8_t*    m_pChr;
    
    uint32_t    m_nProgramSize;
    uint32_t    m_nCharacterSize;
    
    uint8_t*    m_pCartPRGRAM;
    uint32_t    m_nPrgRamSize;
    uint32_t    m_nNVPrgRamSize;
    
    uint8_t*    m_pCartCHRRAM;
    uint32_t    m_nChrRamSize;
    uint32_t    m_nNVChrRamSize;
};

#define MAPPER_HEADER_DECL  using Mapper::Mapper; \
                            virtual void Initialise() override;

#endif /* CartMapperFactory_h */
