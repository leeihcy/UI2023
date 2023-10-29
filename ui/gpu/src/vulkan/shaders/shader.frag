#version 450

//
// 1. 每个fragment执行一次，不是一个vertex执行一次。
//    好像是每个像素执行一次，可以控制每个像素的值
// 2. fragColor是已经基于三个顶点的颜色计算好的一个插件颜色，怎么理解？
// 3. Rasterrization，就是将三个角离散化成每一个具体的像素点。

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}