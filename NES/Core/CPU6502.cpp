//
//  CPU6502.cpp
//  NES
//
//  Created by richardwallis on 15/11/2022.
//  Copyright © 2022 openplanetsoftware. All rights reserved.
//

#include "CPU6502.h"
#include <stdio.h>
#include <string.h>

#if DEBUG && 1
    #define EMULATION_LOG
#endif

enum StatusFlag : uint8_t
{
    Flag_Carry      = 1 << 0,       // unsigned overflow / underflow
    Flag_Zero       = 1 << 1,
    Flag_IRQDisable = 1 << 2,
    Flag_Decimal    = 1 << 3,
    Flag_Break      = 1 << 4,
    Flag_Unused     = 1 << 5,
    Flag_Overflow   = 1 << 6,       // signed overflow / underflow
    Flag_Negative   = 1 << 7
};

// Useful to get 16bit address from two 8 bit values safely
// could pass uint8_t => uint16_t but I think this is safer for that case too
inline uint16_t uint16FromRegisterPair(uint8_t high, uint8_t low)
{
    uint16_t result = high;
    result <<= 8;
    result += low;
    return result;
}

CPU6502::CPU6502(IOBus& bus)
: m_bus(bus)
, m_a(0)
, m_x(0)
, m_y(0)
, m_stack(0)
, m_flags(0)
, m_pc(0)
, m_tickCount(0)
, m_instructionCycle(0)
, m_dataBus(0)
, m_opCode(0)
, m_addressBusH(0)
, m_addressBusL(0)
, m_baseAddressH(0)
, m_baseAddressL(0)
, m_indirectAddressH(0)
, m_indirectAddressL(0)
, m_effectiveAddressH(0)
, m_effectiveAddressL(0)
{
    InitInstructions();
}

CPU6502::~CPU6502()
{}

void CPU6502::SetFlag(uint8_t flag)
{
    m_flags |= flag;
}

void CPU6502::ClearFlag(uint8_t flag)
{
    m_flags &= ~flag;
}

bool CPU6502::TestFlag(uint8_t flag)
{
    return (m_flags & flag) != 0;
}

void CPU6502::ConditionalSetFlag(uint8_t flag, bool bCondition)
{
    if(bCondition)
    {
        SetFlag(flag);
    }
    else
    {
        ClearFlag(flag);
    }
}

void CPU6502::PowerOn()
{
    m_a = 0;
    m_x = 0;
    m_y = 0;
    m_stack = 0xFD;
    m_flags = 0x34;
    
    uint8_t resetAddressL = m_bus.cpuRead(0xFFFC);
    uint8_t resetAddressH = m_bus.cpuRead(0xFFFD);
    m_pc = uint16FromRegisterPair(resetAddressH, resetAddressL);
    
    m_tickCount = 0;
    m_instructionCycle = 0;
    m_dataBus = 0;
    m_opCode = 0;
    m_addressBusH = 0;
    m_addressBusL = 0;
    m_baseAddressH = 0;
    m_baseAddressL = 0;
    m_indirectAddressH = 0;
    m_indirectAddressL = 0;
    m_effectiveAddressH = 0;
    m_effectiveAddressL = 0;
}

void CPU6502::Reset()
{
    m_stack -= 3;
    SetFlag(Flag_IRQDisable);
    
    uint8_t resetAddressL = m_bus.cpuRead(0xFFFC);
    uint8_t resetAddressH = m_bus.cpuRead(0xFFFD);
    m_pc = uint16FromRegisterPair(resetAddressH, resetAddressL);
    
    m_tickCount = 0;
    m_instructionCycle = 0;
    m_dataBus = 0;
    m_opCode = 0;
    m_addressBusH = 0;
    m_addressBusL = 0;
    m_baseAddressH = 0;
    m_baseAddressL = 0;
    m_indirectAddressH = 0;
    m_indirectAddressL = 0;
    m_effectiveAddressH = 0;
    m_effectiveAddressL = 0;
}

void CPU6502::SetPC(uint16_t pc)
{
    m_pc = pc;
}

uint8_t CPU6502::addressBusReadByte()
{
    uint16_t address = uint16FromRegisterPair(m_addressBusH, m_addressBusL);
    return m_bus.cpuRead(address);
}

