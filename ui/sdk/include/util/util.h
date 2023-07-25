#ifndef _UI_UTIL_H_
#define _UI_UTIL_H_

namespace ui { 
	class  C9Region;

	class  CTimeout;
	struct ITimeoutCallback
	{
		virtual void OnTimeout(long lId, long wParam, long lParam) = 0;
	};

	namespace util 
	{
		COLORREF  TranslateHexColor(const wchar_t* szColor);
		COLORREF  UIAPI TranslateColor(const wchar_t* szColor);
		COLORREF  TranslateRGB(const wchar_t* szCol, wchar_t szSep = L',');
		bool  TranslateSIZE(const wchar_t* szText, SIZE* pSize, wchar_t szSep = L',');
		bool  TranslateRECT(const wchar_t* szRect, RECT* pRect, wchar_t szSep = L',');
		bool  TranslateImage9Region(const wchar_t* str, C9Region* p9Region, wchar_t szSep = ',');
		bool  UIAPI IsKeyDown(unsigned int vk);

		void  UIAPI DeflatRect(RECT* pfc, const RECT* pDeflatRc);

		struct ISplitStringEnum
		{
			virtual void  Release() = 0;
			virtual const wchar_t* GetText(unsigned int  nIndex) = 0;
			virtual int   GetCount() = 0;
		};
		unsigned int UIAPI SplitString(
			const wchar_t* szText,
			wchar_t szSep,
			ISplitStringEnum** pEnum
			);

		bool  IsFullPath(const wchar_t* szPath);
		bool  GetPathFileName(const wchar_t* szPath, wchar_t* szOutDir);
		bool  GetPathFileExt(const wchar_t* szPath, wchar_t* szOutExt);

		bool  UIAPI IsWin7OrLater();
		bool  UIAPI IsWin8OrLater();

		class UIAPI CTimeoutWrap
		{
		public:
			CTimeoutWrap();
			~CTimeoutWrap();

			void  Start(int nElapse);
			void  Cancel();
			void  Init(ITimeoutCallback* pCallback);
			void  SetParam(long lId, long wParam = 0, long lParam = 0);

		private:
			CTimeout*  m_Impl;
		};

		enum FixAlphaMode
		{
			// ֱ�ӽ�alpha����Ϊ255
			SET_ALPHA_255,

			// �����ǰalpha==0����alpha����Ϊ255
			SET_ALPHA_255_IF_ALPHA_IS_0,

			// �����ǰ���ز���0����alpha����Ϊ255
			SET_ALPHA_255_IF_RGBA_ISNOT_0,

			// alpha ��ת 0->255, 255->0
			SET_ALPHA_INVERSE_0_255,

			// ��alpha����Ϊָ��ֵwParam
			SET_ALPHA_VALUE,

			// �����ǰalphaֵ��255, ��alpha����Ϊָ��ֵwParam
			// ����û��ֵԤ�˴���
			SET_ALPHA_VALUE_IF_ALPHA_IS_255

			// ...
			// �����ǰalpha==wParam����alpha����ΪlParam
			//	SET_ALPHA_IF_ALPHA_EQUAL,      

			// �����ǰalpha<wParam����alpha����ΪlParam
			//	SET_ALPHA_IF_ALPHA_LESS,       
		};
		struct FixAlphaData
		{
			// ����ָ����hDC��û��ָ��hBitmapʱ����hDC�л�ȡCurrentBitap
			HDC      hDC;
			HBITMAP  hBitmap;
			bool     bTopDownDib;
			RECT*    lprc;
			FixAlphaMode  eMode;
			long   wParam;
			long   lParam;
		};
		bool UIAPI FixBitmapAlpha(FixAlphaData* pData);
		bool UIAPI FixRenderTargetClipRegionAlpha(IRenderTarget*);

		void UIAPI PathInBin(const wchar_t* szRelative, wchar_t szAbsolution[MAX_PATH]);

        bool PathIsDirectory(const wchar_t* path);
        bool PathFileExists(const wchar_t* path);
        int wtoi(const wchar_t* text);
	}
}

#endif // _UI_UTIL_H_