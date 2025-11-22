#include "gpu_layer.h"
#include "src/util.h"
#include "texture_tile.h"
#include <assert.h>
#include <cmath>

using namespace ui;

// http://www.chromium.org/developers/design-documents/gpu-accelerated-compositing-in-chrome

GpuLayer::GpuLayer() {
  m_pParent = m_pChild = m_pNext = nullptr;

  // m_pCompositor = nullptr;

#if 0
	D3DXMatrixIdentity(&m_transform);
	m_bNeedUpdateTransform = false;
    m_bTransformValid = false;

	m_fTranslationX = m_fTranslationY = m_fTranslationZ = 0;
	m_fRotationX = m_fRotationY = m_fRotationZ = 0;
	m_fScaleX = m_fScaleY = m_fScaleZ = 1;
#endif
}

GpuLayer::~GpuLayer() {
  GpuLayer *pChild = m_pChild;
  while (pChild) {
    GpuLayer *pTemp = pChild->m_pNext;
    if (pChild) {
      delete pChild;
      pChild = nullptr;
    }
    pChild = pTemp;
  }
  m_pChild = m_pNext = nullptr;
  m_pParent = nullptr;

  std::list<TextureTile *>::iterator iter = m_listTile.begin();
  for (; iter != m_listTile.end(); ++iter) {
    delete *iter;
  }
  m_listTile.clear();
}

IGpuLayer *GpuLayer::GetIGpuLayerTexture() {
  return static_cast<IGpuLayer *>(this);
}

void GpuLayer::Release() { delete this; }

void GpuLayer::SetGpuCompositor(IGpuCompositor *p) {
  m_pCompositor = p;
}

//----------------------------------------------------------------------------------------
//
// D3D11_USAGE_DYNAMIC
// Dynamic资源可以用于实现每一帧中将将数据从CPU上传到GPU
//
// A resource that is accessible by both the GPU (read only) and the CPU (write
// only). A dynamic resource is a good choice for a resource that will be
// updated by the CPU at least once per frame. To update a dynamic resource, use
// a Map method.
//
// Dynamic资源的使用方法 （How to: Use dynamic resources）
// http://msdn.microsoft.com/zh-cn/subscriptions/downloads/dn508285.aspx
//
// 资源类型 （D3D11_USAGE enumeration）
// http://msdn.microsoft.com/zh-cn/subscriptions/downloads/ff476259.aspx
//
//---------------------------------------------------------------------------------------
//
// D3D10_MAP_WRITE_DISCARD的机制：
//
// 它将抛弃原有的数据，返回一块新的空缓存给CPU，GPU就不用等待CPU了
// 因此你也不能对这块buffer使用脏矩形了，必须完整更新整个buffer. <<--
//    怪不得用脏区域更新会出现脏区域之外的数据花屏、内容丢失的问题了
//
//---------------------------------------------------------------------------------------
//

void GpuLayer::UploadBitmap(GpuUploadBitmap &bitmap) {
   if (bitmap.bpp != 32) {
    assert(false && "Support 32 bpp B8G8R8A8 only.");
    return;
  }

  // 分析受影响的 tile
  int max_x = m_arrayTile.GetCol() - 1;
  int max_y = m_arrayTile.GetRow() - 1;
  if (max_x < 0 || max_y < 0) {
    return;
  }

  for (int i = 0; i < bitmap.dirty_count; i++) {
    const Rect& dirty = bitmap.dirty_list[i];
    // ui::Log("Update Gpu Layer: %d,%d (%d,%d)", rc.left, rc.top, rc.Width(), rc.Height());


    int x_from = std::min(dirty.left / TILE_SIZE, max_x);
    int x_to = std::min((dirty.right - 1) / TILE_SIZE, max_x);
    int y_from = std::min(dirty.top / TILE_SIZE, max_y);
    int y_to = std::min((dirty.bottom - 1) / TILE_SIZE, max_y);

    ui::Rect tile_rect;
    ui::Rect dirty_of_tile;
    ui::Rect dirty_of_layer;

    for (int y = y_from; y <= y_to; y++) {
      for (int x = x_from; x <= x_to; x++) {
        tile_rect.left = x * TILE_SIZE;
        tile_rect.top = y * TILE_SIZE;
        tile_rect.right = tile_rect.left + TILE_SIZE;
        tile_rect.bottom = tile_rect.top + TILE_SIZE;

        dirty.Intersect(tile_rect, &dirty_of_layer);

        dirty_of_tile = dirty_of_layer;
        dirty_of_tile.Offset(-tile_rect.left, -tile_rect.top);

        this->UploadTileBitmap(y, x, dirty_of_tile, dirty_of_layer, bitmap);
      }
    }
  }
}
void GpuLayer::UploadTileBitmap(int row, int col, ui::Rect &dirty_of_tile,
                                ui::Rect &dirty_of_layer,
                                ui::GpuUploadBitmap &source) {
  m_arrayTile[row][col]->Upload(dirty_of_tile, dirty_of_layer, source);
}

