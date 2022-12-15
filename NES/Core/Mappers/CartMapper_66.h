//
//  CartMapper_66.hpp
//  NES
//
//  Created by Richard Wallis on 15/12/2022.
//

#ifndef CartMapper_66_h
#define CartMapper_66_h

#include "Cartridge.h"

class CartMapper_66 : public Mapper
{
public:
    CartMapper_66(IOBus& bus, uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize);
    
    BUS_HEADER_IMPL
    
private:
    uint8_t m_bankSelect;
};

#endif /* CartMapper_66_h */
