#ifndef _UI_SDK_SRC_OBJECT_OBJECTTREE_H_
#define _UI_SDK_SRC_OBJECT_OBJECTTREE_H_

//
//	使用Tree结构实现Object之间的层次关系
//
namespace ui {
class Object;

class ObjectTree {
public:
  void AddChild(Object *pObj);
  void InsertChild(Object *pObj, Object *pInsertAfter);
  void AddNcChild(Object *pObj);
  void InsertAfter(Object *pInsertAfter);
  void InsertBefore(Object *pInsertBefore);
  bool IsMyChild(Object *pChild, bool bFindInGrand);
  bool RemoveChildInTree(Object *pChild);

  Object *EnumChildObject(Object *pObj);
  Object *REnumChildObject(Object *pObj);
  Object *EnumNcChildObject(Object *pObj);
  Object *REnumNcChildObject(Object *pObj);
  Object *EnumAllChildObject(Object *pObj);
  Object *REnumAllChildObject(Object *pObj);
  Object *EnumParentObject(Object *pObj);
  Object *REnumParentObject(Object *pObj);

  Object *GetParentObject();
  Object *GetChildObject();
  Object *GetNcChildObject();
  Object *GetLastChildObject();
  Object *GetLastNcChildObject();
  Object *GetNextObject();
  Object *GetPrevObject();
  Object *GetRootObject();

  void SetParentObjectDirect(Object *p);
  void SetChildObjectDirect(Object *p);
  void SetNcChildObjectDirect(Object *p);
  void SetNextObjectDirect(Object *p);
  void SetPrevObjectDirect(Object *p);

  Object *GetNextTreeItemObject();
  Object *GetPrevTreeItemObject();
  Object *GetNextTreeTabstopItemObject();
  Object *GetPrevTreeTabstopItemObject();

  Object *GetNextTabObject() { return GetNextTreeTabstopItemObject(); }
  Object *GetPrevTabObject() { return GetPrevTreeTabstopItemObject(); }

  void RemoveMeInTheTree();
  void ClearMyTreeRelationOnly();

  void MoveToAsFirstChild();
  void MoveToAsLastChild();
  bool SwapObject(Object *pObj1, Object *pObj2);

  void DestroyChildObject();

protected:
  Object *m_pParent = nullptr;
  Object *m_pChild = nullptr;
  Object *m_pNcChild = nullptr; // 非客户区的子对象，主要用于实现滚动时，不跟随偏移
  Object *m_pNext = nullptr;
  Object *m_pPrev = nullptr;
};

} // namespace ui

#endif