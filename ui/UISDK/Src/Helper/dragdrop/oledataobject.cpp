#include "stdafx.h"
#include "oledataobject.h"

namespace UI
{

void  CreateDataObjectInstance(IDataObject**  pp)
{
    if (nullptr == pp)
        return;

    OleDataObject* p = new OleDataObject();
    *pp = static_cast<IDataObject*>(p);
    p->AddRef();
}

BOOL _AfxCopyStgMedium(CLIPFORMAT cfFormat, LPSTGMEDIUM lpDest, LPSTGMEDIUM lpSource);
void _AfxOleCopyFormatEtc(LPFORMATETC petcDest, LPFORMATETC petcSrc);

OleDataObject::OleDataObject()
{
	m_dwRef = 0;
    m_pDataObjectSource = nullptr;
    m_pDragFeedback = nullptr;
//	m_pMarshal = nullptr;
//	::CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pMarshal);
}
OleDataObject::~OleDataObject()
{
    if (m_pDataObjectSource)
    {
        m_pDataObjectSource->OnDataRelease(static_cast<IDataObject*>(this));
        m_pDataObjectSource = nullptr;
    }

	list<OleDataObjectItem*>::iterator iter = m_list.begin();
	list<OleDataObjectItem*>::iterator iterEnd = m_list.end();

	for (; iter != iterEnd; iter++)
	{
		OleDataObjectItem* pItem = *iter;
		if (nullptr == pItem)
			continue;

		if (pItem->formatetc.ptd)
			CoTaskMemFree(pItem->formatetc.ptd);
		::ReleaseStgMedium(&pItem->stgmedium);

        SAFE_DELETE(pItem); 
	}
//	SAFE_RELEASE(m_pMarshal);
}

HRESULT STDMETHODCALLTYPE OleDataObject::QueryInterface(REFIID riid,void **ppvObject)
{
	if (nullptr == ppvObject)
		return E_INVALIDARG;

	if (::IsEqualIID(IID_IUnknown,riid) || ::IsEqualIID(IID_IDataObject, riid))
	{
		*ppvObject = static_cast<IDataObject*>(this);
	}
    else if (::IsEqualIID(IID_IDataObjectEx,riid))
    {
        *ppvObject = static_cast<IDataObjectEx*>(this);
    }
// 	else if (::IsEqualIID(riid, IID_IMarshal) && nullptr != m_pMarshal)
// 	{
// 		//*ppvObject = m_pMarshal;
// 		return m_pMarshal->QueryInterface(riid, ppvObject);
// 	}
	else
	{
		return E_NOINTERFACE;
	}

	this->AddRef();
	return S_OK;
}
ULONG   STDMETHODCALLTYPE OleDataObject::AddRef(void)
{
	return ++m_dwRef;
}
ULONG   STDMETHODCALLTYPE OleDataObject::Release(void)
{
	-- m_dwRef;
	if (0 == m_dwRef)
	{
		delete this;
		return 0;
	}
	return m_dwRef;
}

void  OleDataObject::SetSource(IDataObjectSource* p)
{
    m_pDataObjectSource = p;
}

void OleDataObject::SetDragFeedback(IDragFeedback* p)
{
    m_pDragFeedback = p;
}
IDragFeedback*  OleDataObject::GetDragFeedback()
{
    return m_pDragFeedback;
}

// 注：如果其它程序粘贴的话，该函数可能在其它线程中被调用
//     但从目前来看，SetData/GetData好像不会同时发生，因此没有添加同步机制
HRESULT STDMETHODCALLTYPE OleDataObject::GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium)
{
	if (nullptr == pformatetcIn || nullptr == pmedium)
		return E_INVALIDARG;

	list<OleDataObjectItem*>::iterator iter = m_list.begin();
	list<OleDataObjectItem*>::iterator iterEnd = m_list.end();

	for (; iter != iterEnd; iter++)
	{
		OleDataObjectItem* pItem = *iter;
		if (nullptr == pItem)
			continue;

		if (pItem->formatetc.cfFormat == pformatetcIn->cfFormat &&
			pItem->formatetc.dwAspect == pformatetcIn->dwAspect &&
			0 != (pformatetcIn->tymed&pItem->formatetc.tymed) )
		{
            if (pItem->stgmedium.tymed == TYMED_NULL)  // 判断是否是延迟提交
            {
                if (!m_pDataObjectSource)
                    return DV_E_FORMATETC;

                if (!m_pDataObjectSource->OnRenderData(
                        static_cast<IDataObject*>(this),
                        &pItem->formatetc, 
                        &pItem->stgmedium))
                {
                    return DV_E_FORMATETC;
                }
            }

			_AfxCopyStgMedium(pformatetcIn->cfFormat, pmedium, &pItem->stgmedium);
			return S_OK;
		}
	}
	return DV_E_FORMATETC;
}
HRESULT STDMETHODCALLTYPE OleDataObject::GetDataHere(FORMATETC *pformatetc, STGMEDIUM *pmedium)
{
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE OleDataObject::QueryGetData(FORMATETC *pformatetcIn)
{
	if (nullptr == pformatetcIn)
		return E_INVALIDARG;

	list<OleDataObjectItem*>::iterator iter = m_list.begin();
	list<OleDataObjectItem*>::iterator iterEnd = m_list.end();

	for (; iter != iterEnd; iter++)
	{
		OleDataObjectItem* pItem = *iter;
		if (nullptr == pItem)
			continue;

		if (pItem->formatetc.cfFormat == pformatetcIn->cfFormat &&
			pItem->formatetc.dwAspect == pformatetcIn->dwAspect &&
			0 != (pformatetcIn->tymed&pItem->formatetc.tymed) )
		{
			return S_OK;
		}
	}
	return DV_E_FORMATETC;
}

HRESULT STDMETHODCALLTYPE OleDataObject::GetCanonicalFormatEtc(FORMATETC *pformatectIn, FORMATETC *pformatetcOut)
{
	return DATA_S_SAMEFORMATETC ;
}

// If FALSE, the caller retains ownership of the storage medium and the data object 
// called uses the storage medium for the duration of the call only.
HRESULT STDMETHODCALLTYPE OleDataObject::SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease)
{
	if (nullptr == pformatetc || nullptr == pmedium)
		return E_INVALIDARG;

	if (pformatetc->tymed != pmedium->tymed && 
        pmedium->tymed != TYMED_NULL) // 不是延迟提交
		return E_INVALIDARG;

	HRESULT hr = this->QueryGetData(pformatetc);
	if (SUCCEEDED(hr))
		return DV_E_FORMATETC;

	OleDataObjectItem* pItem = new OleDataObjectItem;
    memset(pItem, 0, sizeof(OleDataObjectItem));

	pItem->formatetc = *pformatetc;
	if (fRelease)
	{
        UI_LOG_DEBUG(_T("format:%d, value:0x%x"), pItem->formatetc.cfFormat, pmedium->hGlobal);
		pItem->stgmedium = *pmedium;
	}
	else
	{
		if (FALSE == _AfxCopyStgMedium(pformatetc->cfFormat, &pItem->stgmedium, pmedium))
		{
			delete pItem;
			return DV_E_STGMEDIUM;
		}
	}

	m_list.push_back(pItem);
	return S_OK;
}

