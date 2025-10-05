#pragma once
#include "gpu/include/api.h"
#include "sdk/include/common/math/rect.h"
#include "texture_tile_array.h"
#include <list>

class TextureTile;
struct SOURCE_BITMAP_DATA;

// 每一个layer对应一个layerTexture。

//   -----------------   分块   --------------------
//
// 上面我们提说过在软件渲染中，网页上的所有内容都被绘制进同一张位图（保存在
// 内存中），所以我们可以轻易地修改这张位图的任一区域。在硬件加速合成中是很
// 困难的，首先将RenderLayer绘制到一张位图中，然后再将该位图作为一个纹理上
// 传到GPU。RenderLayer有可能非常大：他的大小主要取决于整个网页的大小。我们
// 不能让纹理太大，否则可能会超过GPU所允许的纹理最大值，即使不超过也会占用
// 大量的显存。绘制和上传一张巨大的纹理会花费相当长的时间。
//
// 解决纹理过大这个问题的一种解决方案是只绘制和上传可见部分。也就是纹理绝不
// 可能比显示窗口还大，这样可以规避纹理过大的纹理。但是这又引起新的问题，每
// 次显示窗口发生变化我们都必须重新绘制和上传纹理。这将导致速度非常慢。
//
// 通过分块可以完美解决这个问题。每一个层会被切割为几个256*256的小块。我们
// 只绘制和上传GPU需要的块。我们把这些块保存在起来：只需要上传可见的和被用
// 户操作的块。而且，我们可以通过不上传被遮住的块来优化渲染。一旦所需的块
// 上传完成，我们就可以将这些块合成到一张图片中。
//
// 纹理分块带来的好处是：纹理流。当有一个很大的纹理需要被上传时，为了节省内
// 存带宽我们只需要上传所需的几个小块。对于一个很长的纹理GPU不用一直等着：
// 因为上传的内容被拆分成了许多块。

//
// 总结：从DOM到屏幕
// Chrome是如何将DOM转换为屏幕上的图像的呢？从概念上讲，它：
//
// 1.取得DOM并将其分成若干个层
// 2.分布将这些层绘制到各自的软件位图中
// 3.将绘制好的位图作为纹理上载至GPU之中
// 4.将这些不同的层组合起来形成屏幕上最后显示出的图像
// 这些步骤在Chrome第一次产生Web页面的帧时都需要执行。但是在产生随后的帧时，
// 就可能会走一些捷径：
//
// . 如果有某些CSS属性发生了改变，就并不一定要重绘所有的内容了。Chrome能够
//   将已经作为纹理保存在GPU之中的层重新组合起来，但只是在重新组合时，使用
//   不同的组合属性（比如，在不同的位置、以不同的透明度来组合等等）。
// . 如果某一层中的某个部分的内容变成无效的了，那么该层就需要重绘并在重绘
//   完成后重新上载至GPU中。如果其内容仍然不变，只是其组合属性发生了变化
//  （比如它的位置或者透明度改变了），Chrome就不会对GPU中该层的位图做任何
//   处理，只是通过重新进行组合来生成新的帧。

//
// 纹理的最大尺寸在GPU中是有限定的。就目前主流的显卡来说，这个值一般是2048
// 或者4096每个维度，值得提醒大家的就是：一块显卡，虽然理论上讲它可以支持
// 4096*4096*4096的三维浮点纹理，但实际中受到显卡内存大小的限制，一般来说，
// 它达不到这个数字。
//
// 三缓存技术：
// 如果采用翻转技术实现动画，在向显示申请翻转操作时，由于要等待
// 屏幕回扫，这两个缓存都变得不可写。有人为了节约这一丁点时间，
// 就在第三个缓存上作图，然后让这三个缓存轮转，这就是三缓存技术。
//

