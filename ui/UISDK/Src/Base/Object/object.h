#pragma once
#include "objtree.h"
#include "object_layer.h"

namespace UI
{
class SkinRes;
class UIApplication;
class WindowBase;
class Layer;
interface UIElement;
interface IRenderBase;
interface ITextRenderBase;
interface ILayoutParam;

class Object : public ObjTree
{
    friend class ObjTree;

public:
    Object(IObject*);
    virtual ~Object();

	UI_BEGIN_MSG_MAP()
		UIMSG_ERASEBKGND(OnEraseBkgnd)
		UIMSG_HITTEST(OnHitTest)
		UIMSG_VISIBLE_CHANGED(OnVisibleChanged)
		UIMSG_SERIALIZE(OnSerialize)
		UIMSG_FINALCONSTRUCT(FinalConstruct)
		UIMSG_FINALRELEASE(FinalRelease)
	UI_END_MSG_MAP()

public:
	IObject* GetIObject();

	LPCTSTR  GetId();
	void  SetId(LPCTSTR szText);

	bool  CreateAccesible(IAccessible** pp);
	void  SetDescription(IObjectDescription*);
	IObjectDescription*  GetDescription();

	UIApplication*  GetUIApplication();
	IUIApplication*  GetIUIApplication();
	SkinRes*  GetSkinRes();
	ISkinRes*  GetISkinRes();
	WindowBase*  GetWindowObject();
	HWND  GetHWND();

	Layer*  GetLayer();
	Layer*  GetLayerForAnimate();
	Layer*  GetSelfLayer() const;
	ObjectLayer*  GetLayerEx();
	Object*  GetLayerCreator();
	bool  HasLayer();
	void  OnLayerDestory();
	void  OnLayerCreate();

	void  Invalidate();
	void  Invalidate(LPCRECT prcObj);
	void  Invalidate(RECT* prcObjArray, int nCount);
	void  DrawToLayer__(IRenderTarget* pRenderTarget);
	void  DrawChildObject__(IRenderTarget* pRenderTarget, Object* pChildStart);
	
	Object*  FindObject(LPCTSTR szObjId);
    Object*  TryFindObject(LPCTSTR szObjId);
	Object*  FindNcObject(LPCTSTR szobjId);
	Object*  FindObject(UUID uuid);
    Object*  FindNcObject(UUID uuid);
	unsigned long  GetChildCount();
	Object*  GetChildObjectByIndex(unsigned long lIndex);
	unsigned long  GetChildObjectIndex(Object* pChild);

	bool  IsTransparent();
	void  SetTransparent(bool b);
	bool  IsNoClip();
	bool  NeedClip();
	void  SetNoClip(bool b);
	void  SetClipClient(bool b);
	bool  NeedClipClient();
	bool  IsFocus();
	void  SetFocus(bool b, bool bNoitfy=true);
	bool  SetFocusInWindow();
	bool  IsTabstop();
	bool  CanTabstop();
	void  SetTabstop(bool b);
	bool  IsSelfVisible();
	virtual bool  IsVisible();
	bool  IsCollapsed();
	bool  IsSelfCollapsed();
	virtual bool  IsEnable();
	void  LoadVisibleEx(long l);
	long  SaveVisibleEx();
	void  SetVisible(bool b);
	void  SetVisibleEx(VISIBILITY_TYPE eType);
	void  SetDisableDirect(bool b);
	bool  IsSelfDisable();
	void  SetEnable(bool b, bool bNoitfy = true);
	bool  IsDefault();
	bool  IsHover();
	bool  IsPress();
	bool  IsForceHover();
	bool  IsForcePress();
	bool  IsSelected();
	void  SetDefault(bool b, bool bNotify = true);
	void  SetSelected(bool b, bool bNotify = true);
	void  SetForceHover(bool b, bool bNotify = true);
	void  SetForcePress(bool b, bool bNotify = true);
	void  SetHover(bool b, bool bNotify = true);
	void  SetPress(bool b, bool bNotify=true);
	void  SetAsNcObject(bool b);
	bool  IsNcObject();
	bool  IsRejectMouseMsgAll();
	bool  IsRejectMouseMsgSelf();
	void  SetRejectMouseMsgAll(bool b);
	void  SetRejectMouseMsgSelf(bool b);
	virtual int  GetZorder() override;
	void  SetZorderDirect(int z);
	void  SortChildByZorder();

	void  ModifyObjectStyle(OBJSTYLE* add, OBJSTYLE* remove);
	bool  TestObjectStyle(const OBJSTYLE& test);

