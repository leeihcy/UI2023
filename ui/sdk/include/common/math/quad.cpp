#include "stdafx.h"
#include "quad.h"


void Quad::SetByRect(LPRECT prc)
{
	Ax = Dx = prc->left;
	Bx = Cx = prc->right;
	Ay = By = prc->top;
	Cy = Dy = prc->bottom;

	Az = Bz = Cz = Dz = 0.0f;
}

void  Quad::Offset(POINT pt)
{
	Offset(pt.x, pt.y);
}
void  Quad::Offset(int x, int y)
{
	Ax += x;
	Ay += y;

	Bx += x;
	By += y;

	Cx += x;
	Cy += y;

	Dx += x;
	Dy += y;
}
void  Quad::GetBoundRect2(RECT* prc)
{
	GetBoundRect(prc);

	// 返回的区域不需要是下标范围
	prc->right++;
	prc->bottom++;
}
void  Quad::GetBoundRect(RECT* prc)
{
	prc->left   = min(min(min(Ax,Bx),Cx),Dx);
	prc->top    = min(min(min(Ay,By),Cy),Dy);
	prc->right  = max(max(max(Ax,Bx),Cx),Dx); 
	prc->bottom = max(max(max(Ay,By),Cy),Dy);
}

//////////////////////////////////////////////////////////////////////////

void  QuadF::SetByRect(LPRECTF prc)
{
	Ax = Dx = prc->left;
	Bx = Cx = prc->right;
	Ay = By = prc->top;
	Cy = Dy = prc->bottom;
}

void  QuadF::Offset(float x, float y)
{
	Ax += x;
	Ay += y;

	Bx += x;
	By += y;

	Cx += x;
	Cy += y;

	Dx += x;
	Dy += y;
}

void  QuadF::ToQuad(QUAD* p)
{
	p->Ax = _round(Ax);
	p->Ay = _round(Ay);
	p->Bx = _round(Bx);
	p->By = _round(By);
	p->Cx = _round(Cx);
	p->Cy = _round(Cy);
	p->Dx = _round(Dx);
	p->Dy = _round(Dy);
}