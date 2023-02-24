//
//  IOBus.h
//  NES
//
//  Created by richardwallis on 15/11/2022.
//

#ifndef IOBus_h
#define IOBus_h

#include "CoreDefines.h"

// Common bus functions
class IOBus
{
public:
    // Everything must handle these cases
    virtual uint8_t cpuRead(uint16_t address) = 0;
    virtual void    cpuWrite(uint16_t address, uint8_t byte) = 0;
    virtual uint8_t ppuRead(uint16_t address) = 0;
    virtual void    ppuWrite(uint16_t address, uint8_t byte) = 0;
    
    // Not everything needs or uses these, but they are available on the bus
    virtual float   AudioOut()                      { return 0.f; }
    virtual void    SystemTick(uint64_t cycleCount) {}
};

#define BUS_HEADER_DECL     virtual uint8_t cpuRead(uint16_t address) override; \
                            virtual void cpuWrite(uint16_t address, uint8_t byte) override; \
                            virtual uint8_t ppuRead(uint16_t address) override; \
                            virtual void ppuWrite(uint16_t address, uint8_t byte) override; \

// Core System has extra features
class SystemIOBus : public IOBus
{
public:
    virtual void SignalReset(bool bSignal)      {}
    virtual void SignalNMI(bool bSignal)        {}
    virtual void SignalIRQ(bool bSignal)        {}
    virtual void SetMirrorMode(MirrorMode mode) {}
};

#endif /* IOBus_h */