//
//  注：现在每个分块都采用的是固定大小。位于最右侧和底部的分块也是一样的
//      大小，而不是去匹配真实的窗口大小。
//      这样做的好处是窗口改变大小时不用频繁的删除添加新分块。
//
//  (TODO: 对于窗口的layer可以这么处理，但对于小部件layer，固定大小或用于动画
//   的场景，应该可以限制大小)
//
void GpuLayer::doCreateTile(int width, int height) {
  assert(width > 0 && height > 0);

  int col = (int)ceil((float)width / TILE_SIZE);
  int row = (int)ceil((float)height / TILE_SIZE);

  if (m_arrayTile.GetCol() == col && m_arrayTile.GetRow() == row) {
    return;
  }

  int nCountNew = row * col;
  int nCountNow = m_arrayTile.GetCol() * m_arrayTile.GetRow();

  // 数量补差
  long lDiff = nCountNew - nCountNow;
  if (lDiff > 0) {
    // 添加
    for (int i = 0; i < lDiff; i++) {
      m_listTile.push_back(newTile());
    }

#if ENABLE_TRACE
    char szText[32];
    sprintf(szText, "alloc tiles: %d\r\n", lDiff);
    OutputDebugStringA(szText);
#endif
  } else if (lDiff < 0) {
    // 删除
    std::list<TextureTile *>::iterator iter = m_listTile.begin();
    for (int i = 0; i < lDiff; i++) {
      delete *iter;
      iter = m_listTile.erase(iter);
    }

#if ENABLE_TRACE
    char szText[32];
    sprintf(szText, "release tiles: %d\r\n", lDiff);
    OutputDebugStringA(szText);
#endif
  }

  // 数组位置调整
  m_arrayTile.Create(row, col);

#if ENABLE_TRACE
  char szText[32];
  sprintf(szText, "tile row=%d, col=%d\r\n", row, col);
  OutputDebugStringA(szText);
#endif

  std::list<TextureTile *>::iterator iter = m_listTile.begin();
  for (int y = 0; y < row; y++) {
    for (int x = 0; x < col; x++) {
      // 有些实现类直接在layer层实现了tile逻辑，没有具体的tile类。
      TextureTile* tile = (*iter);
      if (tile) {
        tile->SetIndex(x, y);
      }

      m_arrayTile[y][x] = tile;
      ++iter;
    }
  }
}

void GpuLayer::CalcDrawDestRect(/*__in*/ RECTF *prc, /*__out*/ RECTF *prcfOut) {
  prcfOut->left = prc->left * 2.0f / m_width - 1.0f;
  prcfOut->right = prc->right * 2.0f / m_width - 1.0f;
  prcfOut->top = 1.0f - prc->top * 2.0f / m_height;
  prcfOut->bottom = 1.0f - prc->bottom * 2.0f / m_height;
}
void GpuLayer::CalcDrawDestRect(int xDest, int yDest, int wDest, int hDest,
                                /*__out*/ RECTF *prcfOut) {
  prcfOut->left = xDest * 2.0f / m_width - 1.0f;
  prcfOut->right = (xDest + wDest) * 2.0f / m_width - 1.0f;
  prcfOut->top = 1.0f - yDest * 2.0f / m_height;
  prcfOut->bottom = 1.0f - (yDest + hDest) * 2.0f / m_height;
}
void GpuLayer::CalcDrawDestRect(float xDest, float yDest, float wDest,
                                float hDest, /*__out*/ RECTF *prcfOut) {
  prcfOut->left = xDest * 2.0f / m_width - 1.0f;
  prcfOut->right = (xDest + wDest) * 2.0f / m_width - 1.0f;
  prcfOut->top = 1.0f - yDest * 2.0f / m_height;
  prcfOut->bottom = 1.0f - (yDest + hDest) * 2.0f / m_height;
}

