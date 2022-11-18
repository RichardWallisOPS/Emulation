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

enum StatusFlag
{
    Flag_Carry       = 1 << 0,
    Flag_Zero        = 1 << 1,
    Flag_IRQDisable  = 1 << 2,
    Flag_Decimal     = 1 << 3,
    Flag_Break       = 1 << 4,
    Flag_Unused      = 1 << 5,
    Flag_Overflow    = 1 << 6,
    Flag_Negative    = 1 << 7
};

class CPU6502
{
public:
    CPU6502(IOBus& bus);
    ~CPU6502();
    
    void SetFlag(StatusFlag flag);
    void ClearFlag(StatusFlag flag);
    bool TestFlag(StatusFlag flag);
    
    void Reset();
    void Tick();
            
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
    friend struct CPUInstruction;
    struct CPUInstruction
    {
        void(CPU6502::*m_function)() = &CPU6502::ERROR;
        char const* m_instruction = "UNIMPLEMENTED";
        char const* m_addressMode = "";
        uint8_t m_cycles = 0;
    };
    CPUInstruction m_Instructions[256];
    
private:
    void InitInstructions();
    void ERROR();
    void INC_zpg();
};

#endif /* CPU6502_h */
