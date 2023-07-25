#pragma once
#include "src/object/message.h"

//
//	使用Tree结构实现Object之间的层次关系
//
namespace ui
{
class Object;

class ObjTree : public Message
{
public:
	ObjTree(IMessage* p) : Message(p)
		{ m_pParent = m_pChild = m_pNcChild = m_pNext = m_pPrev = nullptr;	}
	~ObjTree()
		{}

public:
    virtual bool IsNcObject() { return false; }  // 由继承者来扩展实现
	virtual void SetAsNcObject(bool b) {}
	virtual bool CanTabstop() { return false; }
	virtual int  GetZorder() { return 0;  }

	void  AddChild(Object* pObj);
    void  InsertChild(Object* pObj, Object* pInsertAfter);
	void  AddNcChild(Object* pObj);
    void  InsertAfter(Object* pInsertAfter);
    void  InsertBefore(Object* pInsertBefore);
	bool  IsMyChild(Object* pChild, bool bFindInGrand);
	bool  RemoveChildInTree(Object* pChild);

	Object* EnumChildObject(Object* pObj);
	Object* REnumChildObject(Object* pObj);
	Object* EnumNcChildObject(Object* pObj);
	Object* REnumNcChildObject(Object* pObj);
	Object* EnumAllChildObject(Object* pObj);
	Object* REnumAllChildObject(Object* pObj);
	Object* EnumParentObject(Object* pObj);
	Object* REnumParentObject(Object* pObj);

	Object* GetParentObject();
	Object* GetChildObject();
	Object* GetNcChildObject();
	Object* GetLastChildObject();
	Object* GetLastNcChildObject();
	Object* GetNextObject();
	Object* GetPrevObject();
	Object* GetRootObject();

    void SetParentObjectDirect(Object* p);
    void SetChildObjectDirect(Object* p);
    void SetNcChildObjectDirect(Object* p);
    void SetNextObjectDirect(Object* p);
    void SetPrevObjectDirect(Object* p);

	Object* GetNextTreeItemObject();
	Object* GetPrevTreeItemObject();
	Object* GetNextTreeTabstopItemObject();
	Object* GetPrevTreeTabstopItemObject();

	Object* GetNextTabObject()
    {
        return GetNextTreeTabstopItemObject(); 
    }
	Object* GetPrevTabObject()
    {
        return GetPrevTreeTabstopItemObject(); 
    }

	void RemoveMeInTheTree();
	void ClearMyTreeRelationOnly();

    void MoveToAsFirstChild();
    void MoveToAsLastChild();
    bool SwapObject(Object* pObj1, Object* pObj2);

protected:
    void DestroyChildObject();

protected:
	Object*  m_pParent;
	Object*  m_pChild;
	Object*  m_pNcChild;     // 非客户区的子对象，主要用于实现滚动时，不跟随偏移
	Object*  m_pNext;
	Object*  m_pPrev;
};

}

