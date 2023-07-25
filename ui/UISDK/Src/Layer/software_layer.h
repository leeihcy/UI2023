#pragma once
#include "layer.h"

namespace ui
{

class SoftwareLayer : public Layer
{
public:
    SoftwareLayer();
    ~SoftwareLayer();

    virtual LayerType  GetType() override { return Layer_Software; }
	virtual void  UpdateDirty() override;


};

}