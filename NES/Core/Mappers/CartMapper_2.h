//
//  CartMapper_2.h
//  NES
//
//  Created by Richard Wallis on 13/12/2022.
//

#ifndef CartMapper_2_h
#define CartMapper_2_h

#include "CartMapperFactory.h"

class CartMapper_2 : public Mapper
{
public:
    BUS_HEADER_DECL
    MAPPER_HEADER_DECL
private:
    uint8_t m_prgBankSelect;
};

#endif /* CartMapper_2_h */
