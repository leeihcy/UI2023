#pragma once
#include "include/interface/imapattr.h"
#include "src/private_inc.h"

namespace ui {

class CMapAttribute : public IMapAttribute {
public:
  CMapAttribute();
  ~CMapAttribute();
  CMapAttribute(const CMapAttribute &o);

  virtual int GetAttrCount() override;
  virtual bool HasAttrib(const wchar_t *szKey) override;
  virtual const wchar_t *GetAttr(const wchar_t *szKey, bool bErase) override;
  virtual const wchar_t *GetAttr(const wchar_t *szPrefix, const wchar_t *szKey,
                                 bool bErase) override;
  virtual int GetAttr_bool(const wchar_t *szKey, bool bErase,
                            bool *pbGet) override;
  virtual int GetAttr_bool(const wchar_t *szPrefix, const wchar_t *szKey,
                            bool bErase, bool *pbGet) override;
  virtual int GetAttr_int(const wchar_t *szKey, bool bErase,
                           int *pnGet) override;
  virtual int GetAttr_int(const wchar_t *szPrefix, const wchar_t *szKey,
                           bool bErase, int *pnGet) override;
  virtual int GetAttr_intarray(const wchar_t *szKey, bool bErase,
                                int *pIntArray, unsigned int nSize) override;
  virtual int GetAttr_intarray(const wchar_t *szPrefix, const wchar_t *szKey,
                                bool bErase, int *pIntArray,
                                unsigned int nSize) override;
  virtual int GetAttr_REGION4(const wchar_t *szPrefix, const wchar_t *szKey,
                               bool bErase, REGION4 *prcGet) override;
  virtual int GetAttr_Image9Region(const wchar_t *szPrefix,
                                    const wchar_t *szKey, bool bErase,
                                    C9Region *pRegion) override;

  virtual bool AddAttr(const wchar_t *szKey, const wchar_t *szValue) override;
  virtual bool AddAttr_bool(const wchar_t *szKey, bool bValue) override;
  virtual bool AddAttr_REGION4(const wchar_t *szKey, REGION4 *pr) override;
  virtual bool AddAttr_int(const wchar_t *szKey, int nValue) override;

  virtual bool
  ExtractMapAttrByPrefix(const wchar_t *szPrefix, bool bErase,
                         /*out*/ IMapAttribute **ppMapAttribute) override;
  virtual void Delete() override;
  virtual void CopyTo(IMapAttribute *pDestMapAttrib, bool bOverride) override;

  virtual void BeginEnum() override;
  virtual bool EnumNext(const wchar_t **szKey, const wchar_t **szValue) override;
  virtual void EndEnum() override;

public:
  const wchar_t *GetAttr(const String &strKey, bool bErase);
  void AddAttr(const String &strKey, const String &strValue);

protected:
  //	String   m_strTag;      // 标签名，用于充分模拟xml标签数据
  ATTRMAP m_mapAttr;
  ATTRMAP::iterator m_iterEnum;
};

class ListAttrItem {
public:
  ListAttrItem() { pPrev = pNext = nullptr; }

  String strKey;
  String strValue;
  ListAttrItem *pNext;
  ListAttrItem *pPrev;
};

class CListAttribute : public IListAttribute {
public:
  CListAttribute();
  ~CListAttribute();

  // 	virtual void  SetTag(const wchar_t* szKey);
  //     virtual const wchar_t*  GetTag();

  virtual bool AddAttr(const wchar_t *szKey, const wchar_t *szValue);
  virtual const wchar_t *GetAttr(const wchar_t *szKey);
  virtual bool EraseAttr(const wchar_t *szKey);

  virtual void BeginEnum();
  virtual bool EnumNext(const wchar_t **szKey, const wchar_t **szValue);
  virtual void EndEnum();

  virtual int Release();
  virtual int AddRef();

private:
  ListAttrItem *FindItem(const wchar_t *szKey);

private:
  //	String   m_strTag;
  ListAttrItem *m_pFirstItem;
  ListAttrItem *m_pLastItem;

  ListAttrItem *m_pEnum; //  正在被Enum的对象
  int m_lRef;
};

} // namespace ui
