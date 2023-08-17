#ifndef _UI_MSG_H_
#define _UI_MSG_H_

//
// 为了可以在消息响应函数中直接访问当前消息结构，将m_pCurMsg作为成员函数进行访问或设置
// 为了防止在处理一个消息A的过程中，必须再发送另外一个消息B，导致A的m_pCurMsg被B篡改，
// 必须再增加一个临时变量保存当前消息m_pCurMsg，这样当B处理完后，m_pCurMsg恢复成A
// 为了实现还原m_pCurMsg，所有的消息不能直接返回，而是将m_pCurMsg还原之后再return
//
// fix bug: 20120831
//    为了解决派生类将消息CHAIN给父类时，父类在UI_BEGIN_MSG_MAP又会重新响应一次DoHook，
//    导致Hook方收到两次消息。因此增加一个bDoHook参数
//
// ps: 20130320
//    在Ixxx层次，提供IMessage::virtualProcessMessage虚函数和nvProcessMessage非虚函数，
//    在xxx层次，也提供virtualProcessMessage(主要用于非IMessage派生类,IMessageInnerProxy调用)和nvProcessMessage非虚函数
//    同时外部控件可直接继承于MessageProxy提供消息映射
//

#define UI_BEGIN_MSG_MAP()                                                            \
  virtual bool virtualProcessMessage(ui::UIMSG *pMsg, int nMsgMapID = 0,              \
                                     bool bDoHook = false) override {                 \
    return this->nvProcessMessage(pMsg, nMsgMapID, bDoHook);                          \
  }                                                                                   \
  bool nvProcessMessage(ui::UIMSG *pMsg, int nMsgMapID, bool bDoHook) {               \
    /*取出wParam,lParam，以便兼容Window消息处理中的wParam,lParam参数*/ \
    long wParam = pMsg->wParam;                                                       \
    long lParam = pMsg->lParam;                                                       \
    unsigned int uMsg = pMsg->message;                                                \
    unsigned int code = pMsg->nCode;                                                  \
    ui::IMessage *pMsgFrom = pMsg->pMsgFrom;                                          \
    long &lResult = pMsg->lRet; /* 兼容wtl */                                       \
                                                                                      \
    (code); /*未引用变量 C4189*/                                                 \
    (pMsgFrom);                                                                       \
    (lResult);                                                                        \
    (wParam);                                                                         \
    (lParam);                                                                         \
    (uMsg);                                                                           \
                                                                                      \
    this->SetCurMsg(pMsg);                                                            \
                                                                                      \
    /*消息HOOK处理*/                                                              \
    if (bDoHook && this->DoHook(pMsg, nMsgMapID))                                     \
      return true;                                                                    \
                                                                                      \
    switch (nMsgMapID) {                                                              \
    case 0:

#define UI_END_MSG_MAP()                                                       \
  break;                                                                       \
  }                                                                            \
  return false;                                                                \
  }

#define UI_END_MSG_MAP_CHAIN_PARENT(baseclass)                                 \
  break;                                                                       \
  }                                                                            \
  if (baseclass::nvProcessMessage(pMsg, nMsgMapID, false))                     \
    return true;                                                               \
  return false;                                                                \
  }

