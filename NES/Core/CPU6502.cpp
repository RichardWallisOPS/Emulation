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
    Flag_Carry      = 1 << 0,
    Flag_Zero       = 1 << 1,
    Flag_IRQDisable = 1 << 2,
    Flag_Decimal    = 1 << 3,
    Flag_Break      = 1 << 4,
    Flag_Unused     = 1 << 5,
    Flag_Overflow   = 1 << 6,
    Flag_Negative   = 1 << 7
};

// useful to get 16bit address from two 8 bit values safely
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
}

void CPU6502::SetPC(uint16_t pc)
{
    m_pc = pc;
}

#ifdef EMULATION_LOG
int LinePosition = 0;
char LineBuffer[512];
#endif

uint8_t CPU6502::programCounterByteFetch()
{
    uint8_t byte = m_bus.cpuRead(m_pc++);
    
#ifdef EMULATION_LOG
    LinePosition += sprintf(&LineBuffer[LinePosition], " %2X", byte);
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
            (this->*(m_Instructions[m_opCode].m_opOrAddrMode))(0xFF);
        }
    
#ifdef EMULATION_LOG
        if(m_tickCount > 0)
        {
            // Finalize old logging
            while(LinePosition < 16)
            {
                LinePosition += sprintf(&LineBuffer[LinePosition], " ");
            }
            CPUInstruction& instruction = m_Instructions[m_opCode];
            LinePosition += sprintf(&LineBuffer[LinePosition], "  %s %s", instruction.m_opStr ? instruction.m_opStr : "", instruction.m_opAddressModeStr ? instruction.m_opAddressModeStr : "");
            while(LinePosition < 40)
            {
                LinePosition += sprintf(&LineBuffer[LinePosition], " ");
            }
            LinePosition += sprintf(&LineBuffer[LinePosition], "A:%02X X:%02X Y:%02X P:%02X SP:%02X", m_a, m_x, m_y, m_flags, m_stack);
        
            LineBuffer[LinePosition] = 0;
            printf("%s", LineBuffer);
        }

        // begin log of new instruction
        LinePosition = 0;
        LinePosition += sprintf(&LineBuffer[LinePosition], "\n%4X ", m_pc);
#endif

        m_dataBus = 0;
        m_addressBusH = 0;
        m_addressBusL = 0;
        
        m_dataBus = m_opCode = programCounterByteFetch();
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

//
// Generics
// Operations that can be performed on multiple internal CPU registers
// Caller assumer responsibility for correct cycle to call this on
//

// Athmetric Shift one bit left - high bit to carry
void CPU6502::ASL(uint8_t& cpuReg)
{
    ConditionalSetFlag(Flag_Carry, (cpuReg & (1 << 7)) != 0);
    cpuReg <<= 1;
    ConditionalSetFlag(Flag_Negative, (cpuReg & (1 << 7)) != 0);
    ConditionalSetFlag(Flag_Zero, cpuReg == 0);
}

// Logical shift right - right bit to carry
void CPU6502::LSR(uint8_t& cpuReg)
{
    ClearFlag(Flag_Negative);
    ConditionalSetFlag(Flag_Carry, (cpuReg & (1 << 0)) != 0);
    cpuReg >>= 1;
    ConditionalSetFlag(Flag_Zero, cpuReg == 0);
}

// Rotate one bit left
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

// Rotate one bit right
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

//
// single byte 2 cycle instructions
//

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


//
// ReadModifyWrite operations
//

void CPU6502::RMW_ASL(uint8_t Tn)
{
    ASL(m_dataBus);
}

// Data bus decrement - no carry
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

// memory location in zero page
bool CPU6502::ReadModifyWrite_zpg(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_dataBus = programCounterByteFetch();
    }
    else if(Tn == 2)
    {
        m_addressBusH = 0;
        m_addressBusL = m_dataBus;
        
        uint16_t address = uint16FromRegisterPair(m_addressBusH, m_addressBusL);
        m_dataBus = m_bus.cpuRead(address);
    }
    else if(Tn == 3)
    {
        // bus read to write
    }
    else if(Tn == 4)
    {
        (this->*(m_Instructions[m_opCode].m_operation))(Tn);
        
        uint16_t address = uint16FromRegisterPair(m_addressBusH, m_addressBusL);
        m_bus.cpuWrite(address, m_dataBus);
        return true;
    }
    return false;
}

// memory location at absolute address
bool CPU6502::ReadModifyWrite_abs(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_dataBus = programCounterByteFetch();
    }
    else if(Tn == 2)
    {
        m_addressBusL = m_dataBus;
        m_dataBus = programCounterByteFetch();
    }
    else if(Tn == 3)
    {
        m_addressBusH = m_dataBus;
        uint16_t address = uint16FromRegisterPair(m_addressBusH, m_addressBusL);
        m_dataBus = m_bus.cpuRead(address);
    }
    else if(Tn == 4)
    {
        // bus read to write
    }
    else if(Tn == 5)
    {
        (this->*(m_Instructions[m_opCode].m_operation))(Tn);
        
        uint16_t address = uint16FromRegisterPair(m_addressBusH, m_addressBusL);
        m_bus.cpuWrite(address, m_dataBus);
        return true;
    }
    return false;
}

// memory location at zero page plus x-index offset
bool CPU6502::ReadModifyWrite_zpgX(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_dataBus = programCounterByteFetch();
    }
    else if(Tn == 2)
    {
        m_addressBusH = 0;
        m_addressBusL = m_dataBus;
    }
    else if(Tn == 3)
    {
        m_addressBusL += m_x;
        uint16_t address = uint16FromRegisterPair(m_addressBusH, m_addressBusL);
        m_dataBus = m_bus.cpuRead(address);
    }
    else if(Tn == 4)
    {
       // bus read to write
    }
    else if(Tn == 5)
    {
        (this->*(m_Instructions[m_opCode].m_operation))(Tn);
        
        uint16_t address = uint16FromRegisterPair(m_addressBusH, m_addressBusL);
        m_bus.cpuWrite(address, m_dataBus);
        return true;
    }
    return false;
}

bool CPU6502::ReadModifyWrite_absX(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_dataBus = programCounterByteFetch();
    }
    else if(Tn == 2)
    {
        m_addressBusH = 0;
        m_addressBusL = m_dataBus;
        m_dataBus = programCounterByteFetch();
    }
    else if(Tn == 3)
    {
        if(((uint16_t)m_addressBusL + (uint16_t)m_x) > 255)
        {
            m_addressBusH = 1;
        }
        m_addressBusL += m_x;
        m_addressBusH += m_dataBus;
    }
    else if(Tn == 4)
    {
        uint16_t address = uint16FromRegisterPair(m_addressBusH, m_addressBusL);
        m_dataBus = m_bus.cpuRead(address);
    }
    else if(Tn == 5)
    {
        // bus read to write
    }
    else if(Tn == 6)
    {
        (this->*(m_Instructions[m_opCode].m_operation))(Tn);
        
        uint16_t address = uint16FromRegisterPair(m_addressBusH, m_addressBusL);
        m_bus.cpuWrite(address, m_dataBus);
        return true;
    }
    return false;
}
