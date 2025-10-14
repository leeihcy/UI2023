#include "layer.h"
#include "include/interface/renderlibrary.h"
#include "src/thread/render_thread.h"
#include "windowrender.h"
#include "gpu/include/api.h"
#include "include/common/math/math.h"
#include "layer_sync_op.h"
#include "src/application/config/config.h"
#include "src/application/uiapplication.h"
#include "src/attribute/attribute.h"
#include "src/object/object.h"
#include "src/object/object_layer.h"
#include <memory>

namespace ui {

static LayerAnimateParam s_defaultLayerAnimateParam;

enum LAYER_ANIMATE_TYPE {
  STORYBOARD_ID_OPACITY = 1,
  STORYBOARD_ID_XROTATE,
  STORYBOARD_ID_YROTATE,
  STORYBOARD_ID_ZROTATE,
  STORYBOARD_ID_TRANSLATE,
  STORYBOARD_ID_SCALE,
};

static LAYERID s_layerid = 0;

Layer::Layer(LayerType type) : m_iLayer(this), m_type(type) {
  m_window_render = nullptr;
  m_pRenderTarget = nullptr;

  m_layer_id = ++s_layerid;

  m_pParent = nullptr;
  m_pFirstChild = nullptr;
  m_pNext = nullptr;
  m_pPrev = nullptr;

  m_size.width = m_size.height = 0;
  m_pLayerContent = nullptr;
  m_bClipLayerInParentObj = true;
  m_nCurrentStoryboardCount = 0;

  m_nOpacity = m_nOpacity_Render = 255;
  m_fxRotate = 0;
  m_fyRotate = 0;
  m_fzRotate = 0;
  m_fxScale = 1;
  m_fyScale = 1;

  m_xTranslate = 0;
  m_yTranslate = 0;
  m_zTranslate = 0;

  // 	m_transfrom3d.perspective(1000);
  // 	m_transfrom3d.set_transform_rotate_origin(
  // 		TRANSFORM_ROTATE_ORIGIN_LEFT, 0,
  // 		TRANSFORM_ROTATE_ORIGIN_CENTER, 0, 0);

#ifdef _DEBUG
  UI_LOG_DEBUG(L"Create Layer, ptr=0x%08x", this);
#endif
}

Layer::~Layer() {
  if (m_pLayerContent) {
    m_pLayerContent->OnLayerDestory();
  }

  uia::IAnimate *pAni = m_window_render->GetUIApplication().GetAnimate();
  if (pAni) {
    pAni->ClearStoryboardByNotify(
        static_cast<uia::IAnimateEventCallback *>(this));
  }

  RemoveMeInTheTree();

  // 由每个 object 自己去销毁自己的layer
  //     while (m_pFirstChild)
  //     {
  //         Layer* p = m_pFirstChild;
  //         m_pFirstChild = m_pFirstChild->m_pNext;
  //         delete p;
  //     }

  SAFE_RELEASE(m_pRenderTarget);

#ifdef _DEBUG
  UI_LOG_DEBUG(L"Layer Destroy, ptr=0x%08x", this);
#endif
}

ILayer *Layer::GetILayer() { return &m_iLayer; }

void Layer::Serialize(SerializeParam *param) {
  AttributeSerializer s(param, "Layer");
  // s.AddInt(XML_LAYER_OPACITY, m_nOpacity)->AsData();
  s.AddBool(XML_LAYER_NEED_CLEAR, m_need_clear_background)
      ->SetDefault(false)
      ->AsData();
}

void Layer::SetWindowRender(WindowRender *p) { m_window_render = p; }

// bUpdateNow -- 场景：如果是阻塞型的动画，则要立即刷新
void Layer::InvalidateForLayerAnimate(bool bUpdateNow) {
  if (GetType() == Layer_Software) {
    this->Invalidate(nullptr);
    if (bUpdateNow) {
      m_window_render->InvalidateNow();
    }
  } else {
    hardwareSyncLayerProperties();

    DirtyRegion dirty;
    m_window_render->DirectCommit(dirty);
  }
}

void Layer::Invalidate(const Rect *prcDirtyInLayer, uint nCount) {
  for (uint i = 0; i < nCount; i++)
    Invalidate(prcDirtyInLayer + i);
}

void Layer::Invalidate(const Rect *prcDirtyInLayer) {
  Rect rcDirty = {0};

  if (!prcDirtyInLayer) {
    m_dirty_region.Destroy();

    rcDirty.Set(0, 0, m_size.width, m_size.height);
    m_dirty_region.AddRect(rcDirty);
  } else {
    if (prcDirtyInLayer->IsEmpty())
      return;

    rcDirty.CopyFrom(*prcDirtyInLayer);
    m_dirty_region.Union(*prcDirtyInLayer);
  }

  // 如果是软件渲染，向上冒泡
  if (m_pParent && GetType() == Layer_Software) {
    // rcDirty 转换成 父layer位置，并由父layer去请求合成
    Rect rcParent = {0};
    m_pParent->m_pLayerContent->GetParentWindowRect(&rcParent);

    Rect rcSelf = {0};
    m_pLayerContent->GetWindowRect(&rcSelf);

    rcDirty.Offset(rcSelf.left - rcParent.left, rcSelf.top - rcParent.top);

    m_pParent->Invalidate(&rcDirty);
  } else {
    if (m_window_render) {
      m_window_render->RequestInvalidate();
    }
  }
}

bool Layer::AddSubLayer(Layer *pLayer, Layer *pInsertBeforeChild) {
  UIASSERT(pLayer);
  UIASSERT(!pLayer->m_pParent);

  if (pLayer->m_pParent)
    return false;

  if (!m_pFirstChild) {
    m_pFirstChild = pLayer;
  } else {
    // 放在最后面
    if (!pInsertBeforeChild) {
      Layer *p = m_pFirstChild;
      while (p && p->m_pNext) {
        p = p->m_pNext;
      }

      p->m_pNext = pLayer;
      pLayer->m_pPrev = p;
    } else {
      // 作为第一个子结点
      if (m_pFirstChild == pInsertBeforeChild) {
        m_pFirstChild = pLayer;
      }

      if (pInsertBeforeChild->m_pPrev)
        pInsertBeforeChild->m_pPrev->m_pNext = pLayer;
      pLayer->m_pPrev = pInsertBeforeChild->m_pPrev;

      pInsertBeforeChild->m_pPrev = pLayer;
      pLayer->m_pNext = pInsertBeforeChild;
    }
  }

  pLayer->m_pParent = this;

  LayerTreeSyncOperation op;
  op.type = LayerTreeSyncOpType::Add;
  op.id = pLayer->m_layer_id;
  op.parent_id = m_layer_id;
  if (pInsertBeforeChild) {
    op.next_id = pInsertBeforeChild->m_layer_id;
  }

  on_layer_tree_changed(op);
  
  return true;
}

void Layer::RemoveMeInTheTree() {
  LayerTreeSyncOperation op;
  op.type = LayerTreeSyncOpType::Remove;
  op.id = m_layer_id;
  if (m_pParent) {
    op.parent_id = m_pParent->m_layer_id;
  }
  
  if (m_pPrev) {
    m_pPrev->m_pNext = m_pNext;
  } else {
    if (m_pParent)
      m_pParent->m_pFirstChild = m_pNext;
  }

  if (m_pNext)
    m_pNext->m_pPrev = m_pPrev;

  m_pParent = nullptr;
  m_pPrev = nullptr;
  m_pNext = nullptr;
  
  on_layer_tree_changed(op);
}

void Layer::MoveLayer2NewParentEnd(Layer *pOldParent, Layer *pNewParent) {
  UIASSERT(pOldParent && pNewParent);

  RemoveMeInTheTree();
  pNewParent->AddSubLayer(this, nullptr);
}

void Layer::SetContent(ILayerContent *p) { m_pLayerContent = p; }
ILayerContent *Layer::GetContent() { return m_pLayerContent; }

void Layer::on_layer_tree_changed(LayerTreeSyncOperation &op) {
  UIASSERT(m_window_render);
  if (Config::GetInstance().enable_render_thread || m_type == Layer_Hardware) {
    RenderThread::GetIntance().main.AddTask(
        ui::Slot(&WindowRenderRT::OnLayerTreeChanged,
                 m_window_render->m_rt->m_factory.get(), op));
  }
}

Layer *Layer::GetNext() { return m_pNext; }

Layer *Layer::GetFirstChild() { return m_pFirstChild; }

// width/height是逻辑单位
// scale是缩放比例
void Layer::OnSize(uint width, uint height, float scale) {
  m_size.width = width;
  m_size.height = height;

  ui::Rect rc_dirty = ui::Rect::MakeXYWH(0, 0, width, height);
  Invalidate(&rc_dirty);

  if (!m_pRenderTarget) {
    GetRenderTarget();
  }

  int scaled_width = width * scale;
  int scaled_height = height * scale;
  m_pRenderTarget->Resize(scaled_width, scaled_height);
  m_transfrom3d.set_size(scaled_width, scaled_height);

  virtualOnSize(width, height);
}

void Layer::PostCompositorRequest() { m_window_render->RequestInvalidate(); }

void Layer::SetOpacity(byte b, LayerAnimateParam *pParam) {
  if (pParam == DefaultLayerAnimateParam)
    pParam = &s_defaultLayerAnimateParam;

  // 相等也要往下去，以触发param中可能的end callback
  if (m_nOpacity == b) {
    if (pParam && pParam->GetFinishCallback()) {
      LayerAnimateFinishParam info = {0};
      info.endreason = uia::ANIMATE_END_NORMAL;
      pParam->GetFinishCallback()(info);
    }
    return;
  }
  m_nOpacity = b;

  // 开启隐式动画
  if (pParam) {
    uia::IAnimate *pAni = m_window_render->GetUIApplication().GetAnimate();

    pAni->RemoveStoryboardByNotityAndId(
        static_cast<uia::IAnimateEventCallback *>(this), STORYBOARD_ID_OPACITY);

    uia::IStoryboard *pStoryboard = create_storyboard(STORYBOARD_ID_OPACITY);

    pStoryboard->CreateTimeline(0)
        ->SetParam(m_nOpacity_Render, m_nOpacity, pParam->GetDuration())
        ->SetEaseType(pParam->GetEaseType());

    LayerAnimateParam *pSaveParam = new LayerAnimateParam;
    *pSaveParam = *pParam;
    pStoryboard->SetWParam((llong)pSaveParam);

    if (pParam->IsBlock()) {
      pStoryboard->BeginBlock();
    } else {
      pStoryboard->Begin();
    }
  } else {
    m_nOpacity_Render = m_nOpacity;
    InvalidateForLayerAnimate();
    if (m_nOpacity == 255) {
      TryDestroy();
    }
  }
}

byte Layer::GetOpacity() { return m_nOpacity_Render; }

void Layer::RotateYBy(float f, LayerAnimateParam *param) {
  RotateYTo(m_fyRotate + f, param);
}

void Layer::RotateYTo(float f, LayerAnimateParam *pParam) {
  if (pParam == DefaultLayerAnimateParam)
    pParam = &s_defaultLayerAnimateParam;

  // 相等也要往下去，以触发param中可能的end callback
  if (m_fyRotate == f) {
    if (pParam && pParam->GetFinishCallback()) {
      LayerAnimateFinishParam info = {0};
      info.endreason = uia::ANIMATE_END_NORMAL;
      pParam->GetFinishCallback()(info);
    }
    return;
  }

  m_fyRotate = f;

  uia::IAnimate *pAni = m_window_render->GetUIApplication().GetAnimate();

  pAni->RemoveStoryboardByNotityAndId(
      static_cast<uia::IAnimateEventCallback *>(this), STORYBOARD_ID_YROTATE);

  if (pParam) {
    uia::IStoryboard *pStoryboard = create_storyboard(STORYBOARD_ID_YROTATE);

    pStoryboard->CreateTimeline(0)
        ->SetParam(m_transfrom3d.get_rotateY(), m_fyRotate,
                   pParam->GetDuration())
        ->SetEaseType(pParam->GetEaseType());

    LayerAnimateParam *pSaveParam = new LayerAnimateParam;
    *pSaveParam = *pParam;
    pStoryboard->SetWParam((llong)pSaveParam);

    m_transfrom3d.perspective(2000);

    if (pParam->IsBlock()) {
      pStoryboard->BeginBlock();
    } else {
      pStoryboard->Begin();
    }
  } else {
    m_transfrom3d.rotateY(f);
  }

  InvalidateForLayerAnimate();
}

void Layer::RotateXBy(float f, LayerAnimateParam *param) {
  RotateXTo(m_fxRotate + f, param);
}

void Layer::RotateXTo(float f, LayerAnimateParam *pParam) {
  if (pParam == DefaultLayerAnimateParam)
    pParam = &s_defaultLayerAnimateParam;

  // 相等也要往下去，以触发param中可能的end callback
  if (m_fxRotate == f) {
    if (pParam && pParam->GetFinishCallback()) {
      LayerAnimateFinishParam info = {0};
      info.endreason = uia::ANIMATE_END_NORMAL;
      pParam->GetFinishCallback()(info);
    }
    return;
  }

  m_fxRotate = f;

  uia::IAnimate *pAni = m_window_render->GetUIApplication().GetAnimate();

  pAni->RemoveStoryboardByNotityAndId(
      static_cast<uia::IAnimateEventCallback *>(this), STORYBOARD_ID_XROTATE);

  if (pParam) {
    uia::IStoryboard *pStoryboard = create_storyboard(STORYBOARD_ID_XROTATE);

    pStoryboard->CreateTimeline(0)
        ->SetParam(m_transfrom3d.get_rotateX(), m_fxRotate,
                   pParam->GetDuration())
        ->SetEaseType(pParam->GetEaseType());

    LayerAnimateParam *pSaveParam = new LayerAnimateParam;
    *pSaveParam = *pParam;
    pStoryboard->SetWParam((ui::llong)pSaveParam);

    m_transfrom3d.perspective(2000);

    if (pParam->IsBlock()) {
      pStoryboard->BeginBlock();
    } else {
      pStoryboard->Begin();
    }
  } else {
    m_transfrom3d.rotateX(f);
  }

  InvalidateForLayerAnimate();
}

void Layer::RotateZBy(float f, LayerAnimateParam *param) {
  RotateZTo(m_fzRotate + f, param);
}

void Layer::RotateZTo(float f, LayerAnimateParam *pParam) {
  if (pParam == DefaultLayerAnimateParam)
    pParam = &s_defaultLayerAnimateParam;

  // 相等也要往下去，以触发param中可能的end callback
  if (m_fzRotate == f) {
    if (pParam && pParam->GetFinishCallback()) {
      LayerAnimateFinishParam info = {0};
      info.endreason = uia::ANIMATE_END_NORMAL;
      pParam->GetFinishCallback()(info);
    }
    return;
  }

  m_fzRotate = f;

  uia::IAnimate *pAni = m_window_render->GetUIApplication().GetAnimate();

  pAni->RemoveStoryboardByNotityAndId(
      static_cast<uia::IAnimateEventCallback *>(this), STORYBOARD_ID_ZROTATE);

  if (pParam) {
    uia::IStoryboard *pStoryboard = create_storyboard(STORYBOARD_ID_ZROTATE);

    pStoryboard->CreateTimeline(0)
        ->SetParam(m_transfrom3d.get_rotateZ(), m_fzRotate,
                   pParam->GetDuration())
        ->SetEaseType(pParam->GetEaseType());

    LayerAnimateParam *pSaveParam = new LayerAnimateParam;
    *pSaveParam = *pParam;
    pStoryboard->SetWParam((ui::llong)pSaveParam);

    m_transfrom3d.perspective(2000);

    if (pParam->IsBlock()) {
      pStoryboard->BeginBlock();
    } else {
      pStoryboard->Begin();
    }
  } else {
    m_transfrom3d.rotateZ(f);
  }

  InvalidateForLayerAnimate();
}

float Layer::GetYRotate() { return m_fyRotate; }

void Layer::ScaleTo(float x, float y, LayerAnimateParam *pParam) {
  if (pParam == DefaultLayerAnimateParam)
    pParam = &s_defaultLayerAnimateParam;

  // 相等也要往下去，以触发param中可能的end callback
  if (fequ(x, m_fxScale) && fequ(y, m_fyScale)) {
    if (pParam && pParam->GetFinishCallback()) {
      LayerAnimateFinishParam info = {0};
      info.endreason = uia::ANIMATE_END_NORMAL;
      pParam->GetFinishCallback()(info);
    }
    return;
  }

  m_fxScale = x;
  m_fyScale = y;

  uia::IAnimate *pAni = m_window_render->GetUIApplication().GetAnimate();

  pAni->RemoveStoryboardByNotityAndId(
      static_cast<uia::IAnimateEventCallback *>(this), STORYBOARD_ID_SCALE);

  if (pParam) {
    uia::IStoryboard *pStoryboard = create_storyboard(STORYBOARD_ID_SCALE);

    pStoryboard->CreateTimeline(0)
        ->SetParam(m_transfrom3d.get_scaleX(), m_fxScale, pParam->GetDuration())
        ->SetEaseType(pParam->GetEaseType());
    pStoryboard->CreateTimeline(1)
        ->SetParam(m_transfrom3d.get_scaleY(), m_fyScale, pParam->GetDuration())
        ->SetEaseType(pParam->GetEaseType());

    LayerAnimateParam *pSaveParam = new LayerAnimateParam;
    *pSaveParam = *pParam;
    pStoryboard->SetWParam((ui::llong)pSaveParam);

    if (pParam->IsBlock()) {
      pStoryboard->BeginBlock();
    } else {
      pStoryboard->Begin();
    }
  } else {
    m_transfrom3d.scale3d(m_fxScale, m_fyScale, 0);
  }

  InvalidateForLayerAnimate();
}

void Layer::TranslateXTo(float x, LayerAnimateParam *param) {
  TranslateTo(x, m_yTranslate, m_zTranslate, param);
}
void Layer::TranslateYTo(float y, LayerAnimateParam *param) {
  TranslateTo(m_xTranslate, y, m_zTranslate, param);
}

void Layer::TranslateBy(float x, float y, float z, LayerAnimateParam *param) {
  TranslateTo(m_xTranslate + x, m_yTranslate + y, m_zTranslate + z, param);
}
void Layer::TranslateTo(float x, float y, float z, LayerAnimateParam *pParam) {
  if (pParam == DefaultLayerAnimateParam)
    pParam = &s_defaultLayerAnimateParam;

  // 相等也要往下去，以触发param中可能的end callback
  if (m_xTranslate == x && m_yTranslate == y && m_zTranslate == z) {
    if (pParam && pParam->GetFinishCallback()) {
      LayerAnimateFinishParam info = {0};
      info.endreason = uia::ANIMATE_END_NORMAL;
      pParam->GetFinishCallback()(info);
    }
    return;
  }

  m_xTranslate = x;
  m_yTranslate = y;
  m_zTranslate = z;

  uia::IAnimate *pAni = m_window_render->GetUIApplication().GetAnimate();

  pAni->RemoveStoryboardByNotityAndId(
      static_cast<uia::IAnimateEventCallback *>(this), STORYBOARD_ID_TRANSLATE);

  if (pParam) {
    uia::IStoryboard *pStoryboard = create_storyboard(STORYBOARD_ID_TRANSLATE);

    pStoryboard->CreateTimeline(0)
        ->SetParam(m_transfrom3d.get_translateX(), m_xTranslate,
                   pParam->GetDuration())
        ->SetEaseType(pParam->GetEaseType());

    pStoryboard->CreateTimeline(1)
        ->SetParam(m_transfrom3d.get_translateY(), m_yTranslate,
                   pParam->GetDuration())
        ->SetEaseType(pParam->GetEaseType());

    pStoryboard->CreateTimeline(2)
        ->SetParam(m_transfrom3d.get_translateZ(), m_zTranslate,
                   pParam->GetDuration())
        ->SetEaseType(pParam->GetEaseType());

    LayerAnimateParam *pSaveParam = new LayerAnimateParam;
    *pSaveParam = *pParam;
    pStoryboard->SetWParam((ui::llong)pSaveParam);

    if (pParam->IsBlock()) {
      pStoryboard->BeginBlock();
    } else {
      pStoryboard->Begin();
    }
  } else {
    m_transfrom3d.translate3d(x, y, z);
  }

  if (m_pLayerContent) {
    if (GetType() == Layer_Software) {
      Object *obj = GetLayerContentObject();
      if (obj)
        obj->Invalidate();
    } else {
      m_window_render->RequestInvalidate();
    }
  }
}

float Layer::GetXTranslate() { return m_xTranslate; }
float Layer::GetYTranslate() { return m_yTranslate; }
float Layer::GetZTranslate() { return m_zTranslate; }

uia::E_ANIMATE_TICK_RESULT Layer::OnAnimateTick(uia::IStoryboard *pStoryboard) {
  LayerAnimateParam *pParam = (LayerAnimateParam *)pStoryboard->GetWParam();
  bool isblock = false;
  if (pParam)
    isblock = pParam->IsBlock();

  switch (pStoryboard->GetId()) {
  case STORYBOARD_ID_OPACITY: {
    m_nOpacity_Render = (byte)pStoryboard->GetTimeline(0)->GetCurrentIntValue();

    InvalidateForLayerAnimate(isblock);
  } break;

  case STORYBOARD_ID_XROTATE: {
    m_transfrom3d.rotateX(pStoryboard->GetTimeline(0)->GetCurrentValue());

    InvalidateForLayerAnimate(isblock);
  } break;
  case STORYBOARD_ID_YROTATE: {
    m_transfrom3d.rotateY(pStoryboard->GetTimeline(0)->GetCurrentValue());

    InvalidateForLayerAnimate(isblock);
  } break;
  case STORYBOARD_ID_ZROTATE: {
    m_transfrom3d.rotateZ(pStoryboard->GetTimeline(0)->GetCurrentValue());

    InvalidateForLayerAnimate(isblock);
  } break;

  case STORYBOARD_ID_TRANSLATE: {
    m_transfrom3d.translate3d(pStoryboard->GetTimeline(0)->GetCurrentValue(),
                              pStoryboard->GetTimeline(1)->GetCurrentValue(),
                              pStoryboard->GetTimeline(2)->GetCurrentValue());

    InvalidateForLayerAnimate(isblock);
  } break;

  case STORYBOARD_ID_SCALE: {
    m_transfrom3d.scale3d(pStoryboard->GetTimeline(0)->GetCurrentValue(),
                          pStoryboard->GetTimeline(1)->GetCurrentValue(), 0);

    InvalidateForLayerAnimate(isblock);
  } break;
  }

  return uia::ANIMATE_TICK_RESULT_CONTINUE;
}

void Layer::OnAnimateEnd(uia::IStoryboard *pStoryboard,
                         uia::E_ANIMATE_END_REASON e) {
  m_nCurrentStoryboardCount--;
  UIASSERT(m_nCurrentStoryboardCount >= 0);

  std::shared_ptr<LayerAnimateParam> pParam(
      (LayerAnimateParam *)pStoryboard->GetWParam());

  if (pStoryboard->GetId() == STORYBOARD_ID_TRANSLATE) {
    // 偏移类动画结束后，将偏移量转嫁到控件位置坐标上面..z轴仍然保留，如果有的话
    Object *obj = GetLayerContentObject();
    if (obj) {
      Rect rcParent = {0};
      obj->GetParentRect(&rcParent);

      SetPositionFlags flags;
      flags.size = false;
      obj->SetObjectPos(rcParent.left + (int)m_xTranslate,
                        rcParent.top + (int)m_yTranslate, 0, 0, flags);
    }

    m_xTranslate = 0;
    m_yTranslate = 0;
    m_transfrom3d.translate3d(0, 0, m_zTranslate);
  }
  if (pParam->GetFinishCallback()) {
    LayerAnimateFinishParam info = {0};
    info.endreason = e;
    pParam->GetFinishCallback()(info);
  }

  // !=normal时，可能是当前动画正在被新的动画取代，这个时候不去尝试销毁，由新的动画结束后触发
  if (e == uia::ANIMATE_END_NORMAL) {
    TryDestroy();
  }
}

void Layer::CopyDirtyRect(RectRegion *arr) {
  if (!arr) {
    return;
  }
  *arr = m_dirty_region;
}

static void InitLayerOnRT(LAYERID layer_id,
                          std::shared_ptr<WindowRenderRT> widnow_render,
                          bool is_hardware, IRenderTarget *render_target) {

  if (Config::GetInstance().enable_render_thread || is_hardware) {
    render_target->CreateSwapChain(is_hardware,
                                   widnow_render->m_gpu_composition.get());
  }
  if (is_hardware) {
    FrameBufferWithReadLock fb;
    render_target->GetFrontFrameBuffer(&fb);
    widnow_render->BindLayer(layer_id, fb.gpu_layer);
  }
}

IRenderTarget *Layer::GetRenderTarget() {
  if (!m_pRenderTarget) {
    if (!m_window_render)
      return nullptr;

    m_window_render->CreateRenderTarget(&m_pRenderTarget);

    bool is_hardware = m_type == Layer_Hardware;
    m_pRenderTarget->RenderOnThread(ui::Slot(&InitLayerOnRT, m_layer_id, m_window_render->m_rt, is_hardware));
  }
  return m_pRenderTarget;
}

// 强制销毁
void Layer::Destroy() { delete this; }

void Layer::TryDestroy() {
  if (CanDestroy())
    Destroy();
}
// 判断一个控件的layer当前是否可以被销毁，如果有动画，则不销毁。在动画结束后判断一次
bool Layer::CanDestroy() {
  if (m_nCurrentStoryboardCount != 0)
    return false;

  if (m_nOpacity_Render != 255)
    return false;
  if (m_nOpacity != 255)
    return false;

  if (m_xTranslate != 0)
    return false;
  if (m_yTranslate != 0)
    return false;
  if (m_zTranslate != 0)
    return false;

  if (!m_transfrom3d.is_identity())
    return false;

  Object *obj = GetLayerContentObject();
  if (obj) {
    OBJSTYLE s = {0};
    s.layer = 1;
    if (obj->TestObjectStyle(s))
      return false;
  }

  if (!IsEmptyDegree(m_fxRotate))
    return false;

  if (!IsEmptyDegree(m_fyRotate))
    return false;

  if (!IsEmptyDegree(m_fzRotate))
    return false;

  return true;
}

Object *Layer::GetLayerContentObject() {
  if (!m_pLayerContent)
    return nullptr;

  if (m_pLayerContent->Type() != LayerContentTypeObject)
    return nullptr;

  return &static_cast<IObjectLayerContent *>(m_pLayerContent)->GetObj();
}

// 本类中所有的创建动画都走这里，用于数量统计
uia::IStoryboard *Layer::create_storyboard(int id) {
  uia::IAnimate *pAni = m_window_render->GetUIApplication().GetAnimate();

  uia::IStoryboard *pStoryboard = pAni->CreateStoryboard(
      static_cast<uia::IAnimateEventCallback *>(this), id);

  m_nCurrentStoryboardCount++;
  return pStoryboard;
}

void Layer::MapView2Layer(Point *pPoint) {
  if (!m_transfrom3d.is_identity()) {
    m_transfrom3d.mappoint_view_2_layer(pPoint);
  }
}

bool Layer::UpdateDirty() {
  if (m_type == Layer_Hardware) {
    return hardwareUpdateDirty();
  } else {
    return softwareUpdateDirty();
  }
}

bool Layer::softwareUpdateDirty() {
  if (!m_pLayerContent)
    return false;

  if (!m_dirty_region.Count())
    return false;

  IRenderTarget *pRenderTarget = GetRenderTarget();

  float scale = m_pLayerContent->GetLayerScale();
  pRenderTarget->BeginDraw(scale);
  pRenderTarget->SetDirtyRegion(m_dirty_region);

  // 先begin draw，设置好缩放比例，再clear，否则clear区域不正确。
  if (m_need_clear_background) {
    uint count = m_dirty_region.Count();
    for (uint i = 0; i < count; i++)
      pRenderTarget->Clear(*m_dirty_region.GetRectPtrAt(i));
  }

  // 立即销毁无效区域，避免在Draw中再次触发Invalidate逻辑后，dirtyrect又被清空
  // 例如listitem.draw->listitem.delayop->listitem.onsize->invalidate
  m_dirty_region.Destroy();

  m_pLayerContent->Draw(pRenderTarget);
  pRenderTarget->EndDraw();

  if (!Config::GetInstance().enable_render_thread &&
      Config::GetInstance().debug.dump_render_target) {
    static int i = 0;
    char path[64];
#if defined(OS_WIN)
    sprintf(path, "D:\\images\\%p_%d.png", pRenderTarget, i++);
#else
    sprintf(path, "/tmp/images/%p_%d.png", pRenderTarget, i++);
#endif
    pRenderTarget->DumpToImage(path);
  }
  return true;
}

bool Layer::hardwareUpdateDirty() {
  if (!m_pLayerContent)
    return false;

  if (!m_dirty_region.Count())
    return false;

  IRenderTarget *pRenderTarget = GetRenderTarget();
  if (m_need_clear_background) {
    uint count = m_dirty_region.Count();
    for (uint i = 0; i < count; i++)
      pRenderTarget->Clear(*m_dirty_region.GetRectPtrAt(i));
  }

  float scale = m_pLayerContent->GetLayerScale();
  pRenderTarget->BeginDraw(scale);

  pRenderTarget->SetDirtyRegion(m_dirty_region);

  // 立即销毁无效区域，避免在Draw中再次触发Invalidate逻辑后，dirtyrect又被清空
  // 例如listitem.draw->listitem.delayop->listitem.onsize->invalidate
  m_dirty_region.Destroy();

  m_pLayerContent->Draw(pRenderTarget);
  pRenderTarget->EndDraw();

  hardwareSyncLayerProperties();
  return true;
}

// 与LayerRT同步属性。
void Layer::hardwareSyncLayerProperties() {
  LayerTreeProperties properties;
  properties.visible = m_pLayerContent->IsVisible();
  m_pLayerContent->GetWindowRect(&properties.rect_in_window);
  properties.dpi_scale = m_pLayerContent->GetLayerScale();
  properties.opacity = m_nOpacity;
  properties.m_fxRotate = m_fxRotate;
  properties.m_fyRotate = m_fyRotate;
  properties.m_fzRotate = m_fzRotate;
  properties.m_fxScale = m_fxScale;
  properties.m_fyScale = m_fyScale;
  properties.m_xTranslate = m_fyScale;
  properties.m_yTranslate = m_yTranslate;
  properties.m_zTranslate = m_zTranslate;

  if (m_bClipLayerInParentObj && m_window_render->GetRootLayer() != this) {
    m_pLayerContent->GetParentWindowRect(&properties.clip_rect);
  } else {
    m_window_render->GetRootLayer()->GetContent()->GetWindowRect(&properties.clip_rect);
  }

  RenderThread::Main::PostTask(ui::Slot(
    &WindowRenderRT::SyncLayerProperties, 
    m_window_render->m_rt->m_factory.get(),
    m_layer_id, properties
  ));
}

} // namespace ui