#include "include/inc.h"
#include "include/util/util.h"
#include "include/util/struct.h"
#include <algorithm> // transform
#include <filesystem>
#if defined(OS_WIN)
 #include <shlwapi.h>
#else
 #include <sys/stat.h>
 #include <unistd.h>
//  #include <widec.h> 
 #include <wchar.h> 
#endif

namespace ui {
	namespace util
	{

        int wtoi(const wchar_t* text) {
#if defined(OS_WIN)
        return _wtoi(text);
#else
        // return watol(text);
        return wcstol(text, (wchar_t **)NULL, 10);
#endif
        }


		// 在向DLL之间使用stl作为参数进行传递，会导致内存释放时的崩溃。
		// 因此修改为向外部提供一个接口，外部调用完之后再将接口释放。

		class CSplitStringEnum : public ISplitStringEnum
		{
		public:
			virtual void  Release()
			{
				delete this;
			}
			virtual const wchar_t*  GetText(unsigned int nIndex)
			{
				if (nIndex >= (int)m_vecString.size())
					return nullptr;

				return m_vecString[nIndex].c_str();
			}
			virtual int  GetCount()
			{
				return (int)m_vecString.size();
			}

		public:
			std::vector<String>  m_vecString;
		};
		unsigned int  SplitString(const wchar_t* szText, wchar_t szSep, ISplitStringEnum** ppEnum)
		{
			if (!szText || !szText[0] || !ppEnum)
				return 0;

			String str = szText;
			CSplitStringEnum* pEnum = new CSplitStringEnum;

			int nIndex = 0;
			while (true)
			{
				int nResult = (int)str.find(szSep, nIndex);
				if (-1 == nResult)
				{
					pEnum->m_vecString.push_back(str.substr(nIndex, str.length() - nIndex));
					break;
				}
				else
				{
					pEnum->m_vecString.push_back(str.substr(nIndex, nResult - nIndex));
					nIndex = ++nResult;
				}
			}

			*ppEnum = pEnum;
			return (unsigned int)pEnum->m_vecString.size();
		}


		// 将宽字符转换成16进制格式的形式进行传输，例如 "李" ->"4E67"
		void  UnicodeEncode(const wchar_t*  szText, wchar_t* szEncode)
		{
			if (nullptr == szText || nullptr == szEncode)
				return;

			size_t  nByteSize = wcslen(szText) * 2;
			byte*  pbyte = (byte*)szText;

			int high = 0;
			int low = 0;

			const int digit9 = '9';
			const int A = 'A' - 10;  // 如14是E，那么就应该是 A + 4
			const int dight0 = '0';

			for (size_t i = 0; i < nByteSize; i++)
			{
				// _itow(pbyte[i], szEncode+(2*i), 16);  <--  不能使用该方法，例如a，应该是 0x3100，结果该函数会转换成3 1 0

				high = pbyte[i] & 0xF0;
				high >>= 4;
				low = pbyte[i] & 0x0F;

				szEncode[2 * i] = high > 9 ? (high + A) : (high + dight0);
				szEncode[2 * i + 1] = low > 9 ? (low + A) : (low + dight0);
			}
		}

		void  UnicodeDecode(const wchar_t* szText, wchar_t* szDecode)
		{
			if (nullptr == szText || nullptr == szDecode)
				return;

			int  nSrcLen = (int)wcslen(szText);
			nSrcLen--;  // 避免数组越界

			wchar_t* p = (wchar_t*)szText;
			byte* pDst = (byte*)szDecode;

			int high = 0;
			int low = 0;

			const int digit9 = '9';
			const int digit0 = '0';

			const int z = 'z';   // 122, 7a
			const int a = 'a';   // 122, 7a

			const int Z = 'Z';   // 90,  5a
			const int A = 'A';   // 90,  5a

			for (int i = 0, j = 0; i < nSrcLen; i += 2, j++)
			{
				int p1 = p[i];
				int p2 = p[i + 1];

				if (p1 <= digit9 && p1 >= digit0)
					high = p1 - digit0;

				else if (p1 <= Z && p1 >= A)
					high = p1 - A + 10;

				else if (p1 <= z && p1 >= a)
					high = p1 - a + 10;

				else
					continue;


				if (p2 <= digit9 && p2 >= digit0)
					low = p2 - digit0;

				else if (p2 <= Z && p2 >= A)
					low = p2 - A + 10;

				else if (p2 <= z && p2 >= a)
					low = p2 - a + 10;

				else
					continue;

				pDst[j] = (high << 4) + low;
			}
		}

