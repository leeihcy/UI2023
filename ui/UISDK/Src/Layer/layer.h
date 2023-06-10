#pragma once
#include "Src\Util\RectArray\rectarray.h"
#include "Inc\Interface\irenderlayer.h"
#include "transform3d.h"

namespace UI
{
    class Object;
    class Compositor;
}

// 2016.6.4 ��д��Ⱦ����
// Ĭ����֧��Ӳ������Ϊ������Ȼ����������Ⱦ
//
// ÿ��layer����Ӧһ���Դ��ϵ������������Ӳ���ϳ�
//
// layer��������object�����Ե������ڡ�object����Ⱦ��ֱ�ӹ�ϵ��ֻ������
// �ṩlayer�������Լ���������Ϣ��

// http://ariya.ofilabs.com/2013/06/optimizing-css3-for-gpu-compositing.html
// To minimize the amount of texture uploads, you can only animate or
// transition the following properties: opacity, transform, and filter.
// Anything else may trigger a layer update. 
// ʲô���Ķ�������Ҫ�ϴ����ݵ�gpu: �޸�͸���ȡ��任���˾�??
// �任������ɫ�ǲ��еģ���Ҫ�����ϴ�����ͼ������http://codepen.io/ariya/full/xuwgy
// ���ֶ����ͻ�ܿ������ֶ�������ͨ����������ɫ���������ƣ������ϸı�����һ����͸����

// https://wiki.mozilla.org/Gecko:Layers
// Scrolling
// What should we do to scroll?
// Bas: Use a tile cache.
//
// ���⣺layerҲҪ�����ڸ�object���������ƣ�����listitem��layer��ԶҪ��listctrl���棬
//       ��listctrl��һ��������layer�����Խ�layer��Ϊһ����Ⱦ�����������⡣
// �� Compositor ��ʱ�������Ҫ���������layer������Ҫ��parent object rect������clip
//
//
//
// ����. root layer����������
// ÿ����һ��layerʱ����ȥ����������layer tree�ĵ�һ�������Ϊroot layer
//

namespace UI
{

class Object;
class ListItemBase;
class Layer;

enum LayerContentType
{
	LayerContentTypeNormal,
	LayerContentTypeObject,
	LayerContentTypeListItem,

};
interface ILayerContent
{
	virtual LayerContentType Type() = 0;

	// layer������
	virtual void  OnLayerDestory() = 0;
	
	// ������layerʱ����layer���뵽layer����ʱ����Ҫ����layerŲ���Լ�����������
	virtual bool  IsChildOf(ILayerContent*) = 0;

	// ������ɼ����򲻽��кϳɲ���
    virtual bool  IsVisible() = 0;

	// ���ݻ���
    virtual void  Draw(UI::IRenderTarget*) = 0;

	// ��ȡ�Լ��Ĵ������꣬���ںϳ�
	virtual void  GetWindowRect(RECT* prcOut) = 0;
	// ��ȡlayer����ĸ��ؼ������Ǹ�layer���Ĵ������ꡣ����Ҫ��layer���õ����ؼ�����
	virtual void  GetParentWindowRect(RECT* prcOut) = 0;

	// ���ڻ�ȡ�Լ���Object tree�е�λ�ã���Ӧ��Layer tree�е�λ��
	virtual Layer*  GetParentLayer() = 0;
	virtual Layer*  GetNextLayer() = 0;
};

interface IObjectLayerContent : public ILayerContent
{
	virtual LayerContentType Type() { return LayerContentTypeObject;  }
	virtual Object&  GetObject() = 0;
};

interface IListItemLayerContent : public ILayerContent
{
	virtual LayerContentType Type() { return LayerContentTypeListItem; }
	virtual ListItemBase& GetListItem() = 0;
};

enum LayerType
{
    Layer_Software,
    Layer_Hardware,
};

class Layer : public UIA::IAnimateEventCallback
{
protected:
	Layer();
	virtual ~Layer();

public:

	void  Destroy();
	bool  CanDestroy();
	void  TryDestroy();
	
    ILayer*  GetILayer();
    void  SetCompositorPtr(Compositor*);

