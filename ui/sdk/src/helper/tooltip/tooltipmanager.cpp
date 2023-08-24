#include "stdafx.h"
#include "tooltipmanager.h"
// #include "src/Atl\image.h"
#include "include/interface/iobject.h"
#include "src/application/uiapplication.h"
#include "include/interface/iuiapplication.h"

#pragma region //CSystemTooltip
class CSystemTooltip : public IToolTipUI
{
public:
	CSystemTooltip() 
	{
		m_hToolTip = nullptr;
		memset(&m_toolinfo, 0, sizeof(TOOLINFO));

		m_pUIApp = nullptr;
	}
	~CSystemTooltip()
	{
		m_pUIApp = nullptr;  // ��ά��uiapp�����ü���
	}

	virtual bool  Create()
	{
		UIASSERT( m_hToolTip == nullptr );

		m_hToolTip = CreateWindowEx(WS_EX_TOPMOST|WS_EX_TRANSPARENT,
						TOOLTIPS_CLASS,
						nullptr,
						WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,		
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						nullptr,
						nullptr,
						nullptr,
						nullptr
						);

		/*
			���#define _WIN32_WINNT 0x0500����ôtooltipһ������
			����һ��#define _WIN32_WINNT 0x0501�ˣ���ʱ����tooltip.cbSize = sizeof(TOOLINFO)��ʱ��Ͳ���ʾ�ˡ�
			������Բ���ʾ�κδ��󣬵���tooltip������ʾ��������
			��ΪϵͳĬ�ϼ���comctl 5.82������汾�����tooltip��size����û��sizeof(TOOLINFO),
			����û��void *lpReserved;����cbSize����ˣ������ˡ�
			��ʱ��Ҫд��tooltip.cbSize = TTTOOLINFOA_V2_SIZE;
			������exe������ǿ��ָ��comctl 6.0����dll��ָ����Ч��
			#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")
			
			ָ��#define _WIN32_WINNT 0x0501�����³���ֻ��������xp������ϵͳ��
		*/
		m_toolinfo.cbSize = /*sizeof(TOOLINFO)*/ TTTOOLINFOA_V2_SIZE;
		m_toolinfo.uFlags = 0/* | TTF_TRACK | TTF_ABSOLUTE*/; // ע������TTF_TRACK|TTF_ABSOLUTE֮�󽫵�����ʾ��ʧȥ��ʾ����Ļ��Χ֮�ڵĹ���
		m_toolinfo.hwnd   = nullptr;
		m_toolinfo.uId    = (unsigned int)0;
		m_toolinfo.hinst  = nullptr;
		m_toolinfo.lpszText  = _T("leeihcy")/*LPSTR_TEXTCALLBACK*/;
		m_toolinfo.rect.left = m_toolinfo.rect.top = m_toolinfo.rect.bottom = m_toolinfo.rect.right = 0; 

		::SendMessage(m_hToolTip, TTM_ADDTOOL, 0, (long)&m_toolinfo);
		::SendMessage(m_hToolTip, TTM_SETMAXTIPWIDTH, 0, TOOLTIP_MAX_WIDTH);   // ��ע���������������6.0�ؼ�һ��ʹ�õĻ���������һ���ܳ��ĵ���ʱ��������max width������ʾһ��(��win7����Ч)��

		// Deleted. �����ý���Ĭ������µ�����������Ч����������û�����á�
		// ��˾����Լ�����ʵ��һ����ʾ��������ϵͳ�����ƣ�

		// �����6.0�ؼ�ʱ����ʾ�����ݲ����е����⡣�����Է�����6.0�ؼ�������margin��ֵ��������6.0���Ǿ��еģ����Բ��ܣ�
		// Ӣ�Ĳ���ϵͳ������OK�ģ������޸ġ�
		// win7��xp������Ҫ�����Ĳ�����һ�¡�
// 		if (936 == GetACP())
// 		{
// 			if (m_bUnderXpOs)
// 			{
// 				Rect rc = {1,3,0,0};
// 				::SendMessage(this->m_hToolTip, TTM_SETMARGIN, 0, (long)&rc);
// 			}
// 			else
// 			{
// 				Rect rc = {2,3,0,0};
// 				::SendMessage(this->m_hToolTip, TTM_SETMARGIN, 0, (long)&rc);
// 			}
// 		}

#if 0
		const wchar_t* szFontFace =  /*_T("����");*/_T("΢���ź�");
		// ��������Ϊ ΢���ź�
		if ( util::IsFontExist(szFontFace) )
		{
			m_hFont = ::CreateFont(
				14,//18,                        // nHeight
				0,                         // nWidth
				0,                         // nEscapement
				0,                         // nOrientation
				FW_NORMAL,                 // nWeight
				FALSE,                     // bItalic
				FALSE,                     // bUnderline
				0,                         // cStrikeOut
				ANSI_CHARSET,              // nCharSet
				OUT_DEFAULT_PRECIS,        // nOutPrecision
				CLIP_DEFAULT_PRECIS,       // nClipPrecision
				DEFAULT_QUALITY,           // nQuality
				DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				szFontFace);               // lpszFacename

			if (m_hFont)
			{
				::SendMessage(m_hToolTip, WM_SETFONT, (long)m_hFont, 0);

				Rect rcMargin;
				::SendMessage(m_hToolTip, TTM_GETMARGIN, 0, (long)(LPRECT)&rcMargin);
				int a= 0;
			}
		}
#endif
		return true;
	}
	virtual bool  Destroy()
	{
		if( nullptr != m_hToolTip )
		{
			::DestroyWindow(m_hToolTip);
			m_hToolTip = nullptr;
		}
		delete this;
		return true;
	}
	virtual bool  SetText(const wchar_t* szText)
	{
		if (nullptr == m_hToolTip)
		{
			Create();
		}

		String strTooltipText;
		FixStringWordBreakUnderXP(szText, strTooltipText);

		m_toolinfo.lpszText = (wchar_t*)strTooltipText.c_str();
		::SendMessage(m_hToolTip, TTM_UPDATETIPTEXTW, 0, (long)&m_toolinfo );
		return true;
	}
	virtual bool  SetTitle(const wchar_t* szText) 
	{
		if (nullptr == m_hToolTip)
		{
			Create(); 
		}
		return true;
	}
	virtual bool  Show(HWND hWndParent)
	{
		if (nullptr == m_hToolTip)
		{
			Create();
		}

		Point pt;
		::GetCursorPos(&pt);
		::SendMessage(m_hToolTip, TTM_TRACKPOSITION, 0, MAKELPARAM(pt.x, pt.y+22));
		::SendMessage(m_hToolTip, TTM_TRACKACTIVATE, (long)TRUE, (long)&m_toolinfo );
		return true;
	}
	virtual bool  Hide()
	{
        if (m_hToolTip && IsWindowVisible(m_hToolTip))
        {
		    ::SendMessage(this->m_hToolTip, TTM_TRACKACTIVATE, (long)FALSE, (long)&this->m_toolinfo );
        }

		// TODO: ����������Ϣ������⡢ͼ��ȣ������´ε�����ʱ������ʾ���ε�����
		return true;
	}

