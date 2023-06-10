#pragma once
#include "Src/Layer/layer.h"

namespace UI
{
class Object;
class Layer;

class ObjectLayer : public IObjectLayerContent
{
public:
    ObjectLayer(Object& o);
    ~ObjectLayer();
	
	void  CreateLayer();
	void  TryDestroyLayer();
	void  DestroyLayer();

    Layer*  GetLayer() const {
                return m_pLayer; }
    Object&  GetObjet() {
                return m_obj;  }

    void  OnSize(uint nWidth, uint nHeight);
    void  OnObjPosInTreeChanged();

protected:
	virtual Object&  GetObject() override { return m_obj;  }
	virtual bool  IsChildOf(ILayerContent*) override;
    virtual bool  IsVisible() override;
    virtual void  Draw(UI::IRenderTarget*) override;
    virtual void  GetWindowRect(RECT* prcOut) override;
    virtual void  GetParentWindowRect(RECT* prcOut) override;
	virtual void  OnLayerDestory() override;
	virtual Layer*  GetParentLayer() override;
	virtual Layer*  GetNextLayer() override;

private:
    Object&  m_obj;
    Layer*  m_pLayer;
};

}