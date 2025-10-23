#ifndef _UI_IMAPATTR_H_
#define _UI_IMAPATTR_H_
#include "sdk/include/interface.h"
#include <memory.h>

namespace ui {
#define MAPATTR_RET_OK 0
#define MAPATTR_RET_ERROR -1        // 其它错误
#define MAPATTR_RET_NOT_EXIST -2    // 请求的key不存在
#define MAPATTR_RET_INVLID_VALUE -3 // value的值非法

struct IRenderBase;
struct ITextRenderBase;
struct Rect;
struct C9Region;

struct IAttributeMap {
  virtual ~IAttributeMap() {}
  virtual bool HasAttrib(const char *szKey) = 0;
  virtual std::shared_ptr<IAttributeMap>
    ExtractMapAttrByPrefix(const char *szPrefix, bool bErase) = 0;
  // virtual void Destroy() = 0;
  virtual void CopyTo(IAttributeMap *pDestMapAttrib, bool bOverwrite) = 0;
  virtual int GetAttrCount() = 0;

  virtual const char *GetAttr(const char *szKey, bool bErase) = 0;
  virtual const char *GetAttr(const char *szPrefix, const char *szKey,
                                 bool bErase) = 0;
  virtual int GetAttr_bool(const char *szKey, bool bErase, bool *pbGet) = 0;
  virtual int GetAttr_bool(const char *szPrefix, const char *szKey,
                            bool bErase, bool *pbGet) = 0;
  virtual int GetAttr_int(const char *szKey, bool bErase, int *pnGet) = 0;
  virtual int GetAttr_int(const char *szPrefix, const char *szKey,
                           bool bErase, int *pnGet) = 0;
  virtual int GetAttr_intarray(const char *szKey, bool bErase,
                                int *pIntArray, unsigned int nSize) = 0;
  virtual int GetAttr_intarray(const char *szPrefix, const char *szKey,
                                bool bErase, int *pIntArray,
                                unsigned int nSize) = 0;
  virtual int GetAttr_REGION4(const char *szPrefix, const char *szKey,
                               bool bErase, Rect *prcGet) = 0;
  virtual int GetAttr_Image9Region(const char *szPrefix,
                                    const char *szKey, bool bErase,
                                    C9Region *pRegion) = 0;

  virtual bool AddAttr(const char *szKey, const char *szValue) = 0;
  virtual bool AddAttr_bool(const char *szKey, bool bValue) = 0;
  virtual bool AddAttr_REGION4(const char *szKey, Rect *pr) = 0;
  virtual bool AddAttr_int(const char *szKey, int nValue) = 0;

  virtual void BeginEnum() = 0;
  virtual bool EnumNext(const char **szKey, const char **szValue) = 0;
  virtual void EndEnum() = 0;
};

// 与IMapAttribute不同的是，IListAttribute不自动排序，可用于在保存属性时，不改变字段顺序
struct IAttributeList {
  // 	virtual void  SetTag(const char* szKey) = 0;
  //     virtual const char*  GetTag() = 0;

  virtual bool AddAttr(const char *szKey, const char *szValue) = 0;
  virtual const char *GetAttr(const char *szKey) = 0;
  virtual bool EraseAttr(const char *szKey) = 0;

  virtual void BeginEnum() = 0;
  virtual bool EnumNext(const char **szKey, const char **szValue) = 0;
  virtual void EndEnum() = 0;

  virtual int Release() = 0;
  virtual int AddRef() = 0;
};
UIAPI std::shared_ptr<IAttributeMap> UICreateIMapAttribute();
UIAPI int UICreateIListAttribute(IAttributeList **ppOut);

} // namespace ui

#endif // _UI_IMAPATTR_H_