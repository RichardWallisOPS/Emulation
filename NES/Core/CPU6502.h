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
    
    // Debug
    void SetPC(uint16_t pc);
    
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
    uint64_t m_tickCount;
    uint8_t m_instructionCycle;
    uint8_t m_opCode;
    uint8_t m_dataBus;
    uint8_t m_addressBusH;
    uint8_t m_addressBusL;

    // Instructions
private:
    struct CPUInstruction
    {
        bool(CPU6502::*m_opOrAddrMode)(uint8_t Tn)  = &CPU6502::ERROR;
        void(CPU6502::*m_operation)(uint8_t Tn)     = nullptr;
        char const* m_opStr                         = "UNIMPLEMENTED";
        char const* m_opAddressModeStr              = "";
        uint8_t m_cycles                            = 2;                // TODO remove this once we are happy...
    };
    CPUInstruction m_Instructions[256];
    
private:

    void InitInstructions();
    bool ERROR(uint8_t Tn);
    
    uint8_t programCounterByteFetch();
        
    // Generics - same functionality but different registers or current memory bus
    void ASL(uint8_t& cpuReg); void LSR(uint8_t& cpuReg); void ROL(uint8_t& cpuReg); void ROR(uint8_t& cpuReg);
    
    // 1) Single byte instructions
    bool Accum_ASL(uint8_t Tn); bool Accum_LSR(uint8_t Tn); bool Accum_ROL(uint8_t Tn); bool Accum_ROR(uint8_t Tn);
    bool NOP(uint8_t Tn);
    bool SEI(uint8_t Tn); bool SEC(uint8_t Tn); //bool SED(uint8_t);
    bool CLD(uint8_t Tn); bool CLC(uint8_t Tn); bool CLI(uint8_t Tn); bool CLV(uint8_t Tn);
    bool TAX(uint8_t Tn); bool TAY(uint8_t Tn); bool TXA(uint8_t Tn); bool TYA(uint8_t Tn); bool TSX(uint8_t Tn); bool TXS(uint8_t Tn);
    bool INX(uint8_t Tn); bool INY(uint8_t Tn); bool DEX(uint8_t Tn); bool DEY(uint8_t Tn);
    
    // 4) Read Modify Write
    // Instructions
    void RMW_ASL(uint8_t Tn); void RMW_DEC(uint8_t Tn); void RMW_INC(uint8_t Tn); void RMW_LSR(uint8_t Tn); void RMW_ROL(uint8_t Tn); void RMW_ROR(uint8_t Tn);
    // Address Modes
    bool ReadModifyWrite_zpg(uint8_t Tn); bool ReadModifyWrite_abs(uint8_t Tn); bool ReadModifyWrite_zpgX(uint8_t Tn); bool ReadModifyWrite_absX(uint8_t Tn);
};

#endif /* CPU6502_h */
