//
//  CartMapper_152.hpp
//  NES
//
//  Created by Richard Wallis on 24/02/2023.
//

#ifndef CartMapper_152_h
#define CartMapper_152_h

#include "CartMapperFactory.h"

class CartMapper_152 : public Mapper
{
public:
    BUS_HEADER_DECL
    MAPPER_HEADER_DECL
    SERIALISABLE_DECL
private:

    uint8_t m_prgBank;
    uint8_t m_chrBank;
};

#endif /* CartMapper_152_h */
