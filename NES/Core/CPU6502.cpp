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
    bool bInstructionComplete = false;
    
    if(m_instructionCycle == 0)
    {
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
        bInstructionComplete = (this->*(m_Instructions[m_opCode].m_opOrAddrMode))(m_instructionCycle);
    }
    
    if(bInstructionComplete)
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

void CPU6502::InitInstructions()
{
    m_Instructions[0x06].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0x06].m_operation = &CPU6502::RMW_ASL;
    m_Instructions[0x06].m_opStr = "ASL";
    m_Instructions[0x06].m_opAddressModeStr = "zpg";
    m_Instructions[0x06].m_cycles = 5;
    
    m_Instructions[0x0A].m_opOrAddrMode = &CPU6502::Accum_ASL;
    m_Instructions[0x0A].m_operation = nullptr;
    m_Instructions[0x0A].m_opStr = "ASL";
    m_Instructions[0x0A].m_opAddressModeStr = "a";
    m_Instructions[0x0A].m_cycles = 2;
    
    m_Instructions[0x0E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0x0E].m_operation = &CPU6502::RMW_ASL;
    m_Instructions[0x0E].m_opStr = "ASL";
    m_Instructions[0x0E].m_opAddressModeStr = "abs";
    m_Instructions[0x0E].m_cycles = 6;
    
    m_Instructions[0x16].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0x16].m_operation = &CPU6502::RMW_ASL;
    m_Instructions[0x16].m_opStr = "ASL";
    m_Instructions[0x16].m_opAddressModeStr = "zpg,X";
    m_Instructions[0x16].m_cycles = 6;

    m_Instructions[0x1E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0x1E].m_operation = &CPU6502::RMW_ASL;
    m_Instructions[0x1E].m_opStr = "ASL";
    m_Instructions[0x1E].m_opAddressModeStr = "abs,X";
    m_Instructions[0x1E].m_cycles = 7;
    
    m_Instructions[0x26].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0x26].m_operation = &CPU6502::RMW_ROL;
    m_Instructions[0x26].m_opStr = "ROL";
    m_Instructions[0x26].m_opAddressModeStr = "zpg";
    m_Instructions[0x26].m_cycles = 5;

    m_Instructions[0x2A].m_opOrAddrMode = &CPU6502::Accum_ROL;
    m_Instructions[0x2A].m_operation = nullptr;
    m_Instructions[0x2A].m_opStr = "ROL";
    m_Instructions[0x2A].m_opAddressModeStr = "a";
    m_Instructions[0x2A].m_cycles = 2;
    
    m_Instructions[0x2E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0x2E].m_operation = &CPU6502::RMW_ROL;
    m_Instructions[0x2E].m_opStr = "ROL";
    m_Instructions[0x2E].m_opAddressModeStr = "abs";
    m_Instructions[0x2E].m_cycles = 6;
    
    m_Instructions[0x36].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0x36].m_operation = &CPU6502::RMW_ROL;
    m_Instructions[0x36].m_opStr = "ROL";
    m_Instructions[0x36].m_opAddressModeStr = "zpg,X";
    m_Instructions[0x36].m_cycles = 6;
    
    m_Instructions[0x3E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0x3E].m_operation = &CPU6502::RMW_ROL;
    m_Instructions[0x3E].m_opStr = "ROL";
    m_Instructions[0x3E].m_opAddressModeStr = "abs,X";
    m_Instructions[0x3E].m_cycles = 7;
    
    m_Instructions[0x46].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0x46].m_operation = &CPU6502::RMW_LSR;
    m_Instructions[0x46].m_opStr = "LSR";
    m_Instructions[0x46].m_opAddressModeStr = "zpg";
    m_Instructions[0x46].m_cycles = 5;
    
    m_Instructions[0x4A].m_opOrAddrMode = &CPU6502::Accum_LSR;
    m_Instructions[0x4A].m_operation = nullptr;
    m_Instructions[0x4A].m_opStr = "LSR";
    m_Instructions[0x4A].m_opAddressModeStr = "zpg";
    m_Instructions[0x4A].m_cycles = 2;
    
    m_Instructions[0x4E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0x4E].m_operation = &CPU6502::RMW_LSR;
    m_Instructions[0x4E].m_opStr = "LSR";
    m_Instructions[0x4E].m_opAddressModeStr = "abs";
    m_Instructions[0x4E].m_cycles = 6;
    
    m_Instructions[0x56].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0x56].m_operation = &CPU6502::RMW_LSR;
    m_Instructions[0x56].m_opStr = "LSR";
    m_Instructions[0x56].m_opAddressModeStr = "zpg,X";
    m_Instructions[0x56].m_cycles = 6;
    
    m_Instructions[0x5E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0x5E].m_operation = &CPU6502::RMW_LSR;
    m_Instructions[0x5E].m_opStr = "LSR";
    m_Instructions[0x5E].m_opAddressModeStr = "abs,X";
    m_Instructions[0x5E].m_cycles = 7;
    
    m_Instructions[0x78].m_opOrAddrMode = &CPU6502::SEI;
    m_Instructions[0x78].m_operation = nullptr;
    m_Instructions[0x78].m_opStr = "SEI";
    m_Instructions[0x78].m_opAddressModeStr = "";
    m_Instructions[0x78].m_cycles = 2;
    
    m_Instructions[0xC6].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0xC6].m_operation = &CPU6502::RMW_DEC;
    m_Instructions[0xC6].m_opStr = "DEC";
    m_Instructions[0xC6].m_opAddressModeStr = "zpg";
    m_Instructions[0xC6].m_cycles = 5;
    
    m_Instructions[0xCE].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0xCE].m_operation = &CPU6502::RMW_DEC;
    m_Instructions[0xCE].m_opStr = "DEC";
    m_Instructions[0xCE].m_opAddressModeStr = "abs";
    m_Instructions[0xCE].m_cycles = 6;
    
    m_Instructions[0xD6].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0xD6].m_operation = &CPU6502::RMW_DEC;
    m_Instructions[0xD6].m_opStr = "DEC";
    m_Instructions[0xD6].m_opAddressModeStr = "zpg,X";
    m_Instructions[0xD6].m_cycles = 6;
    
    m_Instructions[0xD8].m_opOrAddrMode = &CPU6502::CLD;
    m_Instructions[0xD8].m_operation = nullptr;
    m_Instructions[0xD8].m_opStr = "CLD";
    m_Instructions[0xD8].m_opAddressModeStr = "";
    m_Instructions[0xD8].m_cycles = 2;
    
    m_Instructions[0xDE].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0xDE].m_operation = &CPU6502::RMW_DEC;
    m_Instructions[0xDE].m_opStr = "DEC";
    m_Instructions[0xDE].m_opAddressModeStr = "abs,X";
    m_Instructions[0xDE].m_cycles = 7;
    
    m_Instructions[0xEA].m_opOrAddrMode = &CPU6502::NOP;
    m_Instructions[0xEA].m_operation = nullptr;
    m_Instructions[0xEA].m_opStr = "NOP";
    m_Instructions[0xEA].m_opAddressModeStr = "";
    m_Instructions[0xEA].m_cycles = 2;
    
    m_Instructions[0xE6].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0xE6].m_operation = &CPU6502::RMW_INC;
    m_Instructions[0xE6].m_opStr = "INC";
    m_Instructions[0xE6].m_opAddressModeStr = "zpg";
    m_Instructions[0xE6].m_cycles = 5;
    
    m_Instructions[0xEE].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0xEE].m_operation = &CPU6502::RMW_INC;
    m_Instructions[0xEE].m_opStr = "INC";
    m_Instructions[0xEE].m_opAddressModeStr = "abs";
    m_Instructions[0xEE].m_cycles = 6;
    
    m_Instructions[0xF6].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0xF6].m_operation = &CPU6502::RMW_INC;
    m_Instructions[0xF6].m_opStr = "INC";
    m_Instructions[0xF6].m_opAddressModeStr = "zpg,X";
    m_Instructions[0xF6].m_cycles = 6;
    
    m_Instructions[0xFE].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0xFE].m_operation = &CPU6502::RMW_INC;
    m_Instructions[0xFE].m_opStr = "INC";
    m_Instructions[0xFE].m_opAddressModeStr = "abs,X";
    m_Instructions[0xFE].m_cycles = 7;
    
#if DEBUG
    // implemented and duplicate instruction set checks
    int implementedInstructions = 0;
    for(uint32_t i = 0;i < 256;++i)
    {
        if(m_Instructions[i].m_opOrAddrMode != &CPU6502::ERROR)
        {
            ++implementedInstructions;
            
            for(uint32_t j = 0;j < 256;++j)
            {
                if(i != j)
                {
                    if( m_Instructions[i].m_opOrAddrMode == m_Instructions[j].m_opOrAddrMode &&
                        m_Instructions[i].m_operation == m_Instructions[j].m_operation )
                    {
                        printf("Duplicate instruction found %d vs %d!!!", i, j);
                        ERROR(0);
                    }
                }
            }
        }
    }
    printf("6502 CPU Startup check: %d/130 [256] instructions implemented\n", implementedInstructions);
#endif
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
#else
    NOP(Tn);
#endif

    return false;
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

bool CPU6502::CLD(uint8_t Tn)
{
    if(Tn == 1)
    {
        ClearFlag(Flag_Decimal);
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
