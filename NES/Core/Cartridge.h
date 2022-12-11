//
//  Cartridge.h
//  NES
//
//  Created by richardwallis on 15/11/2022.
//

#ifndef Cartridge_h
#define Cartridge_h

#include "IOBus.h"

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
    uint8_t  m_nMapperID;
    uint8_t* m_pPakData;
    uint8_t* m_pPrg;
    uint8_t* m_pChr;
    uint32_t m_nProgramSize;
    uint32_t m_nCharacterSize;
    
    uint8_t m_cartVRAM[4096];   // TODO - make this generic or only when required?
};

#endif /* Cartridge_h */
