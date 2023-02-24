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
    uint8_t m_Tn;
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
    
private:
    
    struct CPUInstruction
    {
        bool(CPU6502::*m_opOrAddrMode)()  = &CPU6502::HandleError;
        void(CPU6502::*m_operation)()     = nullptr;
    };
    CPUInstruction m_Instructions[256];
    
private:

    bool HandleError();
    void InitInstructions();
    
    // Address modes + their instructions
    void ASL(uint8_t& cpuReg); void LSR(uint8_t& cpuReg); void ROL(uint8_t& cpuReg); void ROR(uint8_t& cpuReg); void REG_CMP(uint8_t& cpuReg); void REG_LOAD(uint8_t& cpuReg);
    
    bool Accum_ASL(); bool Accum_LSR(); bool Accum_ROL(); bool Accum_ROR();
    bool NOP(); bool SEI(); bool SEC(); bool SED();
    bool CLD(); bool CLC(); bool CLI(); bool CLV();
    bool TAX(); bool TAY(); bool TXA(); bool TYA(); bool TSX(); bool TXS();
    bool INX(); bool INY(); bool DEX(); bool DEY();
    
    void ADC(); void AND(); void BIT(); void CMP(); void CPX(); void CPY();
    void EOR(); void LDA(); void LDX(); void LDY(); void ORA(); void SBC();
    bool InternalExecutionMemory_absREG(uint8_t& cpuReg);
    bool InternalExecutionMemory_zpgREG(uint8_t& cpuReg);
    bool InternalExecutionMemory_imm(); bool InternalExecutionMemory_zpg(); bool InternalExecutionMemory_abs();
    bool InternalExecutionMemory_indX(); bool InternalExecutionMemory_indY(); bool InternalExecutionMemory_absX();
    bool InternalExecutionMemory_absY(); bool InternalExecutionMemory_zpgX(); bool InternalExecutionMemory_zpgY();
    
    void STA(); void STX(); void STY();
    bool Store_absREG(uint8_t& cpuReg); bool Store_zpgREG(uint8_t& cpuReg);
    bool Store_zpg(); bool Store_abs(); bool Store_absX(); bool Store_absY();
    bool Store_zpgX(); bool Store_zpgY(); bool Store_indX(); bool Store_indY();
    
    void RMW_ASL(); void RMW_DEC(); void RMW_INC(); void RMW_LSR(); void RMW_ROL(); void RMW_ROR();
    bool ReadModifyWrite_zpg(); bool ReadModifyWrite_abs(); bool ReadModifyWrite_zpgX(); bool ReadModifyWrite_absX();
    
    void PHP(); void PHA(); void PLP(); void PLA();
    bool StackPush(); bool StackPull();
    
    void GenericPushStack(uint8_t data);
    uint8_t GenericPullStack();
    bool JSR(); bool RTS(); bool BRK(); bool RTI(); bool JMP_abs(); bool JMP_ind();
    
    void BCC(); void BCS(); void BEQ(); void BMI(); void BNE(); void BPL(); void BVC(); void BVS();
    bool Branch();
    
    // Extra address mode NOPs FunctionName = Instruction_AddressMode_ByteCount_CycleCount[_(+1)]
    bool NOP_IMPLIED_1_2(); bool NOP_IMMEDIATE_2_2(); bool NOP_ZEROPAGE_2_3(); bool NOP_ZEROPAGE_X_2_4(); bool NOP_ABSOLUTE_3_4(); bool NOP_ABSOLUTE_X_3_4_1();
};

#endif /* CPU6502_h */
