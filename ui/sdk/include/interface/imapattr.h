#ifndef _UI_IMAPATTR_H_
#define _UI_IMAPATTR_H_

namespace ui
{
#define  MAPATTR_RET_OK              0
#define  MAPATTR_RET_ERROR          -1  // ��������
#define  MAPATTR_RET_NOT_EXIST      -2  // �����key������
#define  MAPATTR_RET_INVLID_VALUE   -3  // value��ֵ�Ƿ�

struct IRenderBase;
struct ITextRenderBase;
class C9Region;

struct IMapAttribute
{
    virtual bool  HasAttrib(const wchar_t* szKey) = 0;
    virtual bool  ExtractMapAttrByPrefix(const wchar_t* szPrefix, bool bErase, /*out*/IMapAttribute** pMapAttribute) = 0;
    virtual long  Release() = 0;
    virtual long  AddRef() = 0;
    virtual void  CreateCopy(IMapAttribute** ppNewCopy) = 0;
    virtual void  CopyTo(IMapAttribute* pDestMapAttrib, bool bOverwrite) = 0;
	virtual int   GetAttrCount() = 0;

    virtual const wchar_t* GetAttr(const wchar_t* szKey, bool bErase) = 0;
    virtual const wchar_t* GetAttr(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase) = 0;
    virtual long  GetAttr_bool(const wchar_t* szKey, bool bErase, bool* pbGet) = 0;
    virtual long  GetAttr_bool(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase, bool* pbGet) = 0;
    virtual long  GetAttr_int(const wchar_t* szKey, bool bErase, int* pnGet) = 0;
    virtual long  GetAttr_int(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase, int* pnGet) = 0;
    virtual long  GetAttr_intarray(const wchar_t* szKey, bool bErase, int* pIntArray, unsigned int nSize) = 0;
    virtual long  GetAttr_intarray(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase, int* pIntArray, unsigned int nSize) = 0;
	virtual long  GetAttr_REGION4(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase, RECT* prcGet) = 0;
    virtual long  GetAttr_Image9Region(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase, C9Region* pRegion) = 0;

	virtual bool  AddAttr(const wchar_t* szKey, const wchar_t* szValue) = 0;
	virtual bool  AddAttr_bool(const wchar_t* szKey, bool bValue) = 0;
	virtual bool  AddAttr_REGION4(const wchar_t* szKey, RECT* pr) = 0;
	virtual bool  AddAttr_int(const wchar_t* szKey, int nValue) = 0;

    virtual void  BeginEnum() = 0;
    virtual bool  EnumNext(const wchar_t** szKey, const wchar_t** szValue) = 0;
    virtual void  EndEnum() = 0;
};

// ��IMapAttribute��ͬ���ǣ�IListAttribute���Զ����򣬿������ڱ�������ʱ�����ı��ֶ�˳��
struct IListAttribute 
{
// 	virtual void  SetTag(const wchar_t* szKey) = 0;
//     virtual const wchar_t*  GetTag() = 0;

    virtual bool  AddAttr(const wchar_t* szKey, const wchar_t* szValue) = 0;
    virtual const wchar_t*  GetAttr(const wchar_t* szKey) = 0;
    virtual bool  EraseAttr(const wchar_t* szKey) = 0;

    virtual void  BeginEnum() = 0;
    virtual bool  EnumNext(const wchar_t** szKey, const wchar_t** szValue) = 0;
    virtual void  EndEnum() = 0;

    virtual long  Release() = 0;
    virtual long  AddRef() = 0;
};
UIAPI long  UICreateIMapAttribute(IMapAttribute** ppOut);
UIAPI long  UICreateIListAttribute(IListAttribute** ppOut);


}

#endif // _UI_IMAPATTR_H_