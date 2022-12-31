//
//  CartMapper_3.hpp
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
    BUS_HEADER_IMPL
    MAPPER_HEADER_IMPL
private:
    uint8_t m_chrBankSelect;
};

#endif /* CartMapper_3_h */
