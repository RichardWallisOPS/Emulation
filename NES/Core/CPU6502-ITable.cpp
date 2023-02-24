//
//  CPU6502_InstructionDefTable.cpp
//  NES
//
//  Created by richardwallis on 20/11/2022.
//

#include "CPU6502.h"
#include <stdio.h>
#include <string.h>

void CPU6502::InitInstructions()
{
    m_Instructions[0x00].m_opOrAddrMode = &CPU6502::BRK;
    m_Instructions[0x00].m_operation = nullptr;
    
    m_Instructions[0x01].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indX;
    m_Instructions[0x01].m_operation = &CPU6502::ORA;
    
    m_Instructions[0x05].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0x05].m_operation = &CPU6502::ORA;
    
    m_Instructions[0x06].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0x06].m_operation = &CPU6502::RMW_ASL;
    
    m_Instructions[0x08].m_opOrAddrMode = &CPU6502::StackPush;
    m_Instructions[0x08].m_operation = &CPU6502::PHP;
    
    m_Instructions[0x09].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0x09].m_operation = &CPU6502::ORA;
    
    m_Instructions[0x10].m_opOrAddrMode = &CPU6502::Branch;
    m_Instructions[0x10].m_operation = &CPU6502::BPL;
    
    m_Instructions[0x11].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indY;
    m_Instructions[0x11].m_operation = &CPU6502::ORA;
    
    m_Instructions[0x0A].m_opOrAddrMode = &CPU6502::Accum_ASL;
    m_Instructions[0x0A].m_operation = nullptr;
    
    m_Instructions[0x0D].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0x0D].m_operation = &CPU6502::ORA;
    
    m_Instructions[0x0E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0x0E].m_operation = &CPU6502::RMW_ASL;
    
    m_Instructions[0x15].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgX;
    m_Instructions[0x15].m_operation = &CPU6502::ORA;
    
    m_Instructions[0x16].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0x16].m_operation = &CPU6502::RMW_ASL;
    
    m_Instructions[0x18].m_opOrAddrMode = &CPU6502::CLC;
    m_Instructions[0x18].m_operation = nullptr;
    
    m_Instructions[0x19].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absY;
    m_Instructions[0x19].m_operation = &CPU6502::ORA;
    
    m_Instructions[0x1D].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absX;
    m_Instructions[0x1D].m_operation = &CPU6502::ORA;

    m_Instructions[0x1E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0x1E].m_operation = &CPU6502::RMW_ASL;
    
    m_Instructions[0x20].m_opOrAddrMode = &CPU6502::JSR;
    m_Instructions[0x20].m_operation = nullptr;
    
    m_Instructions[0x21].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indX;
    m_Instructions[0x21].m_operation = &CPU6502::AND;

    m_Instructions[0x24].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0x24].m_operation = &CPU6502::BIT;
    
    m_Instructions[0x25].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0x25].m_operation = &CPU6502::AND;
    
    m_Instructions[0x26].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0x26].m_operation = &CPU6502::RMW_ROL;
    
    m_Instructions[0x28].m_opOrAddrMode = &CPU6502::StackPull;
    m_Instructions[0x28].m_operation = &CPU6502::PLP;
    
    m_Instructions[0x29].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0x29].m_operation = &CPU6502::AND;

    m_Instructions[0x2A].m_opOrAddrMode = &CPU6502::Accum_ROL;
    m_Instructions[0x2A].m_operation = nullptr;
    
    m_Instructions[0x2C].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0x2C].m_operation = &CPU6502::BIT;
    
    m_Instructions[0x2D].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0x2D].m_operation = &CPU6502::AND;
    
    m_Instructions[0x2E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0x2E].m_operation = &CPU6502::RMW_ROL;
    
    m_Instructions[0x30].m_opOrAddrMode = &CPU6502::Branch;
    m_Instructions[0x30].m_operation = &CPU6502::BMI;
    
    m_Instructions[0x31].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indY;
    m_Instructions[0x31].m_operation = &CPU6502::AND;
    
    m_Instructions[0x35].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgX;
    m_Instructions[0x35].m_operation = &CPU6502::AND;
    
    m_Instructions[0x36].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0x36].m_operation = &CPU6502::RMW_ROL;
    
    m_Instructions[0x38].m_opOrAddrMode = &CPU6502::SEC;
    m_Instructions[0x38].m_operation = nullptr;
    
    m_Instructions[0x39].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absY;
    m_Instructions[0x39].m_operation = &CPU6502::AND;
    
    m_Instructions[0x3D].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absX;
    m_Instructions[0x3D].m_operation = &CPU6502::AND;
    
    m_Instructions[0x3E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0x3E].m_operation = &CPU6502::RMW_ROL;
    
    m_Instructions[0x40].m_opOrAddrMode = &CPU6502::RTI;
    m_Instructions[0x40].m_operation = nullptr;
    
    m_Instructions[0x41].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indX;
    m_Instructions[0x41].m_operation = &CPU6502::EOR;
    
    m_Instructions[0x45].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0x45].m_operation = &CPU6502::EOR;
    
    m_Instructions[0x46].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0x46].m_operation = &CPU6502::RMW_LSR;
    
    m_Instructions[0x48].m_opOrAddrMode = &CPU6502::StackPush;
    m_Instructions[0x48].m_operation = &CPU6502::PHA;
    
    m_Instructions[0x49].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0x49].m_operation = &CPU6502::EOR;
    
    m_Instructions[0x4A].m_opOrAddrMode = &CPU6502::Accum_LSR;
    m_Instructions[0x4A].m_operation = nullptr;
    
    m_Instructions[0x4C].m_opOrAddrMode = &CPU6502::JMP_abs;
    m_Instructions[0x4C].m_operation = nullptr;
    
    m_Instructions[0x4D].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0x4D].m_operation = &CPU6502::EOR;
    
    m_Instructions[0x4E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0x4E].m_operation = &CPU6502::RMW_LSR;
    
    m_Instructions[0x50].m_opOrAddrMode = &CPU6502::Branch;
    m_Instructions[0x50].m_operation = &CPU6502::BVC;

    m_Instructions[0x51].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indY;
    m_Instructions[0x51].m_operation = &CPU6502::EOR;
    
    m_Instructions[0x55].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgX;
    m_Instructions[0x55].m_operation = &CPU6502::EOR;
    
    m_Instructions[0x56].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0x56].m_operation = &CPU6502::RMW_LSR;
    
    m_Instructions[0x58].m_opOrAddrMode = &CPU6502::CLI;
    m_Instructions[0x58].m_operation = nullptr;
    
    m_Instructions[0x59].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absY;
    m_Instructions[0x59].m_operation = &CPU6502::EOR;
    
    m_Instructions[0x5D].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absX;
    m_Instructions[0x5D].m_operation = &CPU6502::EOR;
    
    m_Instructions[0x5E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0x5E].m_operation = &CPU6502::RMW_LSR;
    
    m_Instructions[0x60].m_opOrAddrMode = &CPU6502::RTS;
    m_Instructions[0x60].m_operation = nullptr;
    
    m_Instructions[0x61].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indX;
    m_Instructions[0x61].m_operation = &CPU6502::ADC;
    
    m_Instructions[0x65].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0x65].m_operation = &CPU6502::ADC;
    
    m_Instructions[0x66].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0x66].m_operation = &CPU6502::RMW_ROR;
    
    m_Instructions[0x68].m_opOrAddrMode = &CPU6502::StackPull;
    m_Instructions[0x68].m_operation = &CPU6502::PLA;
    
    m_Instructions[0x69].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0x69].m_operation = &CPU6502::ADC;
    
    m_Instructions[0x70].m_opOrAddrMode = &CPU6502::Branch;
    m_Instructions[0x70].m_operation = &CPU6502::BVS;
    
    m_Instructions[0x6A].m_opOrAddrMode = &CPU6502::Accum_ROR;
    m_Instructions[0x6A].m_operation = nullptr;
    
    m_Instructions[0x6C].m_opOrAddrMode = &CPU6502::JMP_ind;
    m_Instructions[0x6C].m_operation = nullptr;
    
    m_Instructions[0x6D].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0x6D].m_operation = &CPU6502::ADC;
    
    m_Instructions[0x6E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0x6E].m_operation = &CPU6502::RMW_ROR;
    
    m_Instructions[0x71].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indY;
    m_Instructions[0x71].m_operation = &CPU6502::ADC;
    
    m_Instructions[0x75].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgX;
    m_Instructions[0x75].m_operation = &CPU6502::ADC;
    
    m_Instructions[0x76].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0x76].m_operation = &CPU6502::RMW_ROR;
    
    m_Instructions[0x78].m_opOrAddrMode = &CPU6502::SEI;
    m_Instructions[0x78].m_operation = nullptr;
    
    m_Instructions[0x79].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absY;
    m_Instructions[0x79].m_operation = &CPU6502::ADC;
    
    m_Instructions[0x7D].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absX;
    m_Instructions[0x7D].m_operation = &CPU6502::ADC;
    
    m_Instructions[0x7E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0x7E].m_operation = &CPU6502::RMW_ROR;
    
    m_Instructions[0x81].m_opOrAddrMode = &CPU6502::Store_indX;
    m_Instructions[0x81].m_operation = &CPU6502::STA;
    
    m_Instructions[0x84].m_opOrAddrMode = &CPU6502::Store_zpg;
    m_Instructions[0x84].m_operation = &CPU6502::STY;
    
    m_Instructions[0x85].m_opOrAddrMode = &CPU6502::Store_zpg;
    m_Instructions[0x85].m_operation = &CPU6502::STA;
    
    m_Instructions[0x86].m_opOrAddrMode = &CPU6502::Store_zpg;
    m_Instructions[0x86].m_operation = &CPU6502::STX;
    
    m_Instructions[0x88].m_opOrAddrMode = &CPU6502::DEY;
    m_Instructions[0x88].m_operation = nullptr;
    
    m_Instructions[0x8A].m_opOrAddrMode = &CPU6502::TXA;
    m_Instructions[0x8A].m_operation = nullptr;
    
    m_Instructions[0x8C].m_opOrAddrMode = &CPU6502::Store_abs;
    m_Instructions[0x8C].m_operation = &CPU6502::STY;
    
    m_Instructions[0x8D].m_opOrAddrMode = &CPU6502::Store_abs;
    m_Instructions[0x8D].m_operation = &CPU6502::STA;
    
    m_Instructions[0x8E].m_opOrAddrMode = &CPU6502::Store_abs;
    m_Instructions[0x8E].m_operation = &CPU6502::STX;
    
    m_Instructions[0x90].m_opOrAddrMode = &CPU6502::Branch;
    m_Instructions[0x90].m_operation = &CPU6502::BCC;
    
    m_Instructions[0x91].m_opOrAddrMode = &CPU6502::Store_indY;
    m_Instructions[0x91].m_operation = &CPU6502::STA;
    
    m_Instructions[0x94].m_opOrAddrMode = &CPU6502::Store_zpgX;
    m_Instructions[0x94].m_operation = &CPU6502::STY;
    
    m_Instructions[0x95].m_opOrAddrMode = &CPU6502::Store_zpgX;
    m_Instructions[0x95].m_operation = &CPU6502::STA;
    
    m_Instructions[0x96].m_opOrAddrMode = &CPU6502::Store_zpgY;
    m_Instructions[0x96].m_operation = &CPU6502::STX;
    
    m_Instructions[0x98].m_opOrAddrMode = &CPU6502::TYA;
    m_Instructions[0x98].m_operation = nullptr;
    
    m_Instructions[0x99].m_opOrAddrMode = &CPU6502::Store_absY;
    m_Instructions[0x99].m_operation = &CPU6502::STA;
    
    m_Instructions[0x9A].m_opOrAddrMode = &CPU6502::TXS;
    m_Instructions[0x9A].m_operation = nullptr;
    
    m_Instructions[0x9D].m_opOrAddrMode = &CPU6502::Store_absX;
    m_Instructions[0x9D].m_operation = &CPU6502::STA;
    
    m_Instructions[0xA0].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0xA0].m_operation = &CPU6502::LDY;
    
    m_Instructions[0xA1].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indX;
    m_Instructions[0xA1].m_operation = &CPU6502::LDA;
    
    m_Instructions[0xA2].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0xA2].m_operation = &CPU6502::LDX;
    
    m_Instructions[0xA4].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0xA4].m_operation = &CPU6502::LDY;
    
    m_Instructions[0xA5].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0xA5].m_operation = &CPU6502::LDA;
    
    m_Instructions[0xA6].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0xA6].m_operation = &CPU6502::LDX;
    
    m_Instructions[0xA8].m_opOrAddrMode = &CPU6502::TAY;
    m_Instructions[0xA8].m_operation = nullptr;
    
    m_Instructions[0xA9].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0xA9].m_operation = &CPU6502::LDA;
    
    m_Instructions[0xAA].m_opOrAddrMode = &CPU6502::TAX;
    m_Instructions[0xAA].m_operation = nullptr;
    
    m_Instructions[0xAC].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0xAC].m_operation = &CPU6502::LDY;

    m_Instructions[0xAD].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0xAD].m_operation = &CPU6502::LDA;
    
    m_Instructions[0xAE].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0xAE].m_operation = &CPU6502::LDX;
    
    m_Instructions[0xB0].m_opOrAddrMode = &CPU6502::Branch;
    m_Instructions[0xB0].m_operation = &CPU6502::BCS;
    
    m_Instructions[0xB1].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indY;
    m_Instructions[0xB1].m_operation = &CPU6502::LDA;
    
    m_Instructions[0xB4].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgX;
    m_Instructions[0xB4].m_operation = &CPU6502::LDY;
    
    m_Instructions[0xB5].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgX;
    m_Instructions[0xB5].m_operation = &CPU6502::LDA;
    
    m_Instructions[0xB6].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgY;
    m_Instructions[0xB6].m_operation = &CPU6502::LDX;
    
    m_Instructions[0xB8].m_opOrAddrMode = &CPU6502::CLV;
    m_Instructions[0xB8].m_operation = nullptr;
    
    m_Instructions[0xB9].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absY;
    m_Instructions[0xB9].m_operation = &CPU6502::LDA;
    
    m_Instructions[0xBA].m_opOrAddrMode = &CPU6502::TSX;
    m_Instructions[0xBA].m_operation = nullptr;
    
    m_Instructions[0xBC].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absX;
    m_Instructions[0xBC].m_operation = &CPU6502::LDY;
    
    m_Instructions[0xBD].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absX;
    m_Instructions[0xBD].m_operation = &CPU6502::LDA;
    
    m_Instructions[0xBE].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absY;
    m_Instructions[0xBE].m_operation = &CPU6502::LDX;
    
    m_Instructions[0xC0].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0xC0].m_operation = &CPU6502::CPY;
    
    m_Instructions[0xC1].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indX;
    m_Instructions[0xC1].m_operation = &CPU6502::CMP;
    
    m_Instructions[0xC4].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0xC4].m_operation = &CPU6502::CPY;
    
    m_Instructions[0xC5].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0xC5].m_operation = &CPU6502::CMP;
    
    m_Instructions[0xC6].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0xC6].m_operation = &CPU6502::RMW_DEC;
    
    m_Instructions[0xC8].m_opOrAddrMode = &CPU6502::INY;
    m_Instructions[0xC8].m_operation = nullptr;
    
    m_Instructions[0xC9].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0xC9].m_operation = &CPU6502::CMP;
    
    m_Instructions[0xCA].m_opOrAddrMode = &CPU6502::DEX;
    m_Instructions[0xCA].m_operation = nullptr;
    
    m_Instructions[0xCC].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0xCC].m_operation = &CPU6502::CPY;
    
    m_Instructions[0xCD].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0xCD].m_operation = &CPU6502::CMP;
    
    m_Instructions[0xCE].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0xCE].m_operation = &CPU6502::RMW_DEC;
    
    m_Instructions[0xD0].m_opOrAddrMode = &CPU6502::Branch;
    m_Instructions[0xD0].m_operation = &CPU6502::BNE;
    
    m_Instructions[0xD1].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indY;
    m_Instructions[0xD1].m_operation = &CPU6502::CMP;
    
    m_Instructions[0xD5].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgX;
    m_Instructions[0xD5].m_operation = &CPU6502::CMP;
    
    m_Instructions[0xD6].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0xD6].m_operation = &CPU6502::RMW_DEC;
    
    m_Instructions[0xD8].m_opOrAddrMode = &CPU6502::CLD;
    m_Instructions[0xD8].m_operation = nullptr;
    
    m_Instructions[0xD9].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absY;
    m_Instructions[0xD9].m_operation = &CPU6502::CMP;
    
    m_Instructions[0xDD].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absX;
    m_Instructions[0xDD].m_operation = &CPU6502::CMP;
    
    m_Instructions[0xDE].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0xDE].m_operation = &CPU6502::RMW_DEC;
    
    m_Instructions[0xE0].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0xE0].m_operation = &CPU6502::CPX;
    
    m_Instructions[0xE1].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indX;
    m_Instructions[0xE1].m_operation = &CPU6502::SBC;
    
    m_Instructions[0xE4].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0xE4].m_operation = &CPU6502::CPX;
    
    m_Instructions[0xE5].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpg;
    m_Instructions[0xE5].m_operation = &CPU6502::SBC;
    
    m_Instructions[0xE9].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_imm;
    m_Instructions[0xE9].m_operation = &CPU6502::SBC;
    
    m_Instructions[0xEA].m_opOrAddrMode = &CPU6502::NOP;
    m_Instructions[0xEA].m_operation = nullptr;
    
    m_Instructions[0xEC].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0xEC].m_operation = &CPU6502::CPX;
    
    m_Instructions[0xED].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_abs;
    m_Instructions[0xED].m_operation = &CPU6502::SBC;
    
    m_Instructions[0xE6].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0xE6].m_operation = &CPU6502::RMW_INC;
    
    m_Instructions[0xE8].m_opOrAddrMode = &CPU6502::INX;
    m_Instructions[0xE8].m_operation = nullptr;
    
    m_Instructions[0xEE].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0xEE].m_operation = &CPU6502::RMW_INC;
    
    m_Instructions[0xF0].m_opOrAddrMode = &CPU6502::Branch;
    m_Instructions[0xF0].m_operation = &CPU6502::BEQ;
    
    m_Instructions[0xF1].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_indY;
    m_Instructions[0xF1].m_operation = &CPU6502::SBC;
    
    m_Instructions[0xF5].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_zpgX;
    m_Instructions[0xF5].m_operation = &CPU6502::SBC;
    
    m_Instructions[0xF6].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0xF6].m_operation = &CPU6502::RMW_INC;
    
    m_Instructions[0xF8].m_opOrAddrMode = &CPU6502::SED;
    m_Instructions[0xF8].m_operation = nullptr;
    
    m_Instructions[0xF9].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absY;
    m_Instructions[0xF9].m_operation = &CPU6502::SBC;
    
    m_Instructions[0xFD].m_opOrAddrMode = &CPU6502::InternalExecutionMemory_absX;
    m_Instructions[0xFD].m_operation = &CPU6502::SBC;
    
    m_Instructions[0xFE].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0xFE].m_operation = &CPU6502::RMW_INC;
    
