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

    // 5x 8KB Program bank- banking can be 8KB+32KB, 8KB+16KB+16KB, 8KB+16KB+8KB+8KB or 5x 8KB
    uint8_t* m_prgBank0;
    uint8_t* m_prgBank1;
    uint8_t* m_prgBank2;
    uint8_t* m_prgBank3;
    uint8_t* m_prgBank4;

    // 8x 1KB Character banks - banking can be 1x 8KB, 2x 4KB, 4x 2KB or 8x 1KB
    uint8_t* m_chrBank0;
    uint8_t* m_chrBank1;
    uint8_t* m_chrBank2;
    uint8_t* m_chrBank3;
    uint8_t* m_chrBank4;
    uint8_t* m_chrBank5;
    uint8_t* m_chrBank6;
    uint8_t* m_chrBank7;

};

#endif /* CartMapper_5_h */
