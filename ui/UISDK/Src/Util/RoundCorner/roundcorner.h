#pragma once

// 处理一张图片的四个角，实现圆角化
// 例如窗口四个角，头像四个角

namespace UI
{

void  RoundCorner_WindowBitmap(HBITMAP hTopDownBitmap, RECT* prc, REGION4* pRadius);
void  RoundCorner_Portrait(HBITMAP hTopDownBitmap, RECT* prc);

}
