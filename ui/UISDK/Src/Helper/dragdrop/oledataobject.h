#pragma once
#include "Inc\Util\dragdrop.h"

// 参照MFC COleDataSource,  oledobj2.cpp

namespace UI
{
    struct OleDataObjectItem
    {
        FORMATETC  formatetc;
        STGMEDIUM  stgmedium;
    };

	// 实现IDataObject对象，用于WindowlessRichEdit::GetClipboardData
	class OleDataObject : public IDataObject, public IDataObjectEx
	{
	public:
		OleDataObject();
		~OleDataObject();

		// IUnknown  Interface
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void **ppvObject);
		virtual ULONG   STDMETHODCALLTYPE AddRef(void);
		virtual ULONG   STDMETHODCALLTYPE Release(void);

#pragma region // IDataObject
		virtual  HRESULT STDMETHODCALLTYPE GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);
		virtual  HRESULT STDMETHODCALLTYPE GetDataHere(FORMATETC *pformatetc, STGMEDIUM *pmedium);
		virtual  HRESULT STDMETHODCALLTYPE QueryGetData(FORMATETC *pformatetc);
		virtual  HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(FORMATETC *pformatectIn, FORMATETC *pformatetcOut);
		virtual  HRESULT STDMETHODCALLTYPE SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease);
		virtual  HRESULT STDMETHODCALLTYPE EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc);
		virtual  HRESULT STDMETHODCALLTYPE DAdvise(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection);
		virtual  HRESULT STDMETHODCALLTYPE DUnadvise(DWORD dwConnection);
		virtual  HRESULT STDMETHODCALLTYPE EnumDAdvise(IEnumSTATDATA **ppenumAdvise);
#pragma endregion

#pragma region // IDataObjectEx
        virtual void  SetSource(IDataObjectSource* p) override;
        virtual void  SetDragFeedback(IDragFeedback* p) override;
        virtual IDragFeedback*  GetDragFeedback() override;
#pragma endregion

	protected:
		list<OleDataObjectItem*>   m_list;
		long     m_dwRef;
//		/*IMarshal*/IUnknown* m_pMarshal;
        IDataObjectSource*  m_pDataObjectSource;  // 延迟数据提供者
        IDragFeedback*  m_pDragFeedback;  // 拖拽时回执显示（如文本），仅GET/SET用

		friend class IEnumFORMATETCImpl;
	};

	// 用于IDataObject::EnumFormatEtc中，实现各种格式的遍历
	// 同时自己实现一个IEnumFORMATETC就能直接引用OleDataObject
	// 中的数据，避免拷贝
	class IEnumFORMATETCImpl : public IEnumFORMATETC
	{
	public:
		IEnumFORMATETCImpl(OleDataObject* pDataObj);
		~IEnumFORMATETCImpl();

		// IUnknown  Interface
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void **ppvObject);
		virtual ULONG   STDMETHODCALLTYPE AddRef(void);
		virtual ULONG   STDMETHODCALLTYPE Release(void);

#pragma region
		virtual HRESULT STDMETHODCALLTYPE Next(ULONG celt, FORMATETC *rgelt, ULONG *pceltFetched);
		virtual HRESULT STDMETHODCALLTYPE Skip(ULONG celt);
		virtual HRESULT STDMETHODCALLTYPE Reset(void);
		virtual HRESULT STDMETHODCALLTYPE Clone(IEnumFORMATETC **ppenum);
#pragma endregion

	private:
		OleDataObject*  m_pDataObject;
		long     m_dwRef;
		int      m_nCurIndex;
	}; 

	
}