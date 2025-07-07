#include "gpu_layer.h"
// #include "D3D10/d3dapp.h"
// #include "d3d10\common/Effects.h"
// #include "d3d10\common/RenderStates.h"
// #include "gpu_compositor.h"
// #include "hard3dtransform.h"
// #include "stdafx.h"
#include "texture_tile.h"
#include <assert.h>
#include <cmath>

using namespace ui;

// http://www.chromium.org/developers/design-documents/gpu-accelerated-compositing-in-chrome

GpuLayer::GpuLayer() {
  m_pParent = m_pChild = m_pNext = nullptr;

  m_pCompositor = nullptr;

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
// --> 解决方案：使用分块机制，将脏区域所在的块更新即可。
//
// To receive a performance improvement when you use dynamic vertex buffers,
// your app must call Map with the appropriate D3D11_MAP values. c
// D3D11_MAP_WRITE_DISCARD indicates that the app doesn't need to keep the old
// vertex or index data in the buffer. If the GPU is still using the buffer
// when you call Map with D3D11_MAP_WRITE_DISCARD, the runtime returns a
// pointer to a new region of memory instead of the old buffer data. This
// allows the GPU to continue using the old data while the app places data in
// the new buffer. No additional memory management is required in the app; the
// old buffer is reused or destroyed automatically when the GPU is finished
// with it.
//
//---------------------------------------------------------------------------------------
//

void GpuLayer::UploadHBITMAP(UploadGpuBitmapInfo &info) {
  int nCount = info.nCount;
  ui::Rect *prcArray = (ui::Rect *)info.prcArray;

  ui::Rect rcFull = {0, 0, info.width, info.height};
  if (0 == info.nCount || nullptr == info.prcArray) {
    nCount = 1;
    prcArray = &rcFull;
  }

  for (int i = 0; i < nCount; i++) {
    ui::Rect rc = rcFull;
    if (prcArray)
      rc.Intersect(prcArray[i], &rc);

    upload_bitmap_rect(rc, info);
  }
}

void GpuLayer::upload_bitmap_rect(ui::Rect &rc, UploadGpuBitmapInfo &source) {
  // 分析受影响的 tile
  // -1: 例如(0~128)，受影响的就是一个区域0，不影响区域1(128/128=1)
  int xIndexFrom = rc.left / TILE_SIZE;
  int xIndexTo = (rc.right - 1) / TILE_SIZE;
  int yIndexFrom = rc.top / TILE_SIZE;
  int yIndexTo = (rc.bottom - 1) / TILE_SIZE;

#if ENABLE_TRACE
  char szText[32];
  sprintf(szText, "Upload2Gpu: %d,%d  %d,%d\r\n", rc.left, rc.top,
          rc.right - rc.left, rc.bottom - rc.top);
  OutputDebugStringA(szText);
#endif

  ui::Rect rcSrc;
  for (int y = yIndexFrom; y <= yIndexTo; y++) {
    for (int x = xIndexFrom; x <= xIndexTo; x++) {
      rcSrc.left = x * TILE_SIZE;
      rcSrc.top = y * TILE_SIZE;
      rcSrc.right = rcSrc.left + TILE_SIZE;
      rcSrc.bottom = rcSrc.top + TILE_SIZE;

      // 超出纹理大小的区域直接忽略。这一般是针对于位于右侧和底部的
      // 那些分块
      if (rcSrc.right > m_width)
        rcSrc.right = m_width;
      if (rcSrc.bottom > m_height)
        rcSrc.bottom = m_height;

      m_arrayTile[y][x]->Upload(rcSrc, source);

#if ENABLE_TRACE
      char szText[32];
      sprintf(szText, "Update Tile: (%d,%d)\r\n", x, y);
      OutputDebugStringA(szText);
#endif
    }
  }
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
      m_arrayTile[y][x] = (*iter);
      (*iter)->SetIndex(x, y);
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
void GpuLayer::CalcDrawDestRect(int xDest, int yDest, int wDest,
                                      int hDest, /*__out*/ RECTF *prcfOut) {
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