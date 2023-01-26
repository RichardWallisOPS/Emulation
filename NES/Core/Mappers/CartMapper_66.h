//
//  CartMapper_66.h
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
    BUS_HEADER_DECL
    MAPPER_HEADER_DECL
    SERIALISABLE_DECL
private:
    uint8_t m_bankSelect;
};

#endif /* CartMapper_66_h */
