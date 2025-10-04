目前响应mac下窗口绘制效率（窗口大小拉伸平滑测试）的点是：
1. root panel背景颜色填充
2. mac commit 

为了提交效率，开始将渲染部分提到单独的RenderThread中。
同时为窗口配备两个FrameBuffer缓存，back用于渲染线程光栅化，front用于主线程提交到窗口中。
当back绘制完成后，将调用swapchain，切换back/front。

两条触发绘制的路线：
1. 控件主动刷新：
   AddInvalidateRect, RequestInvalidate, InvalidateNow, UpdateDirty, Commit

2. 窗口触发的Paint/drawRect事件：
   UpdateDirty, swapchain前的Commit？, swapchain后的DirectCommit
   

TODO:
skia backend，还有一种方法，是将指令直接转换成GPU API绘制指令。
这种方式能更好的利用GPU强大计算能力，避免软件光栅化需要的大量内存，和上传到GPU时需要的内存带宽。同时能保留绘制的矢量效果。
但在稳定性方面不如软件光栅化。

需要将这两种方法进行结合。