	virtual void  OnSerialize(SerializeParam* pData)
	{

	}
	
	virtual bool  SetUIApplication(IApplication* p)
	{
        // ������ά��UIApp�����ü���
		m_pUIApp = p;
		return true;
	}
protected:
	//
	// ��� tooltipһ����������xp���治�ᰴ��max width�Զ����е�bug
	//
	void    FixStringWordBreakUnderXP(const String& src, String& strOut)
	{
		// ֻ��XP�´�����ַ���
		if (m_pUIApp && false == m_pUIApp->IsUnderXpOS())
		{
			strOut = src;
			return ;
		}

		HFONT hFont = (HFONT)SendMessage(m_hToolTip, UI_MSG_GETRENDERFONT, 0,0);
		HDC   hDC   = Image::GetCacheDC();
		HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);
		const wchar_t* szText = src.c_str();

		int   nStart  = 0;
		int   nLength = (int)src.length(); 
		Rect  rcLimit = {0,0, TOOLTIP_MAX_WIDTH, 1};  // ���߶�����Ϊ1����֤DrawTextExֻ�����һ���ı����ַ���
		unsigned int  nDrawTextFlag = DT_EDITCONTROL|DT_WORDBREAK/*|DT_NOFULLWIDTHCHARBREAK*/;

		DRAWTEXTPARAMS  param;
		ZeroMemory(&param, sizeof(DRAWTEXTPARAMS));
		param.cbSize = sizeof(DRAWTEXTPARAMS);

		while (nStart < nLength)
		{
			// ����һ���ı��е��ַ���
			param.uiLengthDrawn = 0;
			DrawTextEx(hDC, (wchar_t*)(szText+nStart), nLength-nStart, &rcLimit, nDrawTextFlag, &param);

			strOut.append(szText+nStart, param.uiLengthDrawn);
			nStart += param.uiLengthDrawn;

			// �ֶ���ӻ��з�
			wchar_t cLast = szText[nStart-1];
			if (cLast != _T('\r') && cLast != _T('\n') && nStart < nLength)
			{
				strOut.append(_T("\n"));
			}
		}

		SelectObject(hDC, hOldFont);
		Image::ReleaseCacheDC(hDC);
	}

