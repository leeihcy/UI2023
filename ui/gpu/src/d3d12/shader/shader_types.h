#ifndef _UI_GPU_SRC_D3D12_SHADER_SHADERTYPES_H_
#define _UI_GPU_SRC_D3D12_SHADER_SHADERTYPES_H_

#include "HlslCompat.h"

#define ROOT_PARAMETER_FRAMEDATA 0
#define ROOT_PARAMETER_LAYERDATA 1
#define ROOT_PARAMETER_TEXTURE 2
#define ROOT_PARAMETER_SAMPLE 3

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
  DirectX::XMMATRIX model;
};

// 每帧更新一次的数据。
struct FrameData {
  DirectX::XMMATRIX view;
  DirectX::XMMATRIX ortho;
};

} // namespace d3d12

#endif