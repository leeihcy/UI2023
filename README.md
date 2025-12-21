# UI2023

现代图形UI架构学习。

## SDK

1. Window对象不再和控件绑定，将两者分拆开 (done)
   
  `<window width height>` 如何决定这两个属性是指带边框的窗口大小，还是窗口里的内容大小(panel)。
   见：Window::onGetDesiredSize
   增加root panel。窗口层面使用像素大小，内部控件、渲染都使用设备无关坐标。

2. SDK中实现一些核心组件，之后只需要通过组合的方式，实现自定义组件 （doing）
   文本 + 按钮 + 列表 + 消息冒泡分发
   
   TODO: List

3. TODO: 光栅化 直接对接 vulkan 后端，优化纹理上传。 
   Image Raster，将数据保存在CPU、GPU都能访问的区域，规避了纹理上传。也叫 Zero Copy Texture Upload.
   Image Copy Raster，（Staging Copy）将数据保存在CPU、GPU都能访问的区域（但这块区域资源有限），然后再拷贝给GPU专用区域。也叫 One Copy Texture Upload。
   Pixel Buffer Raster，由内存上传到显存。
   Direct Raster，由OpenGL直接执行光栅化，直接得到GPU纹理。

## Render Thread

   优化中
   TODO：指令优化过程中，还需处理脏区域合并的场景

## Compositor Thread

   TODO: 

## vulkan/d3d/metal

1. 调试
   a. xcode可以调试查看metal渲染流程。
2. gpu layer tile policy 
   TODO： 按需渲染tile，实现平滑滚动！在资源不足时进行tile回收。
3. 内存占用优化
4. CPU占用优化

## Wayland / X11


## DPI

   全平台DPI逻辑实现
   仅窗口创建、窗口渲染使用px单位，控件、skia都使用dip单位。
   SkiaRenderTarget::BeginDraw 中调用： 
   canvas->scale(scale, scale);
   实现整体的渲染缩放。
 
   TODO: image set. 200% 300% pecent

TODO: 接入googletest单元测试框架。