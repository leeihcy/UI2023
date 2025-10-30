# UI2023
现代图形UI架构学习。

## sdk

1. Window对象不再和控件绑定，将两者分拆开 (done)
   
  `<window width height>` 如何决定这两个属性是指带边框的窗口大小，还是窗口里的内容大小(panel)。
   见：Window::onGetDesiredSize
   增加root panel。窗口层面使用像素大小，内部控件、渲染都使用设备无关坐标。

2. SDK中实现一些核心组件，之后只需要通过组合的方式，实现自定义组件 （doing）
   文本 + 按钮 + 列表 + 消息冒泡分发

## Render Thread
   优化中

## vulkan
1. 引入 chromium swiftshader进行调试
2. gpu layer tile policy
3. 内存占用为什么这么大？

## DPI
   全平台DPI逻辑实现
   仅窗口创建、窗口渲染使用px单位，控件、skia都使用dip单位。
   SkiaRenderTarget::BeginDraw 中调用： 
   canvas->scale(scale, scale);
   实现整体的渲染缩放。
 
   TODO: image set. 200% 300% pecent