// 如果只是直接调用UICHAIN_MSG_MAP_POINT_MEMBER(static_cast<baseinterface*>(m_pI##classname))则只能响应nMapID为0的情况了
// 因此增加UI_END_MSG_MAP_CHAIN_PARENT_Ixxx宏用于将消息传给父对象
#define UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(classname, baseinterface)             \
  break;                                                                       \
  }                                                                            \
  {                                                                            \
    if (m_pI##classname)                                                       \
      if (static_cast<baseinterface *>(m_pI##classname)                        \
              ->nvProcessMessage(pMsg, nMsgMapID, false))                      \
        return true;                                                           \
  }                                                                            \
  return false;                                                                \
  }

#define UI_BEGIN_MSG_MAP_Ixxx(iclassname)                                      \
  static iclassname *CreateInstance(ui::IResource *pSkinRes) {                \
    return ui::ObjectNoImplCreator<iclassname>::CreateInstance(pSkinRes);      \
  }                                                                            \
  UI_BEGIN_MSG_MAP()

//
// 消息链传递
//
//	该宏有个缺点，就是能接收到当前的分支消息，不能传递其它分支消息。可使用UI_BEGIN_CHAIN_ALL_MSG_MAP改进宏

#define UICHAIN_MSG_MAP(theChainClass)                                         \
  if (theChainClass::nvProcessMessage(pMsg, nMsgMapID, false))                 \
    return true;

#define UICHAIN_MSG_MAP_MEMBER(theChainMember)                                 \
  if (theChainMember.ProcessMessage(pMsg, nMsgMapID, false))                   \
    return true;

#define UICHAIN_MSG_MAP_POINT_MEMBER(pTheChainMember)                          \
  if (pTheChainMember)                                                         \
    if (pTheChainMember->ProcessMessage(pMsg, nMsgMapID, false))               \
      return true;

//
//  强行结束当前消息分支，而将消息全部传递给需要的对象
//
//	TODO:
//该宏缺点，强行结束后，不知道用哪个分支再进行开始，这里默认取为0。建议将该宏放在最前面或者最后面
//
#define UI_BEGIN_CHAIN_ALL_MSG_MAP                                             \
  break;                                                                       \
  }

