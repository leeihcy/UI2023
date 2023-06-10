#pragma once
#include <list>
#include "texture_tile_array.h"
class TextureTile;
struct SOURCE_BITMAP_DATA;


// ÿһ��layer��Ӧһ��layerTexture��


//   -----------------   �ֿ�   --------------------
// 
// ����������˵���������Ⱦ�У���ҳ�ϵ��������ݶ������ƽ�ͬһ��λͼ��������
// �ڴ��У����������ǿ������׵��޸�����λͼ����һ������Ӳ�����ٺϳ����Ǻ�
// ���ѵģ����Ƚ�RenderLayer���Ƶ�һ��λͼ�У�Ȼ���ٽ���λͼ��Ϊһ��������
// ����GPU��RenderLayer�п��ܷǳ������Ĵ�С��Ҫȡ����������ҳ�Ĵ�С������
// ����������̫�󣬷�����ܻᳬ��GPU��������������ֵ����ʹ������Ҳ��ռ��
// �������Դ档���ƺ��ϴ�һ�ž޴������Ứ���൱����ʱ�䡣
// 
// ������������������һ�ֽ��������ֻ���ƺ��ϴ��ɼ����֡�Ҳ�����������
// ���ܲ���ʾ���ڻ����������Թ���������������������������µ����⣬ÿ
// ����ʾ���ڷ����仯���Ƕ��������»��ƺ��ϴ������⽫�����ٶȷǳ�����
// 
// ͨ���ֿ�����������������⡣ÿһ����ᱻ�и�Ϊ����256*256��С�顣����
// ֻ���ƺ��ϴ�GPU��Ҫ�Ŀ顣���ǰ���Щ�鱣����������ֻ��Ҫ�ϴ��ɼ��ĺͱ���
// �������Ŀ顣���ң����ǿ���ͨ�����ϴ�����ס�Ŀ����Ż���Ⱦ��һ������Ŀ�
// �ϴ���ɣ����ǾͿ��Խ���Щ��ϳɵ�һ��ͼƬ�С�
// 
// ����ֿ�����ĺô��ǣ�������������һ���ܴ��������Ҫ���ϴ�ʱ��Ϊ�˽�ʡ��
// ���������ֻ��Ҫ�ϴ�����ļ���С�顣����һ���ܳ�������GPU����һֱ���ţ�
// ��Ϊ�ϴ������ݱ���ֳ������顣

// 
// �ܽ᣺��DOM����Ļ 
// Chrome����ν�DOMת��Ϊ��Ļ�ϵ�ͼ����أ��Ӹ����Ͻ������� 
// 
// 1.ȡ��DOM������ֳ����ɸ��� 
// 2.�ֲ�����Щ����Ƶ����Ե����λͼ�� 
// 3.�����ƺõ�λͼ��Ϊ����������GPU֮�� 
// 4.����Щ��ͬ�Ĳ���������γ���Ļ�������ʾ����ͼ�� 
// ��Щ������Chrome��һ�β���Webҳ���֡ʱ����Ҫִ�С������ڲ�������֡ʱ��
// �Ϳ��ܻ���һЩ�ݾ��� 
// 
// . �����ĳЩCSS���Է����˸ı䣬�Ͳ���һ��Ҫ�ػ����е������ˡ�Chrome�ܹ�
//   ���Ѿ���Ϊ��������GPU֮�еĲ����������������ֻ�����������ʱ��ʹ��
//   ��ͬ��������ԣ����磬�ڲ�ͬ��λ�á��Բ�ͬ��͸��������ϵȵȣ��� 
// . ���ĳһ���е�ĳ�����ֵ����ݱ����Ч���ˣ���ô�ò����Ҫ�ػ沢���ػ�
//   ��ɺ�����������GPU�С������������Ȼ���䣬ֻ����������Է����˱仯
//  ����������λ�û���͸���ȸı��ˣ���Chrome�Ͳ����GPU�иò��λͼ���κ�
//   ����ֻ��ͨ�����½�������������µ�֡�� 

