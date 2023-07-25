#pragma once
#include "include/interface/imapattr.h"
#include "src/private_inc.h"

namespace ui
{

class CMapAttribute : public IMapAttribute
{
public:
    CMapAttribute();
    ~CMapAttribute();
    CMapAttribute(const CMapAttribute& o);
    
	virtual int   GetAttrCount() override;
    virtual bool  HasAttrib(const wchar_t* szKey) override;
    virtual const wchar_t* GetAttr(const wchar_t* szKey, bool bErase) override;
    virtual const wchar_t* GetAttr(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase) override;
    virtual long  GetAttr_bool(const wchar_t* szKey, bool bErase, bool* pbGet) override;
    virtual long  GetAttr_bool(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase, bool* pbGet) override;
    virtual long  GetAttr_int(const wchar_t* szKey, bool bErase, int* pnGet) override;
    virtual long  GetAttr_int(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase, int* pnGet) override;
    virtual long  GetAttr_intarray(const wchar_t* szKey, bool bErase, int* pIntArray, unsigned int nSize) override;
    virtual long  GetAttr_intarray(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase, int* pIntArray, unsigned int nSize) override;
	virtual long  GetAttr_REGION4(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase, REGION4* prcGet) override;
    virtual long  GetAttr_Image9Region(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase, C9Region* pRegion) override;

    virtual bool  AddAttr(const wchar_t* szKey, const wchar_t* szValue) override;
	virtual bool  AddAttr_bool(const wchar_t* szKey, bool bValue) override;
	virtual bool  AddAttr_REGION4(const wchar_t* szKey, REGION4* pr) override;
	virtual bool  AddAttr_int(const wchar_t* szKey, int nValue) override;

    virtual bool  ExtractMapAttrByPrefix(const wchar_t* szPrefix, bool bErase, /*out*/IMapAttribute** ppMapAttribute) override;
    virtual long  Release() override;
    virtual long  AddRef() override;
    virtual void  CreateCopy(IMapAttribute** ppNewCopy) override;
    virtual void  CopyTo(IMapAttribute* pDestMapAttrib, bool bOverride) override;

    virtual void  BeginEnum() override;
    virtual bool  EnumNext(const wchar** szKey, const wchar** szValue) override;
    virtual void  EndEnum() override;

public:
    const wchar_t* GetAttr(const String& strKey, bool bErase);
    void  AddAttr(const String& strKey, const String& strValue);

protected:
//	String   m_strTag;      // ��ǩ�������ڳ��ģ��xml��ǩ����
    ATTRMAP  m_mapAttr;
    int      m_lRef;

    ATTRMAP::iterator  m_iterEnum;
};


class ListAttrItem 
{
public:
    ListAttrItem () { pPrev = pNext = nullptr; }

    String  strKey;
    String  strValue;
    ListAttrItem*  pNext;
    ListAttrItem*  pPrev;
};

class CListAttribute : public  IListAttribute 
{
public:
    CListAttribute();
    ~CListAttribute();

// 	virtual void  SetTag(const wchar_t* szKey);
//     virtual const wchar_t*  GetTag();

    virtual bool  AddAttr(const wchar_t* szKey, const wchar_t* szValue);
    virtual const wchar_t*  GetAttr(const wchar_t* szKey);
    virtual bool  EraseAttr(const wchar_t* szKey);

    virtual void  BeginEnum();
    virtual bool  EnumNext(const wchar** szKey, const wchar** szValue);
    virtual void  EndEnum();

    virtual long  Release();
    virtual long  AddRef();

private:
    ListAttrItem* FindItem(const wchar_t* szKey);

private:
//	String   m_strTag; 
    ListAttrItem*  m_pFirstItem;
    ListAttrItem*  m_pLastItem;

    ListAttrItem*  m_pEnum;  //  ���ڱ�Enum�Ķ���
    int  m_lRef;
};

}
