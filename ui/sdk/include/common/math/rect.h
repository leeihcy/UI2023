#ifndef _UI_MATH_RECT_H_
#define _UI_MATH_RECT_H_
#include "sdk/include/util/rect.h"

namespace ui
{

typedef struct tagRECTF
{
	float  left;
	float  top;
	float  right;
	float  bottom;
}
RECTF, *LPRECTF;

class RectF : public RECTF
{
public:
//  RectF();
// 	RectF(const Rect* prc);
// 
//  void  CopyFrom(LPRECTF pScr);
// 	void  Set(const Rect* prc);
// 	void  Set(float _fLeft, float _fTop, float _fRight, float _fBottom);
// 	void  Set(int _Left, int _Top, int _Right, int _Bottom);

	RectF()
	{
		left = top = right = bottom = 0;
	}
	RectF(const Rect* prc)
	{
		Set(prc);
	}

	void  Set(const Rect* prc)
	{
		left = (float)prc->left;
		top = (float)prc->top;
		right = (float)prc->right;
		bottom = (float)prc->bottom;
	}

	void  CopyFrom(LPRECTF pScr)
	{
		left = pScr->left;
		top = pScr->top;
		right = pScr->right;
		bottom = pScr->bottom;
	}
	void  Set(float _fLeft, float _fTop, float _fRight, float _fBottom)
	{
		left = _fLeft;
		top = _fTop;
		right = _fRight;
		bottom = _fBottom;
	}
	void  Set(int _Left, int _Top, int _Right, int _Bottom)
	{
		left = (float)_Left;
		top = (float)_Top;
		right = (float)_Right;
		bottom = (float)_Bottom;
	}
};



}

#endif 