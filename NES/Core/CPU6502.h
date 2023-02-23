//
//  CPU6502.h
//  NES
//
//  Created by richardwallis on 15/11/2022.
//

#ifndef CPU6502_h
#define CPU6502_h

#include "IOBus.h"
#include "Serialise.h"

class CPU6502 : public Serialisable
{
public:
    SERIALISABLE_DECL;

    CPU6502(IOBus& bus);
    ~CPU6502();
    
    void PowerOn();
    void Reset();
    void Tick();
    
    void SignalReset(bool bSignal);
    void SignalNMI(bool bSignal);
    void SignalIRQ(bool bSignal);
    
private:

    void SetFlag(uint8_t flag);
    void ClearFlag(uint8_t flag);
    bool TestFlag(uint8_t flag);
    void ConditionalSetFlag(uint8_t flag, bool bCondition);
    
    uint8_t programCounterReadByte();
    uint8_t addressBusReadByte();
    void addressBusWriteByte(uint8_t data);
            
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
    
    // Aditional registers for indirect access
    uint8_t m_baseAddressH;
    uint8_t m_baseAddressL;
    uint8_t m_indirectAddressH;
    uint8_t m_indirectAddressL;
    uint8_t m_effectiveAddressH;
    uint8_t m_effectiveAddressL;
    
    // Set signal lines
    bool m_bSignalReset;
    bool m_bSignalIRQ;
    bool m_bSignalNMI;
    
    bool m_bBranch;
    

    // Instructions
private:
    struct CPUInstruction
    {
        bool(CPU6502::*m_opOrAddrMode)(uint8_t Tn)  = &CPU6502::ERROR;
        void(CPU6502::*m_operation)(uint8_t Tn)     = nullptr;
        char const* m_opStr                         = "NOT-IMPLEMENTED";
        char const* m_opAddressModeStr              = "";
    };
    CPUInstruction m_Instructions[256];
    
private:

    const uint8_t kTnPreNextOpCodeFetch = 0xFF;
    const uint8_t kTnOpCodeMax          = 0xFC;

    void InitInstructions();
    bool ERROR(uint8_t Tn);
    
    //
    // Address modes + their instructions
    //
    void ASL(uint8_t& cpuReg); void LSR(uint8_t& cpuReg); void ROL(uint8_t& cpuReg); void ROR(uint8_t& cpuReg); void REG_CMP(uint8_t& cpuReg); void REG_LOAD(uint8_t& cpuReg);
    
    bool Accum_ASL(uint8_t Tn); bool Accum_LSR(uint8_t Tn); bool Accum_ROL(uint8_t Tn); bool Accum_ROR(uint8_t Tn);
    bool NOP(uint8_t Tn); bool SEI(uint8_t Tn); bool SEC(uint8_t Tn); bool SED(uint8_t);
    bool CLD(uint8_t Tn); bool CLC(uint8_t Tn); bool CLI(uint8_t Tn); bool CLV(uint8_t Tn);
    bool TAX(uint8_t Tn); bool TAY(uint8_t Tn); bool TXA(uint8_t Tn); bool TYA(uint8_t Tn); bool TSX(uint8_t Tn); bool TXS(uint8_t Tn);
    bool INX(uint8_t Tn); bool INY(uint8_t Tn); bool DEX(uint8_t Tn); bool DEY(uint8_t Tn);
    
    void ADC(uint8_t Tn); void AND(uint8_t Tn); void BIT(uint8_t Tn); void CMP(uint8_t Tn); void CPX(uint8_t Tn); void CPY(uint8_t Tn);
    void EOR(uint8_t Tn); void LDA(uint8_t Tn); void LDX(uint8_t Tn); void LDY(uint8_t Tn); void ORA(uint8_t Tn); void SBC(uint8_t Tn);
    bool InternalExecutionMemory_absREG(uint8_t Tn, uint8_t& cpuReg);
    bool InternalExecutionMemory_zpgREG(uint8_t Tn, uint8_t& cpuReg);
    bool InternalExecutionMemory_imm(uint8_t Tn); bool InternalExecutionMemory_zpg(uint8_t Tn); bool InternalExecutionMemory_abs(uint8_t Tn);
    bool InternalExecutionMemory_indX(uint8_t Tn); bool InternalExecutionMemory_indY(uint8_t Tn); bool InternalExecutionMemory_absX(uint8_t Tn);
    bool InternalExecutionMemory_absY(uint8_t Tn); bool InternalExecutionMemory_zpgX(uint8_t Tn); bool InternalExecutionMemory_zpgY(uint8_t Tn);
    
    void STA(uint8_t Tn); void STX(uint8_t Tn); void STY(uint8_t Tn);
    bool Store_absREG(uint8_t Tn, uint8_t& cpuReg); bool Store_zpgREG(uint8_t Tn, uint8_t& cpuReg);
    bool Store_zpg(uint8_t Tn); bool Store_abs(uint8_t Tn); bool Store_absX(uint8_t Tn); bool Store_absY(uint8_t Tn);
    bool Store_zpgX(uint8_t Tn); bool Store_zpgY(uint8_t Tn); bool Store_indX(uint8_t Tn); bool Store_indY(uint8_t Tn);
    
    void RMW_ASL(uint8_t Tn); void RMW_DEC(uint8_t Tn); void RMW_INC(uint8_t Tn); void RMW_LSR(uint8_t Tn); void RMW_ROL(uint8_t Tn); void RMW_ROR(uint8_t Tn);
    bool ReadModifyWrite_zpg(uint8_t Tn); bool ReadModifyWrite_abs(uint8_t Tn); bool ReadModifyWrite_zpgX(uint8_t Tn); bool ReadModifyWrite_absX(uint8_t Tn);
    
    void PHP(uint8_t Tn); void PHA(uint8_t Tn); void PLP(uint8_t Tn); void PLA(uint8_t Tn);
    bool StackPush(uint8_t Tn); bool StackPull(uint8_t Tn);
    
    void GenericPushStack(uint8_t data);
    uint8_t GenericPullStack();
    bool JSR(uint8_t Tn); bool RTS(uint8_t Tn); bool BRK(uint8_t Tn); bool RTI(uint8_t Tn); bool JMP_abs(uint8_t Tn); bool JMP_ind(uint8_t Tn);
    
    void BCC(uint8_t Tn); void BCS(uint8_t Tn); void BEQ(uint8_t Tn); void BMI(uint8_t Tn); void BNE(uint8_t Tn); void BPL(uint8_t Tn); void BVC(uint8_t Tn); void BVS(uint8_t Tn);
    bool Branch(uint8_t Tn);
    
    // Extra address mode NOPs FunctionName = Instruction_AddressMode_ByteCount_CycleCount[_(+1)]
    bool NOP_IMPLIED_1_2(uint8_t Tn); bool NOP_IMMEDIATE_2_2(uint8_t Tn); bool NOP_ZEROPAGE_2_3(uint8_t Tn); bool NOP_ZEROPAGE_X_2_4(uint8_t Tn); bool NOP_ABSOLUTE_3_4(uint8_t Tn); bool NOP_ABSOLUTE_X_3_4_1(uint8_t Tn);
    
    // Illegal opcodes as required
};

#endif /* CPU6502_h */
