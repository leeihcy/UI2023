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
1. 指令合并，解决快速拖拽窗口大小时，RenderThread出现指令堆积的问题。
2. 脏区域优化
3. 其它接口指令转发