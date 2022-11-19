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
, m_instructionCounter(0)
, m_instructionCycles(0)
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
    
    // set pc to contents of reset vector
    uint8_t resetVectorLow = m_bus.cpuRead(0xFFFC);
    uint8_t resetVectorHigh = m_bus.cpuRead(0xFFFD);
    m_pc = uint16FromRegisterPair(resetVectorHigh, resetVectorLow);
    
    m_instructionCounter = 0;
    m_instructionCycles = 0;
    m_dataBus = 0;
    m_opCode = 0;
    m_addressBusH = 0;
    m_addressBusL = 0;
}

void CPU6502::Reset()
{
    m_stack -= 3;
    SetFlag(Flag_IRQDisable);
    
    // set pc to contents of reset vector
    uint8_t resetVectorLow = m_bus.cpuRead(0xFFFC);
    uint8_t resetVectorHigh = m_bus.cpuRead(0xFFFD);
    m_pc = uint16FromRegisterPair(resetVectorHigh, resetVectorLow);
    
    m_instructionCounter = 0;
    m_instructionCycles = 0;
    m_dataBus = 0;
    m_opCode = 0;
    m_addressBusH = 0;
    m_addressBusL = 0;
}
    
void CPU6502::Tick()
{
    if(m_instructionCounter == 0)
    {
        m_dataBus = 0;
        m_addressBusH = 0;
        m_addressBusL = 0;
        
        m_dataBus = m_opCode = m_bus.cpuRead(m_pc++);
        m_instructionCounter = m_instructionCycles = m_Instructions[m_opCode].m_cycles;
    }
    else
    {
        // Tn == 0 (opCode fetch) should have no processing for any function
        uint8_t Tn = m_instructionCycles - m_instructionCounter;
        (this->*(m_Instructions[m_opCode].m_opAddressMode))(Tn);
    }
    
    --m_instructionCounter;
}

void CPU6502::InitInstructions()
{
    m_Instructions[0x78].m_opAddressMode = &CPU6502::SEI;
    m_Instructions[0x78].m_operation = nullptr;
    m_Instructions[0x78].m_opStr = "SEI";
    m_Instructions[0x78].m_opAddressModeStr = "";
    m_Instructions[0x78].m_cycles = 2;
    
    m_Instructions[0xD8].m_opAddressMode = &CPU6502::CLD;
    m_Instructions[0xD8].m_operation = nullptr;
    m_Instructions[0xD8].m_opStr = "CLD";
    m_Instructions[0xD8].m_opAddressModeStr = "";
    m_Instructions[0xD8].m_cycles = 2;
    
    m_Instructions[0xE6].m_opAddressMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0xE6].m_operation = &CPU6502::INC;
    m_Instructions[0xE6].m_opStr = "INC";
    m_Instructions[0xE6].m_opAddressModeStr = "zpg";
    m_Instructions[0xE6].m_cycles = 5;
    
    m_Instructions[0xEE].m_opAddressMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0xEE].m_operation = &CPU6502::INC;
    m_Instructions[0xEE].m_opStr = "INC";
    m_Instructions[0xEE].m_opAddressModeStr = "abs";
    m_Instructions[0xEE].m_cycles = 6;
    
    m_Instructions[0xF6].m_opAddressMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0xF6].m_operation = &CPU6502::INC;
    m_Instructions[0xF6].m_opStr = "INC";
    m_Instructions[0xF6].m_opAddressModeStr = "zpg,X";
    m_Instructions[0xF6].m_cycles = 6;
    
    m_Instructions[0xFE].m_opAddressMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0xFE].m_operation = &CPU6502::INC;
    m_Instructions[0xFE].m_opStr = "INC";
    m_Instructions[0xFE].m_opAddressModeStr = "abs,X";
    m_Instructions[0xFE].m_cycles = 7;
}

void CPU6502::ERROR(uint8_t Tn)
{
    CPUInstruction& instruction = m_Instructions[m_opCode];
    printf("Halted on instruction [missing address mode] opCode=0x%02X %s %s\n", m_opCode, instruction.m_opStr, instruction.m_opAddressModeStr);
    *(volatile char*)(0) = 5;
}

//
// single byte 2 cycle instructions
//

void CPU6502::SEI(uint8_t Tn)
{
    if(Tn == 1)
    {
        SetFlag(Flag_IRQDisable);
    }
}

void CPU6502::CLD(uint8_t Tn)
{
    if(Tn == 1)
    {
        ClearFlag(Flag_Decimal);
    }
}

//
// ReadModifyWrite operations
//

void CPU6502::ASL(uint8_t Tn)
{

}

void CPU6502::DEC(uint8_t Tn)
{

}

// Data bus increment - no carry
void CPU6502::INC(uint8_t Tn)
{
    ++m_dataBus;
    SetFlag(m_dataBus & Negative_Test);
    ConditionalSetFlag(Flag_Zero, m_dataBus == 0);
}

void CPU6502::LSR(uint8_t Tn)
{

}

void CPU6502::ROL(uint8_t Tn)
{

}

void CPU6502::ROR(uint8_t Tn)
{

}

// memory location in zero page
void CPU6502::ReadModifyWrite_zpg(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_dataBus = m_bus.cpuRead(m_pc++);
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
    }
}

// memory location at absolute address
void CPU6502::ReadModifyWrite_abs(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_dataBus = m_bus.cpuRead(m_pc++);
    }
    else if(Tn == 2)
    {
        m_addressBusL = m_dataBus;
        m_dataBus = m_bus.cpuRead(m_pc++);
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
    }
}

// memory location at zero page plus x-index offset
void CPU6502::ReadModifyWrite_zpgX(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_dataBus = m_bus.cpuRead(m_pc++);
    }
    else if(Tn == 2)
    {
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
    }
}

void CPU6502::ReadModifyWrite_absX(uint8_t Tn)
{
    if(Tn == 1)
    {
        m_dataBus = m_bus.cpuRead(m_pc++);
    }
    else if(Tn == 2)
    {
        m_addressBusL = m_dataBus;
        m_dataBus = m_bus.cpuRead(m_pc++);
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
    }
}