protected:
	HWND       m_hToolTip;
	TOOLINFO   m_toolinfo;
	bool       m_bUnderXpOs;  // �Ƿ���XPϵͳ��������WIN7�����߶���tooltip����ʾ��Щ����
	IApplication*  m_pUIApp;
};
#pragma endregion
//////////////////////////////////////////////////////////////////////////

ToolTipManager::ToolTipManager()
{
	m_pToolTipUI = nullptr;
    m_pUIApplication = nullptr;
}

ToolTipManager::~ToolTipManager()
{
	this->Release();
}

void ToolTipManager::Init(Application* p, IToolTipUI* pTooltipUI)
{
    m_pUIApplication = p;

	if (m_pToolTipUI)
	{
		m_pToolTipUI->Destroy();
	}

	if (nullptr == pTooltipUI)
		m_pToolTipUI = new CSystemTooltip;	
	else
		m_pToolTipUI = pTooltipUI;

    m_pToolTipUI->SetUIApplication(m_pUIApplication->GetIUIApplication());
    m_timeout.Init(this);
}

void ToolTipManager::Release()
{
	if (m_pToolTipUI)
	{
		m_pToolTipUI->Destroy();
		m_pToolTipUI = nullptr;
	}
}

bool ToolTipManager::Show(TOOLTIPITEM* pItemInfo)
{
 	if (nullptr == m_pToolTipUI || nullptr == pItemInfo)
		return false;

	Hide();
	m_tooltipItem.Set(pItemInfo);

	switch (pItemInfo->nToolTipFlag)
	{
	case TOOLTIP_ACTION_FLAG_NORMAL:
		{
			m_timeout.SetParam(TIMEOUT_DELAY_SHOW);
            m_timeout.Start(TOOLTIP_DELAY_SHOW_TIME);

            if (m_tooltipItem.pNotifyObj)
                m_tooltipItem.pNotifyObj->AddDelayRef((void**)&m_tooltipItem.pNotifyObj);
		}
		break;

	}
	return true;
}

bool ToolTipManager::Hide()
{
    m_timeout.Cancel();

	if (m_pToolTipUI)
	{
		m_pToolTipUI->Hide();
	}
    
    if (m_tooltipItem.pNotifyObj)
        m_tooltipItem.pNotifyObj->RemoveDelayRef((void**)&m_tooltipItem.pNotifyObj);

    m_tooltipItem.Reset();
	return true;
}

void ToolTipManager::OnTimeout(long lId, long wParam, long lParam)
{
	if (nullptr == m_pToolTipUI)
		return;

	if (lId == TIMEOUT_AUTO_HIDE)
	{
		Hide();
		return;
	}

    // ֱ��ģʽ
    if (!m_tooltipItem.strContent.empty())
    {
        UIASSERT(nullptr == m_tooltipItem.pNotifyObj);

        m_pToolTipUI->SetText(m_tooltipItem.strContent.c_str());
        m_pToolTipUI->Show(nullptr);
        m_tooltipItem.Reset();

		m_timeout.SetParam(TIMEOUT_AUTO_HIDE);
		m_timeout.Start(TOOLTIP_AUTO_HIDE_TIME);
        return;
    }

    // �п��ܶ���ɾ����
    if (!m_tooltipItem.pNotifyObj)
        return;

    m_tooltipItem.pNotifyObj->RemoveDelayRef((void**)&m_tooltipItem.pNotifyObj);    
	if (0 == UISendMessage(m_tooltipItem.pNotifyObj, UI_MSG_SHOW_TOOLTIP))
	{
        if (UISendMessage(
                m_tooltipItem.pNotifyObj, 
                UI_MSG_GET_TOOLTIPINFO, 
                (long)&m_tooltipItem,
                (long)m_pToolTipUI)
                )
        {
		    m_pToolTipUI->Show(nullptr);

			m_timeout.SetParam(TIMEOUT_AUTO_HIDE);
			m_timeout.Start(TOOLTIP_AUTO_HIDE_TIME);
        }
	}
    m_tooltipItem.Reset();
}


ToolTipItem::ToolTipItem() 
{ 
    Reset();
}
void  ToolTipItem::Reset()
{
    pNotifyObj = nullptr;
    pItemData = nullptr;
    nToolTipFlag = 0;
    strContent.clear();
}
void  ToolTipItem::Set(TOOLTIPITEM* pItem)
{
    Reset();

    if (!pItem)
        return;

    if (pItem->szToolTipContent)
    {
        strContent = pItem->szToolTipContent;
    }
    else
    {
        pNotifyObj = pItem->pNotifyObj;
        pItemData = pItem->pItemData;
        nToolTipFlag = pItem->nToolTipFlag;
    }
}