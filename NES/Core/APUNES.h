//
//  APUNES.h
//  NES
//
//  Created by Richard Wallis on 04/01/2023.
//

#ifndef APUNES_h
#define APUNES_h

#include <cstdint>
#include "Serialise.h"

class APUNES : public Serialisable
{
public:
    SERIALISABLE_DECL
    
    APUNES();
    ~APUNES();
    
    void Tick();
    
    uint8_t cpuRead(uint16_t address);
    void cpuWrite(uint16_t address, uint8_t byte);

private:
};

#endif /* APUNES_h */
