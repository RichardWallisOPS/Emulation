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
    Mapper(uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize)
    : m_pPrg(pPrg)
    , m_pChr(pChr)
    , m_nProgramSize(nProgramSize)
    , m_nCharacterSize(nCharacterSize)
    {}
    
    virtual ~Mapper() {}
        
protected:

    uint8_t* m_pPrg;
    uint8_t* m_pChr;
    uint32_t m_nProgramSize;
    uint32_t m_nCharacterSize;
};

class Cartridge : public IOBus
{
public:
    Cartridge(uint8_t mapperID, uint8_t const* pPakData, uint8_t nPakPrgCount, uint8_t nPakChrCount);
    ~Cartridge();
    
    bool IsValid() const;
    
    virtual uint8_t cpuRead(uint16_t address) override;
    virtual void cpuWrite(uint16_t address, uint8_t byte) override;
    virtual uint8_t ppuRead(uint16_t address) override;
    virtual void ppuWrite(uint16_t address, uint8_t byte) override;
    
private:
    Mapper* m_pMapper;
    uint8_t* m_pPakData;
        
    uint8_t m_cartVRAM[4096];   // TODO - make this generic or only when required?
    uint8_t m_cartCHRRAM[8192]; // Allow homebrew
};

#endif /* Cartridge_h */
