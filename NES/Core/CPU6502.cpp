//
//  CPU6502.cpp
//  NES
//
//  Created by richardwallis on 15/11/2022.
//  Copyright © 2022 openplanetsoftware. All rights reserved.
//

#include "CPU6502.h"
#include <stdio.h>

enum StatusFlag : uint8_t
{
    Flag_Carry      = 1 << 0,
    Flag_Zero       = 1 << 1,
    Flag_IRQDisable = 1 << 2,
    Flag_Decimal    = 1 << 3,
    Flag_Break      = 1 << 4,
    Flag_Unused     = 1 << 5,
    Flag_Overflow   = 1 << 6,
    Flag_Negative   = 1 << 7, Negative_Test = Flag_Negative
};

// useful to get 16bit address from two 8 bit values safely
// could pass uint8_t => uint16_t but I think this is safer for that case too
uint16_t uint16FromRegisterPair(uint8_t high, uint8_t low)
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
, m_instructionCounter(0)
, m_instructionCycles(0)
, m_opData(0)
, m_opCode(0)
, m_operandH(0)
, m_operandL(0)
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

void CPU6502::Reset()
{
    m_a = 0;
    m_x = 0;
    m_y = 0;
    m_stack = 0;
    m_flags = 0;
    m_pc = 0;
    
    m_instructionCounter = 0;
    m_instructionCycles = 0;
    m_opData = 0;
    m_opCode = 0;
    m_operandH = 0;
    m_operandL = 0;
}
    
void CPU6502::Tick()
{
    if(m_instructionCounter == 0)
    {
        m_opData = 0;
        m_operandH = 0;
        m_operandL = 0;
        m_opCode = m_bus.cpuRead(m_pc++);
        m_instructionCounter = m_instructionCycles = m_Instructions[m_opCode].m_cycles;
    }

    uint8_t Tn = m_instructionCycles - m_instructionCounter;
    (this->*(m_Instructions[m_opCode].m_function))(Tn);
    
    --m_instructionCounter;
}

void CPU6502::InitInstructions()
{
    m_Instructions[0xE6].m_function = &CPU6502::INC_zpg;
    m_Instructions[0xE6].m_instruction = "INC";
    m_Instructions[0xE6].m_addressMode = "zpg";
    m_Instructions[0xE6].m_cycles = 5;
}

void CPU6502::ERROR(uint8_t Tn)
{
    CPUInstruction& instruction = m_Instructions[m_opCode];
    printf("Halted on instruction opCode=0x%02X %s %s\n", m_opCode, instruction.m_instruction, instruction.m_addressMode);
    *(volatile char*)(0) = 5;
}

// Inc memory location in zero page
void CPU6502::INC_zpg(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_operandH = 0; // zero page
        m_operandL = m_bus.cpuRead(m_pc++);
    }
    else if(Tn == 2)
    {
        uint16_t address = uint16FromRegisterPair(m_operandH, m_operandL);
        m_opData = m_bus.cpuRead(address);
    }
    else if(Tn == 3)
    {
        ++m_opData;
        
        SetFlag(m_opData & Negative_Test);
        ConditionalSetFlag(Flag_Zero, m_opData == 0);
    }
    else if(Tn == 4)
    {
        uint16_t address = uint16FromRegisterPair(m_operandH, m_operandL);
        m_bus.cpuWrite(address, m_opData);
    }
}
