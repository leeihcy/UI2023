#ifndef _UI_GPU_SRC_METAL2_SHADER_SHADERTYPES_H_
#define _UI_GPU_SRC_METAL2_SHADER_SHADERTYPES_H_

// #include <simd/simd.h>
// 
// typedef enum AAPLVertexInputIndex
// {
//     AAPLVertexInputIndexVertices    = 0,
//     AAPLVertexInputIndexAspectRatio = 1,
// } AAPLVertexInputIndex;
// 
// typedef enum AAPLTextureInputIndex
// {
//     AAPLTextureInputIndexColor = 0,
// } AAPLTextureInputIndex;

typedef struct
{
    vector_float2 position;
    vector_float4 color;
} SimpleVertex;

typedef struct
{
    vector_float2 position;
    vector_float2 texcoord;
} TextureVertex;

#endif
