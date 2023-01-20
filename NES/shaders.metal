//
//
//  Created by richardwallis on 15/11/2022.
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

//
// PUBLIC DOMAIN CRT STYLED SCAN-LINE SHADER
//
//   Original by Timothy Lottes published on ShaderToy
//   Converted to Metal and tweaked a tiny bit!
//
// Square
// 256 x 240 = 768 x 720 = 1024 x 960
// 4:3 Image is notfully 4:3 as there is empty left and right
// 292 x 240 = 876 x 720 = 1168 x 960

constant float hardScan = -6.f;
constant float hardPix = -3.f;

float ToLinear1(float c)
{
    return c <= 0.04045f ? c / 12.92f : pow((c + 0.055f) / 1.055f, 2.4f);
}

float3 ToLinear(float3 c)
{
    return float3(ToLinear1(c.r), ToLinear1(c.g), ToLinear1(c.b));
}

float ToSrgb1(float c)
{
    return (c < 0.0031308f ? c * 12.92:1.055f * pow(c, 0.41666f) -0.055f);
}

float3 ToSrgb(float3 c)
{
    return float3(ToSrgb1(c.r), ToSrgb1(c.g), ToSrgb1(c.b));
}

float3 Fetch(texture2d<float, access::sample> sourceTex, float2 pos, float2 off)
{
    float2 res(sourceTex.get_width(), sourceTex.get_height());
    pos = floor(pos * res + off) / res;
    
    if(max(abs(pos.x - 0.5f), abs(pos.y - 0.5f)) > 0.5f)
    {
        return float3(0.f, 0.f, 0.f);
    }
    
    constexpr sampler s(coord::normalized, filter::nearest);
    return ToLinear(sourceTex.sample(s, pos.xy, 0).rgb);
}

float2 Dist(texture2d<float, access::sample> sourceTex, float2 pos)
{
    float2 res(sourceTex.get_width(), sourceTex.get_height());
    pos = pos * res;
    return -((pos - floor(pos)) - float2(0.5f));
}
    
float Gaus(float pos,float scale)
{
    return exp2(scale*pos*pos);
}

float3 Horz3(texture2d<float, access::sample> sourceTex, float2 pos, float off)
{
    float3 b = Fetch(sourceTex, pos,float2(-1.f,off));
    float3 c = Fetch(sourceTex, pos,float2( 0.f,off));
    float3 d = Fetch(sourceTex, pos,float2( 1.f,off));

    float dst = Dist(sourceTex, pos).x;

    float scale = hardPix;
    float wb = Gaus(dst - 1.f, scale);
    float wc = Gaus(dst + 0.f, scale);
    float wd = Gaus(dst + 1.f, scale);

    return (b * wb + c * wc + d * wd) / (wb + wc + wd);
}

float3 Horz5(texture2d<float, access::sample> sourceTex, float2 pos, float off)
{
    float3 a = Fetch(sourceTex, pos, float2(-2.f, off));
    float3 b = Fetch(sourceTex, pos, float2(-1.f, off));
    float3 c = Fetch(sourceTex, pos, float2( 0.f, off));
    float3 d = Fetch(sourceTex, pos, float2( 1.f, off));
    float3 e = Fetch(sourceTex, pos, float2( 2.f, off));
    
    float dst = Dist(sourceTex, pos).x;
    float scale = hardPix;
    
    float wa = Gaus(dst - 2.f, scale);
    float wb = Gaus(dst - 1.f, scale);
    float wc = Gaus(dst + 0.f, scale);
    float wd = Gaus(dst + 1.f, scale);
    float we = Gaus(dst + 2.f, scale);

    return (a * wa + b * wb + c * wc + d * wd + e * we) / (wa + wb + wc + wd + we);
}

float Scan(texture2d<float, access::sample> sourceTex, float2 pos, float off)
{
    float dst = Dist(sourceTex, pos).y;
    return Gaus(dst + off,hardScan);
}

float3 Tri(texture2d<float, access::sample> sourceTex, float2 pos)
{
    float3 a = Horz3(sourceTex, pos, -1.f);
    float3 b = Horz5(sourceTex, pos, 0.f);
    float3 c = Horz3(sourceTex, pos, 1.f);
    float wa = Scan(sourceTex, pos, -1.f);
    float wb = Scan(sourceTex, pos, 0.f);
    float wc = Scan(sourceTex, pos, 1.f);
    return a * wa + b * wb + c * wc;
}

fragment float4 fragCopy_Monitor(VertexClip vertexClip [[stage_in]], texture2d<float, access::sample> srcTexture [[texture(0)]])
{
    float4 fragColour = float4(Tri(srcTexture, vertexClip.vFragTexCoords), 1.f);
    fragColour.rgb = ToSrgb(fragColour.rgb);
    return fragColour;
}
