#ifndef _UI_IOBJECT_H_
#define _UI_IOBJECT_H_

namespace UI
{
	interface IWindowBase;
	interface ISkinRes;
	interface IObjectDescription;
	interface ITextRenderBase;
	interface IRenderFont;
	interface IRenderBase;
	interface ILayer;
	interface IMapAttribute;
    interface ILayoutParam;
	struct LayerAnimateParam;

	typedef struct tagObjStyle
	{
		bool  initialized : 1;     // �ѳ�ʼ����ɣ����չ���init֪ͨ
		bool  transparent : 1;
		bool  float_on_parent_content : 1;   // ��ˢ�¸ÿؼ�ʱ����������Ҫ�ػ游���󱳾�(wm_erasebkgbkgnd)����Ҫ�ػ游��������(wm_paint)
		bool  post_paint : 1;
		bool  hscroll : 1;
		bool  vscroll : 1;
		bool  ncobject : 1;
		bool  reject_all_mouse_msg : 1;
		bool  reject_self_mouse_msg : 1;
		bool  receive_dragdrop_event : 1;
		bool  enable_ime : 1;
		bool  zindex_overlap : 1;
		bool  noclip : 1;
		bool  clip_client : 1;   // ���������ı������ٻ��ƶ����client����ʱ���Ƿ���Ҫ��client���������á���һ��Ķ������������ڻ������listctrl����Ҫ���ã�������������ݻᳬ�������Ǳ���
		bool  tabstop : 1;
		bool  layer;  // �����ֲ�

		// Ĭ��ֵ�ֶΡ����������า�Ǹ����Ĭ����Ϊ
		// ���Կ���ר��ΪdefaultҲ����һ��ͬ����tagObjStyle�����ڶ�Ӧÿһ����ʽ��Ĭ��ֵ
		bool  default_ncobject : 1;  // scrollbarĬ��Ϊncobj
		bool  default_reject_all_mouse_msg : 1; // Ĭ�ϸÿؼ��Ͳ����������Ϣ����panel/label
		bool  default_reject_self_mouse_msg : 1;
		bool  default_transparent : 1;  // �ÿؼ�Ĭ����͸���ġ�Ĭ�����еĿؼ���͸����������������Ч��
		bool  default_tabstop : 1;  // Ĭ�ϸÿؼ��ܷ�tabstop

	}OBJSTYLE;
	typedef struct tagObjState
	{
		unsigned char visibility_ : 2;  // 0:collapsed 1:visible 2:hidden 
		bool  disable : 1;
		bool  press : 1;
		bool  hover : 1;
		bool  force_press : 1;
		bool  force_hover : 1;
		bool  focus : 1;
		bool  default_ : 1;
		bool  selected : 1;
		bool  checked : 1;
		bool  radio_checked : 1; // ??
		bool  readonly : 1;
		bool  activate : 1;   // ������ʹ��
	}OBJSTATE;

    //struct DECLSPEC_UUID(x);

	class Object;
	interface UIAPI IObject : public IMessage
	{
		IUIApplication* GetUIApplication();
		ISkinRes*  GetSkinRes();
		IWindowBase*  GetWindowObject();
		void  SetDescription(IObjectDescription* p);
		IObjectDescription*  GetDescription();  // ����ֵ����Ϊ��
		LPCTSTR  GetId();
        void  SetId(LPCTSTR szText);
		HWND  GetHWND();
		void  InitDefaultAttrib();
		void  GetMapAttribute(IMapAttribute** ppMapAttribute);
		void  SetOutRef(void** ppOutRef);
        void  LoadAttributeFromMap(
                    IMapAttribute* pMatAttrib, bool bReload);
        void  LoadAttributeFromXml(
                    IUIElement* pXmlElement, bool bReload);
		LPCTSTR   GetAttribute(LPCTSTR szKey, bool bErase);
		void      AddAttribute(LPCTSTR szKey, LPCTSTR  szValue);

		IObject*  GetParentObject();
		IObject*  FindObject(LPCTSTR szObjId);
        IObject*  TryFindObject(LPCTSTR szObjId);
		IObject*  FindNcObject(LPCTSTR szObjId);
		IObject*  FindObject(GUID uuid);
        IObject*  FindNcObject(GUID uuid);
		IObject*  GetChildObject();
		IObject*  GetNcChildObject();
		IObject*  GetNextObject();
		IObject*  GetPrevObject();
        IObject*  GetLastChildObject();

        IObject*  EnumChildObject(IObject* p);
        IObject*  REnumChildObject(IObject* p);
        IObject*  EnumNcChildObject(IObject* p);
        IObject*  EnumAllChildObject(IObject* p);

