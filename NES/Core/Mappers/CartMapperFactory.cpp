//
//  CartMapperFactory.cpp
//  NES
//
//  Created by Richard Wallis on 14/12/2022.
//

#include "CartMapperFactory.h"
#include "CartMapper_0.h"
#include "CartMapper_1.h"
#include "CartMapper_2.h"
#include "CartMapper_3.h"
#include "CartMapper_66.h"
#include <stdio.h>

Mapper* CartMapper::CreateMapper(IOBus& bus, uint8_t mapperID, uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize)
{
    Mapper* pMapper = nullptr;
    
    if(mapperID == 0)
    {
        pMapper = new CartMapper_0(bus, pPrg, nProgramSize, pChr, nCharacterSize);
    }
    else if(mapperID == 1)
    {
        pMapper = new CartMapper_1(bus, pPrg, nProgramSize, pChr, nCharacterSize);
    }
    else if(mapperID == 2)
    {
        pMapper = new CartMapper_2(bus, pPrg, nProgramSize, pChr, nCharacterSize);
    }
    else if(mapperID == 3)
    {
        pMapper = new CartMapper_3(bus, pPrg, nProgramSize, pChr, nCharacterSize);
    }
    else if(mapperID == 66)
    {
        pMapper = new CartMapper_66(bus, pPrg, nProgramSize, pChr, nCharacterSize);
    }
    else
    {
        printf("Cannot create Mapper ID = %d\n", mapperID);
    }
    
    return pMapper;
}
