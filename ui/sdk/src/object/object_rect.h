#ifndef _UI_SDK_SRC_OBJECT_OBJECTRECT_H_
#define _UI_SDK_SRC_OBJECT_OBJECTRECT_H_

#include "include/interface/iobject.h"
namespace ui {
class Object;

class ObjectRect {
public:
  int GetWidth();
  int GetHeight();

  void GetParentRect(Rect *prc);
  Point GetWindowPoint();
  void GetWindowRect(Rect *lprc);
  bool GetRectInWindow(Rect *prc, bool bOnlyVisiblePart);
  void GetClientRectInObject(Rect *prc);
  void GetClientRectWithZeroOffset(Rect *prc);
  bool CalcRectInAncestor(Object *pObjAncestor, const Rect *prcObjPart,
                          bool bCalcVisible, Rect *prcOut);

  int GetParentRectL() { return m_rcParent.left; }
  int GetParentRectT() { return m_rcParent.top; }
  int GetParentRectR() { return m_rcParent.right; }
  int GetParentRectB() { return m_rcParent.bottom; }

  bool IntersectWindowRect(const Rect *prcWindow, Rect *prcIntersectWnd,
                           Rect *prcIntersectObj);
  void WindowPoint2ObjectPoint(const Point *ptWindow, Point *ptObj,
                               bool bCalcTransform);
  void WindowPoint2ObjectClientPoint(const Point *ptWindow, Point *ptClient,
                                     bool bCalcTransform);
  void WindowRect2ObjectClientRect(const Rect *rcWindow, Rect *rcObj);
  void WindowRect2ObjectRect(const Rect *rcWindow, Rect *rcObj);

public:
  static void ParentClientPoint2ChildPoint(Object *pObjChild, const Point *pt,
                                           Point *pOut);
  static void ObjectPoint2ObjectClientPoint(Object *pObj, const Point *pt,
                                            Point *pOut);
  static void ObjectPoint2ObjectNonClientPoint(Object *pObj, const Point *pt,
                                               Point *pOut);
  static void ObjectRect2ObjectClientRect(Object *pObj, const Rect *prc,
                                          Rect *pOut);
  static void ObjectClientRect2WindowRect(Object *pObj, const Rect *prcClient,
                                          Rect *prcWnd);
  static void ObjectRect2WindowRect(Object *pObj, const Rect *prcObj,
                                    Rect *prcWnd);

  static void ParentClientPoint2ChildClientPoint(Object *pObjChild,
                                                 const Point *pt, Point *pOut);
  static void ParentClientRect2ChildClientRect(Object *pObjChild,
                                               const Rect *prc, Rect *pOut);

  static void ParentPoint2ChildPoint(Object *pObjChild, const Point *pt,
                                     Point *pOut);
  static void ParentRect2ChildRect(Object *pObjChild, const Rect *prc,
                                   Rect *pOut);

  static void ChildPoint2ParentClientPoint(Object *pObjChild,
                                           const Point *ptChild, Point *ptOut);
  static void ChildRect2ParentClientRect(Object *pObjChild, const Rect *prc,
                                         Rect *pOut);

  static void ObjectClientPoint2ObjectPoint(Object *pObj, const Point *ptChild,
                                            Point *ptOut);
  static void ObjectClientRect2ObjectRect(Object *pObj, const Rect *prc,
                                          Rect *pOut);

  static void ChildPoint2ParentPoint(Object *pObjChild, const Point *ptChild,
                                     Point *ptOut);
  static void ChildRect2ParentRect(Object *pObjChild, const Rect *prc,
                                   Rect *pOut);

protected:
  // 该对象的范围，相对于parent的client区域.对于Window对象是客户区域位置，即左上角为0，0
  Rect m_rcParent = { 0 }; 
};

} // namespace ui

#endif