// 在OleSetClipboard, SetClipboardFormats时将会调用
HRESULT STDMETHODCALLTYPE OleDataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc)
{
	if (nullptr == ppenumFormatEtc)
		return E_INVALIDARG;

	IEnumFORMATETCImpl* p = new IEnumFORMATETCImpl(this);
	p->AddRef();
	*ppenumFormatEtc = static_cast<IEnumFORMATETC*>(p);

	return S_OK;
}

// 数据改变的通知机制，不实现
HRESULT STDMETHODCALLTYPE OleDataObject::DAdvise(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection)
{
return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE OleDataObject::DUnadvise(DWORD dwConnection)
{
return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE OleDataObject::EnumDAdvise(IEnumSTATDATA **ppenumAdvise)
{
return E_NOTIMPL;
}


IEnumFORMATETCImpl::IEnumFORMATETCImpl(OleDataObject* pDataObj)
{
	m_dwRef = 0;
	m_nCurIndex = 0;
	m_pDataObject = pDataObj;
}
IEnumFORMATETCImpl::~IEnumFORMATETCImpl()
{

}
HRESULT STDMETHODCALLTYPE IEnumFORMATETCImpl::QueryInterface(REFIID riid,void **ppvObject)
{
	if (nullptr == ppvObject)
		return E_INVALIDARG;

	if (::IsEqualIID(IID_IUnknown,riid) || ::IsEqualIID(IID_IEnumFORMATETC, riid))
	{
		*ppvObject = static_cast<IEnumFORMATETC*>(this);
	}
    else
    {
    	return E_NOINTERFACE;
    }

	this->AddRef();
	return S_OK;
}
ULONG   STDMETHODCALLTYPE IEnumFORMATETCImpl::AddRef(void)
{
	return ++m_dwRef;
}
ULONG   STDMETHODCALLTYPE IEnumFORMATETCImpl::Release(void)
{
	-- m_dwRef;
	if (0 == m_dwRef)
	{
		delete this;
		return 0;
	}
	return m_dwRef;
}
HRESULT STDMETHODCALLTYPE IEnumFORMATETCImpl::Next(ULONG celt, FORMATETC *rgelt, ULONG *pceltFetched)
{
	list<OleDataObjectItem*>::iterator iter = m_pDataObject->m_list.begin();
	list<OleDataObjectItem*>::iterator iterEnd = m_pDataObject->m_list.end();

	ULONG j = 0;
	for (int i = 0; iter != iterEnd; iter++, i++)
	{
		if (i < m_nCurIndex)
			continue;
		if (i >= m_nCurIndex+(int)celt)
			break;

		_AfxOleCopyFormatEtc(&rgelt[j++], &(*iter)->formatetc);
	}

	if (pceltFetched)
		*pceltFetched = j;
	m_nCurIndex += j;

	return j==celt?S_OK:S_FALSE;
}
HRESULT STDMETHODCALLTYPE IEnumFORMATETCImpl::Skip(ULONG celt)
{
	m_nCurIndex += celt;
	return S_OK;
}
HRESULT STDMETHODCALLTYPE IEnumFORMATETCImpl::Reset(void)
{
	m_nCurIndex = 0;
	return S_OK;
}
HRESULT STDMETHODCALLTYPE IEnumFORMATETCImpl::Clone(IEnumFORMATETC **ppenum)
{
	IEnumFORMATETCImpl* p = new IEnumFORMATETCImpl(m_pDataObject);
	p->m_nCurIndex = m_nCurIndex;
	*ppenum = p;

	return S_OK;
}

#pragma region
HGLOBAL _AfxCopyGlobalMemory(HGLOBAL hDest, HGLOBAL hSource)
{
	UIASSERT(hSource != nullptr);

	// make sure we have suitable hDest
	ULONG_PTR nSize = ::GlobalSize(hSource);
	if (hDest == nullptr)
	{
		hDest = ::GlobalAlloc(GMEM_SHARE|GMEM_MOVEABLE, nSize);
		if (hDest == nullptr)
			return nullptr;
	}
	else if (nSize > ::GlobalSize(hDest))
	{
		// hDest is not large enough
		return nullptr;
	}

	// copy the bits
	LPVOID lpSource = ::GlobalLock(hSource);
	LPVOID lpDest = ::GlobalLock(hDest);
	UIASSERT(lpDest != nullptr);
	UIASSERT(lpSource != nullptr);
	Checked::memcpy_s(lpDest, (ULONG)::GlobalSize(hDest), lpSource, (ULONG)nSize);
	::GlobalUnlock(hDest);
	::GlobalUnlock(hSource);

	// success -- return hDest
	return hDest;
}
BOOL _AfxCopyStgMedium(CLIPFORMAT cfFormat, LPSTGMEDIUM lpDest, LPSTGMEDIUM lpSource)
{
    // nnd! 系统给我们传过来的数据居然不干净，得自己先重置下。要不然等着崩溃吧
    // 郁闷了我一天才找到这里。另外也只能怪MFC的_AfxCopyStgMedium实现有问题。
    lpDest->pUnkForRelease = nullptr;
    lpDest->hGlobal = nullptr;

	if (lpDest->tymed == TYMED_NULL)
	{
		UIASSERT(lpSource->tymed != TYMED_NULL);
		switch (lpSource->tymed)
		{
		case TYMED_ENHMF:
		case TYMED_HGLOBAL:
			UIASSERT(sizeof(HGLOBAL) == sizeof(HENHMETAFILE));
			lpDest->tymed = lpSource->tymed;
			lpDest->hGlobal = nullptr;
			break;  // fall through to CopyGlobalMemory case

		case TYMED_ISTREAM:
			lpDest->pstm = lpSource->pstm;
			lpDest->pstm->AddRef();
			lpDest->tymed = TYMED_ISTREAM;
			return TRUE;

		case TYMED_ISTORAGE:
			lpDest->pstg = lpSource->pstg;
			lpDest->pstg->AddRef();
			lpDest->tymed = TYMED_ISTORAGE;
			return TRUE;

		case TYMED_MFPICT:
			{
				// copy LPMETAFILEPICT struct + embedded HMETAFILE
				HGLOBAL hDest = _AfxCopyGlobalMemory(nullptr, lpSource->hGlobal);
				if (hDest == nullptr)
					return FALSE;
				LPMETAFILEPICT lpPict = (LPMETAFILEPICT)::GlobalLock(hDest);
				UIASSERT(lpPict != nullptr);
				lpPict->hMF = ::CopyMetaFile(lpPict->hMF, nullptr);
				if (lpPict->hMF == nullptr)
				{
					::GlobalUnlock(hDest);
					::GlobalFree(hDest);
					return FALSE;
				}
				::GlobalUnlock(hDest);

				// fill STGMEDIUM struct
				lpDest->hGlobal = hDest;
				lpDest->tymed = TYMED_MFPICT;
			}
			return TRUE;

		case TYMED_GDI:
			lpDest->tymed = TYMED_GDI;
			lpDest->hGlobal = nullptr;
			break;

		case TYMED_FILE:
			{
				lpDest->tymed = TYMED_FILE;
				UIASSERT(lpSource->lpszFileName != nullptr);
				if (lpSource->lpszFileName == nullptr)
				{
					return FALSE;
				}
				UINT cbSrc = lstrlenW(lpSource->lpszFileName);
				LPOLESTR szFileName = (LPOLESTR)::ATL::AtlCoTaskMemCAlloc((cbSrc+1),sizeof(OLECHAR));
				lpDest->lpszFileName = szFileName;
				if (szFileName == nullptr)
					return FALSE;

				Checked::memcpy_s(szFileName, (cbSrc+1)*sizeof(OLECHAR), 
					lpSource->lpszFileName,  (cbSrc+1)*sizeof(OLECHAR));
				return TRUE;
			}

			// unable to create + copy other TYMEDs
		default:
			return FALSE;
		}
	}
	// UIASSERT(lpDest->tymed == lpSource->tymed);
    if (lpDest->tymed != lpSource->tymed)
        lpDest->tymed = lpSource->tymed;

	switch (lpSource->tymed)
	{
	case TYMED_HGLOBAL:
		{
			HGLOBAL hDest = _AfxCopyGlobalMemory(lpDest->hGlobal,
				lpSource->hGlobal);
			if (hDest == nullptr)
				return FALSE;

			lpDest->hGlobal = hDest;
		}
		return TRUE;

	case TYMED_ISTREAM:
		{
			UIASSERT(lpDest->pstm != nullptr);
			UIASSERT(lpSource->pstm != nullptr);

			// get the size of the source stream
			STATSTG stat;
			if (lpSource->pstm->Stat(&stat, STATFLAG_NONAME) != S_OK)
			{
				// unable to get size of source stream
				return FALSE;
			}
			UIASSERT(stat.pwcsName == nullptr);

			// always seek to zero before copy
			LARGE_INTEGER zero = { 0, 0 };
			lpDest->pstm->Seek(zero, STREAM_SEEK_SET, nullptr);
			lpSource->pstm->Seek(zero, STREAM_SEEK_SET, nullptr);

			// copy source to destination
			if (lpSource->pstm->CopyTo(lpDest->pstm, stat.cbSize,
				nullptr, nullptr) != NULL)
			{
				// copy from source to dest failed
				return FALSE;
			}

			// always seek to zero after copy
			lpDest->pstm->Seek(zero, STREAM_SEEK_SET, nullptr);
			lpSource->pstm->Seek(zero, STREAM_SEEK_SET, nullptr);
		}
		return TRUE;

	case TYMED_ISTORAGE:
		{
			UIASSERT(lpDest->pstg != nullptr);
			UIASSERT(lpSource->pstg != nullptr);

			// just copy source to destination
			if (lpSource->pstg->CopyTo(0, nullptr, nullptr, lpDest->pstg) != S_OK)
				return FALSE;
		}
		return TRUE;

	case TYMED_FILE:
		{
			UIASSERT(lpSource->lpszFileName != nullptr);
			UIASSERT(lpDest->lpszFileName != nullptr);
			String strSource(lpDest->lpszFileName);
			String strDest(lpDest->lpszFileName);
			return CopyFile(lpSource->lpszFileName ? strSource.c_str() : nullptr,
				lpDest->lpszFileName ? strDest.c_str() : nullptr, FALSE);
		}


	case TYMED_ENHMF:
	case TYMED_GDI:
		{
			UIASSERT(sizeof(HGLOBAL) == sizeof(HENHMETAFILE));

			// with TYMED_GDI cannot copy into existing HANDLE
			if (lpDest->hGlobal != nullptr)
				return FALSE;

			// otherwise, use OleDuplicateData for the copy
			lpDest->hGlobal = OleDuplicateData(lpSource->hGlobal, cfFormat, 0);
			if (lpDest->hGlobal == nullptr)
				return FALSE;
		}
		return TRUE;

		// other TYMEDs cannot be copied
	default:
		return FALSE;
	}
}

DVTARGETDEVICE* _AfxOleCopyTargetDevice(DVTARGETDEVICE* ptdSrc)
{
	if (ptdSrc == nullptr)
		return nullptr;

	DVTARGETDEVICE* ptdDest =
		(DVTARGETDEVICE*)CoTaskMemAlloc(ptdSrc->tdSize);
	if (ptdDest == nullptr)
		return nullptr;

	Checked::memcpy_s(ptdDest, (size_t)ptdSrc->tdSize, 
		ptdSrc, (size_t)ptdSrc->tdSize);
	return ptdDest;
}
void _AfxOleCopyFormatEtc(LPFORMATETC petcDest, LPFORMATETC petcSrc)
{
	UIASSERT(petcDest != nullptr);
	UIASSERT(petcSrc != nullptr);

	petcDest->cfFormat = petcSrc->cfFormat;
	petcDest->ptd = _AfxOleCopyTargetDevice(petcSrc->ptd);
	petcDest->dwAspect = petcSrc->dwAspect;
	petcDest->lindex = petcSrc->lindex;
	petcDest->tymed = petcSrc->tymed;
}

#pragma endregion

}