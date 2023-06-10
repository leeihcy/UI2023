#pragma once

#define DEFAULT_SCREEN_DPI  96   // USER_DEFAULT_SCREEN_DPI

//
// DPI 改变通知： Windows 8.1才有效
// Windows 7 中修改DPI后，系统会要求注销才能生效。
//
// wParam:
//   HIWORD:  Y DPI
//   LOWORD:  X DPI
// lParam:
//   窗口期望的新位置
//   RECT* const prcNewWindow = (RECT*)lParam;
//#define WM_DPICHANGED      0x02E0
//

// 应该使用 GetSystemMetrics(SM_CXSCREEN) 或 GetSystemMetrics(SM_CXSCREEN) 来获取屏幕大小尺寸。
// 应该使用 GetSystemMetrics(SM_CXBORDER) 或 GetSystemMetrics(SM_CYBORDER) 来获取边框尺寸。
// 应该使用 GetSystemMetrics(SM_CXICON) 或 GetSystemMetrics(SM_CXSMICON) 来获取大图标尺寸和小图标尺寸。


//
// 图片的DPI自适应缩放目前想到两种方式，
// 1. 在加载图片时，就将图片直接缩放到相应尺寸
//    优点：
//          1.简单，只需要源头处理即可，不影响绘制代码.
//          2.能支持像Button的autosize by bkgimage w/h的场景，
//          3.使用GdiPlus来支持拉伸时抗锯齿功能
//    缺点：
//          1.占用更多的内存
//          2.对于imagelist类型支持不了。
//            比如一个154*14的imagelist，里面的item为14*14
//            放大125%之后宽度变成了 154*1.25 = 192.5， 14*1.25 = 17.5
//            此时无论是将item width设成17还是18都是不对的，没法平均分配了.
//            而且在采用抗锯齿时，第二个item会影响第1个item的边缘，当第1个
//            item的边缘本来是纯透明时受到的影响非常明显
//            -->解决方案：
//             . 将item分离出来，单独加载。
//             . 不使用DPI拉伸，直接9宫拉伸
//             . 把imagelist分段DPI拉伸，每个item只能记索引，不能记坐标区域，因为坐标区域乘缩放系数之后又成小数了。
//
// 2. 在绘制的时候，修改目标宽度和高度，达到缩放的效果
//    重要缺点：GDI AlphaBlend拉伸不支持抗锯齿
//
namespace UI
{
    long  ScaleByDpi_if_gt0(long x);
    long  RestoreByDpi(long x);
    long  RestoreByDpi_if_gt0(long x);
	float  GetDpiScale();
}