		COLORREF  TranslateRGB(const wchar_t* szCol, wchar_t szSep)
		{
			if (nullptr == szCol)
				return 0;

			COLORREF col = 0;

			ISplitStringEnum* pEnum = nullptr;
			int nSize = SplitString(szCol, szSep, &pEnum);
			if (nullptr == pEnum)
				return col;

			if (3 == nSize)
			{
				col = RGB(
					wtoi(pEnum->GetText(0)),
					wtoi(pEnum->GetText(1)),
					wtoi(pEnum->GetText(2)));

				col |= 0xFF000000;  // a 默认为255;
			}

			// r g b a
			else if (4 == nSize)
			{
				col = RGB(
					wtoi(pEnum->GetText(0)),
					wtoi(pEnum->GetText(1)),
					wtoi(pEnum->GetText(2)));

				col |= wtoi(pEnum->GetText(3)) << 24;
			}

			SAFE_RELEASE(pEnum);
			return col;
		}

		byte Letter2Hex(wchar_t c)
		{
			switch (c)
			{
			case _T('0'):
				return 0;
			case _T('1'):
				return 1;
			case _T('2'):
				return 2;
			case _T('3'):
				return 3;
			case _T('4'):
				return 4;
			case _T('5'):
				return 5;
			case _T('6'):
				return 6;
			case _T('7'):
				return 7;
			case _T('8'):
				return 8;
			case _T('9'):
				return 9;
			case _T('a'):
			case _T('A'):
				return 0xa;
			case _T('b'):
			case _T('B'):
				return 0xb;
			case _T('c'):
			case _T('C'):
				return 0xc;
			case _T('d'):
			case _T('D'):
				return 0xd;
			case _T('e'):
			case _T('E'):
				return 0xe;
			case _T('f'):
			case _T('F'):
				return 0xf;
			}
			return 0;
		}
		COLORREF  TranslateHexColor(const wchar_t* szColor)
		{
			if (nullptr == szColor)
				return 0;

			size_t nLength = wcslen(szColor);

			byte r = 0, g = 0, b = 0, a = 0;
			if (3 == nLength)
			{
				byte n = Letter2Hex(szColor[0]);
				r = (n << 4) + n;

				n = Letter2Hex(szColor[1]);
				g = (n << 4) + n;

				n = Letter2Hex(szColor[2]);
				b = (n << 4) + n;

				a = 0xff;
			}
			else if (4 == nLength)
			{
				byte n = Letter2Hex(szColor[1]);
				r = (n << 4) + n;

				n = Letter2Hex(szColor[2]);
				g = (n << 4) + n;

				n = Letter2Hex(szColor[3]);
				b = (n << 4) + n;

				n = Letter2Hex(szColor[0]);
				a = (n << 4) + n;
			}
			else if (6 == nLength)
			{
				r = (Letter2Hex(szColor[0]) << 4) + Letter2Hex(szColor[1]);
				g = (Letter2Hex(szColor[2]) << 4) + Letter2Hex(szColor[3]);
				b = (Letter2Hex(szColor[4]) << 4) + Letter2Hex(szColor[5]);

				a = 0xff;
			}
			else if (8 == nLength)
			{
				// AARRGGBB
				a = (Letter2Hex(szColor[0]) << 4) + Letter2Hex(szColor[1]);
				r = (Letter2Hex(szColor[2]) << 4) + Letter2Hex(szColor[3]);
				g = (Letter2Hex(szColor[4]) << 4) + Letter2Hex(szColor[5]);
				b = (Letter2Hex(szColor[6]) << 4) + Letter2Hex(szColor[7]);
			}
			else
				return 0;

			return (a << 24) | (b << 16) | (g << 8) | r;
		}

