#ifndef _UI_SDK_INCLUDE_UTIL_RECT_H_
#define _UI_SDK_INCLUDE_UTIL_RECT_H_
#include <algorithm>

namespace ui {

struct Point {
  int x;
  int y;
};

struct Size {
  int width;
  int height;
};

struct RectXYWH {
  int x;
  int y;
  int width;
  int height;
};

struct RectLTRB {
  int left;
  int top;
  int right;
  int bottom;
};

struct Rect : public RectLTRB {

  static Rect MakeLTRB(int l, int t, int r, int b) {
    Rect rect = {l,t,r,b};
    return rect;
  }
  static Rect MakeXYWH(int x, int y, int w, int h) {
    Rect rect = {x,y,x+w,y+h};
    return rect;
  }

  void SetEmpty() {
    this->left = 0;
    this->top = 0;
    this->right = 0;
    this->bottom = 0;
  }
  bool IsEmpty() const { return left == right && top == bottom; }
  void Set(int l, int t, int r, int b) {
    left = l;
    top = t;
    right = r;
    bottom = b;
  }
  void CopyFrom(const Rect &p) {
    this->left = p.left;
    this->top = p.top;
    this->right = p.right;
    this->bottom = p.bottom;
  }
  bool PtIn(const Point &pt) {
    return pt.x >= left && pt.x <= right && pt.y >= top && pt.y <= bottom;
  }
  bool Intersect(const Rect &rc, Rect *out) {
    if (!out) {
      return false;
    }

    if (rc.right < left || rc.left > right || rc.bottom < top ||
        rc.top > bottom) {
      return false;
    }

    out->Set(std::max(rc.left, left), std::min(rc.right, right),
             std::max(rc.top, top), std::min(rc.bottom, bottom));
    return true;
  }

  // 简单合并成一个大的区域，可能会增加无用的区域。
  void Union(const Rect &rc, Rect *out) {
    if (!out) {
      return;
    }

    out->Set(std::min(rc.left, left), std::max(rc.right, right),
             std::min(rc.top, top), std::max(rc.bottom, bottom));
  }

  void Offset(int x, int y) {
    left += x;
    right += x;
    top += y;
    bottom += y;
  }

  int width() const { return right - left; }
  int Width() const { return width(); }
  int height() const { return bottom - top; }
  int Height() const { return height(); }
  bool operator==(const Rect &rc) {
    return memcmp(this, &rc, sizeof(Rect)) == 0;
  }
};

typedef Rect REGION4;

} // namespace ui
#endif