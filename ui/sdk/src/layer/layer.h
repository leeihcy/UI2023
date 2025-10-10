#pragma once
#include "include/interface/irenderlayer.h"
#include "include/macro/uidefine.h"
#include "include/util/rect_region.h"
#include "transform3d.h"
#include "layer_sync_op.h"
#include "windowrender.h"

namespace ui {
class Object;
class WindowRender;
class GpuLayerCommitContext;
struct IGpuLayer;
} // namespace ui

namespace ui {

class Object;
class ListItemBase;
class Layer;

enum LayerContentType {
  LayerContentTypeNormal,
  LayerContentTypeObject,
  LayerContentTypeListItem,

};
struct ILayerContent {
  virtual LayerContentType Type() = 0;

  // layer析构了
  virtual void OnLayerDestory() = 0;

  // 创建新layer时，将layer插入到layer树中时，需要将子layer挪到自己的下面来。
  virtual bool IsChildOf(ILayerContent *) = 0;

  // 如果不可见，则不进行合成操作
  virtual bool IsVisible() = 0;
  // 获取当前缩放比例
  virtual float GetLayerScale() = 0;

  // 内容绘制
  virtual void Draw(ui::IRenderTarget *) = 0;

  // 获取自己的窗口坐标，用于合成
  virtual void GetWindowRect(Rect *prcOut) = 0;
  // 获取layer对象的父控件（不是父layer）的窗口坐标。例如要将layer剪裁到父控件以内
  virtual void GetParentWindowRect(Rect *prcOut) = 0;

  // 用于获取自己在Object tree中的位置，对应到Layer tree中的位置
  virtual Layer *GetParentLayer() = 0;
  virtual Layer *GetNextLayer() = 0;
};

struct IObjectLayerContent : public ILayerContent {
  virtual LayerContentType Type() { return LayerContentTypeObject; }
  virtual Object &GetObj() = 0;
};

struct IListItemLayerContent : public ILayerContent {
  virtual LayerContentType Type() { return LayerContentTypeListItem; }
  virtual ListItemBase &GetListItem() = 0;
};

enum LayerType {
  Layer_Software,
  Layer_Hardware,
};

class Layer : public uia::IAnimateEventCallback {
public:
  Layer(LayerType);
  virtual ~Layer();

public:
  LayerType GetType() { return m_type;}

  void Destroy();
  bool CanDestroy();
  void TryDestroy();

  void Serialize(SerializeParam* param);

  ILayer *GetILayer();
  void SetWindowRender(WindowRender *);

  IRenderTarget *GetRenderTarget();

  bool AddSubLayer(Layer *, Layer *pInsertBefore);
  void MoveLayer2NewParentEnd(Layer *pOldParent, Layer *pNewParent);
  void RemoveMeInTheTree();

  void PostCompositorRequest();
  void InvalidateForLayerAnimate(bool bUpdateNow = false);
  void Invalidate(const Rect *prcDirtyInLayer);
  void Invalidate(const Rect *prcArray, uint nCount);
  void CopyDirtyRect(RectRegion* arr);

  void SetContent(ILayerContent *);
  ILayerContent *GetContent();

  Layer *GetNext();
  Layer *GetParent() { return m_pParent; }
  Layer *GetFirstChild();

  void OnSize(uint width, uint height, float scale);

  // 	void  SetColor(color);
  // 	void  SetTransform(matrix);

  // property
  byte GetOpacity();
  void SetOpacity(byte, LayerAnimateParam *param = nullptr);
  void SetNeedClearBackground(bool b) {
    m_need_clear_background = b;
  }

  void RotateXTo(float, LayerAnimateParam *param = nullptr);
  void RotateXBy(float, LayerAnimateParam *param = nullptr);
  void RotateYTo(float, LayerAnimateParam *param = nullptr);
  void RotateZBy(float, LayerAnimateParam *param = nullptr);
  void RotateZTo(float, LayerAnimateParam *param = nullptr);
  void RotateYBy(float, LayerAnimateParam *param = nullptr);
  float GetYRotate();
  void ScaleTo(float x, float y, LayerAnimateParam *param = nullptr);

  void TranslateTo(float x, float y, float z,
                   LayerAnimateParam *param = nullptr);
  void TranslateBy(float x, float y, float z,
                   LayerAnimateParam *param = nullptr);
  void TranslateXTo(float x, LayerAnimateParam *param = nullptr);
  void TranslateYTo(float y, LayerAnimateParam *param = nullptr);

  float GetXTranslate();
  float GetYTranslate();
  float GetZTranslate();

  virtual bool UpdateDirty();
  
  void HardwareCommit(GpuLayerCommitContext *context);
  void MapView2Layer(Point *pPoint);

protected:
  virtual uia::E_ANIMATE_TICK_RESULT OnAnimateTick(uia::IStoryboard *) override;
  virtual void OnAnimateEnd(uia::IStoryboard *,
                            uia::E_ANIMATE_END_REASON e) override;
  virtual void virtualOnSize(uint nWidth, uint nHeight){};

  Object *GetLayerContentObject();

  bool hardwareUpdateDirty();
  void hardwareSyncLayerProperties();
  bool softwareUpdateDirty();
  
private:
  void on_layer_tree_changed(LayerTreeSyncOperation& op);
  uia::IStoryboard *create_storyboard(int id);

protected:
  ILayer m_iLayer;
  WindowRender *m_window_render;
  LayerType m_type;

  IRenderTarget *m_pRenderTarget;
  
  LAYERID m_layer_id = 0;

  // Layer Tree
  Layer *m_pParent;
  Layer *m_pFirstChild;
  Layer *m_pNext;
  Layer *m_pPrev;

  Size m_size;
  DirtyRegion m_dirty_region;

  ILayerContent *m_pLayerContent;

  // 将layer剪裁到父对象中(注：不是父layer中)
  bool m_bClipLayerInParentObj;

  // 统计当前动画数量,只有为0时，才可以自动销毁这个layer
  int m_nCurrentStoryboardCount;

  // 属性
  byte m_nOpacity;        // 设置的值
  byte m_nOpacity_Render; // 动画过程中的值

  // 默认不清空背景，提升效率。
  bool  m_need_clear_background = false;

  Transform3D m_transfrom3d; // 动画过程中的值
  float m_fxRotate;          // 设置的值
  float m_fyRotate;          // 设置的值
  float m_fzRotate;          // 设置的值
  float m_fxScale;           // 设置的值
  float m_fyScale;           // 设置的值
                             
  // 相对于控件m_rcParent的相对值
  float m_xTranslate;        // 设置的值
  float m_yTranslate;        // 设置的值
  float m_zTranslate;        // 设置的值
};
} // namespace ui