#pragma once
#include "Src/Layer/layer.h"

// listitemµÄlayerÇÅÁº

namespace UI
{
class Layer;
class ListItemBase;

class ListItemLayer : public IListItemLayerContent
{
public:
    ListItemLayer(ListItemBase& o);
    ~ListItemLayer();

	void  CreateLayer();
	void  TryDestroyLayer();
	void  DestroyLayer();

	Layer*  GetLayer() const {
		return m_pLayer;
	}
	ListItemBase&  GetObjet() {
		return m_item;
	}

	void  OnSize(uint nWidth, uint nHeight);

protected:
	virtual ListItemBase& GetListItem() override;
	virtual void  Draw(UI::IRenderTarget*) override;
	virtual void  OnLayerDestory() override;
	virtual void  GetWindowRect(RECT* prcOut) override;
	virtual void  GetParentWindowRect(RECT* prcOut) override;
	virtual bool  IsChildOf(ILayerContent*) override;
	virtual bool  IsVisible() override;
	virtual Layer*  GetParentLayer() override;
	virtual Layer*  GetNextLayer() override;
private:
    ListItemBase&  m_item;
    Layer*  m_pLayer;
};

}