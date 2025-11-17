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

*/

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

// Include header shared between this Metal shader code and C code executing Metal API commands
#include "shader_types.h"

// Vertex shader outputs and per-fragment inputs
struct RasterizerData
{
    float4 clipSpacePosition [[position]];  // 用 [[position]] 标记顶点位置
    float3 color;
};

vertex RasterizerData
vertexShader(uint vertexID [[ vertex_id ]],
             constant ShaderVertex *vertexArray [[ buffer(0) ]],
             constant Uniforms &uniforms  [[ buffer(1) ]])

{
    RasterizerData out;

    float4 position;
    position.xy = vertexArray[vertexID].position.xy;
    position.z = 0;
    position.w = 1;

    out.clipSpacePosition = float4(uniforms.ortho * uniforms.view * position);
    out.color = vertexArray[vertexID].color;
    return out;
}

fragment float4
fragmentShader(RasterizerData in [[stage_in]])
{
    return float4(in.color, 1.0);  // 直接返回顶点着色器传递过来的颜色
}


