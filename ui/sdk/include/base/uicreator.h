#ifndef _UI_CREATOR_H_
#define _UI_CREATOR_H_

#include "uidefine.h"
#include "uimsg.h"

//////////////////////////////////////////////////////////////////////////
//
//    对象创建者，为了支持在对象创建和销毁的时候能调用虚函数，要求所有对象
//    调用UICreateInstance进行创建，将初始化代码和释放代码放在FinalConstruct
//    /FinalRelease里面
//
//////////////////////////////////////////////////////////////////////////

namespace ui
{
struct IUIApplication;
struct ISkinRes;
struct UIMSG;

template<class T>
class ObjectCreator : public T
{
public: 
	// 通用型
	static void CreateInstance2(ISkinRes* pSkinRes, void** pp)
	{
		*pp = (void*)ObjectCreator<T>::CreateInstance(pSkinRes);
	}
	// 专用型
	static T* CreateInstance(ISkinRes* pSkinRes)
	{
        if (!pSkinRes)
        {
#ifdef _DEBUG
            DebugBreak();
#endif
            return nullptr;
        }

		T* p = new ObjectCreator<T>;
		
        if (0 != (UISendMessage(
                    p,
                    UI_MSG_FINALCONSTRUCT, 
                    (long)pSkinRes)))
        {
            p->Release();
            return nullptr;
        }

		return p;
	}
	ObjectCreator() : T(E_BOOL_CREATE_IMPL::CREATE_IMPL_TRUE)
	{

	}

	// 确保delete和new在同一模块，由IMessage::Destroy触发
	virtual void virtual_delete_this() override
	{
		UISendMessage(this, UI_MSG_FINALRELEASE);
		delete this;
	}

	// 调用imessage->virtualProcessMessage时，回到最外层对象来处理
	virtual bool virtualProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook) override
	{
		return T::nvProcessMessage(pMsg, nMsgMapID, bDoHook);
	}
};



template<class T>
class ObjectNoImplCreator : public T
{
public: 
	// 通用型
	static void CreateInstance2(ISkinRes* pSkinRes, void** pp)
	{
		*pp = (void*)ObjectNoImplCreator<T>::CreateInstance(pSkinRes);
	}
	// 专用型
	static T* CreateInstance(ISkinRes* pSkinRes)
	{
		T* p = new ObjectNoImplCreator<T>;

		if (FAILED(UISendMessage(
				p,
				UI_MSG_FINALCONSTRUCT, 
				(long)pSkinRes)))
		{
			delete p;
			return nullptr;
		}

		return p;
	}
	ObjectNoImplCreator() : T()
	{

	}

	// 确保delete和new在同一模块，由IMessage::Destroy触发
	virtual void virtual_delete_this() override
	{
		UISendMessage(this, UI_MSG_FINALRELEASE);
		delete this;
	}
};


}
#endif // _UI_CREATOR_H_