// void   GpuLayer::oBitBlt(int xDest, int yDest, int /*wDest*/, int
// /*hDest*/, int xSrc, int ySrc)
// {
//     oStretchBlt((float)xDest, (float)yDest, (float)m_width,
//     (float)m_height, xSrc, ySrc, m_width, m_height);
// }

#if 0
void  GpuLayerTexture::CalcTransform()
{
	if (!m_bNeedUpdateTransform)
		return;

	m_bNeedUpdateTransform = false;


    D3DXMATRIX  temp;
	D3DXMatrixIdentity(&m_transform);

	D3DXMatrixScaling(&m_transform, m_fScaleX, m_fScaleY, m_fScaleZ);

    // 单位弧度
	D3DXMatrixRotationYawPitchRoll(&temp, m_fRotationX, m_fRotationY, m_fRotationZ);
    m_transform *= temp;

	D3DXMatrixTranslation(&temp, m_fTranslationX, m_fTranslationY, m_fTranslationZ);
    m_transform *= temp;

    D3DXMATRIX  identify;
    D3DXMatrixIdentity(&identify);
    if (m_transform == identify)
    {
        m_bTransformValid = false;
    }
    else
    {
        m_bTransformValid = true;
    }
}


void  GpuLayerTexture::Translation(float xPos, float yPos, float zPos)
{
//     GpuLayerTexture* pRoot = m_pCompositor->GetRootLayerTexture();
//     if (!pRoot)
//         return;
// 
//     if (0 == pRoot->m_width || 0 == pRoot->m_height)
//         return;
// 
// 	   m_fTranslationZ = zPos;  // TODO:
// 
//     m_fTranslationX = xPos * 2.0f / pRoot->m_width - 1.0f;
//     m_fTranslationY = 1.0f - yPos * 2.0f / pRoot->m_height;

    m_fTranslationX = xPos;
    m_fTranslationY = yPos;
    m_fTranslationZ = zPos;
	m_bNeedUpdateTransform = true;

// 	TCHAR szText[32] = {0};
// 	wprintf(szText, TEXT("%f\n"), yPos);
// 	::OutputDebugString(szText);
}
void  GpuLayerTexture::TranslationBy(float xPos, float yPos, float zPos)
{
	m_fTranslationX += xPos;
	m_fTranslationY += yPos;
	m_fTranslationZ += zPos;

	m_bNeedUpdateTransform = true;
}

void  GpuLayerTexture::Rotate(float xRotate, float yRotate, float zRotate)
{
    m_fRotationX = xRotate;
    m_fRotationY = yRotate;
    m_fRotationZ = zRotate;

    m_bNeedUpdateTransform = true;
}
void  GpuLayerTexture::RotateBy(float xRotate, float yRotate, float zRotate)
{
    m_fRotationX += xRotate;
    m_fRotationY += yRotate;
    m_fRotationZ += zRotate;

    m_bNeedUpdateTransform = true;
}

void  GpuLayerTexture::Scale(float xScale, float yScale, float zScale)
{
    m_fScaleX = xScale;
    m_fScaleY = yScale;
    m_fScaleZ = zScale;

    m_bNeedUpdateTransform = true;
}
void  GpuLayerTexture::ScaleBy(float xScale, float yScale, float zScale)
{
    m_fScaleX += xScale;
    m_fScaleY += yScale;
    m_fScaleZ += zScale;

    m_bNeedUpdateTransform = true;
}
#endif