
#include "rectregion.h"
#include <assert.h>
using namespace ui;

namespace {

void test_union() {
  RectRegion region;
  assert(region.GetCount() == 0);

  // 单个区域
  {
    RECT r1 = { 10, 10, 20, 20};
    region.Union(r1);
    assert(region.GetCount() == 1);
  }

  // 两个分开的区域
  //  
  //  +-------+
  //  |       |
  //  +-------+
  //
  //  +-------+
  //  |       |
  //  +-------+  
  //       
  region.Destroy();
  assert(region.GetCount() == 0);
  {
    RECT r1 = { 10, 10, 20, 20};
    RECT r2 = { 10, 30, 20, 40};
    region.Union(r1);
    region.Union(r2);
    assert(region.GetCount() == 2);
    assert(*region.GetRectPtrAt(0) == r1);
    assert(*region.GetRectPtrAt(1) == r2);
  }

  // 两个相连的区域
  //  
  //  +-------++----+
  //  |       ||    |
  //  |       ||    |
  //  |       ||    |
  //  +-------++----+
  //     
  region.Destroy();
  {
    RECT r1 = { 10, 10, 20, 20};
    RECT r2 = { 20, 10, 30, 20};
    region.Union(r1);
    region.Union(r2);
    assert(region.GetCount() == 1);
    RECT r3 = {10, 10, 30, 20};
    assert(*region.GetRectPtrAt(0) == r3);
  }

  // 两个相交的区域，但可以合成成一个
  //  
  //  +-------+====+----+
  //  |       |    |    |
  //  |       |    |    |
  //  |       |    |    |
  //  +-------+====+----+
  //   
  region.Destroy();
  {
    RECT r1 = { 10, 10, 20, 20};
    RECT r2 = { 19, 10, 30, 20};
    region.Union(r1);
    region.Union(r2);
    assert(region.GetCount() == 1);
    RECT r3 = {10, 10, 30, 20};
    assert(*region.GetRectPtrAt(0) == r3);
  }

  // 两个包含的区域，但可以合成成一个
  //  
  //  +-----------+
  //  | +-------+ |
  //  | |       | |
  //  | +-------+ |
  //  +-----------+
  //   
  region.Destroy();
  {
    RECT r1 = { 10, 10, 20, 20};
    RECT r2 = { 11, 11, 19, 19};
    region.Union(r1);
    region.Union(r2);
    assert(region.GetCount() == 1);
    assert(*region.GetRectPtrAt(0) == r1);
  }

  // 一个大的区域吃上一个小的区域
  //  
  //  +-----------+
  //  |         +-|----+
  //  |         | |    |
  //  |         +-|----+
  //  +-----------+
  // 
  region.Destroy();
  {
    RECT r1 = { 10, 10, 20, 20};
    RECT r2 = { 19, 11, 30, 19};
    region.Union(r1);
    region.Union(r2);
    assert(region.GetCount() == 3);

    RECT result1 = {10,10, 20, 11};
    RECT result2 = {10,11, 30, 19};
    RECT result3 = {10,19, 20, 20};
    assert(*region.GetRectPtrAt(0) == result1);
    assert(*region.GetRectPtrAt(1) == result2);
    assert(*region.GetRectPtrAt(2) == result3);
  }

  // 一个区域的一角，与另一个区域的一角相交
  //  
  //  +-----------+
  //  |           |
  //  |       +---|----+
  //  +-------|---+    |
  //          +--------+
  //
  region.Destroy();
  {
    RECT r1 = { 10, 10, 20, 20};
    RECT r2 = { 15, 15, 30, 30};
    region.Union(r1);
    region.Union(r2);
    assert(region.GetCount() == 3);

    RECT result1 = {10,10, 20, 15};
    RECT result2 = {10,15, 30, 20};
    RECT result3 = {15,20, 30, 30};
    assert(*region.GetRectPtrAt(0) == result1);
    assert(*region.GetRectPtrAt(1) == result2);
    assert(*region.GetRectPtrAt(2) == result3);
  }

  // 多个区域相交
  //      10      15  20   30      40       50
  //  5                    +-----------------+
  //  10  +-----------+    |                 |
  //      |           |    |                 |
  //  15  |       +---|----|-------+         |
  //  20  +-------|---+    |       |         |
  //  25          |        +-------|---------+
  //  30          +----------------+
  //
  region.Destroy();
  {
    RECT r1 = { 10, 10, 20, 20};
    RECT r2 = { 30,  5, 50, 25};
    RECT r3 = { 15, 15, 40, 30};
    region.Union(r1);
    region.Union(r2);
    region.Union(r3);
    assert(region.GetCount() == 5);

    RECT result1 = {30, 5, 50, 15};
    RECT result2 = {10, 10, 20, 15};
    RECT result3 = {10, 15, 50, 20};
    RECT result4 = {15, 20, 50, 25};
    RECT result5 = {15, 25, 40, 30};
    assert(*region.GetRectPtrAt(0) == result1);
    assert(*region.GetRectPtrAt(1) == result2);
    assert(*region.GetRectPtrAt(2) == result3);
    assert(*region.GetRectPtrAt(3) == result4);
    assert(*region.GetRectPtrAt(4) == result5);
  }
}

}


void rect_region_unittest() {
  test_union();
}