void CPU6502::addressBusWriteByte(uint8_t data)
{
    uint16_t address = uint16FromRegisterPair(m_addressBusH, m_addressBusL);
    m_bus.cpuWrite(address, data);
}

#ifdef EMULATION_LOG
int LinePosition = 0;
const int LineBufferSize = 512;
char LineBuffer[LineBufferSize];
#endif

uint8_t CPU6502::programCounterReadByte()
{
    uint8_t byte = m_bus.cpuRead(m_pc++);
    
#ifdef EMULATION_LOG
    LinePosition += snprintf(&LineBuffer[LinePosition], LineBufferSize - LinePosition, " %02X", byte);
#endif
    
    return byte;
}
    
void CPU6502::Tick()
{
    bool bInstructionTStatesCompleted = false;
    
    if(m_instructionCycle == 0)
    {
        // Some instructions perform final executation during next op code fetch
        // Allow executation to occur during this tick but use max Tn value so they know
        if(m_tickCount > 0)
        {
            (this->*(m_Instructions[m_opCode].m_opOrAddrMode))(nTnPreNextOpCodeFetch);
        }
    
#ifdef EMULATION_LOG
        if(m_tickCount > 0)
        {
            // Finalize old logging
            while(LinePosition < 16)
            {
                LinePosition += snprintf(&LineBuffer[LinePosition], LineBufferSize - LinePosition, " ");
            }
            CPUInstruction& instruction = m_Instructions[m_opCode];
            LinePosition += snprintf(&LineBuffer[LinePosition], LineBufferSize - LinePosition, "  %s %s", instruction.m_opStr ? instruction.m_opStr : "", instruction.m_opAddressModeStr ? instruction.m_opAddressModeStr : "");
            while(LinePosition < 40)
            {
                LinePosition += snprintf(&LineBuffer[LinePosition], LineBufferSize - LinePosition, " ");
            }
            LinePosition += snprintf(&LineBuffer[LinePosition], LineBufferSize - LinePosition, "A:%02X X:%02X Y:%02X P:%02X SP:%02X", m_a, m_x, m_y, m_flags, m_stack);
        
            LineBuffer[LinePosition] = 0;
            printf("%s", LineBuffer);
        }

        // begin log of new instruction
        LinePosition = 0;
        LinePosition += snprintf(&LineBuffer[LinePosition], LineBufferSize - LinePosition, "\n%04X ", m_pc);
#endif

        m_dataBus = 0;
        m_addressBusH = 0;
        m_addressBusL = 0;
        m_baseAddressH = 0;
        m_baseAddressL = 0;
        m_indirectAddressH = 0;
        m_indirectAddressL = 0;
        m_effectiveAddressH = 0;
        m_effectiveAddressL = 0;
        
        m_dataBus = m_opCode = programCounterReadByte();
    }
    else
    {
        // Tn == 0 (opCode fetch) should have no processing for any function
        bInstructionTStatesCompleted = (this->*(m_Instructions[m_opCode].m_opOrAddrMode))(m_instructionCycle);
    }
    
    if(bInstructionTStatesCompleted)
    {
#if DEBUG
        // check we have executed the correct number of cycles for an instruction
        if(m_instructionCycle + 1 != m_Instructions[m_opCode].m_cycles)
        {
            printf("Executation cycle vs instruction cycle mismatch");
            ERROR(m_instructionCycle);
        }
#endif
        m_instructionCycle = 0;
    }
    else
    {
        ++m_instructionCycle;
    }
    ++m_tickCount;
}

bool CPU6502::ERROR(uint8_t Tn)
{
#if DEBUG
    #ifdef EMULATION_LOG
        LineBuffer[LinePosition] = 0;
        printf("%s", LineBuffer);
    #endif
    
    CPUInstruction& instruction = m_Instructions[m_opCode];
    printf("  Halted on instruction Tn=%d opCode=0x%02X %s %s\n", Tn, m_opCode, instruction.m_opStr, instruction.m_opAddressModeStr);
    *(volatile char*)(0) = 65 | 02;
#endif
    
    return NOP(Tn);
}

void CPU6502::ASL(uint8_t& cpuReg)
{
    ConditionalSetFlag(Flag_Carry, (cpuReg & (1 << 7)) != 0);
    cpuReg <<= 1;
    ConditionalSetFlag(Flag_Negative, (cpuReg & (1 << 7)) != 0);
    ConditionalSetFlag(Flag_Zero, cpuReg == 0);
}

