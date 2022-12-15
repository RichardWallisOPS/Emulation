//
//  CartMapper_1.h
//  NES
//
//  Created by Richard Wallis on 15/12/2022.
//

#ifndef CartMapper_1_h
#define CartMapper_1_h

#include "Cartridge.h"

class CartMapper_1 : public Mapper
{
public:
    CartMapper_1(uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize);
    
    BUS_HEADER_IMPL
private:

};


#endif /* CartMapper_1_h */
