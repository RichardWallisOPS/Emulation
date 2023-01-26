//
//  CartMapper_3.h
//  NES
//
//  Created by Richard Wallis on 17/12/2022.
//

#ifndef CartMapper_3_h
#define CartMapper_3_h

#include "CartMapperFactory.h"

class CartMapper_3 : public Mapper
{
public:
    BUS_HEADER_DECL
    MAPPER_HEADER_DECL
    SERIALISABLE_DECL
private:
    uint8_t m_chrBankSelect;
};

#endif /* CartMapper_3_h */
