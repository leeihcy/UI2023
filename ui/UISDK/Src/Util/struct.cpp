#include "stdafx.h"
#include "Inc\Util\struct.h"

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
	WORD* p = (WORD*)this;
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
	topleft = (short)UI::ScaleByDpi(topleft);
	top = (short)UI::ScaleByDpi(top);
	topright = (short)UI::ScaleByDpi(topright);
	left = (short)UI::ScaleByDpi(left);
	right = (short)UI::ScaleByDpi(right);
	bottomleft = (short)UI::ScaleByDpi(bottomleft);
	bottom = (short)UI::ScaleByDpi(bottom);
	bottomright = (short)UI::ScaleByDpi(bottomright);
}
void C9Region::DpiRestore()
{

}

void C9Region::Copy( const C9Region& r )
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

bool C9Region::IsEqual( const C9Region& r )
{
	if (0 == memcmp(this, &r, sizeof(r)))
		return true;

	return false;
}
