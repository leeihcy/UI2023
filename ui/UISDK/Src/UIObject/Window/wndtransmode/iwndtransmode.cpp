#include "stdafx.h"
#include "Inc\Interface\iwndtransmode.h"
#include "layered\layeredwrap.h"
#include "aero\aerowrap.h"
#include "alpha\alphawrap.h"
#include "color\colorwrap.h"
#include "anti\antiwrap.h"
#include "glow\glowwrap.h"

namespace UI
{
    
WINDOW_TRANSPARENT_TYPE  GetTransparentModeTypeFromAttr(IMapAttribute* pMapAttrib)
{
    if (!pMapAttrib)
        return WINDOW_TRANSPARENT_TYPE_INVALID;

    const TCHAR* szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_TYPE, true);
    return GetTransparentModeTypeFromAttr(szText);
}

WINDOW_TRANSPARENT_TYPE  GetTransparentModeTypeFromAttr(const TCHAR* szText)
{
    if (!szText)
        return WINDOW_TRANSPARENT_TYPE_INVALID;

    if (_T('\0') == szText[0])
        return WINDOW_TRANSPARENT_TYPE_NORMAL;

    if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_TYPE_AERO, szText))
    {
        return WINDOW_TRANSPARENT_TYPE_AERO;
    }
    else if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_TYPE_LAYERED, szText))
    {
        return WINDOW_TRANSPARENT_TYPE_LAYERED;
    }
    else if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR, szText))
    {	
        return WINDOW_TRANSPARENT_TYPE_MASKCOLOR;
    }	
    else if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA, szText))
    {
        return WINDOW_TRANSPARENT_TYPE_MASKALPHA;
    }
    else if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_TYPE_ANTIALIASING, szText))
    {
        return WINDOW_TRANSPARENT_TYPE_ANTIALIASING;
    }

    return WINDOW_TRANSPARENT_TYPE_INVALID;
}

IWindowTransparent*  CreateTransparentModeByType(WINDOW_TRANSPARENT_TYPE eType)
{
    switch (eType)
    {
    case WINDOW_TRANSPARENT_TYPE_AERO:
        {
            return static_cast<IWindowTransparent*>(new AeroWindowWrap());
        }
        break;

    case WINDOW_TRANSPARENT_TYPE_LAYERED:
        {
            return static_cast<IWindowTransparent*>(new LayeredWindowWrap());
        }
        break;

    case WINDOW_TRANSPARENT_TYPE_ANTIALIASING:
        {
            return static_cast<IWindowTransparent*>(new AntiWindowWrap());
        }
        break;;

    case WINDOW_TRANSPARENT_TYPE_MASKALPHA:
        {
            return static_cast<IWindowTransparent*>(new AlphaMaskWindowWrap());
        }
        break;;

    case WINDOW_TRANSPARENT_TYPE_MASKCOLOR:
        {
            return static_cast<IWindowTransparent*>(new ColorMaskWindowWrap());
        }
        break;

	case WINDOW_TRANSPARENT_TYPE_GLOW:
		{
			return static_cast<IWindowTransparent*>(new GlowWindowWrap());
		}
		break;

    default:
        return nullptr;
    }

    return nullptr;
}


IAeroWindowWrap::IAeroWindowWrap(AeroWindowWrap*  p)
{
    m_pImpl = p;
}

AERO_MODE  IAeroWindowWrap::GetAeroMode()
{
    return m_pImpl->GetAeroMode();
}
void  IAeroWindowWrap::GetBlurRegion(CRegion4* pregion)
{
    m_pImpl->GetBlurRegion(pregion);
}
void  IAeroWindowWrap::UpdateRgn()
{
    m_pImpl->UpdateRgn();
}
void  IAeroWindowWrap::SetTransparentMargins(RECT* prcMargin)
{
	m_pImpl->SetTransparentMargins(prcMargin);
}


//////////////////////////////////////////////////////////////////////////


ILayeredWindowWrap::ILayeredWindowWrap(LayeredWindowWrap*  p)
{
    m_pImpl = p;
}
}