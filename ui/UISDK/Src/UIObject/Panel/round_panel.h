#pragma once
#include "panel.h"
#include "Src\Atl\image.h"

// 2016.4.1  libo 
// ʵ��Բ��Panel���Ծ����ٵ��ڴ�ռ��ʵ�֡�
//
// ԭ��
//   ������ǰ�����ݻ���һ��������buffer�ϣ�Ȼ���buffer��roundrect�����
//   ��buffer�ύ��dc�ϵķ�����
//
//   1. �ڻ�panel����֮ǰ�����ĸ�Բ�ǵĸ��������ݻ���������
//   2. ��panel���ݣ�
//   3. ��panel������ֱ����roundrect���������»��棻
//   4. ��panel���ĸ���Բ������֮ǰ����ĸ�����������ȥ����ʱ��������ϵ�
//      ���ݽ����ĸ�Բ�ǵ�������ʾ��
//   5. �������ϵ��ĸ�Բ�ǻ�ԭ��dc��ȥ��
//      ���
//

// ע��back.render�����ݲ�����Բ�ǻ���fore.render+child�����ݻᱻԲ�ǻ�

namespace UI
{

class RoundPanel : public Panel
{
public:
    RoundPanel(IRoundPanel*);
    ~RoundPanel();

	UI_BEGIN_MSG_MAP()
		UIMSG_PAINT(OnPaint)
        UIMSG_POSTPAINT(OnPostPaint)
        UIMSG_QUERYINTERFACE(RoundPanel)
		UIMSG_SERIALIZE(OnSerialize)
	UI_END_MSG_MAP_CHAIN_PARENT(Panel)

public:
    void  PrePaint(HDC hDC, int width, int height);
    void  PostPaint(HDC hDC, int width, int height);

    void  SetRadius(
            ULONG lefttop, ULONG righttop,
            ULONG leftbottom, ULONG rightbottom);

protected:
	void  OnSerialize(SERIALIZEDATA* pData);
	void  OnPaint(IRenderTarget* pRenderTarget);
	void  OnPostPaint(IRenderTarget* pRenderTarget);

private:
    ULONG  m_lefttop;
    ULONG  m_righttop;
    ULONG  m_leftbottom;
    ULONG  m_rightbottom;

	IRoundPanel*  m_pIRoundPanel;
};

// ���𱣴�ԭʼ���ݡ�����panelԲ�����ݡ����������ص�������ȥ
class BufferSwitcher
{
public:
    BufferSwitcher();
    ~BufferSwitcher();

    void  SaveLeftTop(HDC hDC, ULONG radius);
    void  SaveRightTop(HDC hDC, ULONG radius, int width);
    void  SaveLeftBottom(HDC hDC, ULONG radius, int height);
    void  SaveRightBottom(HDC hDC, ULONG radius, int width, int height);

    void  UpdateLeftTop(HDC hDC, ULONG radius);
    void  UpdateRightTop(HDC hDC, ULONG radius, int width);
    void  UpdateLeftBottom(HDC hDC, ULONG radius, int height);
    void  UpdateRightBottom(HDC hDC, ULONG radius, int width, int height);

    void  RestoreLeftTop(HDC hDC, ULONG radius);
    void  RestoreRightTop(HDC hDC, ULONG radius, int width);
    void  RestoreLeftBottom(HDC hDC, ULONG radius, int height);
    void  RestoreRightBottom(HDC hDC, ULONG radius, int width, int height);

    void  AddRef();
    void  Release();

private:
    void  save(UI::Image&, HDC hDC, ULONG size, int x, int y);
    void  update(UI::Image&, HDC hDC, ULONG size, int x, int y);
    void  restore(UI::Image&, HDC hDC, ULONG size, int x, int y);

private:
	UI::Image  m_lefttop;
	UI::Image  m_righttop;
	UI::Image  m_leftbottom;
	UI::Image  m_rightbottom;

    long  m_lRef;
};

}