//
// ��������ߴ���GPU�������޶��ġ���Ŀǰ�������Կ���˵�����ֵһ����2048
// ����4096ÿ��ά�ȣ�ֵ�����Ѵ�ҵľ��ǣ�һ���Կ�����Ȼ�����Ͻ�������֧��
// 4096*4096*4096����ά����������ʵ�����ܵ��Կ��ڴ��С�����ƣ�һ����˵��
// ���ﲻ��������֡�
//
// �����漼����
// ������÷�ת����ʵ�ֶ�����������ʾ���뷭ת����ʱ������Ҫ�ȴ�
// ��Ļ��ɨ�����������涼��ò���д������Ϊ�˽�Լ��һ����ʱ�䣬
// ���ڵ�������������ͼ��Ȼ����������������ת������������漼����
//

//
// ��ôʹ�����������������ת���������������ԭ�㣿
/*
    // Χ�����������ת
    D3DXMATRIX matRotateCenter;
    D3DXMatrixIdentity(&matRotateCenter);
    D3DXMatrixTranslation(&matRotateCenter, -250, -200, 0);

    // ��ԭ
    D3DXMATRIX matRotateRestore;
    D3DXMatrixIdentity(&matRotateRestore);
    D3DXMatrixTranslation(&matRotateRestore, 250, 200, 0);

    // ��ת
    D3DXMATRIX matrix;
    D3DXMatrixIdentity(&matrix);
    D3DXMatrixRotationY(&matrix, fRotate);

    // ͸��ϵ��
    D3DXMATRIX  matrixPerspective;
    D3DXMatrixIdentity(&matrixPerspective);
    matrixPerspective.m[2][3] = 1.f/500.f;

    matrix = matrix*matrixPerspective;
    matrix = matRotateCenter*matrix;
    matrix = matrix*matRotateRestore;
*/
// 
// 

// CSS3 ͸�Ӳ���
// http://www.w3school.com.cn/tiy/t.asp?f=css3_perspective1

//////////////////////////////////////////////////////////////////////////
//
//   ע�� [[ �ؼ��� ]] ������1���ʵ�ֵ�
//
//       ���ʵ��һ��������Ļ�������͸����ת???
//
//  1). ����ʵ������ͶӰ��͸��ͶӰֻ�ᵼ������ԽԶԽС��D3DXMatrixOrthoOffCenterLH
//  2). �ڴ˻����ϣ����������͸�Ӳ�����m[2][3] = 1/z;
//  3). ��ת��ʽ��
//      matRotateCenter; matRotateBack; matRotate; matPerspective
//      ��������
//      matRotateCenter *  matRotate * matPerspective * matRotateBack
//
//      ��������matRotateCenter *  matRotate * matRotateBack * matPerspective
//      �����õ��Ľ��������
//    
//
//////////////////////////////////////////////////////////////////////////

namespace UI
{
class GpuComposition;

class GpuRenderLayer : public IGpuRenderLayer
{
public:
	GpuRenderLayer();
	~GpuRenderLayer();

    IGpuRenderLayer*  GetIGpuLayerTexture();
    void  Release();

    void  Compositor(GpuLayerCommitContext* pContext, float* pMatrixTransform);
    void  UploadHBITMAP(UploadGpuBitmapInfo& info);
    void  Commit();
    void  Resize(UINT nWidth, UINT nHeight);

public:
    void  SetHardwareComposition(GpuComposition* p);

protected:
    void  create_tile(ULONG row, ULONG col);

	void  upload_bitmap_rect(RECT& rc, UploadGpuBitmapInfo&);

    void  CalcDrawDestRect(__in RECTF* prc, __out RECTF* prcfOut);
    void  CalcDrawDestRect(int xDest, int yDest, UINT wDest, UINT hDest, __out RECTF* prcfOut);
    void  CalcDrawDestRect(float xDest, float yDest, float wDest, float hDest, __out RECTF* prcfOut);


protected:
    GpuComposition*  m_pCompositor;

    SIZE  m_size;  // �����С

    // �ֿ�
    list<TextureTile*>  m_listTile; // �ֿ��б���ɢ������
    TextureTile2DArray  m_arrayTile;    // ���ֿ�ָ���ų�һ����ά���飬���ڶ�λ

    // Vertex Buffer Object
    // �����е�tile��vbo����һ��buffer���������������������
    ID3D10Buffer*  m_pVertexBuffer;

    // ������
	GpuRenderLayer*  m_pParent;
	GpuRenderLayer*  m_pChild;
	GpuRenderLayer*  m_pNext;

	// ����任

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

}