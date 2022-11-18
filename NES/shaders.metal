//
//  test_shader.metal
//  MetalTest
//
//  Created by richardwallis on 15/11/2016.
//  Copyright © 2016 openplanetsoftware. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

#include "RenderDefs.h"

struct VertexClip
{
    float4 vFragClipPosition [[position]];      //mark as the clip space position output
    float2 vFragTexCoords;
};

vertex VertexClip vertPassThrough(constant Vertex* vertexArray [[ buffer(0) ]],
                                             unsigned int vid [[ vertex_id ]])
{
    VertexClip clip;
    
    clip.vFragClipPosition = vertexArray[vid].vPosition;
    clip.vFragTexCoords    = vertexArray[vid].vTexCoords;

    return clip;
}

fragment float4 fragCopy(VertexClip vertexClip [[stage_in]],
                        texture2d<float,access::sample> srcTexture [[texture(0)]])
{
    sampler s(coord::normalized, address::clamp_to_edge, filter::nearest);
    float4 colour = srcTexture.sample(s, vertexClip.vFragTexCoords);
    return colour;
}
