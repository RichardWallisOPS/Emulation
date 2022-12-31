//
//  CartMapper_66.hpp
//  NES
//
//  Created by Richard Wallis on 15/12/2022.
//

#ifndef CartMapper_66_h
#define CartMapper_66_h

#include "CartMapperFactory.h"

class CartMapper_66 : public Mapper
{
public:
    BUS_HEADER_IMPL
    MAPPER_HEADER_IMPL
private:
    uint8_t m_bankSelect;
};

#endif /* CartMapper_66_h */
