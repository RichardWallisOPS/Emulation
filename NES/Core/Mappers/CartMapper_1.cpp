//
//  CartMapper_1.cpp
//  NES
//
//  Created by Richard Wallis on 15/12/2022.
//

#include "CartMapper_1.h"

CartMapper_1::CartMapper_1(uint8_t* pPrg, uint32_t nProgramSize, uint8_t* pChr, uint32_t nCharacterSize)
: Mapper(pPrg, nProgramSize, pChr, nCharacterSize)
{}

uint8_t CartMapper_1::cpuRead(uint16_t address)
{
    return 0x00;
}
    
void CartMapper_1::cpuWrite(uint16_t address, uint8_t byte)
{

}

uint8_t CartMapper_1::ppuRead(uint16_t address)
{
   return 0;
}

void CartMapper_1::ppuWrite(uint16_t address, uint8_t byte)
{
    
}