	void  LoadAttributeFromMap(IMapAttribute* pMatAttrib, bool bReload);
	void  LoadAttributeFromXml(UIElement* pElement, bool bReload);

	LPCTSTR  GetAttribute(LPCTSTR szKey, bool bErase);
	void  AddAttribute(LPCTSTR szKey, LPCTSTR  szValue);
	void  GetMapAttribute(IMapAttribute** ppMapAttribute);
	void  ClearMapAttribute();
	void  InitDefaultAttrib();

	void  SetOutRef(void** ppOutRef);
	Object*  GetObjectByPos(POINT* pt);
	bool  SetMouseCapture(int nNotifyMsgId);
	bool  ReleaseMouseCapture();
	bool  SetKeyboardCapture(int nNotifyMsgId);
	bool  ReleaseKeyboardCapture();

	
	void  SetBackRender(IRenderBase* p);
	void  SetForegndRender(IRenderBase* p);
	void Object::SetTextRender(ITextRenderBase* p);
	ITextRenderBase*  GetTextRender();
	IRenderBase*  GetBackRender();
	IRenderBase*  GetForeRender();
	IRenderFont*  GetRenderFont();

	void  LoadBkgndRender(LPCTSTR szName);
	void  LoadForegndRender(LPCTSTR szName);
	void  LoadTextRender(LPCTSTR szName);
	LPCTSTR  SaveBkgndRender();
	LPCTSTR  SaveForegndRender();
	LPCTSTR  SaveTextRender();

	SIZE  GetDesiredSize();
	void  UpdateLayout();
	//void  UpdateMyLayout();
	void  UpdateLayoutPos();
	void  UpdateObjectNonClientRegion();
	virtual void  SetObjectPos(int x, int y, int cx, int cy, int nFlag = 0);
	void  SetObjectPos(LPCRECT prc, int nFlag);

	void  GetParentRect(RECT* prc);
	POINT  GetWindowPoint();
	void  GetWindowRect(RECT* lprc);
	bool  GetRectInWindow(RECT* prc, bool bOnlyVisiblePart);
	void  GetClientRectInObject(RECT* prc);
	void  GetObjectClientRect(RECT* prc);
	bool  CalcRectInAncestor(Object*  pObjAncestor, LPCRECT prcObjPart, bool bCalcVisible, RECT* prcOut);
	bool  GetScrollOffset(int* pxOffset, int* pyOffset);
	bool  GetScrollRange(int* pxRange, int* pyRange);

	ILayoutParam*  GetLayoutParam();
    ILayoutParam*  GetSafeLayoutParam();
	ILayout*  GetLayout();
    void  DestroyLayoutParam();
	void  CreateLayoutParam();
	void  SetLayoutParam(ILayoutParam* p);

	// Canvas布局专用接口
	int   GetConfigWidth();
	int   GetConfigHeight();
	int   GetConfigLayoutFlags();
	int   GetConfigLeft();
	int   GetConfigRight();
	int   GetConfigTop();
	int   GetConfigBottom();
	void  SetConfigWidth(int n);
	void  SetConfigHeight(int n);
	void  SetConfigLayoutFlags(int n);
	void  SetConfigLeft(int n);
	void  SetConfigRight(int n);
	void  SetConfigTop(int n);
	void  SetConfigBottom(int n);

	int   GetParentRectL() { return m_rcParent.left; }
	int   GetParentRectT() { return m_rcParent.top; }
	int   GetParentRectR() { return m_rcParent.right; }
	int   GetParentRectB() { return m_rcParent.bottom; }

	void  LoadBorder(REGION4* prc);
	void  SaveBorder(REGION4* prc);
	int   GetBorderL() { return m_rcBorder.left; }
	int   GetBorderT() { return m_rcBorder.top; }
	int   GetBorderR() { return m_rcBorder.right; }
	int   GetBorderB() { return m_rcBorder.bottom; }
	void  SetBorderRegion(RECT* prc);
	void  GetBorderRegion(REGION4* prc) { ::CopyRect(prc, &m_rcBorder); }
	
	void  LoadPadding(REGION4* prc);
	void  SavePadding(REGION4* prc);
	int   GetPaddingL() { return m_rcPadding.left; }
	int   GetPaddingT() { return m_rcPadding.top; }
	int   GetPaddingR() { return m_rcPadding.right; }
	int   GetPaddingB() { return m_rcPadding.bottom; }
	int   GetPaddingW() { return m_rcPadding.left + m_rcPadding.right; }
	int   GetPaddingH() { return m_rcPadding.top + m_rcPadding.bottom; }
	void  GetPaddingRegion(REGION4* prc) { ::CopyRect(prc, &m_rcPadding); }
	void  SetPaddingRegion(REGION4* prc);