		COLORREF  TranslateColor(const wchar_t* szColor)
		{
			if (!szColor)
				return 0;

			if (szColor[0] == _T('#')) // 16进制
				return TranslateHexColor(szColor + 1);
			else if (szColor[0] == _T('0') && szColor[1] == _T('x'))
				return TranslateHexColor(szColor + 2);
			
            // A,R,G,B形式
            return TranslateRGB(szColor);
		}

		bool  TranslateRECT(const wchar_t* szRect, Rect* pRect, wchar_t szSep)
		{
			if (nullptr == szRect)
				return false;

			ISplitStringEnum* pEnum = nullptr;
			int nSize = SplitString(szRect, szSep, &pEnum);
			if (nullptr == pEnum)
				return false;

			if (nSize >= 4)
			{
				pRect->left = wtoi(pEnum->GetText(0));
				pRect->top = wtoi(pEnum->GetText(1));
				pRect->right = wtoi(pEnum->GetText(2));
				pRect->bottom = wtoi(pEnum->GetText(3));
			}
			else if (1 == nSize)
			{
				pRect->bottom = pRect->right = pRect->top = pRect->left = wtoi(pEnum->GetText(0));
			}
			else if (2 == nSize || 3 == nSize)
			{
				pRect->right = pRect->left = wtoi(pEnum->GetText(0));
				pRect->bottom = pRect->top = wtoi(pEnum->GetText(1));
			}

			SAFE_RELEASE(pEnum);
			return true;
		}

		bool TranslateSIZE(const wchar_t* szText, Size* pSize, wchar_t szSep)
		{
			if (nullptr == szText)
				return false;

			ISplitStringEnum* pEnum = nullptr;
			int nSize = SplitString(szText, szSep, &pEnum);
			if (nullptr == pEnum)
				return false;

			if (nSize >= 2)
			{
				pSize->width = wtoi(pEnum->GetText(0));
				pSize->height = wtoi(pEnum->GetText(1));
			}
			else if (1 == nSize)
			{
				pSize->width = pSize->height = wtoi(pEnum->GetText(0));
			}
			else
			{
				pSize->width = pSize->height = 0;
			}

			SAFE_RELEASE(pEnum);
			return true;
		}


        // 用于将一个RECT缩小一个PADDING/MARGIN的大小
        void DeflatRect(Rect* pfc, const Rect* pDeflatRc)
        {
            pfc->left += pDeflatRc->left;
            pfc->top += pDeflatRc->top;
            pfc->right -= pDeflatRc->right;
            pfc->bottom -= pDeflatRc->bottom;
        }


// 		void  FormatV(wchar_t* pszFormat, va_list argList, BSTR& bstr)
// 		{
// 			int nLength = _vsctprintf(pszFormat, argList);
// 
// 			wchar_t*  szBuffer = new wchar_t[nLength + 1];
// 			::ZeroMemory(szBuffer, (nLength + 1)*sizeof(wchar_t));
// 
// 			/* 为了兼容VS2003编译，不使用_s版本 */
// 			_vstprintf_s(szBuffer, nLength + 1, pszFormat, argList);
// 			//_vstprintf( szBuffer, pszFormat, argList );
// 
// #ifdef _UNICODE
// 			bstr = ::SysAllocString(szBuffer);
// #else
// 			bstr = ::SysAllocString(CA2W(szBuffer));
// #endif
// 			delete[] szBuffer;
// 		}


		bool IsFullPath(const wchar_t* szPath)
		{
        #if defined(OS_WIN)
			return !::PathIsRelative(szPath);
        #else
            std::filesystem::path p1 = szPath;
            return p1.is_absolute();
        #endif
		}


