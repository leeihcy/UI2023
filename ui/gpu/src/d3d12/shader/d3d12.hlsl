/*
寄存器类型和含义
| 标识符 | 类型 | 全称 | 用途 |
| -     | -    |  -   |  -  |
| t#    | Shader Resource View (SRV)  | Texture Register         | 纹理、缓冲区等只读资源 |
| s#    | Sampler State	              | Sampler Register         | 采样器状态，控制纹理采样方式 |
| b#    | Constant Buffer View (CBV)  | Constant Buffer Register | 常量缓冲区，存储着色器常量 |
| u#    | Unordered Access View (UAV) | UAV Register             | 读写资源，用于计算着色器 |
*/


struct VSInput
{
    float2 position : POSITION;
    float3 color : COLOR;
    float2 texcoord : TEXCOORD;

    // 实例数据（槽1）
    float2 offset : OFFSET;

    // 系统值，自动提供
    uint instanceId : SV_InstanceID;  
};

cbuffer LayerData : register(b1)
{
    float4x4 model;
};

cbuffer FrameData : register(b0)
{
    float4x4 view;
    float4x4 ortho;
};


struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;

    // 透传给 PS
    uint instanceId : INSTANCEID;
};

// 坑1：
// hlsl中的 * 并不是标准的矩阵乘法，而是逐向量乘法。矩阵相乘请使用mul函数。
// 逐向量乘法即使是乘以一下单位矩阵，得到的结果和原矩阵也不相等。
// ortho * Identity = [1*1, 2*0, 3*0, 4*0] = [1, 0, 0, 0]
//                    [5*0, 6*1, 7*0, 8*0]   [0, 6, 0, 0]
//                    [9*0,10*0,11*1,12*0]   [0, 0,11, 0]
//                    [13*0,14*0,15*0,16*1]  [0, 0, 0,16]
//
PSInput VSMain(VSInput input)
{
    PSInput result;
    float4 pos = float4(input.position + input.offset, 0, 1);

    result.position = mul(pos, mul(model, mul(view, ortho)));
    result.color = float4(input.color, 1.0);
    result.texcoord = input.texcoord;
    result.instanceId = input.instanceId;
    return result;
}

// 纹理数组。一个Layer的所有tile纹理，通过instance id进行索引绘制。
// (t0, space1) 对应 Vulkan 的 set=1, binding=0
Texture2DArray texSamplerArray : register(t0);
SamplerState texSamplerArray_sampler : register(s0);

float4 PSMain(PSInput input) : SV_TARGET
{
    // 注意：HLSL 的纹理坐标通常假设(0,0)在左上角
    // 如果需要与 Vulkan 的坐标系统一致（(0,0)在左下角），可能需要翻转Y坐标
    // float2 texCoord = float2(input.fragTexCoord.x, 1.0 - input.fragTexCoord.y);

    // return input.color;
    return texSamplerArray.Sample(
        texSamplerArray_sampler, 
        float3(input.texcoord, input.instanceId)
    );
}