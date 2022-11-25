//
//  CPU6502_InstructionDefTable.cpp
//  NES
//
//  Created by richardwallis on 20/11/2022.
//  Copyright © 2022 openplanetsoftware. All rights reserved.
//

#include "CPU6502.h"
#include <stdio.h>
#include <string.h>

void CPU6502::InitInstructions()
{
    char const* immediate = "imm";
    char const* zeroPage = "zpg";
    char const* zeroPageX = "zpg,X";
    char const* zeroPageY = "zpg,Y";
    char const* absolute = "abs";
    char const* absoluteX = "abs,X";
    char const* absoluteY = "abs,Y";
    char const* indirectX = "(ind,X)";
    char const* indirectY = "(ind),Y";
    char const* indirect = "(ind)";
    
    m_Instructions[0x00].m_opOrAddrMode = &CPU6502::BRK;
    m_Instructions[0x00].m_operation = nullptr;
    m_Instructions[0x00].m_opStr = "BRK";
    m_Instructions[0x00].m_opAddressModeStr = "";
    
    m_Instructions[0x01].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indX;
    m_Instructions[0x01].m_operation = &CPU6502::ORA;
    m_Instructions[0x01].m_opStr = "ORA";
    m_Instructions[0x01].m_opAddressModeStr = indirectX;
    
    m_Instructions[0x05].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0x05].m_operation = &CPU6502::ORA;
    m_Instructions[0x05].m_opStr = "ORA";
    m_Instructions[0x05].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0x06].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0x06].m_operation = &CPU6502::RMW_ASL;
    m_Instructions[0x06].m_opStr = "ASL";
    m_Instructions[0x06].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0x08].m_opOrAddrMode = &CPU6502::StackPush;
    m_Instructions[0x08].m_operation = &CPU6502::PHP;
    m_Instructions[0x08].m_opStr = "PHP";
    m_Instructions[0x08].m_opAddressModeStr = "";
    
    m_Instructions[0x09].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0x09].m_operation = &CPU6502::ORA;
    m_Instructions[0x09].m_opStr = "ORA";
    m_Instructions[0x09].m_opAddressModeStr = immediate;
    
    m_Instructions[0x10].m_opOrAddrMode = &CPU6502::Branch;
    m_Instructions[0x10].m_operation = &CPU6502::BPL;
    m_Instructions[0x10].m_opStr = "BPL";
    m_Instructions[0x10].m_opAddressModeStr = "";
    
    m_Instructions[0x11].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indY;
    m_Instructions[0x11].m_operation = &CPU6502::ORA;
    m_Instructions[0x11].m_opStr = "ORA";
    m_Instructions[0x11].m_opAddressModeStr = indirectY;
    
    m_Instructions[0x0A].m_opOrAddrMode = &CPU6502::Accum_ASL;
    m_Instructions[0x0A].m_operation = nullptr;
    m_Instructions[0x0A].m_opStr = "ASL";
    m_Instructions[0x0A].m_opAddressModeStr = "";
    
    m_Instructions[0x0D].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0x0D].m_operation = &CPU6502::ORA;
    m_Instructions[0x0D].m_opStr = "ORA";
    m_Instructions[0x0D].m_opAddressModeStr = absolute;
    
    m_Instructions[0x0E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0x0E].m_operation = &CPU6502::RMW_ASL;
    m_Instructions[0x0E].m_opStr = "ASL";
    m_Instructions[0x0E].m_opAddressModeStr = absolute;
    
    m_Instructions[0x15].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgX;
    m_Instructions[0x15].m_operation = &CPU6502::ORA;
    m_Instructions[0x15].m_opStr = "ORA";
    m_Instructions[0x15].m_opAddressModeStr = zeroPageX;
    
    m_Instructions[0x16].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0x16].m_operation = &CPU6502::RMW_ASL;
    m_Instructions[0x16].m_opStr = "ASL";
    m_Instructions[0x16].m_opAddressModeStr = zeroPageX;
    
    m_Instructions[0x18].m_opOrAddrMode = &CPU6502::CLC;
    m_Instructions[0x18].m_operation = nullptr;
    m_Instructions[0x18].m_opStr = "CLC";
    m_Instructions[0x18].m_opAddressModeStr = "";
    
    m_Instructions[0x19].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absY;
    m_Instructions[0x19].m_operation = &CPU6502::ORA;
    m_Instructions[0x19].m_opStr = "ORA";
    m_Instructions[0x19].m_opAddressModeStr = absoluteY;
    
    m_Instructions[0x1D].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absX;
    m_Instructions[0x1D].m_operation = &CPU6502::ORA;
    m_Instructions[0x1D].m_opStr = "ORA";
    m_Instructions[0x1D].m_opAddressModeStr = absoluteX;

    m_Instructions[0x1E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0x1E].m_operation = &CPU6502::RMW_ASL;
    m_Instructions[0x1E].m_opStr = "ASL";
    m_Instructions[0x1E].m_opAddressModeStr = absoluteX;
    
    m_Instructions[0x20].m_opOrAddrMode = &CPU6502::JSR;
    m_Instructions[0x20].m_operation = nullptr;
    m_Instructions[0x20].m_opStr = "JSR";
    m_Instructions[0x20].m_opAddressModeStr = "";
    
    m_Instructions[0x21].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indX;
    m_Instructions[0x21].m_operation = &CPU6502::AND;
    m_Instructions[0x21].m_opStr = "AND";
    m_Instructions[0x21].m_opAddressModeStr = indirectX;

    m_Instructions[0x24].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0x24].m_operation = &CPU6502::BIT;
    m_Instructions[0x24].m_opStr = "BIT";
    m_Instructions[0x24].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0x25].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0x25].m_operation = &CPU6502::AND;
    m_Instructions[0x25].m_opStr = "AND";
    m_Instructions[0x25].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0x26].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0x26].m_operation = &CPU6502::RMW_ROL;
    m_Instructions[0x26].m_opStr = "ROL";
    m_Instructions[0x26].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0x28].m_opOrAddrMode = &CPU6502::StackPull;
    m_Instructions[0x28].m_operation = &CPU6502::PLP;
    m_Instructions[0x28].m_opStr = "PLP";
    m_Instructions[0x28].m_opAddressModeStr = "";
    
    m_Instructions[0x29].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0x29].m_operation = &CPU6502::AND;
    m_Instructions[0x29].m_opStr = "AND";
    m_Instructions[0x29].m_opAddressModeStr = immediate;

    m_Instructions[0x2A].m_opOrAddrMode = &CPU6502::Accum_ROL;
    m_Instructions[0x2A].m_operation = nullptr;
    m_Instructions[0x2A].m_opStr = "ROL";
    m_Instructions[0x2A].m_opAddressModeStr = "";
    
    m_Instructions[0x2C].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0x2C].m_operation = &CPU6502::BIT;
    m_Instructions[0x2C].m_opStr = "BIT";
    m_Instructions[0x2C].m_opAddressModeStr = absolute;;
    
    m_Instructions[0x2D].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0x2D].m_operation = &CPU6502::AND;
    m_Instructions[0x2D].m_opStr = "AND";
    m_Instructions[0x2D].m_opAddressModeStr = absolute;
    
    m_Instructions[0x2E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0x2E].m_operation = &CPU6502::RMW_ROL;
    m_Instructions[0x2E].m_opStr = "ROL";
    m_Instructions[0x2E].m_opAddressModeStr = absolute;
    
    m_Instructions[0x30].m_opOrAddrMode = &CPU6502::Branch;
    m_Instructions[0x30].m_operation = &CPU6502::BMI;
    m_Instructions[0x30].m_opStr = "BMI";
    m_Instructions[0x30].m_opAddressModeStr = "";
    
    m_Instructions[0x31].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indY;
    m_Instructions[0x31].m_operation = &CPU6502::AND;
    m_Instructions[0x31].m_opStr = "AND";
    m_Instructions[0x31].m_opAddressModeStr = indirectY;
    
    m_Instructions[0x35].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgX;
    m_Instructions[0x35].m_operation = &CPU6502::AND;
    m_Instructions[0x35].m_opStr = "AND";
    m_Instructions[0x35].m_opAddressModeStr = zeroPageX;
    
    m_Instructions[0x36].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0x36].m_operation = &CPU6502::RMW_ROL;
    m_Instructions[0x36].m_opStr = "ROL";
    m_Instructions[0x36].m_opAddressModeStr = zeroPageX;
    
    m_Instructions[0x38].m_opOrAddrMode = &CPU6502::SEC;
    m_Instructions[0x38].m_operation = nullptr;
    m_Instructions[0x38].m_opStr = "SEC";
    m_Instructions[0x38].m_opAddressModeStr = "";
    
    m_Instructions[0x39].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absY;
    m_Instructions[0x39].m_operation = &CPU6502::AND;
    m_Instructions[0x39].m_opStr = "AND";
    m_Instructions[0x39].m_opAddressModeStr = absoluteY;
    
    m_Instructions[0x3D].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absX;
    m_Instructions[0x3D].m_operation = &CPU6502::AND;
    m_Instructions[0x3D].m_opStr = "AND";
    m_Instructions[0x3D].m_opAddressModeStr = absoluteX;
    
    m_Instructions[0x3E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0x3E].m_operation = &CPU6502::RMW_ROL;
    m_Instructions[0x3E].m_opStr = "ROL";
    m_Instructions[0x3E].m_opAddressModeStr = absoluteX;
    
    m_Instructions[0x40].m_opOrAddrMode = &CPU6502::RTI;
    m_Instructions[0x40].m_operation = nullptr;
    m_Instructions[0x40].m_opStr = "RTI";
    m_Instructions[0x40].m_opAddressModeStr = "";
    
    m_Instructions[0x41].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indX;
    m_Instructions[0x41].m_operation = &CPU6502::EOR;
    m_Instructions[0x41].m_opStr = "EOR";
    m_Instructions[0x41].m_opAddressModeStr = indirectX;
    
    m_Instructions[0x45].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0x45].m_operation = &CPU6502::EOR;
    m_Instructions[0x45].m_opStr = "EOR";
    m_Instructions[0x45].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0x46].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0x46].m_operation = &CPU6502::RMW_LSR;
    m_Instructions[0x46].m_opStr = "LSR";
    m_Instructions[0x46].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0x48].m_opOrAddrMode = &CPU6502::StackPush;
    m_Instructions[0x48].m_operation = &CPU6502::PHA;
    m_Instructions[0x48].m_opStr = "PHA";
    m_Instructions[0x48].m_opAddressModeStr = "";
    
    m_Instructions[0x49].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0x49].m_operation = &CPU6502::EOR;
    m_Instructions[0x49].m_opStr = "EOR";
    m_Instructions[0x49].m_opAddressModeStr = immediate;
    
    m_Instructions[0x4A].m_opOrAddrMode = &CPU6502::Accum_LSR;
    m_Instructions[0x4A].m_operation = nullptr;
    m_Instructions[0x4A].m_opStr = "LSR";
    m_Instructions[0x4A].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0x4C].m_opOrAddrMode = &CPU6502::JMP_abs;
    m_Instructions[0x4C].m_operation = nullptr;
    m_Instructions[0x4C].m_opStr = "JMP";
    m_Instructions[0x4C].m_opAddressModeStr = absolute;
    
    m_Instructions[0x4D].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0x4D].m_operation = &CPU6502::EOR;
    m_Instructions[0x4D].m_opStr = "EOR";
    m_Instructions[0x4D].m_opAddressModeStr = absolute;
    
    m_Instructions[0x4E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0x4E].m_operation = &CPU6502::RMW_LSR;
    m_Instructions[0x4E].m_opStr = "LSR";
    m_Instructions[0x4E].m_opAddressModeStr = absolute;
    
    m_Instructions[0x50].m_opOrAddrMode = &CPU6502::Branch;
    m_Instructions[0x50].m_operation = &CPU6502::BVC;
    m_Instructions[0x50].m_opStr = "BVC";
    m_Instructions[0x50].m_opAddressModeStr = "";
    
    m_Instructions[0x51].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indY;
    m_Instructions[0x51].m_operation = &CPU6502::EOR;
    m_Instructions[0x51].m_opStr = "EOR";
    m_Instructions[0x51].m_opAddressModeStr = indirectY;
    
    m_Instructions[0x55].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgX;
    m_Instructions[0x55].m_operation = &CPU6502::EOR;
    m_Instructions[0x55].m_opStr = "EOR";
    m_Instructions[0x55].m_opAddressModeStr = zeroPageX;
    
    m_Instructions[0x56].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0x56].m_operation = &CPU6502::RMW_LSR;
    m_Instructions[0x56].m_opStr = "LSR";
    m_Instructions[0x56].m_opAddressModeStr = zeroPageX;
    
    m_Instructions[0x58].m_opOrAddrMode = &CPU6502::CLI;
    m_Instructions[0x58].m_operation = nullptr;
    m_Instructions[0x58].m_opStr = "CLI";
    m_Instructions[0x58].m_opAddressModeStr = "";
    
    m_Instructions[0x59].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absY;
    m_Instructions[0x59].m_operation = &CPU6502::EOR;
    m_Instructions[0x59].m_opStr = "EOR";
    m_Instructions[0x59].m_opAddressModeStr = absoluteY;
    
    m_Instructions[0x5D].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absX;
    m_Instructions[0x5D].m_operation = &CPU6502::EOR;
    m_Instructions[0x5D].m_opStr = "EOR";
    m_Instructions[0x5D].m_opAddressModeStr = absoluteX;
    
    m_Instructions[0x5E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0x5E].m_operation = &CPU6502::RMW_LSR;
    m_Instructions[0x5E].m_opStr = "LSR";
    m_Instructions[0x5E].m_opAddressModeStr = absoluteX;
    
    m_Instructions[0x60].m_opOrAddrMode = &CPU6502::RTS;
    m_Instructions[0x60].m_operation = nullptr;
    m_Instructions[0x60].m_opStr = "RTS";
    m_Instructions[0x60].m_opAddressModeStr = "";
    
    m_Instructions[0x61].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indX;
    m_Instructions[0x61].m_operation = &CPU6502::ADC;
    m_Instructions[0x61].m_opStr = "ADC";
    m_Instructions[0x61].m_opAddressModeStr = indirectX;
    
    m_Instructions[0x65].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0x65].m_operation = &CPU6502::ADC;
    m_Instructions[0x65].m_opStr = "ADC";
    m_Instructions[0x65].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0x66].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0x66].m_operation = &CPU6502::RMW_ROR;
    m_Instructions[0x66].m_opStr = "ROR";
    m_Instructions[0x66].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0x68].m_opOrAddrMode = &CPU6502::StackPull;
    m_Instructions[0x68].m_operation = &CPU6502::PLA;
    m_Instructions[0x68].m_opStr = "PLA";
    m_Instructions[0x68].m_opAddressModeStr = "";
    
    m_Instructions[0x69].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0x69].m_operation = &CPU6502::ADC;
    m_Instructions[0x69].m_opStr = "ADC";
    m_Instructions[0x69].m_opAddressModeStr = immediate;
    
    m_Instructions[0x70].m_opOrAddrMode = &CPU6502::Branch;
    m_Instructions[0x70].m_operation = &CPU6502::BVS;
    m_Instructions[0x70].m_opStr = "BVS";
    m_Instructions[0x70].m_opAddressModeStr = "";
    
    m_Instructions[0x6A].m_opOrAddrMode = &CPU6502::Accum_ROR;
    m_Instructions[0x6A].m_operation = nullptr;
    m_Instructions[0x6A].m_opStr = "ROR";
    m_Instructions[0x6A].m_opAddressModeStr = "";
    
    m_Instructions[0x6C].m_opOrAddrMode = &CPU6502::JMP_ind;
    m_Instructions[0x6C].m_operation = nullptr;
    m_Instructions[0x6C].m_opStr = "JMP";
    m_Instructions[0x6C].m_opAddressModeStr = indirect;
    
    m_Instructions[0x6D].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0x6D].m_operation = &CPU6502::ADC;
    m_Instructions[0x6D].m_opStr = "ADC";
    m_Instructions[0x6D].m_opAddressModeStr = absolute;
    
    m_Instructions[0x6E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0x6E].m_operation = &CPU6502::RMW_ROR;
    m_Instructions[0x6E].m_opStr = "ROR";
    m_Instructions[0x6E].m_opAddressModeStr = absolute;
    
    m_Instructions[0x71].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indY;
    m_Instructions[0x71].m_operation = &CPU6502::ADC;
    m_Instructions[0x71].m_opStr = "ADC";
    m_Instructions[0x71].m_opAddressModeStr = indirectY;
    
    m_Instructions[0x75].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgX;
    m_Instructions[0x75].m_operation = &CPU6502::ADC;
    m_Instructions[0x75].m_opStr = "ADC";
    m_Instructions[0x75].m_opAddressModeStr = zeroPageX;
    
    m_Instructions[0x76].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0x76].m_operation = &CPU6502::RMW_ROR;
    m_Instructions[0x76].m_opStr = "ROR";
    m_Instructions[0x76].m_opAddressModeStr = zeroPageX;
    
    m_Instructions[0x78].m_opOrAddrMode = &CPU6502::SEI;
    m_Instructions[0x78].m_operation = nullptr;
    m_Instructions[0x78].m_opStr = "SEI";
    m_Instructions[0x78].m_opAddressModeStr = "";
    
    m_Instructions[0x79].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absY;
    m_Instructions[0x79].m_operation = &CPU6502::ADC;
    m_Instructions[0x79].m_opStr = "ADC";
    m_Instructions[0x79].m_opAddressModeStr = absoluteY;
    
    m_Instructions[0x7D].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absX;
    m_Instructions[0x7D].m_operation = &CPU6502::ADC;
    m_Instructions[0x7D].m_opStr = "ADC";
    m_Instructions[0x7D].m_opAddressModeStr = absoluteX;
    
    m_Instructions[0x7E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0x7E].m_operation = &CPU6502::RMW_ROR;
    m_Instructions[0x7E].m_opStr = "ROR";
    m_Instructions[0x7E].m_opAddressModeStr = absoluteX;
    
    m_Instructions[0x81].m_opOrAddrMode = &CPU6502::Store_indX;
    m_Instructions[0x81].m_operation = &CPU6502::STA;
    m_Instructions[0x81].m_opStr = "STA";
    m_Instructions[0x81].m_opAddressModeStr = indirectX;
    
    m_Instructions[0x84].m_opOrAddrMode = &CPU6502::Store_zpg;
    m_Instructions[0x84].m_operation = &CPU6502::STY;
    m_Instructions[0x84].m_opStr = "STY";
    m_Instructions[0x84].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0x85].m_opOrAddrMode = &CPU6502::Store_zpg;
    m_Instructions[0x85].m_operation = &CPU6502::STA;
    m_Instructions[0x85].m_opStr = "STA";
    m_Instructions[0x85].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0x86].m_opOrAddrMode = &CPU6502::Store_zpg;
    m_Instructions[0x86].m_operation = &CPU6502::STX;
    m_Instructions[0x86].m_opStr = "STX";
    m_Instructions[0x86].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0x88].m_opOrAddrMode = &CPU6502::DEY;
    m_Instructions[0x88].m_operation = nullptr;
    m_Instructions[0x88].m_opStr = "DEY";
    m_Instructions[0x88].m_opAddressModeStr = "";
    
    m_Instructions[0x8A].m_opOrAddrMode = &CPU6502::TXA;
    m_Instructions[0x8A].m_operation = nullptr;
    m_Instructions[0x8A].m_opStr = "TXA";
    m_Instructions[0x8A].m_opAddressModeStr = "";
    
    m_Instructions[0x8C].m_opOrAddrMode = &CPU6502::Store_abs;
    m_Instructions[0x8C].m_operation = &CPU6502::STY;
    m_Instructions[0x8C].m_opStr = "STY";
    m_Instructions[0x8C].m_opAddressModeStr = absolute;
    
    m_Instructions[0x8D].m_opOrAddrMode = &CPU6502::Store_abs;
    m_Instructions[0x8D].m_operation = &CPU6502::STA;
    m_Instructions[0x8D].m_opStr = "STA";
    m_Instructions[0x8D].m_opAddressModeStr = absolute;
    
    m_Instructions[0x8E].m_opOrAddrMode = &CPU6502::Store_abs;
    m_Instructions[0x8E].m_operation = &CPU6502::STX;
    m_Instructions[0x8E].m_opStr = "STX";
    m_Instructions[0x8E].m_opAddressModeStr = absolute;
    
    m_Instructions[0x90].m_opOrAddrMode = &CPU6502::Branch;
    m_Instructions[0x90].m_operation = &CPU6502::BCC;
    m_Instructions[0x90].m_opStr = "BCC";
    m_Instructions[0x90].m_opAddressModeStr = "";
    
    m_Instructions[0x91].m_opOrAddrMode = &CPU6502::Store_indY;
    m_Instructions[0x91].m_operation = &CPU6502::STA;
    m_Instructions[0x91].m_opStr = "STA";
    m_Instructions[0x91].m_opAddressModeStr = indirectY;
    
    m_Instructions[0x94].m_opOrAddrMode = &CPU6502::Store_zpgX;
    m_Instructions[0x94].m_operation = &CPU6502::STY;
    m_Instructions[0x94].m_opStr = "STY";
    m_Instructions[0x94].m_opAddressModeStr = zeroPageX;
    
    m_Instructions[0x95].m_opOrAddrMode = &CPU6502::Store_zpgX;
    m_Instructions[0x95].m_operation = &CPU6502::STA;
    m_Instructions[0x95].m_opStr = "STA";
    m_Instructions[0x95].m_opAddressModeStr = zeroPageX;
    
    m_Instructions[0x96].m_opOrAddrMode = &CPU6502::Store_zpgY;
    m_Instructions[0x96].m_operation = &CPU6502::STX;
    m_Instructions[0x96].m_opStr = "STX";
    m_Instructions[0x96].m_opAddressModeStr = zeroPageY;
    
    m_Instructions[0x98].m_opOrAddrMode = &CPU6502::TYA;
    m_Instructions[0x98].m_operation = nullptr;
    m_Instructions[0x98].m_opStr = "TYA";
    m_Instructions[0x98].m_opAddressModeStr = "";
    
    m_Instructions[0x99].m_opOrAddrMode = &CPU6502::Store_absY;
    m_Instructions[0x99].m_operation = &CPU6502::STA;
    m_Instructions[0x99].m_opStr = "STA";
    m_Instructions[0x99].m_opAddressModeStr = absoluteY;
    
    m_Instructions[0x9A].m_opOrAddrMode = &CPU6502::TXS;
    m_Instructions[0x9A].m_operation = nullptr;
    m_Instructions[0x9A].m_opStr = "TXS";
    m_Instructions[0x9A].m_opAddressModeStr = "";
    
    m_Instructions[0x9D].m_opOrAddrMode = &CPU6502::Store_absX;
    m_Instructions[0x9D].m_operation = &CPU6502::STA;
    m_Instructions[0x9D].m_opStr = "STA";
    m_Instructions[0x9D].m_opAddressModeStr = absoluteX;
    
    m_Instructions[0xA0].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0xA0].m_operation = &CPU6502::LDY;
    m_Instructions[0xA0].m_opStr = "LDY";
    m_Instructions[0xA0].m_opAddressModeStr = immediate;
    
    m_Instructions[0xA1].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indX;
    m_Instructions[0xA1].m_operation = &CPU6502::LDA;
    m_Instructions[0xA1].m_opStr = "LDA";
    m_Instructions[0xA1].m_opAddressModeStr = indirectX;
    
    m_Instructions[0xA2].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0xA2].m_operation = &CPU6502::LDX;
    m_Instructions[0xA2].m_opStr = "LDX";
    m_Instructions[0xA2].m_opAddressModeStr = immediate;
    
    m_Instructions[0xA4].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0xA4].m_operation = &CPU6502::LDY;
    m_Instructions[0xA4].m_opStr = "LDY";
    m_Instructions[0xA4].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0xA5].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0xA5].m_operation = &CPU6502::LDA;
    m_Instructions[0xA5].m_opStr = "LDA";
    m_Instructions[0xA5].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0xA6].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0xA6].m_operation = &CPU6502::LDX;
    m_Instructions[0xA6].m_opStr = "LDX";
    m_Instructions[0xA6].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0xA8].m_opOrAddrMode = &CPU6502::TAY;
    m_Instructions[0xA8].m_operation = nullptr;
    m_Instructions[0xA8].m_opStr = "TAY";
    m_Instructions[0xA8].m_opAddressModeStr = "";
    
    m_Instructions[0xA9].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0xA9].m_operation = &CPU6502::LDA;
    m_Instructions[0xA9].m_opStr = "LDA";
    m_Instructions[0xA9].m_opAddressModeStr = immediate;
    
    m_Instructions[0xAA].m_opOrAddrMode = &CPU6502::TAX;
    m_Instructions[0xAA].m_operation = nullptr;
    m_Instructions[0xAA].m_opStr = "TAX";
    m_Instructions[0xAA].m_opAddressModeStr = "";
    
    m_Instructions[0xAC].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0xAC].m_operation = &CPU6502::LDY;
    m_Instructions[0xAC].m_opStr = "LDY";
    m_Instructions[0xAC].m_opAddressModeStr = absolute;

    m_Instructions[0xAD].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0xAD].m_operation = &CPU6502::LDA;
    m_Instructions[0xAD].m_opStr = "LDA";
    m_Instructions[0xAD].m_opAddressModeStr = absolute;
    
    m_Instructions[0xAE].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0xAE].m_operation = &CPU6502::LDX;
    m_Instructions[0xAE].m_opStr = "LDX";
    m_Instructions[0xAE].m_opAddressModeStr = absolute;
    
    m_Instructions[0xB0].m_opOrAddrMode = &CPU6502::Branch;
    m_Instructions[0xB0].m_operation = &CPU6502::BCS;
    m_Instructions[0xB0].m_opStr = "BCS";
    m_Instructions[0xB0].m_opAddressModeStr = "";
    
    m_Instructions[0xB1].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indY;
    m_Instructions[0xB1].m_operation = &CPU6502::LDA;
    m_Instructions[0xB1].m_opStr = "LDA";
    m_Instructions[0xB1].m_opAddressModeStr = indirectY;
    
    m_Instructions[0xB4].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgX;
    m_Instructions[0xB4].m_operation = &CPU6502::LDY;
    m_Instructions[0xB4].m_opStr = "LDY";
    m_Instructions[0xB4].m_opAddressModeStr = zeroPageX;
    
    m_Instructions[0xB5].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgX;
    m_Instructions[0xB5].m_operation = &CPU6502::LDA;
    m_Instructions[0xB5].m_opStr = "LDA";
    m_Instructions[0xB5].m_opAddressModeStr = zeroPageX;
    
    m_Instructions[0xB6].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgY;
    m_Instructions[0xB6].m_operation = &CPU6502::LDX;
    m_Instructions[0xB6].m_opStr = "LDX";
    m_Instructions[0xB6].m_opAddressModeStr = zeroPageY;
    
    m_Instructions[0xB8].m_opOrAddrMode = &CPU6502::CLV;
    m_Instructions[0xB8].m_operation = nullptr;
    m_Instructions[0xB8].m_opStr = "CLV";
    m_Instructions[0xB8].m_opAddressModeStr = "";
    
    m_Instructions[0xB9].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absY;
    m_Instructions[0xB9].m_operation = &CPU6502::LDA;
    m_Instructions[0xB9].m_opStr = "LDA";
    m_Instructions[0xB9].m_opAddressModeStr = absoluteY;
    
    m_Instructions[0xBA].m_opOrAddrMode = &CPU6502::TSX;
    m_Instructions[0xBA].m_operation = nullptr;
    m_Instructions[0xBA].m_opStr = "TSX";
    m_Instructions[0xBA].m_opAddressModeStr = "";
    
    m_Instructions[0xBC].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absX;
    m_Instructions[0xBC].m_operation = &CPU6502::LDY;
    m_Instructions[0xBC].m_opStr = "LDY";
    m_Instructions[0xBC].m_opAddressModeStr = absoluteX;
    
    m_Instructions[0xBD].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absX;
    m_Instructions[0xBD].m_operation = &CPU6502::LDA;
    m_Instructions[0xBD].m_opStr = "LDA";
    m_Instructions[0xBD].m_opAddressModeStr = absoluteX;
    
    m_Instructions[0xBE].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absY;
    m_Instructions[0xBE].m_operation = &CPU6502::LDX;
    m_Instructions[0xBE].m_opStr = "LDX";
    m_Instructions[0xBE].m_opAddressModeStr = absoluteY;
    
    m_Instructions[0xC0].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0xC0].m_operation = &CPU6502::CPY;
    m_Instructions[0xC0].m_opStr = "CPY";
    m_Instructions[0xC0].m_opAddressModeStr = immediate;
    
    m_Instructions[0xC1].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indX;
    m_Instructions[0xC1].m_operation = &CPU6502::CMP;
    m_Instructions[0xC1].m_opStr = "CMP";
    m_Instructions[0xC1].m_opAddressModeStr = indirectX;
    
    m_Instructions[0xC4].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0xC4].m_operation = &CPU6502::CPY;
    m_Instructions[0xC4].m_opStr = "CPY";
    m_Instructions[0xC4].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0xC5].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0xC5].m_operation = &CPU6502::CMP;
    m_Instructions[0xC5].m_opStr = "CMP";
    m_Instructions[0xC5].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0xC6].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0xC6].m_operation = &CPU6502::RMW_DEC;
    m_Instructions[0xC6].m_opStr = "DEC";
    m_Instructions[0xC6].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0xC8].m_opOrAddrMode = &CPU6502::INY;
    m_Instructions[0xC8].m_operation = nullptr;
    m_Instructions[0xC8].m_opStr = "INY";
    m_Instructions[0xC8].m_opAddressModeStr = "";
    
    m_Instructions[0xC9].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0xC9].m_operation = &CPU6502::CMP;
    m_Instructions[0xC9].m_opStr = "CMP";
    m_Instructions[0xC9].m_opAddressModeStr = immediate;
    
    m_Instructions[0xCA].m_opOrAddrMode = &CPU6502::DEX;
    m_Instructions[0xCA].m_operation = nullptr;
    m_Instructions[0xCA].m_opStr = "DEX";
    m_Instructions[0xCA].m_opAddressModeStr = "";
    
    m_Instructions[0xCC].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0xCC].m_operation = &CPU6502::CPY;
    m_Instructions[0xCC].m_opStr = "CPY";
    m_Instructions[0xCC].m_opAddressModeStr = absolute;
    
    m_Instructions[0xCD].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0xCD].m_operation = &CPU6502::CMP;
    m_Instructions[0xCD].m_opStr = "CMP";
    m_Instructions[0xCD].m_opAddressModeStr = absolute;
    
    m_Instructions[0xCE].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0xCE].m_operation = &CPU6502::RMW_DEC;
    m_Instructions[0xCE].m_opStr = "DEC";
    m_Instructions[0xCE].m_opAddressModeStr = absolute;
    
    m_Instructions[0xD0].m_opOrAddrMode = &CPU6502::Branch;
    m_Instructions[0xD0].m_operation = &CPU6502::BNE;
    m_Instructions[0xD0].m_opStr = "BNE";
    m_Instructions[0xD0].m_opAddressModeStr = "";
    
    m_Instructions[0xD1].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indY;
    m_Instructions[0xD1].m_operation = &CPU6502::CMP;
    m_Instructions[0xD1].m_opStr = "CMP";
    m_Instructions[0xD1].m_opAddressModeStr = indirectY;
    
    m_Instructions[0xD5].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgX;
    m_Instructions[0xD5].m_operation = &CPU6502::CMP;
    m_Instructions[0xD5].m_opStr = "CMP";
    m_Instructions[0xD5].m_opAddressModeStr = zeroPageX;
    
    m_Instructions[0xD6].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0xD6].m_operation = &CPU6502::RMW_DEC;
    m_Instructions[0xD6].m_opStr = "DEC";
    m_Instructions[0xD6].m_opAddressModeStr = zeroPageX;
    
    m_Instructions[0xD8].m_opOrAddrMode = &CPU6502::CLD;
    m_Instructions[0xD8].m_operation = nullptr;
    m_Instructions[0xD8].m_opStr = "CLD";
    m_Instructions[0xD8].m_opAddressModeStr = "";
    
    m_Instructions[0xD9].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absY;
    m_Instructions[0xD9].m_operation = &CPU6502::CMP;
    m_Instructions[0xD9].m_opStr = "CMP";
    m_Instructions[0xD9].m_opAddressModeStr = absoluteY;
    
    m_Instructions[0xDD].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absX;
    m_Instructions[0xDD].m_operation = &CPU6502::CMP;
    m_Instructions[0xDD].m_opStr = "CMP";
    m_Instructions[0xDD].m_opAddressModeStr = absoluteX;
    
    m_Instructions[0xDE].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0xDE].m_operation = &CPU6502::RMW_DEC;
    m_Instructions[0xDE].m_opStr = "DEC";
    m_Instructions[0xDE].m_opAddressModeStr = absoluteX;
    
    m_Instructions[0xE0].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0xE0].m_operation = &CPU6502::CPX;
    m_Instructions[0xE0].m_opStr = "CPX";
    m_Instructions[0xE0].m_opAddressModeStr = immediate;
    
    m_Instructions[0xE1].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indX;
    m_Instructions[0xE1].m_operation = &CPU6502::SBC;
    m_Instructions[0xE1].m_opStr = "SBC";
    m_Instructions[0xE1].m_opAddressModeStr = indirectX;
    
    m_Instructions[0xE4].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0xE4].m_operation = &CPU6502::CPX;
    m_Instructions[0xE4].m_opStr = "CPX";
    m_Instructions[0xE4].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0xE5].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0xE5].m_operation = &CPU6502::SBC;
    m_Instructions[0xE5].m_opStr = "SBC";
    m_Instructions[0xE5].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0xE9].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0xE9].m_operation = &CPU6502::SBC;
    m_Instructions[0xE9].m_opStr = "SBC";
    m_Instructions[0xE9].m_opAddressModeStr = immediate;
    
    m_Instructions[0xEA].m_opOrAddrMode = &CPU6502::NOP;
    m_Instructions[0xEA].m_operation = nullptr;
    m_Instructions[0xEA].m_opStr = "NOP";
    m_Instructions[0xEA].m_opAddressModeStr = "";
    
    m_Instructions[0xEC].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0xEC].m_operation = &CPU6502::CPX;
    m_Instructions[0xEC].m_opStr = "CPX";
    m_Instructions[0xEC].m_opAddressModeStr = absolute;
    
    m_Instructions[0xED].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0xED].m_operation = &CPU6502::SBC;
    m_Instructions[0xED].m_opStr = "SBC";
    m_Instructions[0xED].m_opAddressModeStr = absolute;
    
    m_Instructions[0xE6].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0xE6].m_operation = &CPU6502::RMW_INC;
    m_Instructions[0xE6].m_opStr = "INC";
    m_Instructions[0xE6].m_opAddressModeStr = zeroPage;
    
    m_Instructions[0xE8].m_opOrAddrMode = &CPU6502::INX;
    m_Instructions[0xE8].m_operation = nullptr;
    m_Instructions[0xE8].m_opStr = "INX";
    m_Instructions[0xE8].m_opAddressModeStr = "";
    
    m_Instructions[0xEE].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0xEE].m_operation = &CPU6502::RMW_INC;
    m_Instructions[0xEE].m_opStr = "INC";
    m_Instructions[0xEE].m_opAddressModeStr = absolute;
    
    m_Instructions[0xF0].m_opOrAddrMode = &CPU6502::Branch;
    m_Instructions[0xF0].m_operation = &CPU6502::BEQ;
    m_Instructions[0xF0].m_opStr = "BEQ";
    m_Instructions[0xF0].m_opAddressModeStr = "";
    
    m_Instructions[0xF1].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indY;
    m_Instructions[0xF1].m_operation = &CPU6502::SBC;
    m_Instructions[0xF1].m_opStr = "SBC";
    m_Instructions[0xF1].m_opAddressModeStr = indirectY;
    
    m_Instructions[0xF5].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgX;
    m_Instructions[0xF5].m_operation = &CPU6502::SBC;
    m_Instructions[0xF5].m_opStr = "SBC";
    m_Instructions[0xF5].m_opAddressModeStr = zeroPageX;
    
    m_Instructions[0xF6].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0xF6].m_operation = &CPU6502::RMW_INC;
    m_Instructions[0xF6].m_opStr = "INC";
    m_Instructions[0xF6].m_opAddressModeStr = zeroPageX;
    
    m_Instructions[0xF8].m_opOrAddrMode = &CPU6502::SED;
    m_Instructions[0xF8].m_operation = nullptr;
    m_Instructions[0xF8].m_opStr = "SED";
    m_Instructions[0xF8].m_opAddressModeStr = "";
    
    m_Instructions[0xF9].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absY;
    m_Instructions[0xF9].m_operation = &CPU6502::SBC;
    m_Instructions[0xF9].m_opStr = "SBC";
    m_Instructions[0xF9].m_opAddressModeStr = absoluteY;
    
    m_Instructions[0xFD].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absX;
    m_Instructions[0xFD].m_operation = &CPU6502::SBC;
    m_Instructions[0xFD].m_opStr = "SBC";
    m_Instructions[0xFD].m_opAddressModeStr = absoluteX;
    
    m_Instructions[0xFE].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0xFE].m_operation = &CPU6502::RMW_INC;
    m_Instructions[0xFE].m_opStr = "INC";
    m_Instructions[0xFE].m_opAddressModeStr = absoluteX;

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
                        printf("Duplicate instruction found %2X vs %2X!!!", i, j);
                        ERROR(0);
                    }
                }
            }
        }
    }
    printf("6502 CPU Startup check: %d instructions implemented\n", implementedInstructions);
#endif
}