//
// 怎么使对象绕自身的中心旋转，而不是坐标轴的原点？
/*
    // 围绕这个中心旋转
    D3DXMATRIX matRotateCenter;
    D3DXMatrixIdentity(&matRotateCenter);
    D3DXMatrixTranslation(&matRotateCenter, -250, -200, 0);

    // 还原
    D3DXMATRIX matRotateRestore;
    D3DXMatrixIdentity(&matRotateRestore);
    D3DXMatrixTranslation(&matRotateRestore, 250, 200, 0);

    // 旋转
    D3DXMATRIX matrix;
    D3DXMatrixIdentity(&matrix);
    D3DXMatrixRotationY(&matrix, fRotate);

    // 透视系数
    D3DXMATRIX  matrixPerspective;
    D3DXMatrixIdentity(&matrixPerspective);
    matrixPerspective.m[2][3] = 1.f/500.f;

    matrix = matrix*matrixPerspective;
    matrix = matRotateCenter*matrix;
    matrix = matrix*matRotateRestore;
*/
//
//

// CSS3 透视测试
// http://www.w3school.com.cn/tiy/t.asp?f=css3_perspective1

//////////////////////////////////////////////////////////////////////////
//
//   注： 如何实现一个对象按屏幕坐标进行透视旋转???
//
//  1).
//  必须实现正交投影。透视投影只会导致物体越远越小。D3DXMatrixOrthoOffCenterLH
//  2). 在此基础上，给矩阵添加透视参数，m[2][3] = 1/z;
//  3). 自转公式：
//      matRotateCenter; matRotateBack; matRotate; matPerspective
//      流程如下
//      matRotateCenter *  matRotate * matPerspective * matRotateBack
//
//      不可以用matRotateCenter *  matRotate * matRotateBack * matPerspective
//      这样得到的结果有问题
//
//
//////////////////////////////////////////////////////////////////////////

namespace ui {

// 分块策略(TODO:)
enum GpuLayerTilePolicy {
  // 严格拆分成TILE_SIZE大小
  Restrict,

  // 当layer面积超过阈值时才按TILE_SIZE拆分。
  // 边缘部分不足TILE_SIZE时，按实际大小拆分。
  Area
};

class GpuLayer : public IGpuLayer {
public:
  GpuLayer();
  virtual ~GpuLayer();

  IGpuLayer *GetIGpuLayerTexture();
  void Release();

  void UploadBitmap(UploadGpuBitmapInfo &info);

  virtual void Compositor(GpuLayerCommitContext *pContext,
                          float *pMatrixTransform) = 0;
  virtual void Resize(int nWidth, int nHeight) = 0;
  virtual TextureTile *newTile() = 0;

public:
  // void SetGpuCompositor(IGpuCompositor *p);

protected:
  void doCreateTile(int width, int height);

  void upload_bitmap_rect(ui::Rect &rc, UploadGpuBitmapInfo &);

  void CalcDrawDestRect(/*__in*/ RECTF *prc, /*__out*/ RECTF *prcfOut);
  void CalcDrawDestRect(int xDest, int yDest, int wDest, int hDest,
                        /*__out*/ RECTF *prcfOut);
  void CalcDrawDestRect(float xDest, float yDest, float wDest, float hDest,
                        /*__out*/ RECTF *prcfOut);

protected:
  // IGpuCompositor *m_pCompositor;

  // 纹理大小
  int m_width = 0;
  int m_height = 0;

  // 分块(TODO:)
  GpuLayerTilePolicy m_tile_pocicy = GpuLayerTilePolicy::Area;
  std::list<TextureTile *> m_listTile; // 分块列表，分散创建的
  TextureTile2DArray m_arrayTile;      // 将分块指针排成一个二维数组，便于定位

  // 层链表
  GpuLayer *m_pParent;
  GpuLayer *m_pChild;
  GpuLayer *m_pNext;

  // 矩阵变换

  // 	D3DXMATRIX  m_transform;
  //     bool   m_bTransformValid;
  // 	bool   m_bNeedUpdateTransform;
  //
  // 	float  m_fTranslationX;
  // 	float  m_fTranslationY;
  // 	float  m_fTranslationZ;
  //
  // 	float  m_fRotationX;
  // 	float  m_fRotationY;
  // 	float  m_fRotationZ;
  //
  // 	float  m_fScaleX;
  // 	float  m_fScaleY;
  // 	float  m_fScaleZ;
};

} // namespace ui