		void  AddChild(IObject* p);
		void  AddNcChild(IObject*p);
        bool  IsMyChild(IObject* pChild, bool bFindInGrand);
        bool  RemoveChildInTree(IObject* pChild);
        void  InsertChild(IObject* pObj, IObject* pInsertAfter);

		void  Invalidate();
		void  Invalidate(LPCRECT prcObj);
		void  Invalidate(RECT* prcObj, int nCount);
		void  SetObjectPos(int x, int y, int cx, int cy, int nFlag);
		void  SetObjectPos(LPCRECT prc, int nFlag);

		bool  IsVisible();
		bool  IsCollapsed();
		bool  IsSelfVisible();
		void  SetVisible(bool b);
		void  SetVisibleEx(VISIBILITY_TYPE b);
		bool  IsHover();
		bool  IsPress();
		bool  IsForcePress();
		bool  IsForceHover();
		bool  IsDefault();
		bool  IsEnable();
		bool  IsFocus();
		bool  CanTabstop();
		void  SetEnable(bool b);
		void  SetForceHover(bool b, bool bNotify = true);
		void  SetForcePress(bool b, bool bNotify = true);
		bool  SetMouseCapture(int nNotifyMsgId);
		bool  ReleaseMouseCapture();
		bool  SetFocusInWindow();

		void  ModifyObjectStyle(OBJSTYLE* add, OBJSTYLE* remove);
		bool  TestObjectStyle(const OBJSTYLE& test);
		void  RemoveMeInTheTree();
		void  SetZorderDirect(int lz);                      
		int   GetZOrder();                                 
		void  SortChildByZorder();

		void  GetParentRect(RECT* prc);
		void  GetWindowRect(RECT* prc);
		SIZE  GetDesiredSize();
		int   GetWidth();
		int   GetHeight();
		int   GetMaxWidth();
		int   GetMaxHeight();
		void  SetMaxWidth(int);
		void  SetMaxHeight(int);
		int   GetMinWidth();
		int   GetMinHeight();
		void  SetMinWidth(int);
		void  SetMinHeight(int);
        ILayoutParam*  GetSafeLayoutParam();
		void  SetPaddingRegion(REGION4* prc);
		void  GetPaddingRegion(REGION4* prc);
		void  SetMarginRegion(REGION4* prc);
		void  GetMarginRegion(REGION4* prc);
		void  GetBorderRegion(REGION4* prc);
		void  GetNonClientRegion(REGION4* prc);
		void  SetExtNonClientRegion(REGION4* prc);
		void  GetExtNonClientRegion(REGION4* prc);
		void  GetClientRectInObject(RECT* prc);
		void  GetObjectClientRect(RECT* prc);
        void  GetClientRectInWindow(RECT* prc);
		void  ClientRect2ObjectRect(const RECT* rcClient, RECT* rcObj);
		void  ClientRect2WindowRect(const RECT* rcClient, RECT* rcWnd);
		void  WindowRect2ObjectRect(const RECT* rcWindow, RECT* rcObj);
		void  WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj, bool bCalcTransform);
		void  WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj, bool bCalcTransform);

		ITextRenderBase*  GetTextRender();
		IRenderFont*  GetRenderFont();
		IRenderBase*  GetBackRender();
		IRenderBase*  GetForeRender();
        void  SetBackRender(IRenderBase*);
		void  SetTextRender(ITextRenderBase*);
		ILayer*  GetLayer();
		ILayer*  GetSelfLayer();

        void ForwardMessageToChildObject(UIMSG* pMsg);
        void ForwardInitializeMessageToDecendant();
		// ��������
		unsigned char GetOpacity() const;
		void  SetOpacity(const unsigned char alpha, LayerAnimateParam* param = nullptr);

		void  TranslateBy(float x, float y, float z, LayerAnimateParam* param = nullptr);
		void  TranslateTo(float x, float y, float z, LayerAnimateParam* param = nullptr);
		void  TranslateToParent(int x, int y, LayerAnimateParam* param = nullptr);
		void  TranslateXTo(float x, LayerAnimateParam* param = nullptr);
		void  TranslateYTo(float x, LayerAnimateParam* param = nullptr);

		void  RotateXTo(float degree, LayerAnimateParam* param = nullptr);
		void  RotateXBy(float degree, LayerAnimateParam* param = nullptr);
		void  RotateYTo(float degree, LayerAnimateParam* param = nullptr);
		void  RotateYBy(float degree, LayerAnimateParam* param = nullptr);
		void  RotateZTo(float degree, LayerAnimateParam* param = nullptr);
		void  RotateZBy(float degree, LayerAnimateParam* param = nullptr);
		
		void  ScaleTo(float x, float y, LayerAnimateParam* param = nullptr); 

		UI_DECLARE_INTERFACE(Object);
	};
}

#endif // _UI_IOBJECT_H_