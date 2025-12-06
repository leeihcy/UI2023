#ifndef _UI_GPU_SRC_D3D12_SHADER_SHADERTYPES_H_
#define _UI_GPU_SRC_D3D12_SHADER_SHADERTYPES_H_

#include "HlslCompat.h"

namespace d3d12 {

// 每个Tile的顶点
struct VertexData {
  float2 position;
  float3 color;
  float2 texcoord;
};

// Layer的每个分块
struct TileData {
  float2 offset; // _in_layer;
};

// 每个Layer更新一次的数据
struct LayerData {
  float4x4 model;
};

// 每帧更新一次的数据。
#define SHADER_REGISTER_INDEX_CONSTAT_BUFFER 0
struct FrameData {
  DirectX::XMMATRIX view;
  DirectX::XMMATRIX ortho;
};

} // namespace d3d12

#endif