    IRenderTarget*  GetRenderTarget();
   
    bool  AddSubLayer(Layer*, Layer* pInsertBefore);
    void  MoveLayer2NewParentEnd(Layer* pOldParent, Layer* pNewParent);
    void  RemoveMeInTheTree();

    void  PostCompositorRequest();
	void  InvalidateForLayerAnimate(bool bUpdateNow = false);
	void  Invalidate(LPCRECT prcDirtyInLayer);
	void  Invalidate(LPCRECT prcArray, uint nCount);
    void  CopyDirtyRect(RectArray& arr);

    void  SetContent(ILayerContent*);
    ILayerContent*  GetContent();

    Layer*  GetNext();
    Layer*  GetParent() {
               return m_pParent; }
    Layer*  GetFirstChild();

	void  OnSize(uint nWidth, uint nHeight);

// 	void  SetColor(color);
// 	void  SetTransform(matrix);

    // property
	byte  GetOpacity();
	void  SetOpacity(byte, LayerAnimateParam* param = nullptr);
    
	void  RotateXTo(float, LayerAnimateParam* param = nullptr);
	void  RotateXBy(float, LayerAnimateParam* param = nullptr);
	void  RotateYTo(float, LayerAnimateParam* param = nullptr);
	void  RotateZBy(float, LayerAnimateParam* param = nullptr);
	void  RotateZTo(float, LayerAnimateParam* param = nullptr);
	void  RotateYBy(float, LayerAnimateParam* param = nullptr);
    float  GetYRotate();
	void  ScaleTo(float x, float y, LayerAnimateParam* param = nullptr);

	void  TranslateTo(float x, float y, float z, LayerAnimateParam* param = nullptr);
	void  TranslateBy(float x, float y, float z, LayerAnimateParam* param = nullptr);
	void  TranslateXTo(float x, LayerAnimateParam* param = nullptr);
	void  TranslateYTo(float y, LayerAnimateParam* param = nullptr);

	float  GetXTranslate();
    float  GetYTranslate();
    float  GetZTranslate();

    virtual void  UpdateDirty() {}
	virtual void  MapView2Layer(POINT* pPoint){};
    virtual LayerType  GetType() = 0;

protected:
	virtual UIA::E_ANIMATE_TICK_RESULT  OnAnimateTick(UIA::IStoryboard*) override;
    virtual void  OnAnimateEnd(UIA::IStoryboard*, UIA::E_ANIMATE_END_REASON e) override;
    virtual void  virtualOnSize(uint nWidth, uint nHeight) {};

	Object*  GetLayerContentObject();

private:
    void  on_layer_tree_changed();
	UIA::IStoryboard*  create_storyboard(int id);

protected:
    ILayer  m_iLayer;
    Compositor*  m_pCompositor;

    IRenderTarget*  m_pRenderTarget;

	// Layer Tree
	Layer*  m_pParent;
	Layer*  m_pFirstChild;
	Layer*  m_pNext;
	Layer*  m_pPrev;

	// 
    SIZE  m_size;
	RectArray  m_dirtyRectangles;
	
    ILayerContent*  m_pLayerContent;

    // ��layer���õ���������(ע�����Ǹ�layer��)
    bool  m_bClipLayerInParentObj;

	// ͳ�Ƶ�ǰ��������,ֻ��Ϊ0ʱ���ſ����Զ��������layer
	int  m_nCurrentStoryboardCount;

	// ����
	byte  m_nOpacity;         // ���õ�ֵ
	byte  m_nOpacity_Render;  // ���������е�ֵ

 	Transform3D  m_transfrom3d;  // ���������е�ֵ
	float  m_fxRotate;   // ���õ�ֵ
	float  m_fyRotate;   // ���õ�ֵ
	float  m_fzRotate;   // ���õ�ֵ
	float  m_fxScale;    // ���õ�ֵ
	float  m_fyScale;    // ���õ�ֵ
	// ����ڿؼ�m_rcParent�����ֵ
    float  m_xTranslate; // ���õ�ֵ
    float  m_yTranslate; // ���õ�ֵ
    float  m_zTranslate; // ���õ�ֵ
};
}