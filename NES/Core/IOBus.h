//
//  Bus.h
//  NES
//
//  Created by richardwallis on 15/11/2022.
//  Copyright © 2022 openplanetsoftware. All rights reserved.
//

#ifndef IOBus_h
#define IOBus_h

#include <cstdint>

class IOBus
{
public:
    virtual uint8_t cpuRead(uint16_t address) = 0;
    virtual void cpuWrite(uint16_t address, uint8_t byte) = 0;
    virtual uint8_t ppuRead(uint16_t address) = 0;
    virtual void ppuWrite(uint16_t address, uint8_t byte) = 0;
};

#endif /* IOBus_h */
