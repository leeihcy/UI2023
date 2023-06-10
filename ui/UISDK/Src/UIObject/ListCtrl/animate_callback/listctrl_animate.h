#pragma once

// 动画的封装实现在 UIAnimate.dll 中，uisdk.dll是依赖于 AnimateBase.dll，
// 不能直接调用 UIAnimate 模块的接口。因此在 uisdk 中预览回调接口，将实现
// 放在 UIAnimate 当中。


namespace UI
{
    class ListItemBase;
}

typedef bool (__stdcall *pfnListItemRectChangedCallback)(
                                IListItemBase& item,
                                LPCRECT prcOld, 
                                LPCRECT prcNew);
class ListCtrlAnimate
{
protected:
    ListCtrlAnimate();
    ~ListCtrlAnimate();

public:
    static ListCtrlAnimate&  Get();

    static bool  HandleItemRectChanged(
                    ListItemBase& item,
                    LPCRECT prcOld, 
                    LPCRECT prcNew);

private:
    pfnListItemRectChangedCallback  m_pfnListItemRectChangedCallback;
};


