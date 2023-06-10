#include "stdafx.h"
#ifdef UNITTEST
#include "Inc\Util\signalslot.h"

void test()
{

}
void /*__stdcall*/ OnClick()
{
	return test();
}

class Test
{
public:
	void click()
	{

	}
};

void main()
{
// 	signal<> clicked;
// 	clicked.connect0(OnClick);
// 	//clicked.connect0(&t, &Test::click);
// 	clicked.emit(99);

}
#endif