#if DEBUG
    // Duplicate instruction set check up to this point - after here then lots of things are getting nopped out
    for(uint32_t i = 0;i < 256;++i)
    {
        if(m_Instructions[i].m_opOrAddrMode != &CPU6502::HandleError)
        {
            for(uint32_t j = 0;j < 256;++j)
            {
                if(i != j)
                {
                    if( m_Instructions[i].m_opOrAddrMode == m_Instructions[j].m_opOrAddrMode &&
                        m_Instructions[i].m_operation == m_Instructions[j].m_operation )
                    {
                        printf("6502 CPU Startup check: Duplicate instruction found %2X vs %2X!!!", i, j);
                        *(volatile char*)(0) = 'I' | 'N' | 'S'| 'T'| 'R'| 'U'| 'C'| 'T'| 'I'| 'O'| 'N';
                    }
                }
            }
        }
    }
#endif

    // Those extra NOPs across address modes
    m_Instructions[0x1A].m_opOrAddrMode = &CPU6502::NOP_IMPLIED_1_2;
    m_Instructions[0x3A].m_opOrAddrMode = &CPU6502::NOP_IMPLIED_1_2;
    m_Instructions[0x5A].m_opOrAddrMode = &CPU6502::NOP_IMPLIED_1_2;
    m_Instructions[0x7A].m_opOrAddrMode = &CPU6502::NOP_IMPLIED_1_2;
    m_Instructions[0xDA].m_opOrAddrMode = &CPU6502::NOP_IMPLIED_1_2;
    m_Instructions[0xFA].m_opOrAddrMode = &CPU6502::NOP_IMPLIED_1_2;

    m_Instructions[0x80].m_opOrAddrMode = &CPU6502::NOP_IMMEDIATE_2_2;
    m_Instructions[0x82].m_opOrAddrMode = &CPU6502::NOP_IMMEDIATE_2_2;
    m_Instructions[0x89].m_opOrAddrMode = &CPU6502::NOP_IMMEDIATE_2_2;
    m_Instructions[0xC2].m_opOrAddrMode = &CPU6502::NOP_IMMEDIATE_2_2;
    m_Instructions[0xE2].m_opOrAddrMode = &CPU6502::NOP_IMMEDIATE_2_2;
    
    m_Instructions[0x04].m_opOrAddrMode = &CPU6502::NOP_ZEROPAGE_2_3;
    m_Instructions[0x44].m_opOrAddrMode = &CPU6502::NOP_ZEROPAGE_2_3;
    m_Instructions[0x64].m_opOrAddrMode = &CPU6502::NOP_ZEROPAGE_2_3;

    m_Instructions[0x14].m_opOrAddrMode = &CPU6502::NOP_ZEROPAGE_X_2_4;
    m_Instructions[0x34].m_opOrAddrMode = &CPU6502::NOP_ZEROPAGE_X_2_4;
    m_Instructions[0x54].m_opOrAddrMode = &CPU6502::NOP_ZEROPAGE_X_2_4;
    m_Instructions[0x74].m_opOrAddrMode = &CPU6502::NOP_ZEROPAGE_X_2_4;
    m_Instructions[0xD4].m_opOrAddrMode = &CPU6502::NOP_ZEROPAGE_X_2_4;
    m_Instructions[0xF4].m_opOrAddrMode = &CPU6502::NOP_ZEROPAGE_X_2_4;

    m_Instructions[0x0C].m_opOrAddrMode = &CPU6502::NOP_ABSOLUTE_3_4;
    
    m_Instructions[0x1C].m_opOrAddrMode = &CPU6502::NOP_ABSOLUTE_X_3_4_1;
    m_Instructions[0x3C].m_opOrAddrMode = &CPU6502::NOP_ABSOLUTE_X_3_4_1;
    m_Instructions[0x5C].m_opOrAddrMode = &CPU6502::NOP_ABSOLUTE_X_3_4_1;
    m_Instructions[0x7C].m_opOrAddrMode = &CPU6502::NOP_ABSOLUTE_X_3_4_1;
    m_Instructions[0xDC].m_opOrAddrMode = &CPU6502::NOP_ABSOLUTE_X_3_4_1;
    m_Instructions[0xFC].m_opOrAddrMode = &CPU6502::NOP_ABSOLUTE_X_3_4_1;
}
