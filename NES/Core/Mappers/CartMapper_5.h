//
//  CartMapper_5.h
//  NES
//
//  Created by Richard Wallis on 20/01/2023.
//
//  NOTE: Not all features of this mapper are implemented (Too many to list - you may get crashes/unexpected behaviour if they are used)

#ifndef CartMapper_5_h
#define CartMapper_5_h

#include "CartMapperFactory.h"

class CartMapper_5 : public Mapper
{
public:
    BUS_HEADER_DECL
    MAPPER_HEADER_DECL
    SERIALISABLE_DECL
    
private:
};

#endif /* CartMapper_5_h */
