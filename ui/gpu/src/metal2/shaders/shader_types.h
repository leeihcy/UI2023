#ifndef _UI_GPU_SRC_METAL2_SHADERS_SHADERTYPES_H_
#define _UI_GPU_SRC_METAL2_SHADERS_SHADERTYPES_H_

#include <simd/matrix_types.h>
#include <simd/simd.h>

enum class VertexShaderInput : int {
  VertexData = 0,
  TileData = 1,
  LayerData = 2,
  FrameData = 3,
};

enum class FragmentShaderInput : int {
  Texture = 0,
};

// 每个Tile的顶点
struct VertexData {
  vector_float2 position;
  vector_float3 color;
  vector_float2 texcoord;
};

// Layer的每个分块
struct TileData {
  vector_float2 offset; // _in_layer;
};

// 每个Layer更新一次的数据
struct LayerData {
  matrix_float4x4 model;
};

// 每帧更新一次的数据。
struct FrameData {
  matrix_float4x4 view;
  matrix_float4x4 ortho;
};


#endif
