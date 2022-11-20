//
//  CPU6502_InstructionDefTable.cpp
//  NES
//
//  Created by richardwallis on 20/11/2022.
//  Copyright © 2022 openplanetsoftware. All rights reserved.
//

#include "CPU6502.h"
#include <stdio.h>

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
    
    m_Instructions[0x66].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpg;
    m_Instructions[0x66].m_operation = &CPU6502::RMW_ROR;
    m_Instructions[0x66].m_opStr = "ROR";
    m_Instructions[0x66].m_opAddressModeStr = "zpg";
    m_Instructions[0x66].m_cycles = 5;
    
    m_Instructions[0x6A].m_opOrAddrMode = &CPU6502::Accum_ROR;
    m_Instructions[0x6A].m_operation = nullptr;
    m_Instructions[0x6A].m_opStr = "ROR";
    m_Instructions[0x6A].m_opAddressModeStr = "a";
    m_Instructions[0x6A].m_cycles = 2;
    
    m_Instructions[0x6E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_abs;
    m_Instructions[0x6E].m_operation = &CPU6502::RMW_ROR;
    m_Instructions[0x6E].m_opStr = "ROR";
    m_Instructions[0x6E].m_opAddressModeStr = "abs";
    m_Instructions[0x6E].m_cycles = 6;
    
    m_Instructions[0x76].m_opOrAddrMode = &CPU6502::ReadModifyWrite_zpgX;
    m_Instructions[0x76].m_operation = &CPU6502::RMW_ROR;
    m_Instructions[0x76].m_opStr = "ROR";
    m_Instructions[0x76].m_opAddressModeStr = "zpg,X";
    m_Instructions[0x76].m_cycles = 6;
    
    m_Instructions[0x78].m_opOrAddrMode = &CPU6502::SEI;
    m_Instructions[0x78].m_operation = nullptr;
    m_Instructions[0x78].m_opStr = "SEI";
    m_Instructions[0x78].m_opAddressModeStr = "";
    m_Instructions[0x78].m_cycles = 2;
    
    m_Instructions[0x7E].m_opOrAddrMode = &CPU6502::ReadModifyWrite_absX;
    m_Instructions[0x7E].m_operation = &CPU6502::RMW_ROR;
    m_Instructions[0x7E].m_opStr = "ROR";
    m_Instructions[0x7E].m_opAddressModeStr = "abs,X";
    m_Instructions[0x7E].m_cycles = 7;
    
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