void CPU6502::LSR(uint8_t& cpuReg)
{
    ClearFlag(Flag_Negative);
    ConditionalSetFlag(Flag_Carry, (cpuReg & (1 << 0)) != 0);
    cpuReg >>= 1;
    ConditionalSetFlag(Flag_Zero, cpuReg == 0);
}

void CPU6502::ROL(uint8_t& cpuReg)
{
    bool carryWasSet = TestFlag(Flag_Carry);
    ConditionalSetFlag(Flag_Carry, (cpuReg & (1 << 7)) != 0);
    cpuReg <<= 1;
    if(carryWasSet)
    {
        cpuReg |= 1 << 0;
    }
    ConditionalSetFlag(Flag_Negative, (cpuReg & (1 << 7)) != 0);
    ConditionalSetFlag(Flag_Zero, cpuReg == 0);
}

void CPU6502::ROR(uint8_t& cpuReg)
{
    // carry into bit 7
    // bit one into carry
    bool bit0WasSet = (cpuReg & (1 << 0)) != 0;
    bool carryWasSet = TestFlag(Flag_Carry);
    cpuReg >>= 1;
    if(carryWasSet)
    {
        cpuReg |= 1 << 7;
    }
    ConditionalSetFlag(Flag_Carry, bit0WasSet);
    ConditionalSetFlag(Flag_Negative, (cpuReg & (1 << 7)) != 0);
    ConditionalSetFlag(Flag_Zero, cpuReg == 0);
}

bool CPU6502::NOP(uint8_t Tn)
{
    return Tn == 1;
}

bool CPU6502::Accum_LSR(uint8_t Tn)
{
    if(Tn == 1)
    {
        LSR(m_a);
    }
    return Tn == 1;
}

bool CPU6502::Accum_ASL(uint8_t Tn)
{
    if(Tn == 1)
    {
        ASL(m_a);
    }
    return Tn == 1;
}

bool CPU6502::Accum_ROL(uint8_t Tn)
{
    if(Tn == 1)
    {
        ROL(m_a);
    }
    return Tn == 1;
}

bool CPU6502::Accum_ROR(uint8_t Tn)
{
    if(Tn == 1)
    {
        ROR(m_a);
    }
    return Tn == 1;
}

bool CPU6502::SEI(uint8_t Tn)
{
    if(Tn == 1)
    {
        SetFlag(Flag_IRQDisable);
    }
    return Tn == 1;
}

bool CPU6502::SEC(uint8_t Tn)
{
    if(Tn == 1)
    {
        SetFlag(Flag_Carry);
    }
    return Tn == 1;
}

bool CPU6502::SED(uint8_t Tn)
{
    // Not implemented in NES CPU
    return ERROR(Tn);
}

bool CPU6502::CLD(uint8_t Tn)
{
    if(Tn == 1)
    {
        ClearFlag(Flag_Decimal);
    }
    return Tn == 1;
}

bool CPU6502::CLC(uint8_t Tn)
{
    if(Tn == 1)
    {
        ClearFlag(Flag_Carry);
    }
    return Tn == 1;
}

bool CPU6502::CLI(uint8_t Tn)
{
    if(Tn == 1)
    {
        ClearFlag(Flag_IRQDisable);
    }
    return Tn == 1;
}

bool CPU6502::CLV(uint8_t Tn)
{
    if(Tn == 1)
    {
        ClearFlag(Flag_Overflow);
    }
    return Tn == 1;
}

bool CPU6502::TAX(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_x = m_a;
        ConditionalSetFlag(Flag_Negative, (m_x & (1 << 7)) != 0);
        ConditionalSetFlag(Flag_Zero, m_x == 0);
    }
    return Tn == 1;
}

bool CPU6502::TAY(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_y = m_a;
        ConditionalSetFlag(Flag_Negative, (m_y & (1 << 7)) != 0);
        ConditionalSetFlag(Flag_Zero, m_y == 0);
    }
    return Tn == 1;
}

bool CPU6502::TXA(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_a = m_x;
        ConditionalSetFlag(Flag_Negative, (m_a & (1 << 7)) != 0);
        ConditionalSetFlag(Flag_Zero, m_a == 0);
    }
    return Tn == 1;
}

