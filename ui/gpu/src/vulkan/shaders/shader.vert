#version 450

// 1. 每个文件都包含一个main函数
// 2. GLSL不使用输入输出参数，而是使用全局变量传递数据
// 3. vec3(1.0, 2.0, 3.0).xy ==》 in vec2.
// 4. vec3(vec2(1.0, 2.0), 3.0).
// 5. 坐标系：左上角（-1，-1），右下角（1，1），中心（0，0）
// 6. 内置变量gl_VertexIndex：main函数会为每一个顶点执行一次，gl_VertexIndex代表当前顶点索引
// 7. 内置变量gl_Position：作为输出变量传给下一步。但不需要给frag，frag步骤不需要
//    因此没必要再单独加一个layout location 0 out vec4 position。
//    但是frag步骤没有内置的gl_Position变量，需要自己再加一个out输出color

// 每帧更新一次。
layout(binding = 0) uniform UniformBufferObject {
   mat4 view;
   // mat4 proj;
   mat4 ortho;
} ubo;

// 极小且高频更新的数据
layout(push_constant) uniform PushData {
  // 每个对象的转换矩阵
  mat4 model;
} push;

// 顶点缓存数据，在Pipeline::build_vertex_input中定义
layout(location = 0) in vec2 inPosition;  // 局部坐标(像素坐标)
layout(location = 1) in vec3 inColor;     //
layout(location = 2) in vec2 inTexCoord;  // 对象的tile纹理坐标(0~1)

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
  gl_Position = ubo.ortho * ubo.view * push.model * vec4(
    inPosition, 0.0, 1.0);

  fragColor = inColor;
  fragTexCoord = inTexCoord;
}