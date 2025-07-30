#ifndef _UI_XMLDEFINE_H_
#define _UI_XMLDEFINE_H_

namespace ui
{

/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//                                XML中的tag定义                               //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

#define XML_SEPARATOR             ','            // xml中的分隔符，如表示颜色、区域
#define XML_PATH_SEPARATOR        '/'            // xml中的路径分隔符（如样式路径，子对象路径）
#define XML_MULTI_SEPARATOR       ';'            // xml中的分隔符,如分隔一个控件的多个style.
#define XML_FLAG_SEPARATOR        '|'            // xml中的分隔符，如文本对齐方式: left|top|vcenter
#define XML_CHILD_SEPARATOR       '.'            // xml中父子之间的分隔符，如Menu.std::string
#define XML_KEYVALUE_SEPARATOR    ':'            // xml中key:value之间的分隔条
#define XML_HEADER                "<?xml version=\"1.0\" encoding=\"utf-8\" ?>"


#define XML_BOOL_VALUE_YES        "yes"
#define XML_BOOL_VALUE_NO         "no"
#define XML_BOOL_VALUE_1          "1"
#define XML_BOOL_VALUE_0          "0"
#define XML_BOOL_VALUE_TRUE       "true"
#define XML_BOOL_VALUE_FALSE      "false"

// #define XML_UI_XMLFILE_EXT   ".ui"          // 工程描述文件的后缀名
#define XML_SKIN_XML              "skin.xml"
#define XML_RESOURCE_XML          "resource.xml"
#define XML_SKIN_PACKET_EXT       "skn"        // 皮肤包后缀名

#define XML_UI               "ui"            // root
#define XML_SKIN             "skin"          // root
#define XML_XML              "xml"
#define XML_LOG              "log"
#define XML_IMG              "image"
#define XML_FONT             "font"
#define XML_COLOR            "color"
#define XML_LAYOUT           "layout"
#define XML_STYLE            "style"
#define XML_I18N             "i18n"
#define XML_I18N_PREFIX      '#'
#define XML_INCLUDE          "include"
#define XML_PROJECT_EXT      "uiproj"          // 工程文件后缀名
#define XML_PROJECT_EXT2     ".uiproj"
#define XML_LOG_UI_ID        "ui"             // ui日志id
#define XML_LAYOUTCONFIG     "layoutconfig"   // 延迟加载皮肤列表
#define XML_PROP             "prop"           // 复杂属性，单独配置在一行
#define XML_KEY              "key"            // <prop> </prop>下面的key

#define XML_ITEM             "item"
#define XML_IMAGE_ITEM_GIF   "gif"
#define XML_IMAGE_ITEM_CURSOR "cursor"
#define XML_ID               "id"
#define XML_NAME             "name"
#define XML_TEXT             "text"
#define XML_TOOLTIP          "tooltip"
#define XML_PATH             "path"
#define XML_VISIBLE          "visible"
#define XML_READONLY         "readonly"
#define XML_DISABLE          "disable"
#define XML_FILE             "file"
#define XML_WINDOW           "window"
#define XML_WINDOW_          "Window"
#define XML_LISTITEM         "listitem"
#define XML_LISTITEM_        "ListItem"

#define XML_ALIGN_LEFT       "left"
#define XML_ALIGN_RIGHT      "right"
#define XML_ALIGN_TOP        "top"
#define XML_ALIGN_BOTTOM     "bottom"
#define XML_ALIGN_CENTER     "center"
#define XML_ALIGN_VCENTER    "vcenter"

enum ALIGN_TYPE
{
    ALIGN_LEFT = 0x00,
    ALIGN_TOP  = 0x01,     
    ALIGN_RIGHT  = 0x02,     
    ALIGN_BOTTOM = 0x04,      
    ALIGN_CENTER =  0x08,
    ALIGN_VCENTER = 0x10,
};


#define XML_FONT_FACENAME    "face"   // 以逗号分隔多个字体，依次进行尝试
#define XML_FONT_SIZE        "size"
#define XML_FONT_WEIGHT      "weight"
#define XML_FONT_ORIENTATION "orientation" 

#define XML_FONT_ITALIC      "italic"
#define XML_FONT_UNDERLINE   "underline"
#define XML_FONT_STRIKEOUT   "strikeout"  
#define XML_FONT_CLEARTYPE   "cleartype"   // 是否使用cleartype样式

#define XML_IMAGE_THEMEHLS            "themehls"    // 改变皮肤色调时，该图片是否参与 0/1，默认为0
#define XML_IMAGE_NEED_ANTIALIASING   "antialiasing"  // 图片是否需要支持抗锯齿（缩放时）
#define XML_IMAGE_DPI_ADAPT           "dpiadapt"    // 是否DPI自适应缩放

#define XML_IMAGE_ITEM_TYPE           "type"          // image类型
#define XML_IMAGE_ITEM_TYPE_IMAGE     "image"         // 默认值
#define XML_IMAGE_ITEM_TYPE_ICON      "icon"          // 图标 
#define XML_IMAGE_ITEM_TYPE_IMAGELIST "imagelist"     // 图片列表
#define XML_IMAGE_ITEM_TYPE_GIF       "gif"           // gif图片
#define XML_IMAGE_ITEM_TYPE_PNGLISTGIF "pnglistgif"   // png列表形式的gif图片
enum IMAGE_ITEM_TYPE
{
    IMAGE_ITEM_TYPE_IMAGE,
    IMAGE_ITEM_TYPE_ICON,
    IMAGE_ITEM_TYPE_IMAGE_LIST,
    IMAGE_ITEM_TYPE_GIF,
    IMAGE_ITEM_TYPE_PNGLISTGIF,
    IMAGE_ITEM_TYPE_CURSOR,
};

#define XML_IMAGE_ICON_WIDTH          "width"         // 要加载的图标宽度
#define XML_IMAGE_ICON_HEIGHT         "height"        // 要加载的图标高度
#define XML_IMAGE_IMAGELIST_COUNT     "count"         // 图片列表项的数目 
#define XML_IMAGE_IMAGELIST_LAYOUT    "layout"        // 图片列表项的排布方式
#define XML_IMAGE_IMAGELIST_LAYOUT_H  "h"  
#define XML_IMAGE_IMAGELIST_LAYOUT_V  "v"

#define XML_COLOR_USESKINHLS          "useskinhls"    // 改变皮肤色调时，该颜色是否参与 0/1，默认为1

// 工程配置
#define XML_PROJECT_NAME       "projectname"     // 工程名称
#define XML_PROJECT_SELECT     "select"          // 当前工程使用了哪款皮肤名称
#define XML_PROJECT_SKIN       "skin"            // 工程中一款皮肤
#define XML_PROJECT_SKIN_NAME  "name"            // 皮肤名称
#define XML_PROJECT_SKIN_PATH  "path"            // 皮肤ui.xml所在路径
#define XML_PROJECT_SKIN_HLS   "hls"             // 该皮肤的色调信息,如"10,0,0"

#define XML_SKINBUILDER_CTRLSDLL "ctrlsdll"      // 外部控件所在的DLL，用于注册创建信息

//
// layout.xml
//

// Object对象属性

//#define XML_ID                   "id"            // 每个对象的标识符
#define XML_STYLECLASS           "style"         // 对象的样式类型
#define XML_WIDTH                "width"         // 对象的宽度，可以为 auto | 数值
#define XML_HEIGHT               "height"        // 对象的高度，可以为 auto | 数值
#define XML_MARGIN               "margin"        // 对象的外边距
#define XML_PADDING              "padding"       // 对象的内边距
#define XML_BORDER               "border"        // 对象的边框
#define XML_MINWIDTH             "minwidth"      // 对象的最小宽度
#define XML_MINHEIGHT            "minheigth"     // 对象的最小高度
#define XML_MAXWIDTH             "maxwidth"      // 对象的最大宽度
#define XML_MAXHEIGHT            "maxheight"     // 对象的最大高度
#define XML_AUTO                 "auto"          // 对象的宽和高设置为由内部内容决定
#define XML_NDEF                 ""              // 未定义
#define XML_ASTERISK             "*"             // 对象的宽和高设置为平分
#define XML_AVERAGE              "avg"           // 对象的宽和高设置为平均
#define XML_PERCENT              "percent"       // 对象的宽和高设置为百分比 pct ?
#define XML_PERCENT_CHAR         '%' 
#define XML_AVERAGE_CHAR         '*'             // 暂不支持 x*,

// EraseBkgnd
#define XML_BACKGND_IS_TRANSPARENT        "transparent"             // 是否需要进行透明处理（刷上父对象的背景）0/1
#define XML_NO_CLIP_DRAW                  "noclip"                  // 在绘制自己时，不需要维护剪裁区域，默认剪裁

enum VISIBILITY_TYPE
{	
	VISIBILITY_COLLAPSED = 0,   // visible=false, need no layout
	VISIBILITY_VISIBLE = 1,     // visible=true
	VISIBILITY_HIDDEN = 2       // visible=false, but layout
};
#define  XML_VISIBILITY_COLLAPSED         "collapsed"
#define  XML_VISIBILITY_VISIBLE           "visible"
#define  XML_VISIBILITY_HIDDEN            "hidden"

#define XML_RENDER                        "render"
#define XML_RENDER_TYPE                   "render.type"             // 设置渲染类型
#define XML_RENDER_TYPE_GRADIENT          "gradient"                // 横向渐变 Color + Color
#define XML_RENDER_TYPE_IMAGE             "image"                   // 贴图Image + Color
#define XML_RENDER_TYPE_IMAGELISTITEM     "imagelistitem"           // 图片列表中的一项，需要指定其索引
#define XML_RENDER_TYPE_IMAGELIST         "imagelist"               // 图片列表
#define XML_RENDER_TYPE_COLOR             "color"                   // 纯色填充 Color
#define XML_RENDER_TYPE_SYSCOLOR          "syscolor"                // 系统色填充 Color
#define XML_RENDER_TYPE_COLORLIST         "colorlist"               // 类似office 2003工具栏式按钮
#define XML_RENDER_TYPE_NOTHEME           "notheme"                 // win2000主题控件
#define XML_RENDER_TYPE_THEME             "theme"                   // 系统主题控件
#define XML_RENDER_TYPE_NULL              "null"                    // 不绘制该render，但也会实例化该render指针，避免控件默认使用theme主题


#define XML_RENDER_THEME_DISABLE          "render.theme.disable"    // 是否强制使用无主题的渲染
#define XML_RENDER_COLOR                  "render.color"            // 填充颜色
#define XML_RENDER_BORDER                 "render.border"           // 边框大小 Rect
#define XML_RENDER_BORDERCOLOR            "render.border.color"     // 边框颜色
#define XML_RENDER_COLORFROM              "render.color.from"       // 渐变起始色
#define XML_RENDER_COLORTO                "render.color.to"         // 渐变终点色
#define XML_RENDER_COLOR_GRADIENT_DIR     "render.color.gradient.dir" // 渐变色方向

#define XML_RENDER_COLOR_GRADIENT_DIR_H   "h"
#define XML_RENDER_COLOR_GRADIENT_DIR_V   "v"
enum
{
    RENDER_COLOR_GRADIENT_DIR_H,
    RENDER_COLOR_GRADIENT_DIR_V,
};

#define XML_RENDER_IMAGE                  "render.image"            // 背景图片
#define XML_RENDER_IMAGE9REGION           "render.image.9region"    // 背景伸缩方式
#define XML_RENDER_LIST_STATEMAPINDEX     "render.statemap"    // 设置控件绘制状态对应的图片项，例如0:0,1:0,2:0,3:0,4:1 表示有两个图片项，但selection用第二张图片，其它用第一张图片
#define XML_RENDER_COLORLIST_COUNT        "render.colorlist.count"  // 颜色列表的数量
// #define XML_RENDER_IMAGELIST_COUNT        "render.imagelist.count"  // 图片列表中的图片数量
// #define XML_RENDER_IMAGELIST_LAYOUT       "render.imagelist.layout" // 图片排列方向(默认为横向)
// #define XML_RENDER_IMAGELIST_LAYOUT_H     "h"                       // 图片横向排列
// #define XML_RENDER_IMAGELIST_LAYOUT_V     "v"                       // 图片纵向排列
#define XML_RENDER_IMAGELIST_ALPHA_ANIMATE "render.imagelist.alphaanimate" // 是否启用alpha渐变动画

#define XML_RENDER_IMAGELISTITEM_INDEX    "render.imagelistitem.index"  // 图片列表项的索引值

#define XML_RENDER_IMAGE_DRAWTYPE         "render.image.drawtype"   // 图片绘制方式
#define XML_RENDER_IMAGE_DRAWTYPE_BITBLT  "bitblt"                  // (0,0)位置开始贴图Image + Color
#define XML_RENDER_IMAGE_DRAWTYPE_TILE    "tile"                    // (0,0)位置开始平铺Image
#define XML_RENDER_IMAGE_DRAWTYPE_STRETCH "stretch"                 // 拉伸模式Image + Image9Region*
#define XML_RENDER_IMAGE_DRAWTYPE_STRETCH_DISABLE_AA "stretch.disableaa"       // 拉伸模式Image + 不要抗锯齿
#define XML_RENDER_IMAGE_DRAWTYPE_ADAPT   "adapt"                   // 自适应居中，不会超出绘制范围
#define XML_RENDER_IMAGE_DRAWTYPE_CENTER  "center"                  // 居中，可能超出绘制范围
#define XML_RENDER_IMAGE_DRAWTYPE_STRETCH_BORDER      "stretchborder"     // 通过9宫拉伸绘制边框，不绘制中间部分
#define XML_RENDER_IMAGE_DRAWTYPE_BITBLT_RIGHTTOP     "bitbltrighttop"    // 左上角对齐
#define XML_RENDER_IMAGE_DRAWTYPE_BITBLT_LEFTBOTTOM   "bitbltleftbottom"  // 左下角对齐
#define XML_RENDER_IMAGE_DRAWTYPE_BITBLT_RIGHTBOTTOM  "bitbltrightbottom" // 右下角对齐
#define XML_RENDER_IMAGE_DRAWTYPE_BITBLT_LEFTVCENTER  "bitbltleftvcenter" // 左中对齐

#define XML_RENDER_IMAGE_ALPHA            "render.image.alpha"      // 图片绘制时使用的alpha值
#define XML_RENDER_IMAGE_SRC_REGION       "render.image.srcregion"  // 图片绘制的源区域

#define XML_BACKGND_RENDER_PREFIX         "back."    // 背景渲染属性前缀，如back.render.type="" fore.render.image=""
#define XML_FOREGND_RENDER_PREFIX         "fore."    // 前景
#define XML_ICON_RENDER_PREFIX            "icon."    // 图标属性渲染前缀
#define XML_BKGCOLOR                      "bkgcolor"  // 背景颜色
#define XML_BKGCOLOR_PREFIX               "bkgcolor." // 背景颜色前缀

enum IMAGELIST_LAYOUT_TYPE
{
    IMAGELIST_LAYOUT_TYPE_H,
    IMAGELIST_LAYOUT_TYPE_V
};

#define XML_TEXTRENDER                      "text.render"            // 文字渲染
#define XML_TEXTRENDER_TYPE                 "text.render.type"       // 设置文字渲染类型
#define XML_TEXTRENDER_TYPE_SIMPLE          "simple"                // color + font
#define XML_TEXTRENDER_TYPE_COLORLIST       "colorlist"             // color list + font
#define XML_TEXTRENDER_TYPE_FONTLIST        "fontlist"              // font list + color
#define XML_TEXTRENDER_TYPE_FONTCOLORLIST   "fontcolorlist"         // font list + color list
#define XML_TEXTRENDER_TYPE_CONTRAST_COLOR    "contrastcolor"       // font + 跟随背景色自适应的黑白color
#define XML_TEXTRENDER_TYPE_CONTRASTCOLORLIST "contrastcolorlist"   // 

#define XML_TEXTRENDER_COLOR                "text.render.color"      // 字体颜色
#define XML_TEXTRENDER_FONT                 "text.render.font"       // 字体
#define XML_TEXTRENDER_COLORLIST_COUNT      "text.render.colorlist.count"
#define XML_TEXTRENDER_FONTCOLORLIST_COUNT  "text.render.fontcolorlist.count"  // 字体、颜色数量
#define XML_TEXTRENDER_ALIGN                "text.render.align"      // 对齐
#define XML_TEXTRENDER_EFFECT               "text.render.effect"     // 特效

#define XML_TEXTRENDER_ALIGN_TOP            "top"
#define XML_TEXTRENDER_ALIGN_BOTTOM         "bottom"
#define XML_TEXTRENDER_ALIGN_LEFT           "left"
#define XML_TEXTRENDER_ALIGN_RIGHT          "right"
#define XML_TEXTRENDER_ALIGN_CENTER         "center"
#define XML_TEXTRENDER_ALIGN_VCENTER        "vcenter"
#define XML_TEXTRENDER_ALIGN_END_ELLIPSIS   "endellipsis"
#define XML_TEXTRENDER_ALIGN_SINGLELINE     "singleline"
#define XML_TEXTRENDER_ALIGN_MULTILINE      "multiline"
#define XML_TEXTRENDER_ALIGN_NO_PREFIX      "noprefix"

#define XML_TEXTRENDER_EFFECT_NONE          "none"
#define XML_TEXTRENDER_EFFECT_HALO          "halo"
#define XML_TEXTRENDER_EFFECT_HALO_RADIUS   "text.render.halo.radius"
#define XML_TEXTRENDER_EFFECT_HALO_SHADOW_COLOR "text.render.halo.shadow.color"
#define XML_TEXTRENDER_EFFECT_ENDALPHAMASK  "endalphamask"

// windows attribute
// #define  XML_WINDOW_MAX_WIDTH                           "maxwidth"
// #define  XML_WINDOW_MAX_HEIGHT                          "maxheight"
// #define  XML_WINDOW_MIN_WIDHT                           "minwidth"
// #define  XML_WINDOW_MIN_HEIGHT                          "minheight"

#define  XML_WINDOW_MAXIMIZE_BORDER                     "maximizeborder"   // int类型，最大化时位于屏幕外的区域大小
#define  XML_WINDOW_RESIZE_BORDER                       "resizeborder"     // int类型，拖拽区域大小
#define  XML_WINDOW_RESIZE_CAPABILITY                   "resizecapability" // 拖拽能力
#define  XML_WINDOW_RESIZE_CAPABILITY_NONE              "none"             // 禁止拖拽
#define  XML_WINDOW_RESIZE_CAPABILITY_CAPTION           "caption"          // 只能拖拽，不能修改大小
#define  XML_WINDOW_RESIZE_CAPABILITY_ALL               "all"              // 能拖拽，能修改大小
#define  XML_WINDOW_RESIZE_CAPABILITY_TRANSPARENT       "transparent"    // 穿透到父窗口拖拽

#define WRSB_TRANSPARENT      0x10000    // 用于实现鼠标在子窗口上拖拽时，能够直接拖拽父窗口
#define WRSB_TOP              0x8000     // ↑
#define WRSB_LEFT             0x4000     // ←
#define WRSB_BOTTOM           0x2000     // ↓
#define WRSB_RIGHT            0x1000     // →
#define WRSB_TOPLEFT          0x0800     // ↖
#define WRSB_TOPRIGHT         0x0400     // ↗
#define WRSB_BOTTOMLEFT       0x0200     // ↙
#define WRSB_BOTTOMRIGHT      0x0100     // ↘
#define WRSB_CAPTION          0x0080     // 
#define WRSB_DRAGSIZE         (WRSB_TOP | WRSB_LEFT | WRSB_BOTTOM | WRSB_RIGHT | WRSB_TOPLEFT | WRSB_TOPRIGHT | WRSB_BOTTOMLEFT | WRSB_BOTTOMRIGHT)
#define WRSB_ALL              (WRSB_CAPTION | WRSB_DRAGSIZE)
#define WRSB_NONE             0x0

// window rgn
#define  XML_WINDOW_TRANSPARENT_TYPE                    "transparent.type"
#define  XML_WINDOW_TRANSPARENT_9REGION                 "transparent.9region" // 窗口透明区域参数（9Region类型，可为4，8，9三种）

#define  XML_WINDOW_TRANSPARENT_TYPE_AERO               "aero"             // 使用分层窗口进行透明
#define  XML_WINDOW_TRANSPARENT_TYPE_LAYERED            "layered"          // 使用分层窗口进行透明
#define  XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR          "maskcolor"        // 使用颜色值
#define  XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA          "maskalpha"        // 使用透明值
#define  XML_WINDOW_TRANSPARENT_TYPE_ANTIALIASING       "anti"             // 抗锯齿圆角
#define  XML_WINDOW_TRANSPARENT_TYPE_GLOW               "glow"             // 类VS2013边框发光效果

#define  XML_WINDOW_TRANSPARENT_MASKCOLOR_VALUE         "transparent.maskcolor"  // 窗口透明色
#define  XML_WINDOW_TRANSPARENT_MASKALPHA_VALUE         "transparent.maskalpha"  // 将窗口中小于alpha值通过的抠掉

#define  XML_WINDOW_TRANSPARENT_ANTI_9REGION            "transparent.anti.9region"       // 左上角圆角度数，上侧缩进，右上角圆角度数，左侧缩进，右侧缩进，左下角圆角度数，下侧缩进，右下角圆角度数
#define  XML_WINDOW_TRANSPARENT_ANTI_BORDERINDENT       "transparent.anti.borderIndent"  // 四周缩进值。默认为全0。用于解决aero blur切换到anti时，四周有一部分阴影的问题
#define  XML_WINDOW_TRANSPARENT_MASKALPHA_9REGION       "transparent.maskalpha.9region"  // 全0表示什么也不做。全-1表示整个窗口过滤。top,left,right.bottom四个值如果大于0表示要过滤这四个边，小于0表示不过滤，仅用于设置四个角的位置参数
#define  XML_WINDOW_TRANSPARENT_MASKCOLOR_9REGION       "transparent.maskcolor.9region"  // 同XML_WINDOW_TRANSPARENT_MASKALPHA_9REGION

#define  XML_WINDOW_TRANSPARENT_AERO_MODE               "transparent.aero.mode"   // aero透明模式
#define  XML_WINDOW_TRANSPARENT_AERO_MODE_TRANS         "trans"
#define  XML_WINDOW_TRANSPARENT_AERO_MODE_BLUR          "blur"
enum AERO_MODE 
{
    AERO_MODE_TRANSPARENT,   // 只透明
    AERO_MODE_BLUR,          // 带模糊
};
#define  XML_WINDOW_TRANSPARENT_AERO_TRANS_MARGINS      "transparent.aero.trans.margins"  // rect 透明区域设置（DwmExtendFrameIntoClientArea参数）
#define  XML_WINDOW_TRANSPARENT_AERO_BLUR_REGION        "transparent.aero.blur.region"    // rect 磨沙效果区域（DwmEnableBlurBehindWindow参数）
#define  XML_WINDOW_TRANSPARENT_AERO_BLUR_AUTOCLIPCHILDCTRL  "transparent.aero.blur.autoclipchildctrl"  // 是否自动剪裁子控件
#define  XML_WINDOW_TRANSPARENT_TYPE_AERO_DISABLE       "transparent.aero.disable"        // aero模式不可用时的窗口透明类型


enum WINDOW_TRANSPARENT_TYPE
{
    WINDOW_TRANSPARENT_TYPE_INVALID = 0,
    WINDOW_TRANSPARENT_TYPE_NORMAL = 1,      
    WINDOW_TRANSPARENT_TYPE_MASKCOLOR = 2,
    WINDOW_TRANSPARENT_TYPE_MASKALPHA = 4,
    WINDOW_TRANSPARENT_TYPE_ANTIALIASING = 8,
    WINDOW_TRANSPARENT_TYPE_LAYERED = 16,   
    WINDOW_TRANSPARENT_TYPE_AERO = 32,     
	WINDOW_TRANSPARENT_TYPE_GLOW = 64,
};

#define XML_WINDOW_GRAPHICS_RENDER_LIBRARY          "graphics"         // 图形引擎类型
#define XML_WINDOW_GRAPHICS_RENDER_LIBRARY_GDI      "gdi"
#define XML_WINDOW_GRAPHICS_RENDER_LIBRARY_GDIPLUS  "gdiplus"
#define XML_WINDOW_GRAPHICS_RENDER_LIBRARY_D2D      "d2d"
#define XML_WINDOW_GRAPHICS_RENDER_LIBRARY_DIRECT2D "direct2d"
#define XML_WINDOW_GRAPHICS_RENDER_LIBRARY_D3D      "d3d"
#define XML_WINDOW_GRAPHICS_RENDER_LIBRARY_DIRECT3D "direct3d"

#define XML_WINDOW_NEED_ALPHACHANNEL                "needalphachannel"  // 窗口是否需要渲染alpha通道。默认为true


enum GRAPHICS_RENDER_LIBRARY_TYPE
{
	//	GRAPHICS_RENDER_LIBRARY_TYPE_AUTO = 0,   // <- 依据窗口类型决定。分层窗口使用GDIPLUS,普通窗口使用GDI，废弃，逻辑太复杂
	GRAPHICS_RENDER_LIBRARY_TYPE_GDI = 1,
	GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS,
	GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D,
	GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT3D,
  GRAPHICS_RENDER_LIBRARY_TYPE_SKIA,
};

#define XML_TEXTURE_RENDER_PREFIX            "texture."         // customwindow用于支持换肤的皮肤render前缀
#define XML_MASK_RENDER_PREFIX               "mask."            // 在绘制完子结点后，再画的一层(PostPaint)
#define XML_PANEL_RENDER_REGION              "render.region"    // 皮肤图片在窗口上显示的区域，类型于padding的配置

enum ROUND_CORNER_TYPE
{
    ROUND_CORNER_NONE,
    ROUND_CORNER_ANTI, // 抗锯齿模式
    ROUND_CORNER_DIRECT_THROW,  // 直接删除圆角处像素值
};

// 其它通用属性
#define XML_CURSOR                           "cursor"       // 对象的鼠标样式
#define XML_CURSOR_IDC_ARROW                 "idc_arrow"    // 各种系统鼠标样式配置
#define XML_CURSOR_IDC_IBEAM                 "idc_ibeam"
#define XML_CURSOR_IDC_WAIT                  "idc_wait"
#define XML_CURSOR_IDC_CROSS                 "idc_cross"
#define XML_CURSOR_IDC_UPARROW               "idc_uparrow"
#define XML_CURSOR_IDC_SIZE                  "idc_size"
#define XML_CURSOR_IDC_ICON                  "idc_icon"
#define XML_CURSOR_IDC_SIZENWSE              "idc_sizenwse"
#define XML_CURSOR_IDC_SIZENESW              "idc_sizenesw"
#define XML_CURSOR_IDC_SIZEWE                "idc_sizewe"
#define XML_CURSOR_IDC_SIZENS                "idc_sizens"
#define XML_CURSOR_IDC_SIZEALL               "idc_sizeall"
#define XML_CURSOR_IDC_NO                    "idc_no"
#define XML_CURSOR_IDC_HAND                  "idc_hand"
#define XML_CURSOR_IDC_APPSTARTING           "idc_appstarting"
#define XML_CURSOR_IDC_HELP                  "idc_help"

#define XML_CURSOR_IDC_DRAG_SEPERATOR        "idc_drag_seperator"
#define XML_CURSOR_IDC_SPLITBAR_H            "idc_splitbar_h"
#define XML_CURSOR_IDC_SPLITBAR_V            "idc_splitbar_v"


// #define XML_TOOLTIP                          "tooltip"      // 对象的提示信息，自定义的提示内容需要代码自行添加
#define XML_CONTROL_STYLE_GROUP              "group"
#define XML_CONTROL_STYLE_TABABLE            "tabstop"
#define XML_REJEST_SELF_MOUSE_MSG            "rejectselfmousemsg"  // 不接受鼠标消息
#define XML_REJEST_MOUSE_MSG                 "rejectmousemsg"  // 不接受鼠标消息
#define XML_ZINDEX_OVERLAP                   "zindexoverlap"   // z序重叠了标识
#define XML_ZORDER                           "zorder"          // 在本层中的z序，当不为0时，将为该对象创建一个layer
#define XML_ISNCCHILD                        "ncchild"         // 是一个ncchild，例如滚动条

#define XML_LAYER                            "layer"           // 为该对象建立一个renderlayer（带缓存）
#define XML_LAYER_OPACITY                    "layer.opacity"   // layer的透明度，0-255
#define XML_LAYER_NEED_CLEAR                 "layer.needclear" // layer刷新时是否需要先清除缓存，默认为false

// window
// #define  XML_WINDOW_BK_TYPE                    "bktype"     // 窗口背景绘制类型
// #define  XML_WINDOW_BK_TYPE_COLOR              "color"      // 窗口背景绘制类型--颜色，画刷画笔
// #define  XML_WINDOW_BK_TYPE_TILE               "tile"       // 窗口背景绘制类型--平铺
// #define  XML_WINDOW_BK_TYPE_SIMPLE             "simple"     // 窗口背景绘制类型--简单，(0,0)bitblt
// #define  XML_WINDOW_BK_TYPE_CENTER             "center"     // 窗口背景绘制类型--居中
// #define  XML_WINDOW_BK_TYPE_STRETCH            "stretch"    // 窗口背景绘制类型--拉伸
// #define  XML_WINDOW_BK_TYPE_FRAME              "frame"      // 窗口背景绘制类型--边框
// #define  XML_WINDOW_BK_IMAGE                   "bkimage"    // 窗口背景图片名称
// #define  XML_WINDOW_BK_TYPE_COLOR_PEN          "bkpencol"   // bktype=color，窗口边框色
// #define  XML_WINDOW_BK_TYPE_COLOR_BRUSH        "bkbrushcol" // bktype=color，窗口背景色
// #define  XML_WINDOW_BK_TYPE_FRAME_9REGION      "bk9region"  // bktype=frame，窗口切割范围

// 系统按钮ID
#define XML_SYS_BTN_RESTORE       "sys_restore"   // 系统还原按钮ID
#define XML_SYS_BTN_MAXIMIZE      "sys_maximize"  // 系统最大化按钮ID
#define XML_SYS_BTN_MINIMIZE      "sys_minimize"  // 系统最小化按钮ID
#define XML_SYS_BTN_CLOSE         "sys_close"     // 系统关闭按钮ID

// hwnd host
#define XML_HWNDHOST_CONTROL_ID   "controlid"     // 控件ID数值，整形

#pragma region // 布局layout的属性
#define XML_LAYOUT_PREFIX        "layout."
#define XML_LAYOUT_TYPE          "layout.type"    // 对象的布局类型
//#define XML_LAYOUT_STACK         "stack"          // 布局类型：stack
#define XML_LAYOUT_VERT          "vert"           // 布局类型：vert
#define XML_LAYOUT_HORZ          "horz"           // 布局类型：horz
#define XML_LAYOUT_NULL          "null"           // 布局类型：null
#define XML_LAYOUT_GRID          "grid"           // 布局类型：grid
#define XML_LAYOUT_DOCK          "dock"           // 布局类型：dock
#define XML_LAYOUT_CANVAS        "canvas"         // 布局类型：canvas
#define XML_LAYOUT_CARD          "card"           // 布局类型：card
#define XML_LAYOUT_AVERAGE       "average"        // 布局类型：average

// stack布局的属性
// #define XML_LAYOUT_STACK_DIRECTION      "layout.direction"  // stack布局的方向
// #define XML_LAYOUT_STACK_LEFTTORIGHT    "lefttoright"       // xml中可配置的值
// #define XML_LAYOUT_STACK_RIGHTTOLEFT    "righttoleft"       // xml中可配置的值
// #define XML_LAYOUT_STACK_LEFTANDRIGHT   "leftandright"      // xml中可配置的值
// #define XML_LAYOUT_STACK_TOPTOBOTTOM    "toptobottom"       // xml中可配置的值
// #define XML_LAYOUT_STACK_BOTTOMTOTOP    "bottomtotop"       // xml中可配置的值
// #define XML_LAYOUT_STACK_TOPANDBOTTOM   "topandbottom"      // xml中可配置的值

// enum LAYOUT_STACK_DIRECTION
// {
//     LAYOUT_STACK_LEFTTORIGHT  = 0,   
//     LAYOUT_STACK_RIGHTTOLEFT  = 1,   
//     LAYOUT_STACK_LEFTANDRIGHT = 2,  
//     LAYOUT_STACK_TOPTOBOTTOM  = 3,   
//     LAYOUT_STACK_BOTTOMTOTOP  = 4,   
//     LAYOUT_STACK_TOPANDBOTTOM = 5   
// };
#define XML_LAYOUT_STACK_GAP            "layout.gap"        // 每一项之间的间隙

// average布局
#define XML_LAYOUT_AVERAGE_DIRECTION    "layout.direction"  // stack布局的方向
#define XML_LAYOUT_AVERAGE_H            "h"
#define XML_LAYOUT_AVERAGE_V            "v"

enum LAYOUT_AVERAGE_DIRECTION
{
    LAYOUT_AVERAGE_H,
    LAYOUT_AVERAGE_V
};

#define XML_LAYOUT_AVERAGE_GAP         "layout.gap"        // 每一项之间的间隙

// dock布局的属性

// grid布局的属性
#define XML_LAYOUT_GRID_WIDTH           "layout.width"      // grid布局的各列的宽度定义
#define XML_LAYOUT_GRID_HEIGHT          "layout.height"     // grid布局的各列的高度定义

#pragma endregion

#pragma region // 布局子元素属性

enum LAYOUT_FLAG
{
    LAYOUT_ITEM_ALIGN_LEFT   = 0x0001,
    LAYOUT_ITEM_ALIGN_TOP    = 0x0002,
    LAYOUT_ITEM_ALIGN_RIGHT  = 0x0004,
    LAYOUT_ITEM_ALIGN_BOTTOM = 0x0010,
    LAYOUT_ITEM_ALIGN_CENTER = 0x0020,
    LAYOUT_ITEM_ALIGN_VCENTER = 0x0040,
    LAYOUT_ITEM_ALIGN_FILL   = 0x0080,  // 填充剩余空间--stack布局
	// LAYOUT_FLAG_FORCE_DESIREDSIZE = 0x0100, // 强制使用控件自己的期望大小，废弃，再调一次get desired size得了

