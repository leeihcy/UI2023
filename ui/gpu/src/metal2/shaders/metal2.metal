/*
Metal Shader 语言基于 C++ 14，并在此基础上进行了大量的扩展和限制。
每个 Metal Shader 都是一个函数，使用特定的关键字来声明其类型。
关键字
  vertex: 声明一个顶点着色器函数。
  fragment: 声明一个片段着色器函数。
  kernel 或 compute: 声明一个并行计算函数。
示例：
  kernel void my_kernel() {
      // 这里是计算逻辑
  }

资源限定符
  device: 表明指针指向的是可读写的设备内存（主内存）。用于缓冲区。
  constant: 表明指针指向的是只读的常量内存。用于小的、被所有线程频繁访问的数据。
  threadgroup: 表明指针指向的是线程组共享的快速内存。仅用于计算着色器，速度极快。
  texture2d<T, access>：表示一个 2D 纹理。T 是数据类型（如 half, float），access 是访问权限（如 read, write, sample）。

索引绑定 [[buffer(n)]], [[texture(n)]]
这是将 Shader 参数与 App 端（Objective-C/Swift）传递过来的资源绑定起来的关键。
  [[buffer(0)]]：绑定到索引为 0 的缓冲区。
  [[texture(1)]]：绑定到索引为 1 的纹理。
  [[sampler(2)]]：绑定到索引为 2 的采样器。


half: 16位半精度浮点数，占用内存：2字节。典型用途：颜色、纹理坐标、法线。
*/

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

// Include header shared between this Metal shader code and C code executing Metal API commands
#include "shader_types.h"

// Vertex shader outputs and per-fragment inputs
struct RasterizerData
{
    float4 pos [[position]];  // 用 [[position]] 标记顶点位置
    float2 texcoord;
    float3 color;
    uint instance_id [[flat]];
};

vertex RasterizerData
vertexShader(uint vid [[ vertex_id ]],
             uint iid [[ instance_id ]],
             device const metal2::VertexData *vertices [[ buffer(0) ]],
             device const metal2::TileData *tiles [[ buffer(1) ]],
             device const metal2::LayerData &layer [[ buffer(2) ]],
             device const metal2::FrameData &frame  [[ buffer(3) ]]
             )
{
    RasterizerData out;
    
    float4 vertex_pos = float4(vertices[vid].position.xy + tiles[iid].offset, 0, 1);
    out.pos = float4(frame.ortho * frame.view * layer.model * vertex_pos);

    out.color = vertices[vid].color;
    out.texcoord = vertices[vid].texcoord;
    out.instance_id = iid;
    return out;
}

fragment half4
fragmentShader(RasterizerData in [[stage_in]],
               texture2d_array< half, access::sample > tex [[texture(0)]])
{
  constexpr sampler s(address::mirrored_repeat, filter::linear);
  return tex.sample( s, in.texcoord, in.instance_id ); // rgba

  // 直接返回顶点着色器传递过来的颜色
  // return half4((half3)in.color, 1.0);  
}


