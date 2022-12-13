//
//  CartMapper_2.h
//  NES
//
//  Created by Richard Wallis on 13/12/2022.
//

#ifndef CartMapper_2_h
#define CartMapper_2_h

#include "Cartridge.h"

class CartMapper_2 : public Mapper
{
public:
    CartMapper_2(uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize);
    
    virtual uint8_t cpuRead(uint16_t address) override;
    virtual void cpuWrite(uint16_t address, uint8_t byte) override;
    virtual uint8_t ppuRead(uint16_t address) override;
    virtual void ppuWrite(uint16_t address, uint8_t byte) override;
    
private:
    uint8_t m_bankSelect;
    
    static const uint32_t nChrRAMSize = 8192;
    uint8_t m_cartCHRRAM[nChrRAMSize];
};

#endif /* CartMapper_2_h */