		//
		//	获取一个路径szPath中的文件名
		//
		bool GetPathFileName(const wchar_t* szPath, wchar_t* szOutDir)
		{
			if (nullptr == szPath || nullptr == szOutDir)
				return false;

			const wchar_t* p = wcsrchr(szPath, _T('\\'));
			if (nullptr == p)
				return false;

			wcscpy(szOutDir, p + 1);
			return true;
		}


		bool GetPathFileExt(const wchar_t* szPath, wchar_t* szOutExt)
		{
			if (nullptr == szPath || nullptr == szOutExt)
				return false;

			const wchar_t* p = wcsrchr(szPath, _T('.'));
			if (nullptr == p)
				return false;

			const wchar_t* p2 = wcsrchr(szPath, _T('\\'));
			if (p2 && p < p2)
				return false;

			wcscpy(szOutExt, p + 1);
			return true;
		}

        std::string wstring2string(const wchar_t* ctext) {

            std::string temp;
            
            wchar_t* text = const_cast<wchar_t*>(ctext);
            char* old_locale = setlocale(LC_ALL, "");
            {
                size_t size = mbstowcs(text, nullptr, 0);
                char* buffer = new char[size+1];
                mbstowcs(text, buffer, size);
                temp.assign(buffer);
                delete[] buffer;
            }
            setlocale(LC_ALL, old_locale);
            return temp;
        }

        bool PathIsDirectory(const wchar_t* path) {
            if (!path) {
                return false;
            }
        #if defined(OS_WIN)
            return PathIsDirectory(szPath);
        #else
            // std::string path2 = wstring2string(path);
            
            // struct stat sbuf;
            // stat(path2.c_str(), &sbuf);
            // return S_ISDIR(sbuf.st_mode);
            std::filesystem::path p = path;
            return std::filesystem::is_directory(p);
        #endif
        }

        bool PathFileExists(const wchar_t* path) 
        {
           if (!path) {
                return false;
            }
        #if defined(OS_WIN)
            return PathFileExists(path);
        #else
            // std::string path2 = wstring2string(path);
            // return ::access(path, F_OK) == 0;
            std::filesystem::path p = path;
            return std::filesystem::exists(p);
        #endif
        }