	void  LoadMargin(REGION4* prc);
	void  SaveMargin(REGION4* prc);
	void  SetMarginRegion(REGION4* prc) { m_rcMargin.CopyRect(prc); }
	void  GetMarginRegion(REGION4* prc) { ::CopyRect(prc, &m_rcMargin); }
	int   GetMarginL() { return m_rcMargin.left; }
	int   GetMarginT() { return m_rcMargin.top; }
	int   GetMarginR() { return m_rcMargin.right; }
	int   GetMarginB() { return m_rcMargin.bottom; }
	int   GetMarginW() { return m_rcMargin.left + m_rcMargin.right; }
	int   GetMarginH() { return m_rcMargin.top + m_rcMargin.bottom; }

	void  GetNonClientRegion(REGION4* prc);
	void  SetExtNonClientRegion(REGION4* prc) { m_rcExtNonClient.CopyRect(prc); }
	void  GetExtNonClientRegion(REGION4* prc) { ::CopyRect(prc, &m_rcExtNonClient); }

	void  GetClientRectInParent(RECT* prc);
	void  GetClientRectInWindow(RECT* prc);
	int   GetWidth();
	int   GetHeight();
	int   GetWidthWithMargins();
	int   GetHeightWithMargins();
	int   GetMaxWidth();
	int   GetMaxHeight();
	void  SetMaxWidth(int);
	void  SetMaxHeight(int);
	int   GetMinWidth();
	int   GetMinHeight();
	void  SetMinWidth(int);
	void  SetMinHeight(int);

	bool  IntersectWindowRect(LPCRECT prcWindow, RECT* prcIntersectWnd, RECT* prcIntersectObj);
	void  WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj, bool bCalcTransform);
	void  WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptClient, bool bCalcTransform);
	void  WindowRect2ObjectClientRect(LPCRECT rcWindow, RECT* rcObj);
	void  WindowRect2ObjectRect(LPCRECT rcWindow, RECT* rcObj);

	static void  ParentClientPoint2ChildPoint(Object* pObjChild, const POINT* pt, POINT* pOut);

	static void  ObjectPoint2ObjectClientPoint(Object* pObj, const POINT* pt, POINT* pOut);
	static void  ObjectPoint2ObjectNonClientPoint(Object* pObj, const POINT* pt, POINT* pOut);
	static void  ObjectRect2ObjectClientRect(Object* pObj, LPCRECT prc, RECT* pOut);
	static void  ObjectClientRect2WindowRect(Object* pObj, LPCRECT prcClient, LPRECT prcWnd);
	static void  ObjectRect2WindowRect(Object* pObj, LPCRECT prcObj, LPRECT prcWnd);

	static void  ParentClientPoint2ChildClientPoint(Object* pObjChild, const POINT* pt, POINT* pOut);
	static void  ParentClientRect2ChildClientRect(Object* pObjChild, LPCRECT prc, RECT* pOut);

	static void  ParentPoint2ChildPoint(Object* pObjChild, const POINT* pt, POINT* pOut);
	static void  ParentRect2ChildRect(Object* pObjChild, LPCRECT prc, RECT* pOut);

	static void  ChildPoint2ParentClientPoint(Object* pObjChild, const POINT* ptChild, POINT*  ptOut);
	static void  ChildRect2ParentClientRect(Object* pObjChild, LPCRECT prc, RECT*  pOut);

	static void  ObjectClientPoint2ObjectPoint(Object*  pObj, const POINT* ptChild, POINT*  ptOut);
	static void  ObjectClientRect2ObjectRect(Object*  pObj, LPCRECT prc, RECT*  pOut);

	static void  ChildPoint2ParentPoint(Object* pObjChild, const POINT* ptChild, POINT*  ptOut);
	static void  ChildRect2ParentRect(Object* pObjChild, LPCRECT prc, RECT*  pOut);

	// 动画属性
	unsigned char GetOpacity() const;
	void  SetOpacity(const unsigned char alpha, LayerAnimateParam* param);

	void  TranslateBy(float x, float y, float z, LayerAnimateParam* param);
	void  TranslateTo(float x, float y, float z, LayerAnimateParam* param);
	void  TranslateXTo(float x, LayerAnimateParam* param);
	void  TranslateYTo(float y, LayerAnimateParam* param);
	void  TranslateToParent(int x, int y, LayerAnimateParam* param);

	void  RotateXTo(float degree, LayerAnimateParam* param);
	void  RotateXBy(float degree, LayerAnimateParam* param);
	void  RotateYTo(float degree, LayerAnimateParam* param);
	void  RotateYBy(float degree, LayerAnimateParam* param);
	void  RotateZTo(float degree, LayerAnimateParam* param);
	void  RotateZBy(float degree, LayerAnimateParam* param);

	void  ScaleTo(float x, float y, LayerAnimateParam* param = nullptr);