bool CPU6502::TYA(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_a = m_y;
        ConditionalSetFlag(Flag_Negative, (m_a & (1 << 7)) != 0);
        ConditionalSetFlag(Flag_Zero, m_a == 0);
    }
    return Tn == 1;
}

bool CPU6502::TSX(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_x = m_stack;
        ConditionalSetFlag(Flag_Negative, (m_x & (1 << 7)) != 0);
        ConditionalSetFlag(Flag_Zero, m_x == 0);
    }
    return Tn == 1;
}

bool CPU6502::TXS(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_stack = m_x;
        // no flag changes
    }
    return Tn == 1;
}

bool CPU6502::INX(uint8_t Tn)
{
    if(Tn == 1)
    {
        ++m_x;
        ConditionalSetFlag(Flag_Negative, (m_x & (1 << 7)) != 0);
        ConditionalSetFlag(Flag_Zero, m_x == 0);
    }
    return Tn == 1;
}

bool CPU6502::INY(uint8_t Tn)
{
    if(Tn == 1)
    {
        ++m_y;
        ConditionalSetFlag(Flag_Negative, (m_y & (1 << 7)) != 0);
        ConditionalSetFlag(Flag_Zero, m_y == 0);
    }
    return Tn == 1;
}

bool CPU6502::DEX(uint8_t Tn)
{
    if(Tn == 1)
    {
        --m_x;
        ConditionalSetFlag(Flag_Negative, (m_x & (1 << 7)) != 0);
        ConditionalSetFlag(Flag_Zero, m_x == 0);
    }
    return Tn == 1;
}

bool CPU6502::DEY(uint8_t Tn)
{
    if(Tn == 1)
    {
        --m_y;
        ConditionalSetFlag(Flag_Negative, (m_y & (1 << 7)) != 0);
        ConditionalSetFlag(Flag_Zero, m_y == 0);
    }
    return Tn == 1;
}

void CPU6502::RMW_ASL(uint8_t Tn)
{
    ASL(m_dataBus);
}

void CPU6502::RMW_DEC(uint8_t Tn)
{
    --m_dataBus;
    ConditionalSetFlag(Flag_Zero, m_dataBus == 0);
    ConditionalSetFlag(Flag_Negative, (m_dataBus & (1 << 7)) != 0);
}

// Data bus increment - no carry
void CPU6502::RMW_INC(uint8_t Tn)
{
    ++m_dataBus;
    ConditionalSetFlag(Flag_Zero, m_dataBus == 0);
    ConditionalSetFlag(Flag_Negative, (m_dataBus & (1 << 7)) != 0);
}

void CPU6502::RMW_LSR(uint8_t Tn)
{
    LSR(m_dataBus);
}

void CPU6502::RMW_ROL(uint8_t Tn)
{
    ROL(m_dataBus);
}

void CPU6502::RMW_ROR(uint8_t Tn)
{
    ROR(m_dataBus);
}

bool CPU6502::ReadModifyWrite_zpg(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_dataBus = programCounterReadByte();
    }
    else if(Tn == 2)
    {
        m_addressBusH = 0;
        m_addressBusL = m_dataBus;
        m_dataBus = addressBusReadByte();
    }
    else if(Tn == 3)
    {
        // bus read to write
    }
    else if(Tn == 4)
    {
        (this->*(m_Instructions[m_opCode].m_operation))(Tn);
        addressBusWriteByte(m_dataBus);
        return true;
    }
    return false;
}

bool CPU6502::ReadModifyWrite_abs(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_dataBus = programCounterReadByte();
    }
    else if(Tn == 2)
    {
        m_addressBusL = m_dataBus;
        m_dataBus = programCounterReadByte();
    }
    else if(Tn == 3)
    {
        m_addressBusH = m_dataBus;
        m_dataBus = addressBusReadByte();
    }
    else if(Tn == 4)
    {
        // bus read to write
    }
    else if(Tn == 5)
    {
        (this->*(m_Instructions[m_opCode].m_operation))(Tn);
        addressBusWriteByte(m_dataBus);
        return true;
    }
    return false;
}

