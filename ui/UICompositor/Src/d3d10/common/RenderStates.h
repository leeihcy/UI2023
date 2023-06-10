#pragma once
#include <D3D10.h>

class RenderStates
{
public:
    static bool Init(ID3D10Device* device);
    static void Release();

    // Ĭ��״̬������͸�� 
    static ID3D10BlendState*  pBlendStateTransparent;

    // ���޸ĺ�̨���档������ֻ���ģ�建�档
    static ID3D10BlendState*  pBlendStateDisableWriteRenderTarget;
    
    // ����ģ�建��
    static ID3D10DepthStencilState*  pStencilStateDisable;

    // �޸�ģ�建�档����������/����������
    static ID3D10DepthStencilState*  pStencilStateCreateClip;			

    // ��ģ�建��Ļ����ϣ������������󡣳���������
    static ID3D10DepthStencilState*  pStencilStateClip;	

    static ID3D10RasterizerState*  pRasterizerState;
};