protected:
	HRESULT  FinalConstruct(ISkinRes* pSkinRes);
	void  FinalRelease();
	UINT  OnHitTest(POINT* ptInParent, __out POINT* ptInChild);
	void  OnVisibleChanged(BOOL bVisible, IObject* pObjChanged);
	void  OnSerialize(SERIALIZEDATA* pData);
	void  OnEraseBkgnd(IRenderTarget* pRenderTarget);

    void  position_in_tree_changed();

protected:
	Object*  find_child_object(LPCTSTR szobjId, bool bFindDecendant);
	Object*  find_child_object(UUID uuid, bool bFindDecendant);
    Object*  find_ncchild_object(UUID uuid, bool bFindDecendant);
	void  load_renderbase(LPCTSTR szName, IRenderBase*& pRender);
	void  load_textrender(LPCTSTR szName, ITextRenderBase*& pTextRender);
	LPCTSTR  get_renderbase_name(IRenderBase*& pRender);
	LPCTSTR  get_textrender_name(ITextRenderBase*& pTextRender);

	void  load_layer_config(bool b);

public:
	void  notify_WM_SIZE(UINT nType, UINT nWidth, UINT nHeight);
	void  notify_WM_MOVE(int x, int y);

	IMKMgr* GetIMKMgr();

protected: // virtual
	virtual  IMKMgr*  virtualGetIMKMgr() { return nullptr; }
	virtual  void  virtualSetVisibleEx(VISIBILITY_TYPE eType);
	virtual  void  virtualSetEnable(bool b);
	virtual  void  virtualOnSize(UINT nType, UINT nWidth, UINT nHeight);
	virtual  void  virtualOnMove();
	virtual  void  virtualOnPostDrawObjectErasebkgnd() {};

public:
	virtual  void  virtualOnLoad();

public: 
	static void Object::ForwardMessageToChildObject(
			Object* pParent, UIMSG* pMsg);
    static void Object::ForwardInitializeMessageToDecendant(Object* pParent);

protected:
	IObject*   m_pIObject;
	SkinRes*   m_pSkinRes;  // 用于支持多皮肤包共存（插件模式）
	IObjectDescription*  m_pDescription; // 对象的一些静态属性，仅保存指针，通常这是一个static对象地址。

	String  m_strId;                    // 该对象在XML中的标识
#ifdef EDITOR_MODE
    String  m_strStyle;                 // 控件样式
#endif

#pragma region //坐标相关数据
	CRect      m_rcParent;              // 该对象的范围，相对于parent的client区域.对于Window对象是客户区域位置，即左上角为0，0
	CRegion4   m_rcExtNonClient;        // 扩展的非客户区，与border、padding共同做为对象的非客户区。
	CRegion4   m_rcMargin;
	CRegion4   m_rcPadding;
	CRegion4   m_rcBorder;
	//HRGN     m_hRgn;                  // （未使用）如果该对象是一个不规则区域，必须设置该值，该值对window类型对象无效. rgn是相对于窗口左上角的。
	ILayoutParam*  m_pLayoutParam;      // 布局参数。由Object负责释放
#pragma endregion

	OBJSTYLE   m_objStyle;
	OBJSTATE   m_objState;

	ObjectLayer  m_objLayer;

	int  m_lzOrder;                   // 控件z序，用于实现控件重叠时的刷新判断依据

	// 控件的最大尺寸限制
	int  m_nMaxWidth;             
	int  m_nMaxHeight;  
	long  m_lMinWidth;               
	long  m_lMinHeight;

	IMapAttribute*    m_pIMapAttributeRemain;  // 用于扩展。未解析的属性
	IRenderBase*      m_pBkgndRender;          // 背景渲染
	IRenderBase*      m_pForegndRender;        // 前景渲染
	ITextRenderBase*  m_pTextRender;           // 文字渲染，由control负责读取该属性
	IAccessible*      m_pAccessible;

	void**     m_ppOutRef;              // 为了解决一个类成员对象，有可能被自己的父对象删除后，这个类却不知道，再删除该对象时崩溃了.

	friend class ObjectAccessible;
};


}


