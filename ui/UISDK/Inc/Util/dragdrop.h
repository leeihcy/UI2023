#ifndef _UI_DRAGDROP_H_
#define _UI_DRAGDROP_H_

interface IDataObject;

namespace UI
{

	// 创建一个IDataObject，用于实现拖拽源、Ole对象。避免每个拖拽源都得再实现一次IDataObject IDropSource接口
	interface  IDataObjectSource
	{
		virtual BOOL OnRenderData(
			IDataObject* pDataObject,
			LPFORMATETC lpFormatEtc,
			LPSTGMEDIUM lpStgMedium
			) = 0;

		virtual void  OnDataRelease(
			IDataObject* p
			) = 0;
	};

	// DROPEFFECT_COPY/DROPEFFECT_MOVE等的基础上，添加一个WAITING，
	// 用于可以接受拖拽，但还没有准备好的情况。
#define DROPEFFECT_NOTREADY  0x00001000

	// 用于拖拽过程中，显示更详细的信息。如果拖拽文本
	interface IDragFeedback
	{
		virtual void  SetDescription(HBITMAP hIcon, LPCWSTR szText) = 0;
		virtual HRESULT  GiveFeedback(DWORD dwEffect) = 0;
	};

	//
	// 1. 用于设置/取消IDataObject的IDataObjectSource对象，避免IDataObject被
	//    AddRef引用，但IDataObjectSource却先释放了，导致最后出现崩溃的问题
	// 2. 用于保存/获取IDragFeedback接口
	//
	// {30BF2E89-84D8-4d54-98B8-7A5CCE9E46C1}
	static const GUID IID_IDataObjectEx =
	{ 0x30bf2e89, 0x84d8, 0x4d54, { 0x98, 0xb8, 0x7a, 0x5c, 0xce, 0x9e, 0x46, 0xc1 } };
	interface IDataObjectEx : public IUnknown
	{
		virtual void  SetSource(IDataObjectSource* p) = 0;
		virtual void  SetDragFeedback(IDragFeedback* p) = 0;
		virtual IDragFeedback*  GetDragFeedback() = 0;
	};

	// {4F0BD04A-BBD4-497b-91EA-C3362E573AC6}
	static const GUID IID_IDropSourceEx =
	{ 0x4f0bd04a, 0xbbd4, 0x497b, { 0x91, 0xea, 0xc3, 0x36, 0x2e, 0x57, 0x3a, 0xc6 } };
	interface IDropSourceEx : public IUnknown
	{
		virtual void  SetDragFeedback(IDragFeedback* p) = 0;
		virtual IDragFeedback*  GetDragFeedback() = 0;
	};

	UIAPI void  CreateDataObjectInstance(IDataObject**  pp);
	UIAPI void  CreateDropSourceInstance(IDropSource**  pp);
	UIAPI LRESULT  UIDoDragDrop(
		IDataObject* pDataObject,   // Pointer to the data object
		IDropSource* pDropSource,   // Pointer to the source
		DWORD dwOKEffect,           // Effects allowed by the source
		DWORD* pdwEffect,           // Pointer to effects on the source
		HBITMAP hbmpDragImage,      // The drag image's bitmap handle 
		POINT ptOffset);            // A POINT structure that specifies the location of the cursor within the drag image.

}
#endif