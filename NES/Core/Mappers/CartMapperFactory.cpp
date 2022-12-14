//
//  CartMapperFactory.cpp
//  NES
//
//  Created by Richard Wallis on 14/12/2022.
//

#include "CartMapperFactory.h"
#include "CartMapper_0.h"
#include "CartMapper_2.h"

Mapper* CartMapper::CreateMapper(uint8_t mapperID, uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize)
{
    Mapper* pMapper = nullptr;
    
    if(mapperID == 0)
    {
        pMapper = new CartMapper_0(pPrg, nProgramSize, pChr, nCharacterSize);
    }
    else if(mapperID == 2)
    {
        pMapper = new CartMapper_2(pPrg, nProgramSize, pChr, nCharacterSize);
    }
    
    return pMapper;
}
