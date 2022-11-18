//
//  CPU6502.h
//  NES
//
//  Created by richardwallis on 15/11/2022.
//  Copyright © 2022 openplanetsoftware. All rights reserved.
//

#ifndef CPU6502_h
#define CPU6502_h

#include <cstdint>
#include "IOBus.h"

class CPU6502
{
public:
    CPU6502(IOBus& bus);
    ~CPU6502();
    
    void PowerOn();
    void Reset();
    void Tick();
    
private:

    void SetFlag(uint8_t flag);
    void ClearFlag(uint8_t flag);
    bool TestFlag(uint8_t flag);
    void ConditionalSetFlag(uint8_t flag, bool bCondition);
            
private:

    // IO
    IOBus& m_bus;
    
    // registers
    uint8_t m_a;
    uint8_t m_x;
    uint8_t m_y;
    uint8_t m_stack;
    uint8_t m_flags;
    uint16_t m_pc;
    
    // emulation
    uint8_t m_instructionCounter;
    uint8_t m_instructionCycles;
    uint8_t m_opData;
    uint8_t m_opCode;
    uint8_t m_operandH;
    uint8_t m_operandL;

    // Instructions
private:
    struct CPUInstruction
    {
        void(CPU6502::*m_function)(uint8_t Tn) = &CPU6502::ERROR;
        char const* m_instruction = "UNIMPLEMENTED";
        char const* m_addressMode = "";
        uint8_t m_cycles = 0;
    };
    CPUInstruction m_Instructions[256];
    
private:

    void InitInstructions();
    void ERROR(uint8_t Tn);
    
    void SEI(uint8_t Tn); void CLD(uint8_t Tn);
    void INC_zpg(uint8_t Tn);
};

#endif /* CPU6502_h */
