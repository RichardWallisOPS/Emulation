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

// Implementation notes:

// Instruction cycle Tn states taken from 6502 data sheet
// Address bus and data bus values try to follow the rules for these states

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
    uint8_t m_opCode;
    uint8_t m_dataBus;
    uint8_t m_addressBusH;
    uint8_t m_addressBusL;

    // Instructions
private:
    struct CPUInstruction
    {
        void(CPU6502::*m_opAddressMode)(uint8_t Tn) = &CPU6502::ERROR;
        void(CPU6502::*m_operation)(uint8_t Tn)     = &CPU6502::ERROR;
        char const* m_opStr                         = "UNIMPLEMENTED";
        char const* m_opAddressModeStr              = "";
        uint8_t m_cycles                            = 0;
    };
    CPUInstruction m_Instructions[256];
    
private:

    void InitInstructions();
    void ERROR(uint8_t Tn);
    
    // Generics - same functionality but different registers
    void ASL(uint8_t& cpuReg);
    
    // 1) NSingle byte instructions
    void Accum_ASL(uint8_t Tn); void SEI(uint8_t Tn); void CLD(uint8_t Tn);
    
    // 4) Read Modify Write
    // Instructions
    void RMW_ASL(uint8_t Tn); void RMW_DEC(uint8_t Tn); void RMW_INC(uint8_t Tn); void RMW_LSR(uint8_t Tn); void RMW_ROL(uint8_t Tn); void RMW_ROR(uint8_t Tn);
    // Address Modes
    void ReadModifyWrite_zpg(uint8_t Tn); void ReadModifyWrite_abs(uint8_t Tn); void ReadModifyWrite_zpgX(uint8_t Tn); void ReadModifyWrite_absX(uint8_t Tn);
};

#endif /* CPU6502_h */
