//
//  APUNES.cpp
//  NES
//
//  Created by Richard Wallis on 04/01/2023.
//

#include "APUNES.h"

APUNES::APUNES()
{

}

APUNES::~APUNES()
{

}

void APUNES::Load(Archive& rArchive)
{

}

void APUNES::Save(Archive& rArchive)
{

}

void APUNES::Tick()
{

}

uint8_t APUNES::cpuRead(uint8_t port)
{
    // 0x00 - 0x1F (0x4000 - 0x401F)
    return 0;
}

void APUNES::cpuWrite(uint8_t port, uint8_t byte)
{
    // 0x00 - 0x1F (0x4000 - 0x401F)
}
