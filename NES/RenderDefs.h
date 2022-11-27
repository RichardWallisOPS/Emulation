//
//  RenderDefs.h
//
//  Created by Richard Wallis on 26/02/2022
//

#if defined(__METAL_MACOS__) || defined(__METAL_IOS__)
#define COMPILER_GPU_METAL
#else
#define COMPILER_CPU_OBJC
#endif


// CPU side - Define simd types to map to Metal so we can use data structs across both CPU and GPU
// Types should pack correctly
#ifdef COMPILER_CPU_OBJC
    #import "MetalKit/MetalKit.h"       // @import MetalKit;
    typedef simd_float2 float2;
    typedef simd_float3 float3;
    typedef simd_float4 float4;
    typedef simd_float4x4 float4x4;
#endif

typedef struct
{
    float4 vPosition;       // local space
    float2 vTexCoords;		// local space
} Vertex;
