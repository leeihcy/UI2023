#ifndef _UI_UTIL_H_
#define _UI_UTIL_H_

namespace UI { 
	class  C9Region;

	class  CTimeout;
	interface ITimeoutCallback
	{
		virtual void OnTimeout(long lId, WPARAM wParam, LPARAM lParam) PURE;
	};

	namespace Util 
	{
		COLORREF  TranslateHexColor(LPCTSTR szColor);
		COLORREF  UIAPI TranslateColor(LPCTSTR szColor);
		COLORREF  TranslateRGB(LPCTSTR szCol, TCHAR szSep = _T(','));
		bool  TranslateSIZE(LPCTSTR szText, SIZE* pSize, TCHAR szSep = _T(','));
		bool  TranslateRECT(LPCTSTR szRect, RECT* pRect, TCHAR szSep = _T(','));
		bool  TranslateImage9Region(LPCTSTR str, C9Region* p9Region, TCHAR szSep = _T(','));
		bool  UIAPI IsKeyDown(UINT vk);

		void  UIAPI DeflatRect(RECT* pfc, LPCRECT pDeflatRc);

		interface ISplitStringEnum
		{
			virtual void  Release() = 0;
			virtual LPCTSTR GetText(UINT nIndex) = 0;
			virtual int   GetCount() = 0;
		};
		UINT UIAPI SplitString(
			LPCTSTR szText,
			TCHAR szSep,
			ISplitStringEnum** pEnum
			);

		BOOL  IsFullPath(LPCTSTR szPath);
		BOOL  GetPathFileName(LPCTSTR szPath, TCHAR* szOutDir);
		BOOL  GetPathFileExt(LPCTSTR szPath, TCHAR* szOutExt);

		BOOL  UIAPI IsWin7OrLater();
		BOOL  UIAPI IsWin8OrLater();

		class UIAPI CTimeoutWrap
		{
		public:
			CTimeoutWrap();
			~CTimeoutWrap();

			void  Start(int nElapse);
			void  Cancel();
			void  Init(ITimeoutCallback* pCallback);
			void  SetParam(long lId, WPARAM wParam = 0, LPARAM lParam = 0);

		private:
			CTimeout*  m_Impl;
		};

		enum FixAlphaMode
		{
			// 直接将alpha设置为255
			SET_ALPHA_255,

			// 如果当前alpha==0，将alpha设置为255
			SET_ALPHA_255_IF_ALPHA_IS_0,

			// 如果当前像素不是0，则将alpha设置为255
			SET_ALPHA_255_IF_RGBA_ISNOT_0,

			// alpha 反转 0->255, 255->0
			SET_ALPHA_INVERSE_0_255,

			// 将alpha设置为指定值wParam
			SET_ALPHA_VALUE,

			// 如果当前alpha值是255, 将alpha设置为指定值wParam
			// 但是没有值预乘处理
			SET_ALPHA_VALUE_IF_ALPHA_IS_255

			// ...
			// 如果当前alpha==wParam，将alpha设置为lParam
			//	SET_ALPHA_IF_ALPHA_EQUAL,      

			// 如果当前alpha<wParam，将alpha设置为lParam
			//	SET_ALPHA_IF_ALPHA_LESS,       
		};
		struct FixAlphaData
		{
			// 当仅指定了hDC，没有指定hBitmap时，从hDC中获取CurrentBitap
			HDC      hDC;
			HBITMAP  hBitmap;
			BOOL     bTopDownDib;
			RECT*    lprc;
			FixAlphaMode  eMode;
			WPARAM   wParam;
			LPARAM   lParam;
		};
		BOOL UIAPI FixBitmapAlpha(FixAlphaData* pData);
		BOOL UIAPI FixRenderTargetClipRegionAlpha(IRenderTarget*);

		void UIAPI PathInBin(LPCTSTR szRelative, TCHAR szAbsolution[MAX_PATH]);
	}
}

#endif // _UI_UTIL_H_