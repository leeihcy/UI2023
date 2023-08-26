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
  virtual bool HasAttrib(const char *szKey) override;
  virtual const char *GetAttr(const char *szKey, bool bErase) override;
  virtual const char *GetAttr(const char *szPrefix, const char *szKey,
                                 bool bErase) override;
  virtual int GetAttr_bool(const char *szKey, bool bErase,
                            bool *pbGet) override;
  virtual int GetAttr_bool(const char *szPrefix, const char *szKey,
                            bool bErase, bool *pbGet) override;
  virtual int GetAttr_int(const char *szKey, bool bErase,
                           int *pnGet) override;
  virtual int GetAttr_int(const char *szPrefix, const char *szKey,
                           bool bErase, int *pnGet) override;
  virtual int GetAttr_intarray(const char *szKey, bool bErase,
                                int *pIntArray, unsigned int nSize) override;
  virtual int GetAttr_intarray(const char *szPrefix, const char *szKey,
                                bool bErase, int *pIntArray,
                                unsigned int nSize) override;
  virtual int GetAttr_REGION4(const char *szPrefix, const char *szKey,
                               bool bErase, REGION4 *prcGet) override;
  virtual int GetAttr_Image9Region(const char *szPrefix,
                                    const char *szKey, bool bErase,
                                    C9Region *pRegion) override;

  virtual bool AddAttr(const char *szKey, const char *szValue) override;
  virtual bool AddAttr_bool(const char *szKey, bool bValue) override;
  virtual bool AddAttr_REGION4(const char *szKey, REGION4 *pr) override;
  virtual bool AddAttr_int(const char *szKey, int nValue) override;

  virtual bool
  ExtractMapAttrByPrefix(const char *szPrefix, bool bErase,
                         /*out*/ IMapAttribute **ppMapAttribute) override;
  virtual void Delete() override;
  virtual void CopyTo(IMapAttribute *pDestMapAttrib, bool bOverride) override;

  virtual void BeginEnum() override;
  virtual bool EnumNext(const char **szKey, const char **szValue) override;
  virtual void EndEnum() override;

public:
  const char *GetAttr(const std::string &strKey, bool bErase);
  void AddAttr(const std::string &strKey, const std::string &strValue);

protected:
  //	std::string   m_strTag;      // 标签名，用于充分模拟xml标签数据
  ATTRMAP m_mapAttr;
  ATTRMAP::iterator m_iterEnum;
};

class ListAttrItem {
public:
  ListAttrItem() { pPrev = pNext = nullptr; }

  std::string strKey;
  std::string strValue;
  ListAttrItem *pNext;
  ListAttrItem *pPrev;
};

class CListAttribute : public IListAttribute {
public:
  CListAttribute();
  ~CListAttribute();

  // 	virtual void  SetTag(const char* szKey);
  //     virtual const char*  GetTag();

  virtual bool AddAttr(const char *szKey, const char *szValue);
  virtual const char *GetAttr(const char *szKey);
  virtual bool EraseAttr(const char *szKey);

  virtual void BeginEnum();
  virtual bool EnumNext(const char **szKey, const char **szValue);
  virtual void EndEnum();

  virtual int Release();
  virtual int AddRef();

private:
  ListAttrItem *FindItem(const char *szKey);

private:
  //	std::string   m_strTag;
  ListAttrItem *m_pFirstItem;
  ListAttrItem *m_pLastItem;

  ListAttrItem *m_pEnum; //  正在被Enum的对象
  int m_lRef;
};

} // namespace ui
