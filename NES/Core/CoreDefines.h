//
//  CoreDefines.h
//  NES
//
//  Created by Richard Wallis on 13/02/2023.
//

#ifndef CoreDefines_h
#define CoreDefines_h

#ifdef __cplusplus
    #include <cstdint>
    #include <string>
#endif

enum MirrorMode : uint8_t
{
    VRAM_MIRROR_H = 0,
    VRAM_MIRROR_V,
    VRAM_MIRROR_CART4,
    VRAM_MIRROR_SINGLEA,
    VRAM_MIRROR_SINGLEB
};

enum ControllerButton : uint8_t
{
    Controller_Right    = 7,
    Controller_Left     = 6,
    Controller_Down     = 5,
    Controller_Up       = 4,
    Controller_Start    = 3,
    Controller_Select   = 2,
    Controller_B        = 1,
    Controller_A        = 0
};

#endif /* CoreDefines_h */
