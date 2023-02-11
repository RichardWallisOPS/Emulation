//
//  CartMapper_23.h
//  NES
//
//  Created by Richard Wallis on 11/02/2023.
//

#ifndef CartMapper_23_h
#define CartMapper_23_h

#include "CartMapperFactory.h"

class CartMapper_23 : public Mapper
{
public:
    BUS_HEADER_DECL
    MAPPER_HEADER_DECL
    SERIALISABLE_DECL
    
private:

    void SetChrBank(uint8_t** pChrBank, uint8_t bank);
        
private:

    uint8_t* m_prgBank0;
    uint8_t* m_prgBank1;
    uint8_t* m_prgBank2;
    
    uint8_t* m_chrBank0;
    uint8_t* m_chrBank1;
    uint8_t* m_chrBank2;
    uint8_t* m_chrBank3;
    uint8_t* m_chrBank4;
    uint8_t* m_chrBank5;
    uint8_t* m_chrBank6;
    uint8_t* m_chrBank7;
    
    uint8_t m_microWireLatch;
    
    uint8_t m_regChrBank0;
    uint8_t m_regChrBank1;
    uint8_t m_regChrBank2;
    uint8_t m_regChrBank3;
    uint8_t m_regChrBank4;
    uint8_t m_regChrBank5;
    uint8_t m_regChrBank6;
    uint8_t m_regChrBank7;
};

#endif /* CartMapper_23_h */
