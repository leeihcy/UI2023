#version 450

// 纹理数组。一个Layer的所有tile纹理，通过instance id进行索引绘制。
layout(set=1, binding=0) uniform sampler2DArray texSamplerArray;

// 从 vertex shader 中传递过来的参数
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in flat uint instanceId;

// 输出结果：
layout(location = 0) out vec4 outColor;

void main() {
  outColor = texture(texSamplerArray, vec3(fragTexCoord, float(instanceId)));
}