    LAYOUT_ITEM_ALIGN_LTRB = LAYOUT_ITEM_ALIGN_LEFT|LAYOUT_ITEM_ALIGN_TOP|LAYOUT_ITEM_ALIGN_RIGHT|LAYOUT_ITEM_ALIGN_BOTTOM,

	// todo: 使用-1作为NDEF有问题
	LAYOUT_ITEM_LEFT_NOT_DEFINE = 0x0100,
	LAYOUT_ITEM_TOP_NOT_DEFINE = 0x0200,
	LAYOUT_ITEM_RIGHT_NOT_DEFINE = 0x0400,
	LAYOUT_ITEM_BOTTOM_NOT_DEFINE = 0x0800,
};


// canvas布局中的子元素的属性
#define XML_LAYOUT_ITEM_LEFT        "layout.left"
#define XML_LAYOUT_ITEM_TOP         "layout.top"
#define XML_LAYOUT_ITEM_RIGHT       "layout.right"
#define XML_LAYOUT_ITEM_BOTTOM      "layout.bottom"

// stack布局中的子元素的属性
#define XML_LAYOUT_ITEM_ALIGN          "layout.align"       // 当layout.direction=leftandright||topandbottom时，需要指定子元素到底停靠在哪一侧
#define XML_LAYOUT_ITEM_ALIGN_LEFT     "left"
#define XML_LAYOUT_ITEM_ALIGN_RIGHT    "right"
#define XML_LAYOUT_ITEM_ALIGN_TOP      "top"
#define XML_LAYOUT_ITEM_ALIGN_BOTTOM   "bottom"

#define XML_LAYOUT_ITEM_ALIGN_CENTER   "center"  // -- stack/dock layout
#define XML_LAYOUT_ITEM_ALIGN_VCENTER  "vcenter" // -- stack layout
#define XML_LAYOUT_ITEM_ALIGN_FILL     "fill"    // -- stack layout

// 子控件四边全对齐布局，父对象在Measure自己时，如何处理该子控件的desiredsize
// 1. 正常情况下，由于子控件采用的是lrtb对齐，将不测试自己的大小，而是直接依赖于父对象
// 2. 在菜单窗口场景下，窗口的大小则是取决于菜单内容大小，但菜单又是lrtb对齐布局的，
//    因此窗口在遍历时，菜单的desiredsize不应该依赖父对象，而是返回真实大小
// 废弃，再调一次get desired size得了
//#define XML_LAYOUT_ALIGN_FORCE_DESIREDSIZE  "xxxx"


// enum LAYOUT_STACK_ITEM_ALIGN
// {
// 	LAYOUT_STACK_ITEM_ALIGN_LEFT = 0,
// 	LAYOUT_STACK_ITEM_ALIGN_TOP = 0,
// 	LAYOUT_STACK_ITEM_ALIGN_RIGHT = 1,
// 	LAYOUT_STACK_ITEM_ALIGN_BOTTOM = 1,
// };

// dock布局中的子元素的属性
// #define XML_LAYOUT_DOCK_DOCK          "layout.dock"       // 子元素停靠在哪一个方向
// #define XML_LAYOUT_DOCK_DOCK_LEFT     "left"
// #define XML_LAYOUT_DOCK_DOCK_RIGHT    "right"
// #define XML_LAYOUT_DOCK_DOCK_TOP      "top"
// #define XML_LAYOUT_DOCK_DOCK_BOTTOM   "bottom"
// #define XML_LAYOUT_DOCK_DOCK_CENTER   "center"

// grid布局中的子元素的属性
#define XML_LAYOUT_GRID_ROW          "layout.row"        // 对应 m_nConfigTop
#define XML_LAYOUT_GRID_COL          "layout.col"        // 对应 m_nConfigLeft
#define XML_LAYOUT_GRID_ROWSPAN      "layout.rowspan"    // 对应 m_nConfigBottom
#define XML_LAYOUT_GRID_COLSPAN      "layout.colspan"    // 对应 m_nConfigRight

#pragma endregion
//
// style.xml ===>
//
enum STYLE_SELECTOR_TYPE
{
    STYLE_SELECTOR_TYPE_ID,
    STYLE_SELECTOR_TYPE_CLASS,
    STYLE_SELECTOR_TYPE_TAG
};
#define  XML_STYLE_SELECTOR_TYPE_ID            "id"       // id 选择器
#define  XML_STYLE_SELECTOR_TYPE_CLASS         "class"    // class选择器
#define  XML_STYLE_INHERIT                     "inherit"  // 继承属性

#define  STYLE_ID_PREFIX                       '#'        // id选择器前缀
#define  STYLE_CLASS_PREFIX                    '.'        // class选择器前缀

// Control

// Label

// Picture

// GifPicture
#define  XML_GIFCTRL_GIF   "gif"       // gif资源id
#define  XML_GIFCTRL_AUTOPLAY    "autoplay"   // 是否自动开始，默认为false，需要手动调用start函数

// GroupBox
#define XML_GROUPBOX_TEXTINDENT   "textindent"  // 文字缩进值
#define XML_GROUPBOX_TEXTMARGIN   "textmargin"  // 文字与边框的间距
#define XML_GROUPBOX_TEXTALIGN    "textalign"   // 文字对齐方式

//////////////////////////////////////////////////////////////////////////
// Button

#define  XML_BUTTON_DEFPUSHBUTTON       "defpushbutton"    // 是否是默认按钮
#define  XML_BUTTON_DRAW_FOCUS_TYPE     "drawfocus"        // 如何绘制焦点
#define  XML_BUTTON_DRAW_FOCUS_TYPE_NONE   "none"        
#define  XML_BUTTON_DRAW_FOCUS_TYPE_DOT    "dot"    
#define  XML_BUTTON_HAND_CUROSR         "handcursor"       // 显示手型鼠标样式

typedef enum
{
    BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE,                               // 不绘制
    BUTTON_RENDER_DRAW_FOCUS_TYPE_DOT,                                // 使用虚线点绘制
}BUTTON_RENDER_DRAW_FOCUS_TYPE;

#define XML_BUTTON_AUTOSIZE_TYPE           "autosize"     // 当没有指定按钮大小时，如何计算其大小
#define XML_BUTTON_AUTOSIZE_TYPE_BKIMAGE   "backimage"      // 背景图片的大小决定按钮大小
#define XML_BUTTON_AUTOSIZE_TYPE_CONTENT   "content"      // 背景图片的内容(图标+文本)决定按钮大小
#define XML_BUTTON_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH "bkimagehcontentw"  // 图片的高度+内容的宽度

enum BUTTON_AUTO_SIZE_TYPE
{
    BUTTON_AUTOSIZE_TYPE_NDEF,
    BUTTON_AUTOSIZE_TYPE_BKIMAGE,
    BUTTON_AUTOSIZE_TYPE_CONTENT,
    BUTTON_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH
};

#define XML_BUTTON_ICON_ALIGN  "iconalign"    // 按钮图标的位置

// #define  XML_BUTTON_ICON                             "icon"             // 按钮图标
// #define  XML_BUTTON_ICON_RENDER_TYPE                 "icontype"         // 按钮图标绘制类型
// #define  XML_BUTTON_ICON_RENDER_TYPE_STATIC          "static"           // 静态图标
// #define  XML_BUTTON_ICON_RENDER_TYPE_8STATE          "8state"           // 8态图标
// #define  XML_BUTTON_ICON_RENDER_TYPE_THEME_RADIO     "themeradio"       // 主题单选图标
// #define  XML_BUTTON_ICON_RENDER_TYPE_THEME_CHECK     "themecheck"       // 主题复选图标
// typedef enum
// {
// 	BUTTON_ICON_RENDER_TYPE_NONE,        // 不绘制图标
// 	BUTTON_ICON_RENDER_TYPE_STATIC,
// 	BUTTON_ICON_RENDER_TYPE_GIF,
// 	BUTTON_ICON_RENDER_TYPE_4STATE,
// 	BUTTON_ICON_RENDER_TYPE_8STATE,
// 	BUTTON_ICON_RENDER_TYPE_THEME_RADIO,
// 	BUTTON_ICON_RENDER_TYPE_THEME_CHECK
// }BUTTON_ICON_RENDER_TYPE;

#define   XML_BUTTON_ICON_TEXT_SPACE           "icontextspace"   // 文本与图标之间的间距
// #define  XML_BUTTON_TEXT_RENDER_TYPE           "texttype"         // 文本绘制类型
// #define  XML_BUTTON_TEXT_RENDER_TYPE_SINGLELINE "singleline"      // 单行文本
// #define  XML_BUTTON_TEXT_RENDER_TYPE_HYPERLINK  "hyperlink"       // 超链接模式
// typedef enum
// {
// 	BUTTON_TEXT_RENDER_TYPE_NONE,         // 不绘制文本
// 	BUTTON_TEXT_RENDER_TYPE_SINGLELINE,   // 单行文本
// 	BUTTON_TEXT_RENDER_TYPE_HYPERLINK,    // 超链接
// 
// }BUTTON_TEXT_RENDER_TYPE;

#define XML_BUTTON_CHECKED_TOOLTIP      "checked.tooltip"  // 被选中时的提示信息

// HyperLink
#define  XML_HYPERLINK_HREF              "href"     // 超链接


// #define  XML_BRUSHBUTTON_BKHOVERCOL            "bkhovercol"
// #define  XML_BRUSHBUTTON_BKPRESSCOL            "bkpresscol"
// #define  XML_BRUSHBUTTON_BORDERHOVERCOL        "borderhovercol"
// #define  XML_BRUSHBUTTON_BORDERPRESSCOL        "borderpresscol"
// #define  XML_BRUSHBUTTON_TEXTNORMALCOL         "textnormalcol"
// #define  XML_BRUSHBUTTON_TEXTHOVERCOL          "texthovercol"
// #define  XML_BRUSHBUTTON_TEXTPRESSCOL          "textpresscol"
// #define  XML_BRUSHBUTTON_TEXTDISABLECOL        "textdisablecol"
// #define  XML_BRUSHBUTTON_TEXT                  "text"
// #define  XML_BRUSHBUTTON_ICON                  "icon"

//#define  XML_BUTTON_BK_TYPE                    "bktype"     // 按钮背景绘制类型
//#define  XML_BUTTON_BK_TYPE_COLOR              "color"    

//////////////////////////////////////////////////////////////////////////
// ProgressCtrl / SliderCtrl

#define  XML_PROGRESSBAR_DIRECTION            "direction"       // 滚动条的方向
#define  XML_PROGRESSBAR_DIRECTION_LEFTRIGHT  "lefttoright"       // 左->右
#define  XML_PROGRESSBAR_DIRECTION_BOTTOMTOP  "bottomtotop"       // 下->上
#define  XML_PROGRESSBAR_DIRECTION_RIGHTLEFT  "righttoleft"       // 右->左
#define  XML_PROGRESSBAR_DIRECTION_TOPBOTTOM  "toptobottom"       // 上->下

enum PROGRESSBAR_SCROLL_DIRECTION_TYPE
{
    PROGRESSBAR_SCROLL_LEFT_2_RIGHT,
    PROGRESSBAR_SCROLL_BOTTOM_2_TOP,
    PROGRESSBAR_SCROLL_RIGHT_2_LEFT,
    PROGRESSBAR_SCROLL_TOP_2_BOTTOM
};

#define  XML_SLIDERCTRL_THUMB_POINT_DIRECTION          "thumbarrow"  // 滑块按钮的指向
#define  XML_SLIDERCTRL_THUMB_POINT_DIRECTION_BOTH     "both"            // 默认值
#define  XML_SLIDERCTRL_THUMB_POINT_DIRECTION_LEFT     "left"    
#define  XML_SLIDERCTRL_THUMB_POINT_DIRECTION_TOP      "top"     
#define  XML_SLIDERCTRL_THUMB_POINT_DIRECTION_RIGHT    "right"   
#define  XML_SLIDERCTRL_THUMB_POINT_DIRECTION_BOTTOM   "bottom"   


// #define  PROGRESSCTRL_FLAG_DIRECTION_LEFTRIGHT    0x0000
// #define  PROGRESSCTRL_FLAG_DIRECTION_TOPBOTTOM    0x0001                
// #define  PROGRESSCTRL_FLAG_DIRECTION_RIGHTLEFT    0x0002
// #define  PROGRESSCTRL_FLAG_DIRECTION_BOTTOMTOP    0x0003

#define  XML_SLIDERCTRL_BUTTON_ATTR_PRIFIX     "trackbtn."       // 滑动控件的按钮属性前缀

#define  XML_IMAGEPROGRESSCTRL_FOREIMAGE       "foreimage"       // 前景图片（一张）
#define  XML_IMAGEPROGRESSCTRL_BKGNDIMAGE      "bkgndimage"      // 背景图片（一张）

// Edit
#define  XML_EDIT_TEXT                        "text"             // 文字内容
#define  XML_EDIT_TEXT_ALIGN                  "align"            // 文字对齐方式

#define  XML_EDIT_COLOR                       "color.text"       // 正常文字显示颜色
#define  XML_EDIT_SELECT_COLOR                "color.select"     // 文字被选中时的颜色
#define  XML_EDIT_SELECT_BK_COLOR             "color.select.back"// 文字被选中时背景的颜色
#define  XML_EDIT_DISABLE_COLOR               "color.disable"    // 被禁用时的文本颜色
#define  XML_EDIT_READONLY_COLOR              "color.readonly"   // 只读时的文本颜色
#define  XML_INSTANTEDIT_COLOR                "color.quiet"      // 即时编辑时，未开启编辑时的文本颜色 
#define  XML_EDIT_MAX_LENGTH                  "limit"            // 内容的最长值
#define  XML_EDIT_MAX_LENGTH_BYTELIMIT        "bytelimit"        // 按字节限制长度，汉字算两个
#define  XML_EDIT_CARET_HEIGHT                "caretheight"      // 光标及选中文本背景的高度
//#define  XML_EDIT_MAX_INT

// Scroll Bar
#define  XML_SCROLLBAR_RANGE                  "range"            // 滚动条范围设置
#define  XML_SCROLLBAR_PAGE                   "page"             // 滚动条页大小设置
#define  XML_SCROLLBAR_POS                    "pos"              // 滚动条位置设置
#define  XML_SCROLLBAR_BTNLINE                "btnline"          // 滚动条点击按钮后滚动大小设置
#define  XML_SCROLLBAR_WHEELLINE              "wheelline"        // 使用鼠标滑轮后滚动大小设置

#define  XML_HSCROLLBAR                           "hscrollbar"  // 横向滚动条类型
#define  XML_VSCROLLBAR                           "vscrollbar"  // 纵向滚动条类型
#define  XML_SCROLLBAR_NONE                       "none"        // 该方向无滚动条
#define  XML_SCROLLBAR_AUTO                       "auto"        // 该方向上的滚动条在需要时显示
#define  XML_SCROLLBAR_ALWAYSSHOW                 "show"        // 该方向上的滚动条保持显示
#define  XML_SCROLLBAR_ALWAYSHIDE                 "hide"        // 该方向上支持滚动，但不显示滚动条

enum SCROLLBAR_VISIBLE_TYPE
{
    SCROLLBAR_VISIBLE_NONE,          // 不使用
    SCROLLBAR_VISIBLE_AUTO,          // 当需要滚动的时候再显示
    SCROLLBAR_VISIBLE_SHOW_ALWAYS,   // 一直显示
    SCROLLBAR_VISIBLE_HIDE_ALWAYS,   // 一直隐藏，逻辑上的
};
enum SCROLLBAR_DIRECTION_TYPE
{
    HSCROLLBAR = 1,
    VSCROLLBAR = 2
};

// #define  XML_SCROLLBAR_TYPE                       "type"        // 前接XML_xxSCROLLBAR_PRIFIX，滚动条的类型
// #define  XML_SCROLLBAR_TYPE_SYSTEM                "system"      // 系统样式的滚动条，两个按钮+一个拖动条
// #define  XML_SCROLLBAR_TYPE_LOGIC                 "logic"       // 无滚动条显示，但维护绑定对象进行滚动的数据
// #define  XML_SCROLLBAR_TYPE_MENU                  "menu"        // 
// 
// enum SCROLLBAR_RENDER_TYPE
// {
//     SCROLLBAR_RENDER_TYPE_SYSTEM,
//     SCROLLBAR_RENDER_TYPE_LOGIC,
//     SCROLLBAR_RENDER_TYPE_MENU,
// };

#define  XML_HSCROLLBAR_PRIFIX                    "hscrollbar." // 横向滚动条的属性前缀
#define  XML_VSCROLLBAR_PRIFIX                    "vscrollbar." // 纵向滚动条的属性前缀

#define  XML_SCROLLBAR_RENDER_TYPE                "type"        // 设置滚动条的渲染类

// scrollpanel
#define  XML_SCROLLPANEL_VIEWSIZE                 "viewsize"    // 视图大小，格式为 "100,100"

// listbox
#define  XML_LISTCTRL_ITEM_HEIGHT                 "itemheight"  // 列表控件每一项的高度
#define  XML_LISTCTRL_ITEM_PADDING                "itempadding" // 列表项的缩进

#define  XML_LISTCTRL_MIN_WIDTH                   "minwidth"    //  auto size列表框的最小、大尺寸
#define  XML_LISTCTRL_MIN_HEIGHT                  "minheight"
// #define  XML_LISTCTRL_MAX_WIDTH                   "maxwidth"
// #define  XML_LISTCTRL_MAX_HEIGHT                  "maxheight"

#define  XML_LISTCTRL_EDIT_PRIFIX                 "edit."       // 编辑框的属性前缀
#define  XML_LISTCTRL_BUTTON_PRIFIX               "button."     // 按钮的属性前缀
#define  XML_LISTCTRL_COMBOBOX_PRIFIX             "combobox."   // 下拉列表的属性前缀

// combobox
#define  XML_COMBOBOX_EDIT_PRIFIX                 "edit."       // 编辑框的属性前缀
#define  XML_COMBOBOX_BUTTON_PRIFIX               "button."     // 按钮的属性前缀
#define  XML_COMBOBOX_LISTBOX_PRIFIX              "listbox."    // 下拉列表的属性前缀
#define  XML_COMBOBOX_DROPWNDID                   "dropwnd.id"  // 下拉窗口id

// ledctrl
#define  XML_LEDCTRL_INDEXMAP                     "indexmap"    // 图片索引映射关系
#define  XML_LEDCTRL_ITEMWIDTH                    "itemwidth"   // 配置每一项的显示宽度（默认为图片宽度）
#define  XML_LEDCTRL_ITEMHEIGHT                   "itemheight"  // 配置每一项的显示高度（默认为图片高度）
#define  XML_LEDEXCTRL_SAMEWIDTH                  "samewidth"   // bool，如果大小相同的话，就不用配置posmap属性了
#define  XML_LEDEXCTRL_POSMAP                     "posmap"      // 图片项位置映射关系  0,10;10,16;16,6;22,6;28,6;34,6;40,6;46,6;52,6;58,6;64,6 ...

// menu
#define  XML_MENU                 "Menu"      // layout.xml中菜单结点的tag name
#define  XML_MENU_STRINGITEM      "std::string"    //
#define  XML_MENU_POPUPITEM       "Popup"     //
#define  XML_MENU_SEPARATORITEM   "Separator" // 
#define  XML_MENU_DISABLE         "disable"   // menu item是否可用，为1或者true表示被禁用
#define  XML_MENU_POPUP_WNDID     "wnd.id"    // popup子菜单皮肤资源窗口id
#define  XML_MENU_POPUP_MENUID    "menu.id"   // popup子菜单在窗口中的控件id

#define  XML_MENU_ICONGUTTERWIDTH            "icongutterwidth"  // 菜单图标列的宽度
#define  XML_MENU_POPUPTRIANGLEWIDTH         "trianglewidth"    // 右侧三角形的占用宽度
#define  XML_MENU_SEPARATORHEIGHT            "separatorheight"  // 分隔条的高度
#define  XML_MENU_LEFT_TEXT_INDENT           "textleftindent"   // 左侧文本缩进
#define  XML_MENU_RIGHT_TEXT_INDENT          "textrightindent"  // 左侧文本缩进
#define  XML_MENU_SUBMENU_GAP                "submenugap"       // 子菜单与父菜单之间的间隙，默认为-2

#define  XML_MENU_RADIO_ICON_RENDER_PREFIX   "radioicon."       // 选中图标绘制render前缀
#define  XML_MENU_CHECK_ICON_RENDER_PREFIX   "checkicon."       // 选中图标绘制render前缀
#define  XML_MENU_RADIOCHECK_ICONBK_RENDER_PREFIX   "radiocheckiconbk." // 选中图标背景绘制render前缀
#define  XML_MENU_SEPARATOR_RENDEER_PREFIX   "separator."       // 分隔条绘制render前缀
#define  XML_MENU_TRAIANGLE_RENDER_PREFIX    "triangle."        // 弹出箭头绘制render前缀

#define  XML_MENU_ITEM_ICON_RENDER_PREFIX    "icon."            // 菜单项的图标

#define  MENU_SEPARATOR_ID    -1
#define  MENU_POPUP_ID        0

// header ctrl
#define  XML_HEADERCTRL_RIGHT_BLANK_RENDER_PRIFIX  "rightblank." // 右侧空白区域的背景绘制前缘


// listview ctrl
#define  XML_LISTVIEWCTRL_HEADERCTRL_PREFIX   "headerctrl."  // 列表控件中的header属性前缀

// Flash Wrap
#define  XML_FLASH_URI                        "uri"          // flash文件地址
#define  XML_FLASH_WMODE                      "wmode"        // flash窗口模式
#define  XML_FLASH_WMODE_WINDOW               "window"
#define  XML_FLASH_WMODE_OPAQUE               "opaque"
#define  XML_FLASH_WMODE_TRANSPARENT          "transparent"
enum FLASH_WMODE
{
    FLASH_WMODE_WINDOW,
    FLASH_WMODE_OPAQUE,
    FLASH_WMODE_TRANSPARENT,
};

// Layer -- 过期
// #define  XML_LAYER_TYPE             "type"    // render chain中的layer类型
// #define  XML_LAYER_TYPE_DIRECT      "direct"  // 直接渲染，无需缓存。renderbase类型
// #define  XML_LAYER_TYPE_CONTROL     "control" // 内嵌一个控件，例如flash

//#define  XML_LAYER_TYPE_FLASH       "flash"   // flash层


// TabCtrl
#define   XML_TABCTRL_HEAD_HEIGHT               "head.height"  // 头部的高度
#define   XML_TABCTRL_HEAD_LAYOUT_TYPE          "head.layout"  // 头部布局方式
#define   XML_TABCTRL_HEAD_LAYOUT_TYPE_Win32    "win32"
#define   XML_TABCTRL_HEAD_LAYOUT_TYPE_AVERAGE  "average"
#define   XML_TABCTRL_HEAD_LAYOUT_TYPE_FIX      "fix"
#define   XML_TABCTRL_CONTENTPANEL_PREFIX       "contentpanel."

enum TABCTRL_HEAD_LAYOUT_TYPE
{
    TABCTRL_HEAD_LAYOUT_TYPE_FIX = 0,     // 固定大小排列在左侧
    TABCTRL_HEAD_LAYOUT_TYPE_AVERAGE = 1, // 按照TABCTRL的大小平均设置每个按钮的宽度
    TABCTRL_HEAD_LAYOUT_TYPE_Win32 = 2,   // 操作系统类型
};

// TreeView
#define   XML_EXPANDICON_RENDER_TYPE_PREFIX   "expandicon."   // 展开/收缩图标render前缀


}

#endif 