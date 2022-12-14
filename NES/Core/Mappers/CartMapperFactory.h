//
//  CartMapperFactory.h
//  NES
//
//  Created by Richard Wallis on 14/12/2022.
//

#ifndef CartMapperFactory_h
#define CartMapperFactory_h

#include "Cartridge.h"

namespace CartMapper
{
    Mapper* CreateMapper(uint8_t mapperID, uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize);
};

#endif /* CartMapperFactory_h */
