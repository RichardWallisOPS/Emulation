//
//  CartMapper_3.hpp
//  NES
//
//  Created by Richard Wallis on 17/12/2022.
//

#ifndef CartMapper_3_h
#define CartMapper_3_h

#include "Cartridge.h"

class CartMapper_3 : public Mapper
{
public:
    CartMapper_3(IOBus& bus, uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize);
    
    BUS_HEADER_IMPL
private:
    uint8_t m_chrBankSelect;
};

#endif /* CartMapper_3_h */
