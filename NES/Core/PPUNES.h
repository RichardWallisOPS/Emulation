//
//  PPUNES.h
//  NES
//
//  Created by richardwallis on 15/11/2022.
//  Copyright © 2022 openplanetsoftware. All rights reserved.
//

#ifndef PPUNES_h
#define PPUNES_h

#include <cstdint>
#include "IOBus.h"

class PPUNES
{
public:
    PPUNES(IOBus& bus);
    ~PPUNES();
    
    void PowerOn();
    void Reset();
    void Tick();

private:
    IOBus& m_bus;
};

#endif /* PPUNES_h */
