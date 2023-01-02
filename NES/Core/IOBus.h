//
//  IOBus.h
//  NES
//
//  Created by richardwallis on 15/11/2022.
//

#ifndef IOBus_h
#define IOBus_h

#include <cstdint>

// Common bus functions
class IOBus
{
public:
    virtual uint8_t cpuRead(uint16_t address) = 0;
    virtual void cpuWrite(uint16_t address, uint8_t byte) = 0;
    virtual uint8_t ppuRead(uint16_t address) = 0;
    virtual void ppuWrite(uint16_t address, uint8_t byte) = 0;
};

#define BUS_HEADER_IMPL     virtual uint8_t cpuRead(uint16_t address) override; \
                            virtual void cpuWrite(uint16_t address, uint8_t byte) override; \
                            virtual uint8_t ppuRead(uint16_t address) override; \
                            virtual void ppuWrite(uint16_t address, uint8_t byte) override; \
                            
enum MirrorMode : uint8_t
{
    VRAM_MIRROR_H = 0,
    VRAM_MIRROR_V,
    VRAM_MIRROR_CART4,
    
    VRAM_MIRROR_CART_SOLDER,
};

// Core System has extra features
class SystemIOBus : public IOBus
{
public:
    virtual void SignalReset(bool bSignal)      {}
    virtual void SignalNMI(bool bSignal)        {}
    virtual void SignalIRQ(bool bSignal)        {}
    virtual void SetMirrorMode(MirrorMode mode) {}
    virtual uint64_t CycleCount()               {return 0;}
};

#endif /* IOBus_h */
