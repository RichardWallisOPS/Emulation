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
#include "CartMapper_66.h"

Mapper* Mapper::CreateMapper(   IOBus& bus, uint8_t mapperID,
                                uint8_t* pPrg, uint32_t nProgramSize,
                                uint8_t* pChr, uint32_t nCharacterSize,
                                uint32_t nPrgRamSize, uint32_t nNVPrgRamSize,
                                uint32_t nChrRamSize, uint32_t nChrNVRamSize)
{
    Mapper* pMapper = nullptr;
    
    if(mapperID == 0)
    {
        pMapper = new CartMapper_0(bus, pPrg, nProgramSize, pChr, nCharacterSize, nPrgRamSize, nNVPrgRamSize, nChrRamSize, nChrNVRamSize);
    }
    else if(mapperID == 1)
    {
        pMapper = new CartMapper_1(bus, pPrg, nProgramSize, pChr, nCharacterSize, nPrgRamSize, nNVPrgRamSize, nChrRamSize, nChrNVRamSize);
    }
    else if(mapperID == 2)
    {
        pMapper = new CartMapper_2(bus, pPrg, nProgramSize, pChr, nCharacterSize, nPrgRamSize, nNVPrgRamSize, nChrRamSize, nChrNVRamSize);
    }
    else if(mapperID == 3)
    {
        pMapper = new CartMapper_3(bus, pPrg, nProgramSize, pChr, nCharacterSize, nPrgRamSize, nNVPrgRamSize, nChrRamSize, nChrNVRamSize);
    }
    else if(mapperID == 4)
    {
        pMapper = new CartMapper_4(bus, pPrg, nProgramSize, pChr, nCharacterSize, nPrgRamSize, nNVPrgRamSize, nChrRamSize, nChrNVRamSize);
    }
    else if(mapperID == 66)
    {
        pMapper = new CartMapper_66(bus, pPrg, nProgramSize, pChr, nCharacterSize, nPrgRamSize, nNVPrgRamSize, nChrRamSize, nChrNVRamSize);
    }
    
    if(pMapper != nullptr)
    {
        printf("Created cart mapper id = %d\n", mapperID);
    }
    else
    {
        printf("Cannot create cart mapper ID = %d\n", mapperID);
    }
    
    return pMapper;
}
