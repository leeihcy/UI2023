#ifndef _UI_GPU_SRC_D3D12_SHADER_SHADERTYPES_H_
#define _UI_GPU_SRC_D3D12_SHADER_SHADERTYPES_H_

#include "HlslCompat.h"

namespace d3d12 {

struct VertexData {
  float3 position;
  float4 color;
  float2 texcoord;
};

} // namespace d3d12

#endif