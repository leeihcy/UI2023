#pragma once

// 采用webkit的方式生成阴影

// QT shadow blur注释
// https://gitorious.org/ofi-labs/x2/commit/2bb67f86d262ccf20fefccdb29056c092b2398cf?diffmode=sidebyside



namespace ui
{
	//
	// 参数：
	//
	//	hBitmap
	//		[in,out] 必须是32位的top-down位图
	//  colorShadow
	//		[in] 阴影颜色
	//	RECT
	//		[in] HBITMAP上要进行blur的区域。如果为nullptr表示整个图片范围
	//	nRadius
	//		[in] 2-128之间
	//
    void  ShadowBlur(HBITMAP hBitmap, COLORREF colorShadow, RECT* prcBlur, int nRadius);
}