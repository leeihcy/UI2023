#include "src/thread/paint_op.h"
#include "include/interface/renderlibrary.h"
#include "src/message_loop/message_loop.h"

namespace ui {

void PaintOp::processOnRenderThread(IRenderTarget* rt) {
  PaintOp* op = this;

  switch (op->type) {
  case PaintOpType::BeginDraw:
    rt->BeginDraw(static_cast<BeginDrawOp*>(op)->scale);
    break;
  case PaintOpType::EndDraw:
    rt->EndDraw();
    break;
  case PaintOpType::Clear:
    rt->Clear(static_cast<ClearOp*>(op)->rect);
    break;
  case PaintOpType::DrawRect: {
    auto param = static_cast<DrawRectOp*>(op);
    rt->DrawRect(param->rect, param->color);
    break;
  }
  case PaintOpType::Resize: {
    auto param = static_cast<ResizeOp*>(op);
    rt->Resize(param->width, param->height);
    break;
  }
  case PaintOpType::Save: {
    rt->Save();
    break;
  }
  case PaintOpType::Restore: {
    rt->Restore();
    break;
  }
  case PaintOpType::SetDirtyRegion: {
    rt->SetDirtyRegion(static_cast<SetDirtyRegionOp*>(op)->dirty_region);
    break;
  }
  case PaintOpType::ClipRoundRect: {
    auto param = static_cast<ClipRoundRectOp*>(op);
    rt->ClipRoundRect(param->rect, param->radius);
    break;
  }
  case PaintOpType::ClipRect: {
    auto param = static_cast<ClipRectOp*>(op);
    rt->ClipRect(param->rect);
    break;
  }
  case PaintOpType::SetOrigin: {
    auto param = static_cast<SetOriginOp*>(op);
    rt->SetOrigin(param->x, param->y);
    break;
  }
  case PaintOpType::OffsetOrigin: {
    auto param = static_cast<OffsetOriginOp*>(op);
    rt->OffsetOrigin(param->x, param->y);
    break;
  }
  case PaintOpType::DrawString: {
    auto param = static_cast<DrawStringOp*>(op);
    rt->_DrawString2(param->blob.get(), param->color, param->x, param->y);
    break;
  }
  case PaintOpType::CreateSwapChain: {
    rt->CreateSwapChain(static_cast<CreateSwapChainOp*>(op)->is_hardware);
    break;
  }
  case PaintOpType::SwapChain: {
    if (rt->SwapChain(Slot<void()>([](){}))) {
      PostTaskToUIThread(std::move(static_cast<SwapChainOp*>(op)->callback));
    }
    break;
  }
  case PaintOpType::DumpToImage: {
    rt->DumpToImage(static_cast<DumpToImageOp*>(op)->path.c_str());
    break;
  }
  case PaintOpType::RenderOnThread: {
    rt->RenderOnThread(std::move(static_cast<RenderOnThreadOp*>(op)->callback));
    break;
  }
  default: {
    assert(false && "TODO:");
  }
  }
}

}