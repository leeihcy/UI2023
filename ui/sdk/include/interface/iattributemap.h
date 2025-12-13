#ifndef _UI_IMAPATTR_H_
#define _UI_IMAPATTR_H_
#include "sdk/include/interface.h"
#include <memory.h>

namespace ui {

struct IRenderBase;
struct ITextRenderBase;
struct Rect;
struct C9Region;
struct IAttributeEditorProxy;
struct IResourceBundle;


#define MAPATTR_RET_OK 0
#define MAPATTR_RET_ERROR -1        // 其它错误
#define MAPATTR_RET_NOT_EXIST -2    // 请求的key不存在
#define MAPATTR_RET_INVLID_VALUE -3 // value的值非法


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



// 对象序列化消息。用于取代WM_SETATTRIBUTE
enum SERIALIZEFLAG {
  SERIALIZEFLAG_LOAD = 0x01,
  SERIALIZEFLAG_RELOAD = 0x03, // reload也是load
  SERIALIZEFLAG_SAVE = 0x04,
  SERIALIZEFLAG_EDITOR = 0x08, // 获取属性列表和提示信息

  // load 标识
  SERIALIZEFLAG_LOAD_ERASEATTR =
      0x0200, // 获取到属性后，将该属性从mapattr中删除

  // save 标识
};

struct SerializeParam {
  union {
    IAttributeMap *attribute_map;                    // load [in] / getlist [out]
    IAttributeList *attribute_list;                  // save [out]
    IAttributeEditorProxy *attribute_editor_proxy;   // editor [in]
  };

  IResourceBundle *resource;
  const char *szPrefix;    // 属性前缀
  const char *szParentKey; // 父属性（仅用于editor），如bkg.render.type
  unsigned int nFlags;

  bool IsReload() {
    return ((nFlags & SERIALIZEFLAG_RELOAD) == SERIALIZEFLAG_RELOAD);
  }
  bool IsLoad() {
    return ((nFlags & SERIALIZEFLAG_LOAD) == SERIALIZEFLAG_LOAD);
  }
  bool IsSave() {
    return ((nFlags & SERIALIZEFLAG_SAVE) == SERIALIZEFLAG_SAVE);
  }
  bool IsEditor() {
    return ((nFlags & SERIALIZEFLAG_EDITOR) == SERIALIZEFLAG_EDITOR);
  }
  bool NeedErase() {
    return (nFlags & SERIALIZEFLAG_LOAD_ERASEATTR);
  }
  void SetErase(bool b) {
    if (b) {
      nFlags |= SERIALIZEFLAG_LOAD_ERASEATTR;
    } else {
      nFlags &= ~SERIALIZEFLAG_LOAD_ERASEATTR;
    }
  }
};

} // namespace ui

#endif // _UI_IMAPATTR_H_