bool CPU6502::ReadModifyWrite_zpgX(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_dataBus = programCounterReadByte();
        m_baseAddressL = m_dataBus;
    }
    else if(Tn == 2)
    {
        m_addressBusH = 0;
        m_addressBusL = m_baseAddressL;
    }
    else if(Tn == 3)
    {
        m_addressBusL = m_baseAddressL + m_x;
        m_dataBus = addressBusReadByte();
    }
    else if(Tn == 4)
    {
       // bus read to write
    }
    else if(Tn == 5)
    {
        (this->*(m_Instructions[m_opCode].m_operation))(Tn);
        addressBusWriteByte(m_dataBus);
        return true;
    }
    return false;
}

bool CPU6502::ReadModifyWrite_absX(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_dataBus = programCounterReadByte();
        m_baseAddressL = m_dataBus;
    }
    else if(Tn == 2)
    {
        m_dataBus = programCounterReadByte();
        m_baseAddressH = m_dataBus;
    }
    else if(Tn == 3)
    {
        m_addressBusL = m_baseAddressL + m_x;
        m_addressBusH = m_baseAddressH;
        if(uint16_t(m_baseAddressL) + uint16_t(m_x) > 255)
        {
            m_addressBusH += 1;
        }
    }
    else if(Tn == 4)
    {
        m_dataBus = addressBusReadByte();
    }
    else if(Tn == 5)
    {
        // bus read to write
    }
    else if(Tn == 6)
    {
        (this->*(m_Instructions[m_opCode].m_operation))(Tn);
        addressBusWriteByte(m_dataBus);
        return true;
    }
    return false;
}

void CPU6502::ADC(uint8_t Tn)
{
    uint8_t u8Carry = (m_flags & Flag_Carry) != 0 ? 1 : 0;
    int16_t s16Result = int16_t(uint8_t(m_a)) + int16_t(uint8_t(m_dataBus)) + int16_t(uint8_t(u8Carry));
    int16_t s8_16Result = int16_t(int8_t(m_a)) + int16_t(int8_t(m_dataBus)) + int16_t(int8_t(u8Carry));
    
    m_a = m_a + m_dataBus + u8Carry;
    
    ConditionalSetFlag(Flag_Zero, m_a == 0);
    ConditionalSetFlag(Flag_Carry, s16Result > 255);
    ConditionalSetFlag(Flag_Negative, (m_a & (1 << 7)) != 0);
    ConditionalSetFlag(Flag_Overflow, s8_16Result > 127 || s8_16Result < -128);
}

void CPU6502::SBC(uint8_t Tn)
{
    uint8_t u8Carry = (m_flags & Flag_Carry) != 0 ? 1 : 0;
    int16_t s16Result =  int16_t(m_a) - int16_t(m_dataBus) - int16_t(u8Carry);
    int16_t s8_16Result = int16_t(int8_t(m_a)) - int16_t(int8_t(m_dataBus)) - int16_t(int8_t(u8Carry));
    
    m_a = m_a - m_dataBus - u8Carry;
    
    ConditionalSetFlag(Flag_Zero, m_a == 0);
    ConditionalSetFlag(Flag_Carry, s16Result < 0);
    ConditionalSetFlag(Flag_Negative, (m_a & (1 << 7)) != 0);
    ConditionalSetFlag(Flag_Overflow, s8_16Result > 127 || s8_16Result < -128);
}

void CPU6502::AND(uint8_t Tn)
{
    m_a = m_a & m_dataBus;
    ConditionalSetFlag(Flag_Zero, m_a == 0);
    ConditionalSetFlag(Flag_Negative, (m_a & (1 << 7)) != 0);
}

void CPU6502::BIT(uint8_t Tn)
{
    uint8_t result = m_a & m_dataBus;
    ConditionalSetFlag(Flag_Zero, result == 0);
    ConditionalSetFlag(Flag_Negative, (m_dataBus & (1 << 7)) != 0);
    ConditionalSetFlag(Flag_Overflow, (m_dataBus & (1 << 6)) != 0);
}

void CPU6502::EOR(uint8_t Tn)
{
    m_a = m_a ^ m_dataBus;
    ConditionalSetFlag(Flag_Zero, m_a == 0);
    ConditionalSetFlag(Flag_Negative, (m_a & (1 << 7)) != 0);
}

void CPU6502::ORA(uint8_t Tn)
{
    m_a = m_a | m_dataBus;
    ConditionalSetFlag(Flag_Zero, m_a == 0);
    ConditionalSetFlag(Flag_Negative, (m_a & (1 << 7)) != 0);
}

