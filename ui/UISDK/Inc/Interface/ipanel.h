#ifndef _UI_IPANEL_H_
#define _UI_IPANEL_H_

#include "iobject.h"

namespace UI
{
interface ILayout;
interface IObject;
interface IRenderBase;
class Panel;

interface UIAPI_UUID(A591D454-65ED-40C3-8D84-612ABA3C8167) IPanel
 : public IObject
{
    ILayout*   GetLayout();
    void  SetLayoutType(LAYOUTTYPE eLayoutType);

    void  SetTextureRender(IRenderBase* p);
    IRenderBase*  GetTextureRender();

    UI_DECLARE_INTERFACE(Panel);
};

class RoundPanel;
interface UIAPI_UUID(4B9262CE-3380-4742-8C37-5FEF9FFE8B07) IRoundPanel 
	: public IPanel
{
	UI_DECLARE_INTERFACE(RoundPanel);
};

}

#endif  