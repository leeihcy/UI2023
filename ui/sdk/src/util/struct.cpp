#include "include/inc.h"
#include "include/util/struct.h"

namespace ui {
	
void C9Region::Set( short w )
{
	topleft = top = topright = left = right = bottomleft = bottom = bottomright = w;
}

void C9Region::Set( short wLeftRight, short wTopBottom )
{
	topleft = topright = left = right = bottomleft = bottomright = wLeftRight;
	top = bottom = wTopBottom;
}

void C9Region::Set( short wLeft, short wTop, short wRight, short wBottom )
{
	topleft = left = bottomleft = wLeft;
	top = wTop;
	topright = right = bottomright = wRight;
	bottom = wBottom;
}
bool C9Region::IsAll_0()
{
	unsigned short* p = (unsigned short*)this;
	for (int i = 0; i < 8; i++)
	{
		if (p[i] != 0)
			return false;
	}
	return true;
}
bool C9Region::IsAll_1()
{
	short* p = (short*)this;
	for (int i = 0; i < 8; i++)
	{
		if (p[i] != -1)
			return false;
	}
	return true;
}

bool C9Region::IsSimpleValue()
{
	short* p = (short*)this;
	for (int i = 1; i < 8; i++)
	{
		if (p[i] != p[0])
			return false;
	}
	return true;
}

bool C9Region::IsSimpleRect()
{
	if (topleft != left || bottomleft != left)
		return false;

	if (topright != right || bottomright != right)
		return false;

	return true;
}

void C9Region::DpiScale()
{
	topleft = (short)ui::ScaleByDpi(topleft);
	top = (short)ui::ScaleByDpi(top);
	topright = (short)ui::ScaleByDpi(topright);
	left = (short)ui::ScaleByDpi(left);
	right = (short)ui::ScaleByDpi(right);
	bottomleft = (short)ui::ScaleByDpi(bottomleft);
	bottom = (short)ui::ScaleByDpi(bottom);
	bottomright = (short)ui::ScaleByDpi(bottomright);
}
void C9Region::DpiRestore()
{

}

void C9Region::Copy( const struct C9Region& r )
{
	topleft = r.topleft;
	top = r.top;
	topright = r.topright;
	left = r.left;
	right = r.right;
	bottomleft = r.bottomleft;
	bottom = r.bottom;
	bottomright = r.bottomright;
}

bool C9Region::IsEqual( const struct C9Region& r )
{
	if (0 == memcmp(this, &r, sizeof(r)))
		return true;

	return false;
}

}