void CPU6502::REG_CMP(uint8_t& cpuReg)
{
    int16_t result = int16_t(cpuReg) - int16_t(m_dataBus);
    ConditionalSetFlag(Flag_Zero, result == 0);
    ConditionalSetFlag(Flag_Negative, (result & (1 << 7)) != 0);
    ConditionalSetFlag(Flag_Carry, result = 0 || result > 0);
}

void CPU6502::CMP(uint8_t Tn)
{
    REG_CMP(m_a);
}

void CPU6502::CPX(uint8_t Tn)
{
    REG_CMP(m_x);
}

void CPU6502::CPY(uint8_t Tn)
{
    REG_CMP(m_y);
}

void CPU6502::REG_LOAD(uint8_t& cpuReg)
{
    cpuReg = m_dataBus;
    ConditionalSetFlag(Flag_Zero, cpuReg == 0);
    ConditionalSetFlag(Flag_Negative, (cpuReg & (1 << 7)) != 0);
}

void CPU6502::LDA(uint8_t Tn)
{
    REG_LOAD(m_a);
}

void CPU6502::LDX(uint8_t Tn)
{
    REG_LOAD(m_x);
}

void CPU6502::LDY(uint8_t Tn)
{
    REG_LOAD(m_y);
}

bool CPU6502::InternalExecutionMemory_imm(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_dataBus = programCounterReadByte();
        return true;
    }
    else if(Tn == nTnPreNextOpCodeFetch)
    {
        (this->*(m_Instructions[m_opCode].m_operation))(Tn);
        return true;
    }
    return false;
}

bool CPU6502::InternalExecutionMemory_zpg(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_dataBus = programCounterReadByte();
    }
    else if(Tn == 2)
    {
        m_addressBusH = 0;
        m_addressBusL = m_dataBus;
        m_dataBus = addressBusReadByte();
        return true;
    }
    else if(Tn == nTnPreNextOpCodeFetch)
    {
        (this->*(m_Instructions[m_opCode].m_operation))(Tn);
        return true;
    }
    return false;
}

bool CPU6502::InternalExecutionMemory_abs(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_dataBus = programCounterReadByte();
    }
    else if(Tn == 2)
    {
        m_addressBusL = m_dataBus;
        m_dataBus = programCounterReadByte();
    }
    else if(Tn == 3)
    {
        m_addressBusH = m_dataBus;
        m_dataBus = addressBusReadByte();
        return true;
    }
    else if(Tn == nTnPreNextOpCodeFetch)
    {
        (this->*(m_Instructions[m_opCode].m_operation))(Tn);
        return true;
    }
    return false;
}

bool CPU6502::InternalExecutionMemory_indX(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_dataBus = programCounterReadByte();
        m_baseAddressL = m_dataBus;
    }
    else if(Tn == 2)
    {
        m_addressBusH = 0;
        m_addressBusL = m_baseAddressL;
    }
    else if(Tn == 3)
    {
        m_addressBusL = m_baseAddressL + m_x;
        m_dataBus = addressBusReadByte();
        m_effectiveAddressL = m_dataBus;
    }
    else if(Tn == 4)
    {
        m_addressBusL = m_baseAddressL + m_x + 1;
        m_dataBus = addressBusReadByte();
        m_effectiveAddressH = m_dataBus;
    }
    else if(Tn == 5)
    {
        m_addressBusH = m_effectiveAddressH;
        m_addressBusL = m_effectiveAddressL;
        m_dataBus = addressBusReadByte();
        return true;
    }
    else if(Tn == nTnPreNextOpCodeFetch)
    {
        (this->*(m_Instructions[m_opCode].m_operation))(Tn);
        return true;
    }
    return false;
}

bool CPU6502::InternalExecutionMemory_absX(uint8_t)
{
    return false;
}

bool CPU6502::InternalExecutionMemory_absY(uint8_t)
{
    return false;
}

bool CPU6502::InternalExecutionMemory_zpgX(uint8_t)
{
    return false;
}

bool CPU6502::InternalExecutionMemory_zpgY(uint8_t)
{
    return false;
}

bool CPU6502::InternalExecutionMemory_indY(uint8_t)
{
    return false;
}
