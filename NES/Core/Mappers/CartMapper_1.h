//
//  CartMapper_1.h
//  NES
//
//  Created by Richard Wallis on 15/12/2022.
//

#ifndef CartMapper_1_h
#define CartMapper_1_h

#include "CartMapperFactory.h"

class CartMapper_1 : public Mapper
{
public:
    BUS_HEADER_DECL
    MAPPER_HEADER_DECL
private:

    uint8_t m_shiftRegister;
    uint8_t m_shiftCount;
    uint8_t m_ctrl;
    uint8_t m_chrBank0;
    uint8_t m_chrBank1;
    uint8_t m_prgBank;
};

#endif /* CartMapper_1_h */
