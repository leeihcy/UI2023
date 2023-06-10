#pragma once
#include "compositor.h"
namespace UI
{

// 软件合成与硬件合成的区别：
// 1. 硬件合成中，每个层的数据是独享的，层与层之间不影响，在最后由gpu统一合成。
//    软件成全中，一个层上的数据包含了其所有子孙结点层的数据。子结点刷新，会影响所有的祖先结点。
//
// 2. 在设置脏区域时，软件渲染会向上进行冒泡，将祖先layer都设置上相应的脏区域，
//    这样在从上往下画的时候，就能根据当前layer是否有脏区域来进行优化了。没有脏区域的层不需要重画
//
class SoftwareCompositor : public Compositor
{
public:
    virtual Layer*  virtualCreateLayer() override;
	virtual void  virtualBindHWND(HWND) override;
    virtual void  virtualCommit(const RectArray& arrDirtyInWindow) override;
	virtual void  UpdateDirty(__out_opt  RectArray& arrDirtyInWindow) override;
	virtual void  Resize(uint nWidth, uint nSize) override;

private:
    void  update_dirty_recursion(Layer* p);
    void  commit_recursion(Layer* p);
};
}