#pragma once
#include "include/interface/imeta.h"
#include "include/interface/iwindow.h"
// #include "include/interface/icustomwindow.h"

namespace ui {

struct WindowMeta : public MetaImpl<IWindow> {
  static WindowMeta& Get() {
    static WindowMeta s;
    return s;
  }

  Uuid UUID() override { return {0x5C36801E, 0x5929, 0x4512, {0xA9, 0x98, 0xF9, 0x71, 0x9D, 0xCC, 0x69, 0x03}}; }

  const char* Name() override { return "Window"; }

  MetaDetail Detail() override {
    MetaDetail param;
    param.major_type = OBJ_WINDOW;
    param.minor_type = WINDOW_SIMPLE;
    param.category = CATEGORY_WINDOW;
    param.descrption = "窗口";
    param.default_size = {500, 500};
    return param;
  }
};

// struct WindowMeta : public IMeta {
//   static IMeta *Get() {
//     static WindowMeta s;
//     return &s;
//   }
//   static IWindowPtr create(IResourceBundle *p) {
//     IWindow *p = new ui::ObjectCreator<IWindow>();

//     FinalConstructMessage msg;
//     msg.resource = p;
//     p->Route(&msg)
//     if (!msg.success) {
//       delete p;
//       return IWindowPtr(nullptr, WindowMEta::destroy);
//     }
//     return IWindowPtr(p, WindowMEta::destroy);
//   }
//   static void destroy(IWindow* p) {
//     FinalReleaseMessage msg;
//     p->Route(&msg);

//     delete p;  
//   }

  
//   void Create(IResourceBundle *p, void **pp) override {
//     *pp = WindowMeta::create(p);
//   }
//   void Destroy(void* obj) override {
//     WindowMeta::destroy((IWindow*)obj)
//   }

//   Uuid GetUuid() override { return {0x5C36801E, 0x5929, 0x4512, {0xA9, 0x98, 0xF9, 0x71, 0x9D, 0xCC, 0x69, 0x03}}; }

//   void Route(IMessage* obj, Msg* msg) override {
//     static_cast<IWindow*>(obj)->MessageRoute(msg);
//   }

//   EditorParam GetEditorParam() override {
//     EditorParam param;
//     param.major_type = OBJ_WINDOW;
//     param.minor_type = WINDOW_SIMPLE;
//     param.category = CATEGORY_WINDOW;
//     param.name = L"Window";
//     param.descrption = L"窗口";
//     param.default_size = {500, 500};
//     return param;
//   }
// };

struct RootObjectMeta : public MetaImpl<IRootObject> {
  static RootObjectMeta& Get() {
    static RootObjectMeta s;
    return s;
  }

  Uuid UUID() override { return {0xA9E5EAE8, 0x6496, 0x11F0, {0x97, 0x7D, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const char* Name() override { return "RootObject"; }

  MetaDetail Detail() override {
    MetaDetail param = {0};
    param.major_type = OBJ_ROOT;
    return param;
  }
};


#if 0
class CustomWindowDescription : public IMeta
{
public:
	static IMeta*  Get()
	{
		static CustomWindowDescription s;
		return &s;
	}

    virtual void  CreateInstance(IResourceBundle* p, void** pp) override
    {
        *pp = UI::ICustomWindow::create(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_WINDOW;
	}
	virtual long  GetMinorType() override
	{
		return WINDOW_CUSTOM;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_WINDOW; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"CustomWindow";
	}
	virtual const UUID&  GetUuid() override
	{
		return __guid(ICustomWindow);
	}

	virtual void  GetDefaultSize(long& w, long& h) override
	{
		w = 500;
		h = 500;
	}
	virtual HBITMAP  LoadIcon() override 
	{
		return 0;
	}
};
#endif
} // namespace ui