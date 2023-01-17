//
//  CartMapper_7.h
//  NES
//
//  Created by Richard Wallis on 15/01/2023.
//

#ifndef CartMapper_7_h
#define CartMapper_7_h

#include "CartMapperFactory.h"

class CartMapper_7 : public Mapper
{
public:
    BUS_HEADER_DECL
    MAPPER_HEADER_DECL
    SERIALISABLE_DECL

private:
    uint8_t m_prgBankSelect;
};

#endif /* CartMapper_7_h */