		bool FixBitmapAlpha(FixAlphaData* pData)
		{
    #if defined(OS_WIN)
			if (!pData)
				return false;

			HBITMAP hBitmap = pData->hBitmap;
			if (!hBitmap)
			{
				if (!pData->hDC)
					return false;

				hBitmap = (HBITMAP)GetCurrentObject(pData->hDC, OBJ_BITMAP);
			}

			DIBSECTION  dib = {0};
			if (sizeof(dib) != GetObject(hBitmap, sizeof(dib), &dib))  // bm.bmBits指针数据开始的地址，但这个地址不一定是第一行，也有可能是最后一行
				return false;

			BITMAP& bm = dib.dsBm;
			if (bm.bmBitsPixel != 32)
				return false;

			Rect rcDest;
			if (pData->lprc)
			{
				Rect rcBitmap = {0, 0, bm.bmWidth, bm.bmHeight};
				if (false == ::IntersectRect(&rcDest, &rcBitmap, pData->lprc))
					return false;
			}
			else
			{
				SetRect(&rcDest, 0, 0, bm.bmWidth, bm.bmHeight);
			}

			unsigned char* pBits = (unsigned char*)bm.bmBits;
			int nPitch = bm.bmWidthBytes;
			if (!pData->bTopDownDib)
			{
				pBits = LPBYTE( pBits )+((bm.bmHeight-1)*nPitch);
				nPitch = -nPitch;
			}

			unsigned char* p = nullptr;
			unsigned char* pEnd = nullptr;
			pBits += rcDest.top*nPitch;


			switch (pData->eMode)
			{
			case SET_ALPHA_255:
				{
					for (int y = rcDest.top ; y < rcDest.bottom; y++)
					{
						p    = pBits + (rcDest.left << 2) + 3;
						pEnd = pBits + (rcDest.right << 2) + 3;

						while (p < pEnd)
						{
							*p = 0xFF;
							p += 4;
						}

						pBits += nPitch;
					}
				}
				break;

			case SET_ALPHA_VALUE:
				{
					byte alpha = static_cast<byte>(pData->wParam);
					for (int y = rcDest.top ; y < rcDest.bottom; y++)
					{
						p    = pBits + (rcDest.left << 2) + 3;
						pEnd = pBits + (rcDest.right << 2) + 3;

						while (p < pEnd)
						{
							*p = alpha;
							p += 4;
						}

						pBits += nPitch;
					}
				}
				break;

			case SET_ALPHA_255_IF_ALPHA_IS_0:
				{
					for (int y = rcDest.top ; y < rcDest.bottom; y++)
					{
						p    = pBits + (rcDest.left << 2) + 3;
						pEnd = pBits + (rcDest.right << 2) + 3;

						while (p < pEnd)
						{
							if (0 == (*p))
							{
								*p = 0xFF;
							}
							p += 4;
						}

						pBits += nPitch;
					}
				}
				break;

			case  SET_ALPHA_255_IF_RGBA_ISNOT_0:
				{
					unsigned int* p4 = nullptr;
					unsigned int* p4End = nullptr;
					for (int y = rcDest.top ; y < rcDest.bottom; y++)
					{
						p4    = (unsigned int*)(pBits + (rcDest.left << 2));
						p4End = (unsigned int*)(pBits + (rcDest.right << 2));

						while (p4 < p4End)
						{
							if (0 != (*p4))
							{
								*p4 |= 0xFF000000;
							}
							p4 ++;
						}

						pBits += nPitch;
					}
				}
				break;

			case SET_ALPHA_INVERSE_0_255:
				{
					for (int y = rcDest.top ; y < rcDest.bottom; y++)
					{
						p    = pBits + (rcDest.left << 2) + 3;
						pEnd = pBits + (rcDest.right << 2) + 3;

						while (p < pEnd)
						{
							*p = *p^0xFF;  // 异或
							p += 4;
						}

						pBits += nPitch;
					}
				}
				break;

			case SET_ALPHA_VALUE_IF_ALPHA_IS_255:
				{
					byte alpha = static_cast<byte>(pData->wParam);
					for (int y = rcDest.top ; y < rcDest.bottom; y++)
					{
						p    = pBits + (rcDest.left << 2) + 3;
						pEnd = pBits + (rcDest.right << 2) + 3;

						while (p < pEnd)
						{
							if (255 == (*p))
								*p = alpha;
                    
							p += 4;

						}

						pBits += nPitch;
					}
				}
				break;
			}
			return true;
        #else
            return false;
        #endif
		}

		// 用于gdi控件绘制完成后，将控件中的区域alpha填充为255，如edit/richedit控件
		bool FixRenderTargetClipRegionAlpha(IRenderTarget* pRenderTarget)
		{
        #if defined(OS_WIN)
			if (!pRenderTarget->IsRenderAlphaChannel())
				return true;

			util::FixAlphaData data = { 0 };
			data.hDC = pRenderTarget->GetHDC();

			Rect  rc = { 0 };
			GetClipBox(data.hDC, &rc);

			int x = 0; int y = 0;
			pRenderTarget->GetOrigin(&x, &y);
			::OffsetRect(&rc, x, y);

			data.lprc = &rc;
			data.bTopDownDib = true;
			data.eMode = util::SET_ALPHA_255_IF_ALPHA_IS_0;
			util::FixBitmapAlpha(&data);

			return true;
        #else
            return false;
        #endif
		}

		// 检测一个按键是否按下，如CTRL,SHIFT,ALT
		bool IsKeyDown(unsigned int vk)
		{
        #if defined(OS_WIN)
			return ((GetKeyState(vk) & 0x8000) != 0);  // 或者 ( GetKeyState( VK_CONTROL ) < 0 ) ??
        #else
            UIASSERT(false);
            return false;
        #endif
		}



