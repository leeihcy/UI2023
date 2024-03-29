#include "sdk/include/inc.h"
#include "UISDK\Kernel\Inc\Util\math\rect.h"

using namespace ui;

RectF::RectF()
{
	left = top = right = bottom = 0;
}
RectF::RectF(const Rect* prc)
{
	Set(prc);
}

void  RectF::Set(const Rect* prc)
{
	left = (float)prc->left;
	top = (float)prc->top;
	right = (float)prc->right;
	bottom = (float)prc->bottom;
}

void  RectF::CopyFrom(LPRECTF pScr)
{
	left = pScr->left;
	top = pScr->top;
	right = pScr->right;
	bottom = pScr->bottom;
}
void  RectF::Set(float _fLeft, float _fTop, float _fRight, float _fBottom)
{
	left = _fLeft;
	top = _fTop;
	right = _fRight;
	bottom = _fBottom;
}
void  RectF::Set(int _Left, int _Top, int _Right, int _Bottom)
{
	left = (float)_Left;
	top = (float)_Top;
	right = (float)_Right;
	bottom = (float)_Bottom;
}