//
//  CPU6502.cpp
//  NES
//
//  Created by richardwallis on 15/11/2022.
//  Copyright © 2022 openplanetsoftware. All rights reserved.
//

#include "CPU6502.h"
#include <stdio.h>

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

void CPU6502::SetFlag(StatusFlag flag)
{
    m_flags |= flag;
}

void CPU6502::ClearFlag(StatusFlag flag)
{
    m_flags &= ~flag;
}

bool CPU6502::TestFlag(StatusFlag flag)
{
    return (m_flags & flag) != 0;
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

    (this->*(m_Instructions[m_opCode].m_function))();
    --m_instructionCounter;
}

void CPU6502::InitInstructions()
{
    m_Instructions[0xE6].m_function = &CPU6502::INC_zpg;
    m_Instructions[0xE6].m_instruction = "INC";
    m_Instructions[0xE6].m_addressMode = "zpg";
    m_Instructions[0xE6].m_cycles = 5;
}

void CPU6502::ERROR()
{
    printf("Halted on instruction opCode=%d %s %s\n", m_opCode, m_Instructions[m_opCode].m_instruction, m_Instructions[m_opCode].m_addressMode);
    *(volatile char*)(0) = 5;
}

// Inc memory location in zero page
void CPU6502::INC_zpg()
{
    uint8_t Tn = m_instructionCycles - m_instructionCounter;
    
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
    }
    else if(Tn == 4)
    {
        uint16_t address = uint16FromRegisterPair(m_operandH, m_operandL);
        m_bus.cpuWrite(address, m_opData);
    }
}
