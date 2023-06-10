#ifndef INCLUDED_UI_IOBJECTDESCRIPTION
#define INCLUDED_UI_IOBJECTDESCRIPTION

namespace UI
{
enum OBJ_TYPE
{
    OBJ_UNKNOWN          = 0,
    OBJ_CONTROL          = 1,
    OBJ_HWNDHOST         = 2,
    OBJ_PANEL            = 3,
    OBJ_WINDOW           = 4,
};
enum CONTROL_TYPE
{
    // Extent Type
	CONTROL_UNKNOWN      = 0,
    CONTROL_BUTTON       = 1,
    CONTROL_GROUPBOX     = 2,
    CONTROL_CHECKBUTTON  = 3,
    CONTROL_RADIOBUTTON  = 4,
    CONTROL_HYPERLINK    = 5,
    CONTROL_LABEL        = 6,
    CONTROL_PICTURE      = 7,
    CONTROL_EDIT         = 8,
    CONTROL_PROGRESSBAR  = 9,
    CONTROL_SLIDERCTRL   = 10,
    CONTROL_LISTBOX      = 11,
    CONTROL_POPUPLISTBOX = 12,
    CONTROL_HEADERCTRL   = 13,
    CONTROL_LISTVIEWCTRL = 14,
    CONTROL_HSCROLLBAR   = 15,
    CONTROL_VSCROLLBAR   = 16,
    CONTROL_COMBOBOX     = 17,
    CONTROL_RICHEDIT     = 18,
    CONTROL_LEDCTRL      = 19,
    CONTROL_GIFCTRL      = 20,
    CONTROL_MENU         = 21,
//    CONTROL_TOOLTIPWINDOW= 22, -> WINDOW_TOOLTIP
    CONTROL_TREEVIEWCTRL = 23,
    CONTROL_LISTTREEVIEWCTRL = 24,
    CONTROL_FLASH        = 25,
    CONTROL_IE           = 26,
    CONTROL_LISTCTRLITEM = 27, 
    CONTROL_TABCTRL      = 28, 
    CONTROL_SPLITTERBAR  = 29,
    CONTROL_CEF          = 30,
	CONTROL_RICHTEXT     = 31,
};

enum WINDOW_TYPE
{
	WINDOW_SIMPLE = 0,
	WINDOW_CUSTOM = 1,
    WINDOW_TOOLTIP = 2,
};

enum PANEL_TYPE
{
	PANEL_SIMPLE = 0,
    PANEL_SCROLL = 1 ,
    PANEL_LISTCTRLITEMROOT = 2 ,
    PANEL_STAGE3D = 3 ,
};

enum LAYER_TYPE
{
	LAYER_DIRECT   = 1,
	LAYER_BUFFER   = 2,
	LAYER_3D       = 3,
};

#define  CATEGORY_CONTROL    _T("Control");
#define  CATEGORY_CONTAINER  _T("Container");
#define  CATEGORY_WINDOW     _T("Window");

interface  IObjectDescription
{
    virtual void  CreateInstance(UI::ISkinRes* p, void**) = 0;
	virtual OBJ_TYPE  GetMajorType() = 0;
	virtual long  GetMinorType() = 0;
	virtual const wchar_t*  GetCategory() = 0;
	virtual const wchar_t*  GetTagName() = 0;
	virtual const GUID&  GetGUID() = 0;
    virtual void  GetDefaultSize(long& w, long& h) = 0;
    virtual HBITMAP  LoadIcon() = 0;
};


}
#endif 