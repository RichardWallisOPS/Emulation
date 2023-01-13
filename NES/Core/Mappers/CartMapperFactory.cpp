//
//  CartMapperFactory.cpp
//  NES
//
//  Created by Richard Wallis on 14/12/2022.
//

#include <stdio.h>

#include "CartMapperFactory.h"
#include "CartMapper_0.h"
#include "CartMapper_1.h"
#include "CartMapper_2.h"
#include "CartMapper_3.h"
#include "CartMapper_4.h"
#include "CartMapper_9.h"
#include "CartMapper_66.h"

#define CART_MAPPER(X) new CartMapper_##X(bus, pPrg, nProgramSize, pChr, nCharacterSize, pCartPRGRAM, nPrgRamSize, nNVPrgRamSize, pCartCHRRAM, nChrRamSize, nNVChrRamSize);

Mapper* Mapper::CreateMapper(SystemIOBus& bus, uint8_t mapperID,
                                uint8_t* pPrg, uint32_t nProgramSize,
                                uint8_t* pChr, uint32_t nCharacterSize,
                                uint8_t* pCartPRGRAM, uint32_t nPrgRamSize, uint32_t nNVPrgRamSize,
                                uint8_t* pCartCHRRAM, uint32_t nChrRamSize, uint32_t nNVChrRamSize)
{
    Mapper* pMapper = nullptr;
    
    if(mapperID == 0)
    {
        pMapper = CART_MAPPER(0);
    }
    else if(mapperID == 1)
    {
        pMapper = CART_MAPPER(1);
    }
    else if(mapperID == 2)
    {
        pMapper = CART_MAPPER(2);
    }
    else if(mapperID == 3)
    {
        pMapper = CART_MAPPER(3);
    }
    else if(mapperID == 4)
    {
        pMapper = CART_MAPPER(4);
    }
    else if(mapperID == 9)
    {
        pMapper = CART_MAPPER(9);
    }
    else if(mapperID == 66)
    {
        pMapper = CART_MAPPER(66);
    }
    
    if(pMapper != nullptr)
    {
        pMapper->Initialise();
        
        printf("Created cart mapper id = %d\n", mapperID);
    }
    else
    {
        printf("Cannot create cart mapper ID = %d\n", mapperID);
    }
    
    return pMapper;
}
