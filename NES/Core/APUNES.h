//
//  APUNES.h
//  NES
//
//  Created by Richard Wallis on 04/01/2023.
//

#ifndef APUNES_h
#define APUNES_h

#include <cstdint>
#include "IOBus.h"
#include "Serialise.h"

class APUNES : public Serialisable
{
public:
    SERIALISABLE_DECL
    
    APUNES(SystemIOBus& bus);
    ~APUNES();
    
    void Tick();
    
    uint8_t cpuRead(uint16_t address);
    void cpuWrite(uint16_t address, uint8_t byte);

private:
    SystemIOBus& m_bus;
    uint16_t m_frameCounter;
    
    uint8_t m_frameCountModeAndInterrupt;
};

#endif /* APUNES_h */
