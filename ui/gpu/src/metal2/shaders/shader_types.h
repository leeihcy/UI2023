#ifndef _UI_GPU_SRC_METAL2_SHADERS_SHADERTYPES_H_
#define _UI_GPU_SRC_METAL2_SHADERS_SHADERTYPES_H_

#include <simd/matrix_types.h>
#include <simd/simd.h>

enum class VertexInputIndex: int
{
    Vertices = 0,
    Uniforms = 1,
};

struct ShaderVertex
{
    vector_float2 position;
    vector_float3 color;
    vector_float2 texcoord;
};
struct Uniforms
{
    matrix_float4x4 view;
    matrix_float4x4 ortho;
    // float scale;
    // vector_uint2 viewportSize;
};


// typedef enum AAPLVertexInputIndex
// {
//     AAPLVertexInputIndexVertices = 0,
//     AAPLVertexInputIndexUniforms = 1,
// } AAPLVertexInputIndex;

// typedef struct
// {
//     // Positions in pixel space (i.e. a value of 100 indicates 100 pixels from the origin/center)
//     vector_float2 position;

//     // 2D texture coordinate
//     vector_float3 color;
// } AAPLVertex;

// typedef struct
// {
//     float scale;
//     vector_uint2 viewportSize;
// } AAPLUniforms;

#endif
