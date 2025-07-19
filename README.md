# UI2023
an ui framework can cross platform 

# TODO:
## sdk
2. 全平台DPI逻辑实现
3. MacOS坐标原点逻辑统一
4. Window对象不再和控件绑定，将两者分拆开
   . 用于解决OnSize时，width/height是指窗口大小，还是客户区大小。 
   . 用于窗口合并、拆分逻辑时，控件对象生命周期的快速转移。

## vulkan
1. 引入 chromium swiftshader进行调试
2. gpu layer tile policy

# Done

1. `<window width height>` 如何决定这两个属性是指带边框的窗口大小，还是窗口里的内容大小(panel)。
   见：Window::onGetDesiredSize