//
//  CartMapper_9.h
//  NES
//
//  Created by Richard Wallis on 13/01/2023.
//

#ifndef CartMapper_9_h
#define CartMapper_9_h

#include "CartMapperFactory.h"

class CartMapper_9 : public Mapper
{
public:
    BUS_HEADER_DECL
    MAPPER_HEADER_DECL
    SERIALISABLE_DECL
private:

    uint8_t m_prgBankSelect;
    uint8_t m_chrBankSelect0;
    uint8_t m_chrBankSelect1;
    uint8_t m_chrBankSelect2;
    uint8_t m_chrBankSelect3;
    uint8_t m_latch0;
    uint8_t m_latch1;
};

#endif /* CartMapper_9_h */