#define UI_END_CHAIN_ALL_MSG_MAP                                               \
  switch (nMsgMapID) {                                                         \
  case 0:

#define UIALT_MSG_MAP(msgMapID)                                                \
  break;                                                                       \
  case msgMapID:

#define CHAIN_MSG_MAP_MEMBER_P(theChainMember)                                 \
  {                                                                            \
    if (theChainMember)                                                        \
      if (theChainMember->ProcessWindowMessage(hWnd, uMsg, wParam, lParam,     \
                                               lResult))                       \
        return true;                                                           \
  }

#define VIRTUAL_BEGIN_MSG_MAP(theClass)                                        \
public:                                                                        \
  virtual bool ProcessWindowMessage(HWND hWnd, unsigned int uMsg, long wParam, \
                                    long lParam, long &lResult,                \
                                    unsigned int dwMsgMapID = 0) {             \
    bool bHandled = true;                                                      \
    (hWnd);                                                                    \
    (uMsg);                                                                    \
    (wParam);                                                                  \
    (lParam);                                                                  \
    (lResult);                                                                 \
    (bHandled);                                                                \
    switch (dwMsgMapID) {                                                      \
    case 0:

#define UI_MSG_NOTIFY 168272329


//
//	弹出提示条通知
//		message:UI_WM_SHOW_TOOLTIP
//		code:   0
//		wparam:
//		lParam:
//		pMsgFrom: pWindow
//
//	如果对象不处理这个消息或返回FALSE，则将弹出默认的提示条，显示tooltip属性内容。
//
#define UI_MSG_SHOW_TOOLTIP 149150933

//
//	当itemdata不为空时，向控件获取item的提示信息
//		message:UI_WM_GET_TOOLTIPINFO
//		code:
//		wparam:  TOOLTIPITEM*
//		lParam:  IToolTipUI*
//		pMsgFrom:
//
//	返回0表示失败，不显示提示条。返回1将立即显示提示条
//
#define UI_MSG_GET_TOOLTIPINFO 149150934
// bool  OnGetToolTipInfo(TOOLTIPITEM* pItem, IToolTipUI* pUI);
#define UIMSG_GET_TOOLTIPINFO(func)                                            \
  if (uMsg == UI_MSG_GET_TOOLTIPINFO) {                                        \
    SetMsgHandled(true);                                                       \
    lResult = func((TOOLTIPITEM *)wParam, (IToolTipUI *)lParam);               \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// wParam: SERIALIZEDATA
// lParam:
// return:
#define UI_MSG_SERIALIZE 139281928
// void  OnSerialize(SERIALIZEDATA* pData);
#define UIMSG_SERIALIZE(func)                                                  \
  if (uMsg == UI_MSG_SERIALIZE) {                                              \
    SetMsgHandled(true);                                                       \
    func((SERIALIZEDATA *)wParam);                                             \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

//
//  接口查询
//
//  message : UI_WM_QUERYINTERFACE
//  code : NA
//  wparam : UUID*   pguid
//  lparam : void**  ppOut
//
#define UI_MSG_QUERYINTERFACE 165312200

#define UIMSG_QUERYINTERFACE(className)                                        \
  if (uMsg == UI_MSG_QUERYINTERFACE) {                                         \
    SetMsgHandled(true);                                                       \
    if (I##className::UUID() == (*(const Uuid *)wParam)) {                     \
      SetMsgHandled(true);                                                     \
      lResult = (long)(void *)m_pI##className;                                 \
      return true;                                                             \
    }                                                                          \
  }

// void*  QueryInterface(const IID* pIID);
#define UIMSG_QUERYINTERFACE2(className)                                       \
  if (uMsg == UI_MSG_QUERYINTERFACE) {                                         \
    SetMsgHandled(true);                                                       \
    lResult = func(*(const IID *)wParam);                                      \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

//
//	测试一个坐标在指定的对象上面的位置
//
//		message: UI_WM_HITTEST
//		code:
//		wparam:  [in]  Point*  ptInParent
//		lparam:  [out,optional] Point*  ptInChild
//(例如减去自己的左上角偏移) 		pMsgFrom:
//
//	Return
//
//		HTNOWHERE表示不在该对象上面
//
#define UI_MSG_HITTEST 168261616
// long*  OnHitTest(Point* ptInParent, Point* ptInChild)
#define UIMSG_HITTEST(func)                                                    \
  if (uMsg == UI_MSG_HITTEST) {                                                \
    Point ptInParent = *(Point *)wParam;                                       \
    SetMsgHandled(true);                                                       \
    lResult = func(&ptInParent, (Point *)lParam);                              \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

//	祖、父对象中的可见状态发生了改变（主要是用于在父对象隐藏时，能自动将HwndObj对象也隐藏）
//		message: UI_WM_PARENT_VISIBLE_CHANGED
//		code:
//		wparam:  bool bVisible, (保持与WM_SHOWWINDOW一致)
//		lparam:  Object*
#define UI_MSG_VISIBLE_CHANGED 168261620

// void  OnVisibleChanged(bool bVisible, IObject* pObjChanged)
#define UIMSG_VISIBLE_CHANGED(func)                                            \
  if (uMsg == UI_MSG_VISIBLE_CHANGED) {                                        \
    SetMsgHandled(true);                                                       \
    func(wParam ? true : false, (ui::IObject *)lParam);                        \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

//
//	MouseManager通知一个对象，它的当前状态发生改变(hover,press...)
//  专门用于刷新处理，不要在这个消息中添加其它逻辑处理，因为可能会被HOOK掉，导致状态不正确
//
//    UI_WM_STATECHANGED,

// wParam: nChangeMask
#define UI_MSG_STATECHANGED 168261626
// void  OnStateChanged(unsigned int nMask);
#define UIMSG_STATECHANGED(func)                                               \
  if (uMsg == UI_MSG_STATECHANGED) {                                           \
    SetMsgHandled(true);                                                       \
    func((unsigned int)wParam);                                                \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// void  OnPaint(IRenderTarget* pRenderTarget)
#define UIMSG_PAINT(func)                                                      \
  if (uMsg == WM_PAINT) {                                                      \
    SetMsgHandled(true);                                                       \
    func((ui::IRenderTarget *)wParam);                                         \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// void  OnEraseBkgnd(ui::IRenderTarget* pRenderTarget)
//	remark
//		参数HDC，是已经做过了双缓冲处理了，可以直接使用。原始的WM_ERASEBKGND消息已被过滤了
//		这里去掉了返回值，以防调用者自己也不知道到底该返回什么（其实这里的返回值没有意思）
#define UIMSG_ERASEBKGND(func)                                                 \
  if (uMsg == WM_ERASEBKGND) {                                                 \
    SetMsgHandled(true);                                                       \
    func((ui::IRenderTarget *)wParam);                                         \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

//
//  如果这个对象支持布局，返回布局对象指针。如OBJ_WINDOW,OBJ_PANEL,OBJ_COMPOUND_CONTROL
//
//		message: UI_WM_GETLAYOUT
//		code:
//		wparam:
//		lparam
//
//	Return:  ILayout*
//
#define UI_MSG_GETLAYOUT 168261637

//
//	获取当前绘制的偏移量，用于OnDraw
//
//		wparam:  [out] int* xOffset
//		lparam:  [out] int* yOffset
//
//	Return
//
#define UI_MSG_GETSCROLLOFFSET 168261657
// void  OnGetScrollOffset(int* pxOffset, int* pyOffset);
#define UIMSG_GETSCROLLOFFSET(func)                                            \
  if (uMsg == UI_MSG_GETSCROLLOFFSET) {                                        \
    SetMsgHandled(true);                                                       \
    func((int *)wParam, (int *)lParam);                                        \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

//
//	获取当前滚动范围
//
//		wparam:  [out] int* xRange
//		lparam:  [out] int* yRange
//
//	Return
//
#define UI_MSG_GETSCROLLRANGE 168261658
// void  OnGetScrollRange(int* pxRange, int* pyRange);
#define UIMSG_GETSCROLLRANGE(func)                                             \
  if (uMsg == UI_MSG_GETSCROLLRANGE) {                                         \
    SetMsgHandled(true);                                                       \
    func((int *)wParam, (int *)lParam);                                        \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

//
//  父对象需要计算它的non client region(不包括padding,border)区域时，给
//  non client child object发送该消息
//       message: UI_WM_CALC_PARENT_NONCLIENTRECT
//       wparam:  Rect*  prcNonClient.
//       例如滚动条在右侧时，prcNonClient.right+=20;
//
#define UI_MSG_CALC_PARENT_NONCLIENTRECT 168261659

// void OnCalcParentNonClientRect(Rect* prcNonClientRegion)
#define UIMSG_CALCPARENTNONCLIENTRECT(func)                                    \
  if (uMsg == UI_MSG_CALC_PARENT_NONCLIENTRECT) {                              \
    SetMsgHandled(true);                                                       \
    func((Rect *)wParam);                                                      \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// void  OnPaint(IRenderTarget* pRenderTarget)
#define UIMSG_PAINT(func)                                                      \
  if (uMsg == WM_PAINT) {                                                      \
    SetMsgHandled(true);                                                       \
    func((ui::IRenderTarget *)wParam);                                         \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// 控件在自己的子对象画完之后，再接受一个消息用于后处理
// 需要控件有post_paint样式
#define UI_MSG_POSTPAINT 147281159
// void OnPostPaint(IRenderTarget* pRenderTarget)
#define UIMSG_POSTPAINT(func)                                                  \
  if (uMsg == UI_MSG_POSTPAINT) {                                              \
    SetMsgHandled(true);                                                       \
    func((ui::IRenderTarget *)wParam);                                         \
    lResult = 0;                                                               \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// void  OnEraseBkgnd(ui::IRenderTarget* pRenderTarget)
#define UIMSG_ERASEBKGND(func)                                                 \
  if (uMsg == WM_ERASEBKGND) {                                                 \
    SetMsgHandled(true);                                                       \
    func((ui::IRenderTarget *)wParam);                                         \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

//
//	在ui中实现post message（稍后再响应）
//	见CForwardPostMessageWindow
//
#define UI_MSG_POSTMESSAGE (WM_USER + 829)

// long OnMessageHandlerEX(unsigned int uMsg, long wParam, long lParam)
#define UIMSG_HANDLER_EX(msg, func)                                            \
  if (uMsg == msg) {                                                           \
    SetMsgHandled(true);                                                       \
    lResult = func(uMsg, wParam, lParam);                                      \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

struct GETDESIREDSIZEINFO {
  wchar_t *szText;
  int nLimitWidth;
};

//
//  获取对象期望大小，不包含对象的Margin，但需要自己去计算padding/border等
//
//  wparam : [out] Size*
//  lparam : GETDESIREDSIZEINFO* (目前仅在ITextRenderBase值不为nullptr)
//
#define UI_MSG_GETDESIREDSIZE 168261803

// void  GetDesiredSize(Size* pSize);
#define UIMSG_GETDESIREDSIZE(func)                                             \
  if (uMsg == UI_MSG_GETDESIREDSIZE) {                                         \
    SetMsgHandled(true);                                                       \
    func((Size *)wParam);                                                      \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// void GetDesiredSize(Size* pSize, GETDESIREDSIZEINFO* pInfo);
#define UIMSG_GETDESIREDSIZE2(func)                                            \
  if (uMsg == UI_MSG_GETDESIREDSIZE) {                                         \
    SetMsgHandled(true);                                                       \
    func((Size *)wParam, (GETDESIREDSIZEINFO *)lParam);                        \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

//
//  RenderBase绘制消息
//
//  message : UI_WM_RENDERBASE_DRAWSTATE
//  code : NA
//  wparam :
//  lparam : RENDERBASE_DRAWSTATE*
//
#define UI_MSG_RENDERBASE_DRAWSTATE 168261825

// void DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
#define UIMSG_RENDERBASE_DRAWSTATE(func)                                       \
  if (uMsg == UI_MSG_RENDERBASE_DRAWSTATE) {                                   \
    SetMsgHandled(true);                                                       \
    func((RENDERBASE_DRAWSTATE *)wParam);                                      \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// void OnDrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct);
#define UIMSG_TEXTRENDERBASE_DRAWSTATE(func)                                   \
  if (uMsg == UI_MSG_RENDERBASE_DRAWSTATE) {                                   \
    SetMsgHandled(true);                                                       \
    func((TEXTRENDERBASE_DRAWSTATE *)wParam);                                  \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// 设置窗口上的默认按钮 (该消息不负责刷新对象)
// message:  UI_DM_SETDEFID
// wParam:   IObject*
#define UI_DM_SETDEFID 138011443

// void OnSetDefId(IObject* pButton);
#define UIMSG_DM_SETDEFID(func)                                                \
  if (uMsg == UI_DM_SETDEFID) {                                                \
    SetMsgHandled(true);                                                       \
    func((ui::IObject *)wParam);                                               \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// 获取窗口上的默认按钮
// message: UI_DM_GETDEFID
// return: IObject*
#define UI_DM_GETDEFID 138011444

// IObject* OnGetDefId();
#define UIMSG_DM_GETDEFID(func)                                                \
  if (uMsg == UI_DM_GETDEFID) {                                                \
    SetMsgHandled(true);                                                       \
    lResult = (long)func();                                                    \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

//
//  修改创建窗口数据
//
//  message: CREATESTRUCT*
//
#define UI_MSG_PRECREATEWINDOW 168270752

// bool  PreCreateWindow(CREATESTRUCT* pcs);
#define UIMSG_PRECREATEWINDOW(func)                                            \
  if (uMsg == UI_MSG_PRECREATEWINDOW) {                                        \
    SetMsgHandled(true);                                                       \
    lResult = func((CREATESTRUCT *)wParam);                                    \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

//  获取窗口的透明类型，用于判断当前窗口是分层的，还是aero
//  wparam:
//  lparam:
//  Return:
//      WINDOW_TRANSPARENT_MODE
//
#define UI_MSG_GET_WINDOW_TRANSPARENT_MODE 132831133

// ui::WINDOW_TRANSPARENT_MODE  GetTransparentMode()
#define UIMSG_GET_WINDOW_TRANSPARENT_MODE(func)                                \
  if (uMsg == UI_MSG_GET_WINDOW_TRANSPARENT_MODE) {                            \
    SetMsgHandled(true);                                                       \
    lResult = (long)func();                                                    \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

//
//  加载完所有控件后，由窗口发起的初始化
//
//  初始化顺序：
//    1. 初始化自己UI_WM_INITIALIZE，
//    2. 初始化自己的子对象
//    3. 给自己再发送一个UI_WM_INITIALIZE2消息表示子对象初始化完成
//    4. 给自己再发送一个UI_MSG_BINDPLZ消息,表示可以去绑定子控件了
//    每次换布局（换肤）后，会再发送一次UI_MSG_BINDPLZ通知，但init消息不会再发送了
//    换肤前会给每个控件发送一个UI_MSG_UNBINDPLZ通知，用于取消各种绑定
//
#define UI_MSG_INITIALIZE 155051209
#define UI_MSG_INITIALIZE2 155051210
#define UI_MSG_DOBINDPLZ 174191106 // wParam: true绑定， false取消绑定

// void  OnInitialize();
#define UIMSG_INITIALIZE(func)                                                 \
  if (uMsg == UI_MSG_INITIALIZE) {                                             \
    SetMsgHandled(true);                                                       \
    func();                                                                    \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// void  OnInitialize2();
#define UIMSG_INITIALIZE2(func)                                                \
  if (uMsg == UI_MSG_INITIALIZE2) {                                            \
    SetMsgHandled(true);                                                       \
    func();                                                                    \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// void  DoBindPlz(bool bind);
#define UIMSG_DOBINDPLZ(func)                                                  \
  if (uMsg == UI_MSG_DOBINDPLZ) {                                              \
    SetMsgHandled(true);                                                       \
    func(wParam ? true : false);                                               \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

#define UI_MSG_GETRENDERFONT 168271806
//
// 获取Render Font
//
// IRenderFont*  OnGetFont()
#define UIMSG_GETRENDERFONT(func)                                              \
  if (uMsg == UI_MSG_GETRENDERFONT) {                                          \
    SetMsgHandled(true);                                                       \
    lResult = (long)(long *)(IRenderFont *)func();                             \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

#pragma region // 换肤
//
//  在窗口换肤之前，给窗口发送一个通知。窗口可以在这里阻止自己参与换肤
//		message: UI_WM_SKINCHANGING
//		code:
//		wparam:  bool* pbChangeSkin
//		lparam:
//
//	Return:
//
#define UI_MSG_SKINCHANGING 168271815

//
//	在窗口换肤完成后，给窗口发送一个通知
//
//		message: UI_WM_SKINCHANGED
//		code:
//		wparam:
//		lparam:
//
//	Return:
//
#define UI_MSG_SKINCHANGED 168271814

//
//	在窗口的HLS变化后，给窗口发送一个通知
//
#define UI_MSG_SKINHLSCHANGED 168271813

//
//	IRenderFont字体被修改后给listener发出来的通知事件
//
//		message: UI_WM_FONTMODIFIED
//		code:
//		wparam:  IRenderFont*
//		lparam:
//
//	Return:
//
//    UI_WM_FONTMODIFIED, -- 机制做的不好，废弃

// 图片换肤
#define UI_MSG_SKINTEXTURECHANGED 168271812

#pragma endregion

// 返回值：滚动达到边缘，需要进行边缘回弹动画
#define GESTURE_RETURN_NEED_BOUNCE_EDGE 2

// 触摸消息：平移
// wParam: MAKEWPARAM(xOffset, yOffset)
// lParam: ui::GESTUREINFO*
// return: HANDLED | GESTURE_RETURN_NEED_BOUNCE_EDGE
#define UI_WM_GESTURE_PAN 151221941

// wParam: MAKEWPARAM(xOffset, yOffset)
// lParam: ui::GESTUREINFO*
// return: HANDLED | GESTURE_RETURN_NEED_BOUNCE_EDGE2
#define UI_WM_GESTURE_PRESSANDTAP 151221942

//
// 开始触摸前，向被触摸的对象发送请求消息是否需要手势的消息。
// 如果自己不需要，则继续向该对象的父对象进行请求。
//
// wParam: GESTUREINFO*
// return: 0表示不需要，1表示需要该消息
//
#define UI_WM_GESTURE_BEGIN_REQ 152270927

// windows接收到mousewheel之后，先发送给
// press object，询问当前是否允许mousewheel
//
// wParam/lParam: WM_MOUSEWHEEL消息参数
//
// return 0 表示pressobj没处理，允许mousewheel
// return 1 表示pressobj已处理，不允许分发mousewheel
//
#define UI_MSG_MOUSEWHEEL_REQ 142841001

//
//  gif刷新的通知
//  wParam:  GifImageRender*
//  lParam:  notify.lparam
#define UI_MSG_GIFFRAME_TICK 168281226
// void  OnGifframeTick(long wParam, long lParam)
#define UIMSG_WM_GIFFRAME_TICK(func)                                           \
  if (uMsg == UI_MSG_GIFFRAME_TICK) {                                          \
    SetMsgHandled(true);                                                       \
    func(wParam, lParam);                                                      \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

namespace ui {
struct IUIAccessible;
struct IUIAccessibleCreator {
  virtual void Add(IUIAccessible *) = 0;
};
}
// 创建控件的IAccessible
// long: IUIAccessibleCreator*
#define UI_WM_CREATE_ACCESSIBLE 155042030

// void  OnCreateAccessible(IUIAccessibleCreator* p)
#define UIMSG_CREATE_ACCESSIBLE(func)                                          \
  if (uMsg == UI_WM_CREATE_ACCESSIBLE) {                                       \
    SetMsgHandled(true);                                                       \
    func((ui::IUIAccessibleCreator *)wParam);                                  \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// void  OnSetFocus(IObject* pOldFocusObj)
#define UIMSG_SETFOCUS(func)                                                   \
  if (uMsg == WM_SETFOCUS) {                                                   \
    SetMsgHandled(true);                                                       \
    func((IObject *)wParam);                                                   \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// void  OnKillFocus(IObject* pNewFocusObj)
#define UIMSG_KILLFOCUS(func)                                                  \
  if (uMsg == WM_KILLFOCUS) {                                                  \
    SetMsgHandled(true);                                                       \
    func((IObject *)wParam);                                                   \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// void  OnHScroll(int nSBCode, int nPos, IMessage* pMsgFrom)
#define UIMSG_HSCROLL(func)                                                    \
  if (uMsg == WM_HSCROLL) {                                                    \
    SetMsgHandled(true);                                                       \
    func((int)wParam, (int)lParam, pMsgFrom);                                  \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// void  OnVScroll(int nSBCode, int nPos, IMessage* pMsgFrom)
#define UIMSG_VSCROLL(func)                                                    \
  if (uMsg == WM_VSCROLL) {                                                    \
    SetMsgHandled(true);                                                       \
    func((int)wParam, (int)lParam, pMsgFrom);                                  \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// ScrollBarMgr通知控件平滑滚动，用于控件自己刷新或者设置其它标识
// message: UI_MSG_INERTIAVSCROLL
// wParam: nOldPos
// wParam: nNewPos
#define UI_MSG_INERTIAVSCROLL 138181247

// long  OnNotify(long w, long l);
#define UIMSG_NOTIFY(_code, func)                                              \
  if (uMsg == UI_MSG_NOTIFY && _code == code) {                                \
    SetMsgHandled(true);                                                       \
    lResult = func(wParam, lParam);                                            \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }
// long  OnNotify(long w, long l);
#define UIMSG_NOTIFY_CODE_FROM(_code, _From, func)                             \
  if (uMsg == UI_MSG_NOTIFY && _code == code &&                                \
      static_cast<IMessage *>(_From) == pMsgFrom) {                            \
    SetMsgHandled(true);                                                       \
    lResult = func(wParam, lParam);                                            \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// void  OnNotify(long w, long l);
#define UIMSG_NOTIFY_NORET(_code, func)                                        \
  if (uMsg == UI_MSG_NOTIFY && _code == code) {                                \
    SetMsgHandled(true);                                                       \
    func(wParam, lParam);                                                      \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }
// void  OnNotify(long w, long l);
#define UIMSG_NOTIFY_CODE_FROM_NORET(_code, _From, func)                       \
  if (uMsg == UI_MSG_NOTIFY && _code == code &&                                \
      static_cast<IMessage *>(_From) == pMsgFrom) {                            \
    SetMsgHandled(true);                                                       \
    func(wParam, lParam);                                                      \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// void  OnNotify(long w, long l);
#define UIMSG_NOTIFY_CODE(_code, func)                                         \
  if (uMsg == UI_MSG_NOTIFY && _code == code) {                                \
    SetMsgHandled(true);                                                       \
    lResult = func(wParam, lParam);                                            \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

//	void  OnSize(unsigned int nType, int cx, int cy);
#define UIMSG_SIZE(func)                                                       \
  if (uMsg == WM_SIZE) {                                                       \
    unsigned int nType = (unsigned int)wParam;                                 \
    int cx = LOWORD(lParam);                                                   \
    int cy = HIWORD(lParam);                                                   \
    SetMsgHandled(true);                                                       \
    func(nType, cx, cy);                                                       \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// 比传递WM_TIMER增加一个LPARAM
// void  OnTimer(unsigned int* nIDEvent, long lParam)
#define UIMSG_TIMER(func)                                                      \
  if (uMsg == WM_TIMER) {                                                      \
    SetMsgHandled(true);                                                       \
    func((unsigned int *)wParam, lParam);                                      \
    lResult = 0;                                                               \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

//
//  PopupControlWindow发送给LISTBOX、MENU的消息，LISTBOX转发给COMBOBOX的消息，COMBOBOX/MENU转发给窗口的消息pMsgFrom将指向消息发送方
//
#define UI_WM_INITPOPUPCONTROLWINDOW 147301646
#define UI_WM_UNINITPOPUPCONTROLWINDOW 147301647
#define UI_WM_POPUPWINDOWDESTROY 147301648

// void OnContextMenu(HWND hWnd, Point point)
#define UIMSG_WM_CONTEXTMENU MSG_WM_CONTEXTMENU

// void OnInitPopupControlWindow()
#define UIMSG_INITPOPUPCONTROLWINDOW(func)                                     \
  if (uMsg == UI_WM_INITPOPUPCONTROLWINDOW) {                                  \
    SetMsgHandled(true);                                                       \
    func();                                                                    \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// void OnUnInitPopupControlWindow()
#define UIMSG_UNINITPOPUPCONTROLWINDOW(func)                                   \
  if (uMsg == UI_WM_UNINITPOPUPCONTROLWINDOW) {                                \
    SetMsgHandled(true);                                                       \
    func();                                                                    \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

//
// 作为目标对象的拖拽事件
//  wParam : DROPTARGETEVENT_TYPE
//  lParam : DROPTARGETEVENT_DATA*
//
#define UI_MSG_DROPTARGETEVENT 136041933

// void OnDropTargetEvent(ui::DROPTARGETEVENT_TYPE eType,
// ui::DROPTARGETEVENT_DATA* pData)
#define UIMSG_DROPTARGETEVENT(func)                                            \
  if (uMsg == UI_MSG_DROPTARGETEVENT) {                                        \
    SetMsgHandled(true);                                                       \
    func((ui::DROPTARGETEVENT_TYPE)wParam,                                     \
         (ui::DROPTARGETEVENT_DATA *)lParam);                                  \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

#define UIALT_CALLLESS 154062051

namespace ui {
struct IApplication;
struct IUIEditor;
struct IUIElement;
}
struct CREATEBYEDITORDATA {
  ui::IApplication *pUIApp;
  ui::IUIEditor *pEditor;
  ui::IUIElement *pXml;
};

// wParam: CREATEBYEDITORDATA* pData
// alt id: UIALT_MSG_MAP(UIALT_CALLLESS)
#define UI_MSG_CREATEBYEDITOR 154061549
#define UIMSG_CREATEBYEDITOR(func)                                             \
  if (uMsg == UI_MSG_CREATEBYEDITOR) {                                         \
    SetMsgHandled(true);                                                       \
    func((CREATEBYEDITORDATA *)wParam);                                        \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

// 回车，作用到了默认按钮上面
#define UI_MSG_DEFAULTBUTTON_VKRETURN_EVENT 169281816

#endif