		bool TranslateImage9Region(const wchar_t* szText, C9Region* p9Region, wchar_t szSep)
		{
			if (nullptr == szText || nullptr == p9Region)
				return false;

			C9Region* pReal9Region = (C9Region*)p9Region;

			ISplitStringEnum* pEnum = nullptr;
			int nSize = SplitString(szText, szSep, &pEnum);
			if (0 == nSize || nullptr == pEnum)
				return false;

			switch (nSize)
			{
			case 1:
			{
				int n1 = wtoi(pEnum->GetText(0));
				pReal9Region->Set(n1);
			}
			break;

			case 2:
			{
				int n1 = wtoi(pEnum->GetText(0));
				int n2 = wtoi(pEnum->GetText(1));
				pReal9Region->Set(n1, n2);
			}
			break;

			case 4:
			{
				int n1 = wtoi(pEnum->GetText(0));
				int n2 = wtoi(pEnum->GetText(1));
				int n3 = wtoi(pEnum->GetText(2));
				int n4 = wtoi(pEnum->GetText(3));
				pReal9Region->Set(n1, n2, n3, n4);
			}
			break;

			case 8:
			{
				int n1 = wtoi(pEnum->GetText(0));
				int n2 = wtoi(pEnum->GetText(1));
				int n3 = wtoi(pEnum->GetText(2));
				int n4 = wtoi(pEnum->GetText(3));
				int n5 = wtoi(pEnum->GetText(4));
				int n6 = wtoi(pEnum->GetText(5));
				int n7 = wtoi(pEnum->GetText(6));
				int n8 = wtoi(pEnum->GetText(7));
				pReal9Region->topleft = n1;
				pReal9Region->top = n2;
				pReal9Region->topright = n3;
				pReal9Region->left = n4;
				pReal9Region->right = n5;
				pReal9Region->bottomleft = n6;
				pReal9Region->bottom = n7;
				pReal9Region->bottomright = n8;
			}
			break;

			default:
				SAFE_RELEASE(pEnum);
				return false;
			}

			SAFE_RELEASE(pEnum);
			return true;
		}

		void UIAPI PathInBin(const wchar_t* szRelative, wchar_t szAbsolution[MAX_PATH])
		{
        #if defined(OS_WIN)
			GetModuleFileName(GetModuleHandle(nullptr), szAbsolution, MAX_PATH);
			wchar_t* p = _tcsrchr(szAbsolution, '\\');
			if (p != nullptr)
				*(p + 1) = 0;

			_tcscat(szAbsolution, szRelative);
        #else
            UIASSERT(false);
        #endif
		}

#if defined(OS_WIN)
		bool  IsWin8OrLater()
		{
			static long s_bool = -1;

			if (-1 == s_bool)
			{
				// Initialize the OSVERSIONINFOEX structure.
				OSVERSIONINFOEX osvi;
				ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
				osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
				osvi.dwMajorVersion = 6;
				osvi.dwMinorVersion = 2;

				// Initialize the condition mask.
				DWORDLONG dwlConditionMask = 0;
				VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
				VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER);

				// Perform the test.
				s_bool = VerifyVersionInfo(&osvi,
					VER_MAJORVERSION | VER_MINORVERSION,
					dwlConditionMask) ? true : false;
			}

			return s_bool;
		}

		bool IsWin7OrLater()
		{
			// Initialize the OSVERSIONINFOEX structure.
			OSVERSIONINFOEX osvi;
			ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
			osvi.dwMajorVersion = 6;
			osvi.dwMinorVersion = 1;

			// Initialize the condition mask.
			DWORDLONG dwlConditionMask = 0;
			VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
			VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);

			// Perform the test.
			return VerifyVersionInfo(&osvi,
				VER_MAJORVERSION | VER_MINORVERSION,
				dwlConditionMask);
		}
    #endif
	}
}