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
#include "CartMapper_4.h"
#include "CartMapper_7.h"
#include "CartMapper_9.h"
#include "CartMapper_23.h"
#include "CartMapper_24.h"
#include "CartMapper_66.h"
#include "CartMapper_69.h"
#include "CartMapper_152.h"

#define CART_MAPPER(X) new CartMapper_##X(bus, mapperID, submapperID, pPrg, nProgramSize, pChr, nCharacterSize, pCartPRGRAM, nPrgRamSize, nNVPrgRamSize, pCartCHRRAM, nChrRamSize, nNVChrRamSize)

Mapper* Mapper::CreateMapper(SystemIOBus& bus, uint16_t mapperID, uint16_t submapperID,
                                uint8_t* pPrg, uint32_t nProgramSize,
                                uint8_t* pChr, uint32_t nCharacterSize,
                                uint8_t* pCartPRGRAM, uint32_t nPrgRamSize, uint32_t nNVPrgRamSize,
                                uint8_t* pCartCHRRAM, uint32_t nChrRamSize, uint32_t nNVChrRamSize)
{
    Mapper* pMapper = nullptr;
    
    // TODO: Better factory lookup - Templates/Create function pointers?
    if(mapperID == 0)                           pMapper = CART_MAPPER(0);
    else if(mapperID == 1)                      pMapper = CART_MAPPER(1);
    else if(mapperID == 2)                      pMapper = CART_MAPPER(2);
    else if(mapperID == 3)                      pMapper = CART_MAPPER(3);
    else if(mapperID == 4)                      pMapper = CART_MAPPER(4);
    else if(mapperID == 7)                      pMapper = CART_MAPPER(7);
    else if(mapperID == 9)                      pMapper = CART_MAPPER(9);
    else if(mapperID == 23 && submapperID == 3) pMapper = CART_MAPPER(23);
    else if(mapperID == 24)                     pMapper = CART_MAPPER(24);
    else if(mapperID == 66)                     pMapper = CART_MAPPER(66);
    else if(mapperID == 69)                     pMapper = CART_MAPPER(69);
    else if(mapperID == 71)                     pMapper = CART_MAPPER(2);   // Use existing compatible mapper
    else if(mapperID == 152)                    pMapper = CART_MAPPER(152);
    else if(mapperID == 206)                    pMapper = CART_MAPPER(4);   // Use existing compatible mapper
    
    if(pMapper != nullptr)
    {
        pMapper->Initialise();
        
#if DEBUG
        printf("Created Mapper = %d,%d\n", mapperID, submapperID);
        if(nProgramSize > 0)    printf("PrgROM   = %dKB\n", nProgramSize / 1024);
        if(nCharacterSize > 0)  printf("ChrROM   = %dKB\n", nCharacterSize / 1024);
        if(nPrgRamSize > 0)     printf("PrgRAM   = %dKB\n", nPrgRamSize / 1024);
        if(nNVPrgRamSize > 0)   printf("PrgNVRAM = %dKB\n", nNVPrgRamSize / 1024);
        if(nChrRamSize > 0)     printf("ChrRAM   = %dKB\n", nChrRamSize / 1024);
        if(nNVChrRamSize > 0)   printf("ChrNVRAM = %dKB\n", nNVChrRamSize / 1024);
#endif
    }
    
    return pMapper;
}
