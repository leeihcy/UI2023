#pragma once
#include "include/interface/iattribute.h"
#include "src/resource/res_bundle.h"

namespace ui {
class AttributeSerializer;
class AttributeEditorProxy;
class Application;

// 属性基类
class AttributeBase {
public:
  AttributeBase();

  virtual ~AttributeBase(){};
  virtual void Release() { delete this; };

  virtual const char *Get() = 0;
  virtual void Set(const char *) = 0;
  // reload时，如果在xml中没有显式配置该属性，则调用Reset，而不是Set
  virtual void Reset() = 0;
  virtual bool IsDefaultValue() = 0;

  // 加载、保存、编辑器显示
  virtual void Load(SerializeParam *);
  virtual void Save(SerializeParam *);
  virtual void Editor(SerializeParam *pData, AttributeEditorProxy *p,
                      EditorAttributeFlag e) = 0;

  // 已过期，将删除。修改为各个类自己的方法来设置。参照StringAttribute.
  virtual void SetBindValue(void *){};
  virtual void SetBindFuction(void *_this, void *_setter, void *_getter){};

public:
  void SetKey(const char *szKey);
  const char *GetKey();
  AttributeBase *SetCompatibleKey(const char *szKey);
  const char *GetCompatibleKey();
  bool IsKey(const char *szKey);

  const char *GetDesc();
  AttributeBase *AsData();
  bool IsData();
  AttributeBase *ReloadOnChanged();
  bool IsReloadOnChanged();
  
  AttributeBase *SetParentKey(const char *szParent);
  const char *GetParentKey();

  void SetGroupName(const char *szGroupName);
  const char *GetGroupName();
  void SetUIApplication(Application *p);
  void SetResource(Resource *p);
  Application *GetUIApplication();

protected:
  const char *ConstructTempLPCTSTR(long lValue);
  
private:
  std::string m_strKey;
  
  // 兼容key。例如：width="100"，也可以写成
  // layout.width="100"
  std::string m_strKeyCompatible; 
                             
  std::string m_strDesc;

  // 例如back.render.image，它的groupname是ImageRender，它的
  // parentkey是back.render.type

  // 属性所属的对象名，例如"Object" "ImageRender"
  std::string m_strGroupName;
  // 父结点fullkey。例如bkg.render.type
  std::string m_strParentKey;
  bool m_bParentKeyIsNull;

  // 该属性是否是一个控件data。data在换肤的时候不用再加载一次
  bool m_bData;

  // 对于bkg.render.type属性修改时，在editor中重新加载这个控件的所有属性，来切换具体的render属性
  // 目前没想到一个较好的办法来只加载/删除相应的属性。
  bool m_bReloadOnChanged;

protected:
  Resource *m_resource;
};
} // namespace ui
