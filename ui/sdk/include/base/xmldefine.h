#ifndef _UI_XMLDEFINE_H_
#define _UI_XMLDEFINE_H_

namespace ui
{

/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//                                XML中的tag定义                               //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

#define XML_SEPARATOR             L','            // xml中的分隔符，如表示颜色、区域
#define XML_PATH_SEPARATOR        L'/'            // xml中的路径分隔符（如样式路径，子对象路径）
#define XML_MULTI_SEPARATOR       L';'            // xml中的分隔符,如分隔一个控件的多个style.
#define XML_FLAG_SEPARATOR        L'|'            // xml中的分隔符，如文本对齐方式: left|top|vcenter
#define XML_CHILD_SEPARATOR       L'.'            // xml中父子之间的分隔符，如Menu.String
#define XML_KEYVALUE_SEPARATOR    L':'            // xml中key:value之间的分隔条
#define XML_HEADER                L"<?xml version=\"1.0\" encoding=\"utf-8\" ?>"


#define XML_BOOL_VALUE_YES        L"yes"
#define XML_BOOL_VALUE_NO         L"no"
#define XML_BOOL_VALUE_1          L"1"
#define XML_BOOL_VALUE_0          L"0"
#define XML_BOOL_VALUE_TRUE       L"true"
#define XML_BOOL_VALUE_FALSE      L"false"

// #define XML_UI_XMLFILE_EXT   L".ui"          // 工程描述文件的后缀名
#define XML_SKIN_XML              L"skin.xml"
#define XML_SKIN_PACKET_EXT       L"skn"        // 皮肤包后缀名

#define XML_UI               L"ui"            // root
#define XML_SKIN             L"skin"          // root
#define XML_XML              L"xml"
#define XML_LOG              L"log"
#define XML_IMG              L"image"
#define XML_FONT             L"font"
#define XML_COLOR            L"color"
#define XML_LAYOUT           L"layout"
#define XML_STYLE            L"style"
#define XML_I18N             L"i18n"
#define XML_I18N_PREFIX      L'#'
#define XML_INCLUDE          L"include"
#define XML_PROJECT_EXT      L"uiproj"          // 工程文件后缀名
#define XML_PROJECT_EXT2     L".uiproj"
#define XML_LOG_UI_ID        L"ui"             // ui日志id
#define XML_LAYOUTCONFIG     L"layoutconfig"   // 延迟加载皮肤列表
#define XML_PROP             L"prop"           // 复杂属性，单独配置在一行
#define XML_KEY              L"key"            // <prop> </prop>下面的key

#define XML_ITEM             L"item"
#define XML_IMAGE_ITEM_GIF   L"gif"
#define XML_IMAGE_ITEM_CURSOR L"cursor"
#define XML_ID               L"id"
#define XML_NAME             L"name"
#define XML_TEXT             L"text"
#define XML_TOOLTIP          L"tooltip"
#define XML_PATH             L"path"
#define XML_VISIBLE          L"visible"
#define XML_READONLY         L"readonly"
#define XML_DISABLE          L"disable"
#define XML_FILE             L"file"
#define XML_WINDOW           L"window"
#define XML_WINDOW_          L"Window"
#define XML_LISTITEM         L"listitem"
#define XML_LISTITEM_        L"ListItem"

#define XML_ALIGN_LEFT       L"left"
#define XML_ALIGN_RIGHT      L"right"
#define XML_ALIGN_TOP        L"top"
#define XML_ALIGN_BOTTOM     L"bottom"
#define XML_ALIGN_CENTER     L"center"
#define XML_ALIGN_VCENTER    L"vcenter"

enum ALIGN_TYPE
{
    ALIGN_LEFT = 0x00,
    ALIGN_TOP  = 0x01,     
    ALIGN_RIGHT  = 0x02,     
    ALIGN_BOTTOM = 0x04,      
    ALIGN_CENTER =  0x08,
    ALIGN_VCENTER = 0x10,
};


#define XML_FONT_FACENAME    L"face"
#define XML_FONT_FACENAME2   L"face2"  // 当facename字体不存在时，尝试facename2
#define XML_FONT_HEIGHT      L"size"
#define XML_FONT_ORIENTATION L"orientation" 
#define XML_FONT_BOLD        L"bold"
#define XML_FONT_ITALIC      L"italic"
#define XML_FONT_UNDERLINE   L"underline"
#define XML_FONT_STRIKEOUT   L"strikeout"  
#define XML_FONT_CLEARTYPE   L"cleartype"   // 是否使用cleartype样式

#define XML_IMAGE_THEMEHLS            L"themehls"    // 改变皮肤色调时，该图片是否参与 0/1，默认为0
#define XML_IMAGE_NEED_ANTIALIASING   L"antialiasing"  // 图片是否需要支持抗锯齿（缩放时）
#define XML_IMAGE_DPI_ADAPT           L"dpiadapt"    // 是否DPI自适应缩放

#define XML_IMAGE_ITEM_TYPE           L"type"          // image类型
#define XML_IMAGE_ITEM_TYPE_IMAGE     L"image"         // 默认值
#define XML_IMAGE_ITEM_TYPE_ICON      L"icon"          // 图标 
#define XML_IMAGE_ITEM_TYPE_IMAGELIST L"imagelist"     // 图片列表
#define XML_IMAGE_ITEM_TYPE_GIF       L"gif"           // gif图片
#define XML_IMAGE_ITEM_TYPE_PNGLISTGIF L"pnglistgif"   // png列表形式的gif图片
enum IMAGE_ITEM_TYPE
{
    IMAGE_ITEM_TYPE_IMAGE,
    IMAGE_ITEM_TYPE_ICON,
    IMAGE_ITEM_TYPE_IMAGE_LIST,
    IMAGE_ITEM_TYPE_GIF,
    IMAGE_ITEM_TYPE_PNGLISTGIF,
    IMAGE_ITEM_TYPE_CURSOR,
};

#define XML_IMAGE_ICON_WIDTH          L"width"         // 要加载的图标宽度
#define XML_IMAGE_ICON_HEIGHT         L"height"        // 要加载的图标高度
#define XML_IMAGE_IMAGELIST_COUNT     L"count"         // 图片列表项的数目 
#define XML_IMAGE_IMAGELIST_LAYOUT    L"layout"        // 图片列表项的排布方式
#define XML_IMAGE_IMAGELIST_LAYOUT_H  L"h"  
#define XML_IMAGE_IMAGELIST_LAYOUT_V  L"v"

#define XML_COLOR_USESKINHLS          L"useskinhls"    // 改变皮肤色调时，该颜色是否参与 0/1，默认为1

// 工程配置
#define XML_PROJECT_NAME       L"projectname"     // 工程名称
#define XML_PROJECT_SELECT     L"select"          // 当前工程使用了哪款皮肤名称
#define XML_PROJECT_SKIN       L"skin"            // 工程中一款皮肤
#define XML_PROJECT_SKIN_NAME  L"name"            // 皮肤名称
#define XML_PROJECT_SKIN_PATH  L"path"            // 皮肤ui.xml所在路径
#define XML_PROJECT_SKIN_HLS   L"hls"             // 该皮肤的色调信息,如"10,0,0"

#define XML_SKINBUILDER_CTRLSDLL L"ctrlsdll"      // 外部控件所在的DLL，用于注册创建信息

//
// layout.xml
//

// Object对象属性

//#define XML_ID                   L"id"            // 每个对象的标识符
#define XML_STYLECLASS           L"style"         // 对象的样式类型
#define XML_WIDTH                L"width"         // 对象的宽度，可以为 auto | 数值
#define XML_HEIGHT               L"height"        // 对象的高度，可以为 auto | 数值
#define XML_MARGIN               L"margin"        // 对象的外边距
#define XML_PADDING              L"padding"       // 对象的内边距
#define XML_BORDER               L"border"        // 对象的边框
#define XML_MINWIDTH             L"minwidth"      // 对象的最小宽度
#define XML_MINHEIGHT            L"minheigth"     // 对象的最小高度
#define XML_MAXWIDTH             L"maxwidth"      // 对象的最大宽度
#define XML_MAXHEIGHT            L"maxheight"     // 对象的最大高度
#define XML_AUTO                 L"auto"          // 对象的宽和高设置为由内部内容决定
#define XML_NDEF                 L""              // 未定义
#define XML_ASTERISK             L"*"             // 对象的宽和高设置为平分
#define XML_AVERAGE              L"avg"           // 对象的宽和高设置为平均
#define XML_PERCENT              L"percent"       // 对象的宽和高设置为百分比 pct ?
#define XML_PERCENT_CHAR         L'%' 
#define XML_AVERAGE_CHAR         L'*'             // 暂不支持 x*,

// EraseBkgnd
#define XML_BACKGND_IS_TRANSPARENT        L"transparent"             // 是否需要进行透明处理（刷上父对象的背景）0/1
#define XML_NO_CLIP_DRAW                  L"noclip"                  // 在绘制自己时，不需要维护剪裁区域，默认剪裁

enum VISIBILITY_TYPE
{	
	VISIBILITY_COLLAPSED = 0,   // visible=false, need no layout
	VISIBILITY_VISIBLE = 1,     // visible=true
	VISIBILITY_HIDDEN = 2       // visible=false, but layout
};
#define  XML_VISIBILITY_COLLAPSED         L"collapsed"
#define  XML_VISIBILITY_VISIBLE           L"visible"
#define  XML_VISIBILITY_HIDDEN            L"hidden"

#define XML_RENDER                        L"render"
#define XML_RENDER_TYPE                   L"render.type"             // 设置渲染类型
#define XML_RENDER_TYPE_GRADIENT          L"gradient"                // 横向渐变 Color + Color
#define XML_RENDER_TYPE_IMAGE             L"image"                   // 贴图Image + Color
#define XML_RENDER_TYPE_IMAGELISTITEM     L"imagelistitem"           // 图片列表中的一项，需要指定其索引
#define XML_RENDER_TYPE_IMAGELIST         L"imagelist"               // 图片列表
#define XML_RENDER_TYPE_COLOR             L"color"                   // 纯色填充 Color
#define XML_RENDER_TYPE_SYSCOLOR          L"syscolor"                // 系统色填充 Color
#define XML_RENDER_TYPE_COLORLIST         L"colorlist"               // 类似office 2003工具栏式按钮
#define XML_RENDER_TYPE_NOTHEME           L"notheme"                 // win2000主题控件
#define XML_RENDER_TYPE_THEME             L"theme"                   // 系统主题控件
#define XML_RENDER_TYPE_NULL              L"null"                    // 不绘制该render，但也会实例化该render指针，避免控件默认使用theme主题


#define XML_RENDER_THEME_DISABLE          L"render.theme.disable"    // 是否强制使用无主题的渲染
#define XML_RENDER_COLOR                  L"render.color"            // 填充颜色
#define XML_RENDER_BORDER                 L"render.border"           // 边框大小 RECT
#define XML_RENDER_BORDERCOLOR            L"render.border.color"     // 边框颜色
#define XML_RENDER_COLORFROM              L"render.color.from"       // 渐变起始色
#define XML_RENDER_COLORTO                L"render.color.to"         // 渐变终点色
#define XML_RENDER_COLOR_GRADIENT_DIR     L"render.color.gradient.dir" // 渐变色方向

#define XML_RENDER_COLOR_GRADIENT_DIR_H   L"h"
#define XML_RENDER_COLOR_GRADIENT_DIR_V   L"v"
enum
{
    RENDER_COLOR_GRADIENT_DIR_H,
    RENDER_COLOR_GRADIENT_DIR_V,
};

#define XML_RENDER_IMAGE                  L"render.image"            // 背景图片
#define XML_RENDER_IMAGE9REGION           L"render.image.9region"    // 背景伸缩方式
#define XML_RENDER_LIST_STATEMAPINDEX     L"render.statemap"    // 设置控件绘制状态对应的图片项，例如0:0,1:0,2:0,3:0,4:1 表示有两个图片项，但selection用第二张图片，其它用第一张图片
#define XML_RENDER_COLORLIST_COUNT        L"render.colorlist.count"  // 颜色列表的数量
// #define XML_RENDER_IMAGELIST_COUNT        L"render.imagelist.count"  // 图片列表中的图片数量
// #define XML_RENDER_IMAGELIST_LAYOUT       L"render.imagelist.layout" // 图片排列方向(默认为横向)
// #define XML_RENDER_IMAGELIST_LAYOUT_H     L"h"                       // 图片横向排列
// #define XML_RENDER_IMAGELIST_LAYOUT_V     L"v"                       // 图片纵向排列
#define XML_RENDER_IMAGELIST_ALPHA_ANIMATE L"render.imagelist.alphaanimate" // 是否启用alpha渐变动画

#define XML_RENDER_IMAGELISTITEM_INDEX    L"render.imagelistitem.index"  // 图片列表项的索引值

#define XML_RENDER_IMAGE_DRAWTYPE         L"render.image.drawtype"   // 图片绘制方式
#define XML_RENDER_IMAGE_DRAWTYPE_BITBLT  L"bitblt"                  // (0,0)位置开始贴图Image + Color
#define XML_RENDER_IMAGE_DRAWTYPE_TILE    L"tile"                    // (0,0)位置开始平铺Image
#define XML_RENDER_IMAGE_DRAWTYPE_STRETCH L"stretch"                 // 拉伸模式Image + Image9Region*
#define XML_RENDER_IMAGE_DRAWTYPE_STRETCH_DISABLE_AA L"stretch.disableaa"       // 拉伸模式Image + 不要抗锯齿
#define XML_RENDER_IMAGE_DRAWTYPE_ADAPT   L"adapt"                   // 自适应居中，不会超出绘制范围
#define XML_RENDER_IMAGE_DRAWTYPE_CENTER  L"center"                  // 居中，可能超出绘制范围
#define XML_RENDER_IMAGE_DRAWTYPE_STRETCH_BORDER      L"stretchborder"     // 通过9宫拉伸绘制边框，不绘制中间部分
#define XML_RENDER_IMAGE_DRAWTYPE_BITBLT_RIGHTTOP     L"bitbltrighttop"    // 左上角对齐
#define XML_RENDER_IMAGE_DRAWTYPE_BITBLT_LEFTBOTTOM   L"bitbltleftbottom"  // 左下角对齐
#define XML_RENDER_IMAGE_DRAWTYPE_BITBLT_RIGHTBOTTOM  L"bitbltrightbottom" // 右下角对齐
#define XML_RENDER_IMAGE_DRAWTYPE_BITBLT_LEFTVCENTER  L"bitbltleftvcenter" // 左中对齐

#define XML_RENDER_IMAGE_ALPHA            L"render.image.alpha"      // 图片绘制时使用的alpha值
#define XML_RENDER_IMAGE_SRC_REGION       L"render.image.srcregion"  // 图片绘制的源区域

#define XML_BACKGND_RENDER_PREFIX         L"back."    // 背景渲染属性前缀，如back.render.type="" fore.render.image=""
#define XML_FOREGND_RENDER_PREFIX         L"fore."    // 前景
#define XML_ICON_RENDER_PREFIX            L"icon."    // 图标属性渲染前缀
#define XML_BKGCOLOR                      L"bkgcolor"  // 背景颜色
#define XML_BKGCOLOR_PREFIX               L"bkgcolor." // 背景颜色前缀

enum IMAGELIST_LAYOUT_TYPE
{
    IMAGELIST_LAYOUT_TYPE_H,
    IMAGELIST_LAYOUT_TYPE_V
};

#define XML_TEXTRENDER                      L"text.render"            // 文字渲染
#define XML_TEXTRENDER_TYPE                 L"text.render.type"       // 设置文字渲染类型
#define XML_TEXTRENDER_TYPE_SIMPLE          L"simple"                // color + font
#define XML_TEXTRENDER_TYPE_COLORLIST       L"colorlist"             // color list + font
#define XML_TEXTRENDER_TYPE_FONTLIST        L"fontlist"              // font list + color
#define XML_TEXTRENDER_TYPE_FONTCOLORLIST   L"fontcolorlist"         // font list + color list
#define XML_TEXTRENDER_TYPE_CONTRAST_COLOR    L"contrastcolor"       // font + 跟随背景色自适应的黑白color
#define XML_TEXTRENDER_TYPE_CONTRASTCOLORLIST L"contrastcolorlist"   // 

#define XML_TEXTRENDER_COLOR                L"text.render.color"      // 字体颜色
#define XML_TEXTRENDER_FONT                 L"text.render.font"       // 字体
#define XML_TEXTRENDER_COLORLIST_COUNT      L"text.render.colorlist.count"
#define XML_TEXTRENDER_FONTCOLORLIST_COUNT  L"text.render.fontcolorlist.count"  // 字体、颜色数量
#define XML_TEXTRENDER_ALIGN                L"text.render.align"      // 对齐
#define XML_TEXTRENDER_EFFECT               L"text.render.effect"     // 特效

#define XML_TEXTRENDER_ALIGN_TOP            L"top"
#define XML_TEXTRENDER_ALIGN_BOTTOM         L"bottom"
#define XML_TEXTRENDER_ALIGN_LEFT           L"left"
#define XML_TEXTRENDER_ALIGN_RIGHT          L"right"
#define XML_TEXTRENDER_ALIGN_CENTER         L"center"
#define XML_TEXTRENDER_ALIGN_VCENTER        L"vcenter"
#define XML_TEXTRENDER_ALIGN_END_ELLIPSIS   L"endellipsis"
#define XML_TEXTRENDER_ALIGN_SINGLELINE     L"singleline"
#define XML_TEXTRENDER_ALIGN_MULTILINE      L"multiline"
#define XML_TEXTRENDER_ALIGN_NO_PREFIX      L"noprefix"

#define XML_TEXTRENDER_EFFECT_NONE          L"none"
#define XML_TEXTRENDER_EFFECT_HALO          L"halo"
#define XML_TEXTRENDER_EFFECT_HALO_RADIUS   L"text.render.halo.radius"
#define XML_TEXTRENDER_EFFECT_HALO_SHADOW_COLOR L"text.render.halo.shadow.color"
#define XML_TEXTRENDER_EFFECT_ENDALPHAMASK  L"endalphamask"

// windows attribute
// #define  XML_WINDOW_MAX_WIDTH                           L"maxwidth"
// #define  XML_WINDOW_MAX_HEIGHT                          L"maxheight"
// #define  XML_WINDOW_MIN_WIDHT                           L"minwidth"
// #define  XML_WINDOW_MIN_HEIGHT                          L"minheight"

#define  XML_WINDOW_MAXIMIZE_BORDER                     L"maximizeborder"   // int类型，最大化时位于屏幕外的区域大小
#define  XML_WINDOW_RESIZE_BORDER                       L"resizeborder"     // int类型，拖拽区域大小
#define  XML_WINDOW_RESIZE_CAPABILITY                   L"resizecapability" // 拖拽能力
#define  XML_WINDOW_RESIZE_CAPABILITY_NONE              L"none"             // 禁止拖拽
#define  XML_WINDOW_RESIZE_CAPABILITY_CAPTION           L"caption"          // 只能拖拽，不能修改大小
#define  XML_WINDOW_RESIZE_CAPABILITY_ALL               L"all"              // 能拖拽，能修改大小
#define  XML_WINDOW_RESIZE_CAPABILITY_TRANSPARENT       L"transparent"    // 穿透到父窗口拖拽

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
#define  XML_WINDOW_TRANSPARENT_TYPE                    L"transparent.type"
#define  XML_WINDOW_TRANSPARENT_9REGION                 L"transparent.9region" // 窗口透明区域参数（9Region类型，可为4，8，9三种）

#define  XML_WINDOW_TRANSPARENT_TYPE_AERO               L"aero"             // 使用分层窗口进行透明
#define  XML_WINDOW_TRANSPARENT_TYPE_LAYERED            L"layered"          // 使用分层窗口进行透明
#define  XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR          L"maskcolor"        // 使用颜色值
#define  XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA          L"maskalpha"        // 使用透明值
#define  XML_WINDOW_TRANSPARENT_TYPE_ANTIALIASING       L"anti"             // 抗锯齿圆角
#define  XML_WINDOW_TRANSPARENT_TYPE_GLOW               L"glow"             // 类VS2013边框发光效果

#define  XML_WINDOW_TRANSPARENT_MASKCOLOR_VALUE         L"transparent.maskcolor"  // 窗口透明色
#define  XML_WINDOW_TRANSPARENT_MASKALPHA_VALUE         L"transparent.maskalpha"  // 将窗口中小于alpha值通过的抠掉

#define  XML_WINDOW_TRANSPARENT_ANTI_9REGION            L"transparent.anti.9region"       // 左上角圆角度数，上侧缩进，右上角圆角度数，左侧缩进，右侧缩进，左下角圆角度数，下侧缩进，右下角圆角度数
#define  XML_WINDOW_TRANSPARENT_ANTI_BORDERINDENT       L"transparent.anti.borderIndent"  // 四周缩进值。默认为全0。用于解决aero blur切换到anti时，四周有一部分阴影的问题
#define  XML_WINDOW_TRANSPARENT_MASKALPHA_9REGION       L"transparent.maskalpha.9region"  // 全0表示什么也不做。全-1表示整个窗口过滤。top,left,right.bottom四个值如果大于0表示要过滤这四个边，小于0表示不过滤，仅用于设置四个角的位置参数
#define  XML_WINDOW_TRANSPARENT_MASKCOLOR_9REGION       L"transparent.maskcolor.9region"  // 同XML_WINDOW_TRANSPARENT_MASKALPHA_9REGION

#define  XML_WINDOW_TRANSPARENT_AERO_MODE               L"transparent.aero.mode"   // aero透明模式
#define  XML_WINDOW_TRANSPARENT_AERO_MODE_TRANS         L"trans"
#define  XML_WINDOW_TRANSPARENT_AERO_MODE_BLUR          L"blur"
enum AERO_MODE 
{
    AERO_MODE_TRANSPARENT,   // 只透明
    AERO_MODE_BLUR,          // 带模糊
};
#define  XML_WINDOW_TRANSPARENT_AERO_TRANS_MARGINS      L"transparent.aero.trans.margins"  // rect 透明区域设置（DwmExtendFrameIntoClientArea参数）
#define  XML_WINDOW_TRANSPARENT_AERO_BLUR_REGION        L"transparent.aero.blur.region"    // rect 磨沙效果区域（DwmEnableBlurBehindWindow参数）
#define  XML_WINDOW_TRANSPARENT_AERO_BLUR_AUTOCLIPCHILDCTRL  L"transparent.aero.blur.autoclipchildctrl"  // 是否自动剪裁子控件
#define  XML_WINDOW_TRANSPARENT_TYPE_AERO_DISABLE       L"transparent.aero.disable"        // aero模式不可用时的窗口透明类型


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

#define XML_WINDOW_GRAPHICS_RENDER_LIBRARY          L"graphics"         // 图形引擎类型
#define XML_WINDOW_GRAPHICS_RENDER_LIBRARY_GDI      L"gdi"
#define XML_WINDOW_GRAPHICS_RENDER_LIBRARY_GDIPLUS  L"gdiplus"
#define XML_WINDOW_GRAPHICS_RENDER_LIBRARY_D2D      L"d2d"
#define XML_WINDOW_GRAPHICS_RENDER_LIBRARY_DIRECT2D L"direct2d"
#define XML_WINDOW_GRAPHICS_RENDER_LIBRARY_D3D      L"d3d"
#define XML_WINDOW_GRAPHICS_RENDER_LIBRARY_DIRECT3D L"direct3d"

#define XML_WINDOW_NEED_ALPHACHANNEL                L"needalphachannel"  // 窗口是否需要渲染alpha通道。默认为true


enum GRAPHICS_RENDER_LIBRARY_TYPE
{
	//	GRAPHICS_RENDER_LIBRARY_TYPE_AUTO = 0,   // <- 依据窗口类型决定。分层窗口使用GDIPLUS,普通窗口使用GDI，废弃，逻辑太复杂
	GRAPHICS_RENDER_LIBRARY_TYPE_GDI = 1,
	GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS,
	GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D,
	GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT3D,
  GRAPHICS_RENDER_LIBRARY_TYPE_SKIA,
};

#define XML_TEXTURE_RENDER_PREFIX            L"texture."         // customwindow用于支持换肤的皮肤render前缀
#define XML_MASK_RENDER_PREFIX               L"mask."            // 在绘制完子结点后，再画的一层(PostPaint)
#define XML_PANEL_RENDER_REGION              L"render.region"    // 皮肤图片在窗口上显示的区域，类型于padding的配置

#define XML_ROUNDPANEL_CORNER_LEFTTOP        L"corner.lefttop"
#define XML_ROUNDPANEL_CORNER_RIGHTTOP       L"corner.righttop"
#define XML_ROUNDPANEL_CORNER_LEFTBOTTOM     L"corner.leftbottom"
#define XML_ROUNDPANEL_CORNER_RIGHTBOTTOM    L"corner.rightbottom"

enum ROUND_CORNER_TYPE
{
    ROUND_CORNER_NONE,
    ROUND_CORNER_ANTI, // 抗锯齿模式
    ROUND_CORNER_DIRECT_THROW,  // 直接删除圆角处像素值
};

// 其它通用属性
#define XML_CURSOR                           L"cursor"       // 对象的鼠标样式
#define XML_CURSOR_IDC_ARROW                 L"idc_arrow"    // 各种系统鼠标样式配置
#define XML_CURSOR_IDC_IBEAM                 L"idc_ibeam"
#define XML_CURSOR_IDC_WAIT                  L"idc_wait"
#define XML_CURSOR_IDC_CROSS                 L"idc_cross"
#define XML_CURSOR_IDC_UPARROW               L"idc_uparrow"
#define XML_CURSOR_IDC_SIZE                  L"idc_size"
#define XML_CURSOR_IDC_ICON                  L"idc_icon"
#define XML_CURSOR_IDC_SIZENWSE              L"idc_sizenwse"
#define XML_CURSOR_IDC_SIZENESW              L"idc_sizenesw"
#define XML_CURSOR_IDC_SIZEWE                L"idc_sizewe"
#define XML_CURSOR_IDC_SIZENS                L"idc_sizens"
#define XML_CURSOR_IDC_SIZEALL               L"idc_sizeall"
#define XML_CURSOR_IDC_NO                    L"idc_no"
#define XML_CURSOR_IDC_HAND                  L"idc_hand"
#define XML_CURSOR_IDC_APPSTARTING           L"idc_appstarting"
#define XML_CURSOR_IDC_HELP                  L"idc_help"

#define XML_CURSOR_IDC_DRAG_SEPERATOR        L"idc_drag_seperator"
#define XML_CURSOR_IDC_SPLITBAR_H            L"idc_splitbar_h"
#define XML_CURSOR_IDC_SPLITBAR_V            L"idc_splitbar_v"


#define XML_TOOLTIP                          L"tooltip"      // 对象的提示信息，自定义的提示内容需要代码自行添加
#define XML_CONTROL_STYLE_GROUP              L"group"
#define XML_CONTROL_STYLE_TABABLE            L"tabstop"
#define XML_REJEST_SELF_MOUSE_MSG            L"rejectselfmousemsg"  // 不接受鼠标消息
#define XML_REJEST_MOUSE_MSG                 L"rejectmousemsg"  // 不接受鼠标消息
#define XML_ZINDEX_OVERLAP                   L"zindexoverlap"   // z序重叠了标识
#define XML_ZORDER                           L"zorder"          // 在本层中的z序，当不为0时，将为该对象创建一个layer
#define XML_ISNCCHILD                        L"ncchild"         // 是一个ncchild，例如滚动条

#define XML_LAYER                            L"layer"           // 为该对象建立一个renderlayer（带缓存）

// window
// #define  XML_WINDOW_BK_TYPE                    L"bktype"     // 窗口背景绘制类型
// #define  XML_WINDOW_BK_TYPE_COLOR              L"color"      // 窗口背景绘制类型--颜色，画刷画笔
// #define  XML_WINDOW_BK_TYPE_TILE               L"tile"       // 窗口背景绘制类型--平铺
// #define  XML_WINDOW_BK_TYPE_SIMPLE             L"simple"     // 窗口背景绘制类型--简单，(0,0)bitblt
// #define  XML_WINDOW_BK_TYPE_CENTER             L"center"     // 窗口背景绘制类型--居中
// #define  XML_WINDOW_BK_TYPE_STRETCH            L"stretch"    // 窗口背景绘制类型--拉伸
// #define  XML_WINDOW_BK_TYPE_FRAME              L"frame"      // 窗口背景绘制类型--边框
// #define  XML_WINDOW_BK_IMAGE                   L"bkimage"    // 窗口背景图片名称
// #define  XML_WINDOW_BK_TYPE_COLOR_PEN          L"bkpencol"   // bktype=color，窗口边框色
// #define  XML_WINDOW_BK_TYPE_COLOR_BRUSH        L"bkbrushcol" // bktype=color，窗口背景色
// #define  XML_WINDOW_BK_TYPE_FRAME_9REGION      L"bk9region"  // bktype=frame，窗口切割范围

// 系统按钮ID
#define XML_SYS_BTN_RESTORE       L"sys_restore"   // 系统还原按钮ID
#define XML_SYS_BTN_MAXIMIZE      L"sys_maximize"  // 系统最大化按钮ID
#define XML_SYS_BTN_MINIMIZE      L"sys_minimize"  // 系统最小化按钮ID
#define XML_SYS_BTN_CLOSE         L"sys_close"     // 系统关闭按钮ID

// hwnd host
#define XML_HWNDHOST_CONTROL_ID   L"controlid"     // 控件ID数值，整形

#pragma region // 布局layout的属性
#define XML_LAYOUT_PREFIX        L"layout."
#define XML_LAYOUT_TYPE          L"layout.type"    // 对象的布局类型
//#define XML_LAYOUT_STACK         L"stack"          // 布局类型：stack
#define XML_LAYOUT_VERT          L"vert"           // 布局类型：vert
#define XML_LAYOUT_HORZ          L"horz"           // 布局类型：horz
#define XML_LAYOUT_NULL          L"null"           // 布局类型：null
#define XML_LAYOUT_GRID          L"grid"           // 布局类型：grid
#define XML_LAYOUT_DOCK          L"dock"           // 布局类型：dock
#define XML_LAYOUT_CANVAS        L"canvas"         // 布局类型：canvas
#define XML_LAYOUT_CARD          L"card"           // 布局类型：card
#define XML_LAYOUT_AVERAGE       L"average"        // 布局类型：average

enum LAYOUTTYPE
{
    LAYOUT_TYPE_CANVAS = 0,
    //LAYOUT_TYPE_STACK,
	LAYOUT_TYPE_HORZ,
	LAYOUT_TYPE_VERT,
    LAYOUT_TYPE_GRID,
    LAYOUT_TYPE_DOCK,
    LAYOUT_TYPE_CARD,
    LAYOUT_TYPE_AVERAGE,
};

// stack布局的属性
// #define XML_LAYOUT_STACK_DIRECTION      L"layout.direction"  // stack布局的方向
// #define XML_LAYOUT_STACK_LEFTTORIGHT    L"lefttoright"       // xml中可配置的值
// #define XML_LAYOUT_STACK_RIGHTTOLEFT    L"righttoleft"       // xml中可配置的值
// #define XML_LAYOUT_STACK_LEFTANDRIGHT   L"leftandright"      // xml中可配置的值
// #define XML_LAYOUT_STACK_TOPTOBOTTOM    L"toptobottom"       // xml中可配置的值
// #define XML_LAYOUT_STACK_BOTTOMTOTOP    L"bottomtotop"       // xml中可配置的值
// #define XML_LAYOUT_STACK_TOPANDBOTTOM   L"topandbottom"      // xml中可配置的值

// enum LAYOUT_STACK_DIRECTION
// {
//     LAYOUT_STACK_LEFTTORIGHT  = 0,   
//     LAYOUT_STACK_RIGHTTOLEFT  = 1,   
//     LAYOUT_STACK_LEFTANDRIGHT = 2,  
//     LAYOUT_STACK_TOPTOBOTTOM  = 3,   
//     LAYOUT_STACK_BOTTOMTOTOP  = 4,   
//     LAYOUT_STACK_TOPANDBOTTOM = 5   
// };
#define XML_LAYOUT_STACK_GAP            L"layout.gap"        // 每一项之间的间隙

// average布局
#define XML_LAYOUT_AVERAGE_DIRECTION    L"layout.direction"  // stack布局的方向
#define XML_LAYOUT_AVERAGE_H            L"h"
#define XML_LAYOUT_AVERAGE_V            L"v"

enum LAYOUT_AVERAGE_DIRECTION
{
    LAYOUT_AVERAGE_H,
    LAYOUT_AVERAGE_V
};

#define XML_LAYOUT_AVERAGE_GAP         L"layout.gap"        // 每一项之间的间隙

// dock布局的属性

// grid布局的属性
#define XML_LAYOUT_GRID_WIDTH           L"layout.width"      // grid布局的各列的宽度定义
#define XML_LAYOUT_GRID_HEIGHT          L"layout.height"     // grid布局的各列的高度定义

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
#define XML_LAYOUT_ITEM_LEFT        L"layout.left"
#define XML_LAYOUT_ITEM_TOP         L"layout.top"
#define XML_LAYOUT_ITEM_RIGHT       L"layout.right"
#define XML_LAYOUT_ITEM_BOTTOM      L"layout.bottom"

// stack布局中的子元素的属性
#define XML_LAYOUT_ITEM_ALIGN          L"layout.align"       // 当layout.direction=leftandright||topandbottom时，需要指定子元素到底停靠在哪一侧
#define XML_LAYOUT_ITEM_ALIGN_LEFT     L"left"
#define XML_LAYOUT_ITEM_ALIGN_RIGHT    L"right"
#define XML_LAYOUT_ITEM_ALIGN_TOP      L"top"
#define XML_LAYOUT_ITEM_ALIGN_BOTTOM   L"bottom"

#define XML_LAYOUT_ITEM_ALIGN_CENTER   L"center"  // -- stack/dock layout
#define XML_LAYOUT_ITEM_ALIGN_VCENTER  L"vcenter" // -- stack layout
#define XML_LAYOUT_ITEM_ALIGN_FILL     L"fill"    // -- stack layout

// 子控件四边全对齐布局，父对象在Measure自己时，如何处理该子控件的desiredsize
// 1. 正常情况下，由于子控件采用的是lrtb对齐，将不测试自己的大小，而是直接依赖于父对象
// 2. 在菜单窗口场景下，窗口的大小则是取决于菜单内容大小，但菜单又是lrtb对齐布局的，
//    因此窗口在遍历时，菜单的desiredsize不应该依赖父对象，而是返回真实大小
// 废弃，再调一次get desired size得了
//#define XML_LAYOUT_ALIGN_FORCE_DESIREDSIZE  L"xxxx"


// enum LAYOUT_STACK_ITEM_ALIGN
// {
// 	LAYOUT_STACK_ITEM_ALIGN_LEFT = 0,
// 	LAYOUT_STACK_ITEM_ALIGN_TOP = 0,
// 	LAYOUT_STACK_ITEM_ALIGN_RIGHT = 1,
// 	LAYOUT_STACK_ITEM_ALIGN_BOTTOM = 1,
// };

// dock布局中的子元素的属性
// #define XML_LAYOUT_DOCK_DOCK          L"layout.dock"       // 子元素停靠在哪一个方向
// #define XML_LAYOUT_DOCK_DOCK_LEFT     L"left"
// #define XML_LAYOUT_DOCK_DOCK_RIGHT    L"right"
// #define XML_LAYOUT_DOCK_DOCK_TOP      L"top"
// #define XML_LAYOUT_DOCK_DOCK_BOTTOM   L"bottom"
// #define XML_LAYOUT_DOCK_DOCK_CENTER   L"center"

// grid布局中的子元素的属性
#define XML_LAYOUT_GRID_ROW          L"layout.row"        // 对应 m_nConfigTop
#define XML_LAYOUT_GRID_COL          L"layout.col"        // 对应 m_nConfigLeft
#define XML_LAYOUT_GRID_ROWSPAN      L"layout.rowspan"    // 对应 m_nConfigBottom
#define XML_LAYOUT_GRID_COLSPAN      L"layout.colspan"    // 对应 m_nConfigRight

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
#define  XML_STYLE_SELECTOR_TYPE_ID            L"id"       // id 选择器
#define  XML_STYLE_SELECTOR_TYPE_CLASS         L"class"    // class选择器
#define  XML_STYLE_INHERIT                     L"inherit"  // 继承属性

#define  STYLE_ID_PREFIX                       L'#'        // id选择器前缀
#define  STYLE_CLASS_PREFIX                    L'.'        // class选择器前缀

// Control

// Label

// Picture

// GifPicture
#define  XML_GIFCTRL_GIF   L"gif"       // gif资源id
#define  XML_GIFCTRL_AUTOPLAY    L"autoplay"   // 是否自动开始，默认为false，需要手动调用start函数

// GroupBox
#define XML_GROUPBOX_TEXTINDENT   L"textindent"  // 文字缩进值
#define XML_GROUPBOX_TEXTMARGIN   L"textmargin"  // 文字与边框的间距
#define XML_GROUPBOX_TEXTALIGN    L"textalign"   // 文字对齐方式

//////////////////////////////////////////////////////////////////////////
// Button

#define  XML_BUTTON_DEFPUSHBUTTON       L"defpushbutton"    // 是否是默认按钮
#define  XML_BUTTON_DRAW_FOCUS_TYPE     L"drawfocus"        // 如何绘制焦点
#define  XML_BUTTON_DRAW_FOCUS_TYPE_NONE   L"none"        
#define  XML_BUTTON_DRAW_FOCUS_TYPE_DOT    L"dot"    
#define  XML_BUTTON_HAND_CUROSR         L"handcursor"       // 显示手型鼠标样式

typedef enum
{
    BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE,                               // 不绘制
    BUTTON_RENDER_DRAW_FOCUS_TYPE_DOT,                                // 使用虚线点绘制
}BUTTON_RENDER_DRAW_FOCUS_TYPE;

#define XML_BUTTON_AUTOSIZE_TYPE           L"autosize"     // 当没有指定按钮大小时，如何计算其大小
#define XML_BUTTON_AUTOSIZE_TYPE_BKIMAGE   L"backimage"      // 背景图片的大小决定按钮大小
#define XML_BUTTON_AUTOSIZE_TYPE_CONTENT   L"content"      // 背景图片的内容(图标+文本)决定按钮大小
#define XML_BUTTON_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH L"bkimagehcontentw"  // 图片的高度+内容的宽度

enum BUTTON_AUTO_SIZE_TYPE
{
    BUTTON_AUTOSIZE_TYPE_NDEF,
    BUTTON_AUTOSIZE_TYPE_BKIMAGE,
    BUTTON_AUTOSIZE_TYPE_CONTENT,
    BUTTON_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH
};

#define XML_BUTTON_ICON_ALIGN  L"iconalign"    // 按钮图标的位置

// #define  XML_BUTTON_ICON                             L"icon"             // 按钮图标
// #define  XML_BUTTON_ICON_RENDER_TYPE                 L"icontype"         // 按钮图标绘制类型
// #define  XML_BUTTON_ICON_RENDER_TYPE_STATIC          L"static"           // 静态图标
// #define  XML_BUTTON_ICON_RENDER_TYPE_8STATE          L"8state"           // 8态图标
// #define  XML_BUTTON_ICON_RENDER_TYPE_THEME_RADIO     L"themeradio"       // 主题单选图标
// #define  XML_BUTTON_ICON_RENDER_TYPE_THEME_CHECK     L"themecheck"       // 主题复选图标
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

#define   XML_BUTTON_ICON_TEXT_SPACE           L"icontextspace"   // 文本与图标之间的间距
// #define  XML_BUTTON_TEXT_RENDER_TYPE           L"texttype"         // 文本绘制类型
// #define  XML_BUTTON_TEXT_RENDER_TYPE_SINGLELINE L"singleline"      // 单行文本
// #define  XML_BUTTON_TEXT_RENDER_TYPE_HYPERLINK  L"hyperlink"       // 超链接模式
// typedef enum
// {
// 	BUTTON_TEXT_RENDER_TYPE_NONE,         // 不绘制文本
// 	BUTTON_TEXT_RENDER_TYPE_SINGLELINE,   // 单行文本
// 	BUTTON_TEXT_RENDER_TYPE_HYPERLINK,    // 超链接
// 
// }BUTTON_TEXT_RENDER_TYPE;

#define XML_BUTTON_CHECKED_TOOLTIP      L"checked.tooltip"  // 被选中时的提示信息

// HyperLink
#define  XML_HYPERLINK_HREF              L"href"     // 超链接


// #define  XML_BRUSHBUTTON_BKHOVERCOL            L"bkhovercol"
// #define  XML_BRUSHBUTTON_BKPRESSCOL            L"bkpresscol"
// #define  XML_BRUSHBUTTON_BORDERHOVERCOL        L"borderhovercol"
// #define  XML_BRUSHBUTTON_BORDERPRESSCOL        L"borderpresscol"
// #define  XML_BRUSHBUTTON_TEXTNORMALCOL         L"textnormalcol"
// #define  XML_BRUSHBUTTON_TEXTHOVERCOL          L"texthovercol"
// #define  XML_BRUSHBUTTON_TEXTPRESSCOL          L"textpresscol"
// #define  XML_BRUSHBUTTON_TEXTDISABLECOL        L"textdisablecol"
// #define  XML_BRUSHBUTTON_TEXT                  L"text"
// #define  XML_BRUSHBUTTON_ICON                  L"icon"

//#define  XML_BUTTON_BK_TYPE                    L"bktype"     // 按钮背景绘制类型
//#define  XML_BUTTON_BK_TYPE_COLOR              L"color"    

//////////////////////////////////////////////////////////////////////////
// ProgressCtrl / SliderCtrl

#define  XML_PROGRESSBAR_DIRECTION            L"direction"       // 滚动条的方向
#define  XML_PROGRESSBAR_DIRECTION_LEFTRIGHT  L"lefttoright"       // 左->右
#define  XML_PROGRESSBAR_DIRECTION_BOTTOMTOP  L"bottomtotop"       // 下->上
#define  XML_PROGRESSBAR_DIRECTION_RIGHTLEFT  L"righttoleft"       // 右->左
#define  XML_PROGRESSBAR_DIRECTION_TOPBOTTOM  L"toptobottom"       // 上->下

enum PROGRESSBAR_SCROLL_DIRECTION_TYPE
{
    PROGRESSBAR_SCROLL_LEFT_2_RIGHT,
    PROGRESSBAR_SCROLL_BOTTOM_2_TOP,
    PROGRESSBAR_SCROLL_RIGHT_2_LEFT,
    PROGRESSBAR_SCROLL_TOP_2_BOTTOM
};

#define  XML_SLIDERCTRL_THUMB_POINT_DIRECTION          L"thumbarrow"  // 滑块按钮的指向
#define  XML_SLIDERCTRL_THUMB_POINT_DIRECTION_BOTH     L"both"            // 默认值
#define  XML_SLIDERCTRL_THUMB_POINT_DIRECTION_LEFT     L"left"    
#define  XML_SLIDERCTRL_THUMB_POINT_DIRECTION_TOP      L"top"     
#define  XML_SLIDERCTRL_THUMB_POINT_DIRECTION_RIGHT    L"right"   
#define  XML_SLIDERCTRL_THUMB_POINT_DIRECTION_BOTTOM   L"bottom"   


// #define  PROGRESSCTRL_FLAG_DIRECTION_LEFTRIGHT    0x0000
// #define  PROGRESSCTRL_FLAG_DIRECTION_TOPBOTTOM    0x0001                
// #define  PROGRESSCTRL_FLAG_DIRECTION_RIGHTLEFT    0x0002
// #define  PROGRESSCTRL_FLAG_DIRECTION_BOTTOMTOP    0x0003

#define  XML_SLIDERCTRL_BUTTON_ATTR_PRIFIX     L"trackbtn."       // 滑动控件的按钮属性前缀

#define  XML_IMAGEPROGRESSCTRL_FOREIMAGE       L"foreimage"       // 前景图片（一张）
#define  XML_IMAGEPROGRESSCTRL_BKGNDIMAGE      L"bkgndimage"      // 背景图片（一张）

// Edit
#define  XML_EDIT_TEXT                        L"text"             // 文字内容
#define  XML_EDIT_TEXT_ALIGN                  L"align"            // 文字对齐方式

#define  XML_EDIT_COLOR                       L"color.text"       // 正常文字显示颜色
#define  XML_EDIT_SELECT_COLOR                L"color.select"     // 文字被选中时的颜色
#define  XML_EDIT_SELECT_BK_COLOR             L"color.select.back"// 文字被选中时背景的颜色
#define  XML_EDIT_DISABLE_COLOR               L"color.disable"    // 被禁用时的文本颜色
#define  XML_EDIT_READONLY_COLOR              L"color.readonly"   // 只读时的文本颜色
#define  XML_INSTANTEDIT_COLOR                L"color.quiet"      // 即时编辑时，未开启编辑时的文本颜色 
#define  XML_EDIT_MAX_LENGTH                  L"limit"            // 内容的最长值
#define  XML_EDIT_MAX_LENGTH_BYTELIMIT        L"bytelimit"        // 按字节限制长度，汉字算两个
#define  XML_EDIT_CARET_HEIGHT                L"caretheight"      // 光标及选中文本背景的高度
//#define  XML_EDIT_MAX_INT

// Scroll Bar
#define  XML_SCROLLBAR_RANGE                  L"range"            // 滚动条范围设置
#define  XML_SCROLLBAR_PAGE                   L"page"             // 滚动条页大小设置
#define  XML_SCROLLBAR_POS                    L"pos"              // 滚动条位置设置
#define  XML_SCROLLBAR_BTNLINE                L"btnline"          // 滚动条点击按钮后滚动大小设置
#define  XML_SCROLLBAR_WHEELLINE              L"wheelline"        // 使用鼠标滑轮后滚动大小设置

#define  XML_HSCROLLBAR                           L"hscrollbar"  // 横向滚动条类型
#define  XML_VSCROLLBAR                           L"vscrollbar"  // 纵向滚动条类型
#define  XML_SCROLLBAR_NONE                       L"none"        // 该方向无滚动条
#define  XML_SCROLLBAR_AUTO                       L"auto"        // 该方向上的滚动条在需要时显示
#define  XML_SCROLLBAR_ALWAYSSHOW                 L"show"        // 该方向上的滚动条保持显示
#define  XML_SCROLLBAR_ALWAYSHIDE                 L"hide"        // 该方向上支持滚动，但不显示滚动条

enum SCROLLBAR_VISIBLE_TYPE
{
    SCROLLBAR_VISIBLE_NONE,          // 不使用
    SCROLLBAR_VISIBLE_AUTO,          // 当需要滚动的时候再显示
    SCROLLBAR_VISIBLE_SHOW_ALWAYS,   // 一直显示
    SCROLLBAR_VISIBLE_HIDE_ALWAYS,   // 一直隐藏，逻辑上的
};
enum SCROLLBAR_DIRECTION_TYPE
{
#if defined(OS_WIN)
    HSCROLLBAR = WM_HSCROLL,
    VSCROLLBAR = WM_VSCROLL
#else
    HSCROLLBAR = 1,
    VSCROLLBAR = 2
#endif
};

// #define  XML_SCROLLBAR_TYPE                       L"type"        // 前接XML_xxSCROLLBAR_PRIFIX，滚动条的类型
// #define  XML_SCROLLBAR_TYPE_SYSTEM                L"system"      // 系统样式的滚动条，两个按钮+一个拖动条
// #define  XML_SCROLLBAR_TYPE_LOGIC                 L"logic"       // 无滚动条显示，但维护绑定对象进行滚动的数据
// #define  XML_SCROLLBAR_TYPE_MENU                  L"menu"        // 
// 
// enum SCROLLBAR_RENDER_TYPE
// {
//     SCROLLBAR_RENDER_TYPE_SYSTEM,
//     SCROLLBAR_RENDER_TYPE_LOGIC,
//     SCROLLBAR_RENDER_TYPE_MENU,
// };

#define  XML_HSCROLLBAR_PRIFIX                    L"hscrollbar." // 横向滚动条的属性前缀
#define  XML_VSCROLLBAR_PRIFIX                    L"vscrollbar." // 纵向滚动条的属性前缀

#define  XML_SCROLLBAR_RENDER_TYPE                L"type"        // 设置滚动条的渲染类

// scrollpanel
#define  XML_SCROLLPANEL_VIEWSIZE                 L"viewsize"    // 视图大小，格式为 "100,100"

// listbox
#define  XML_LISTCTRL_ITEM_HEIGHT                 L"itemheight"  // 列表控件每一项的高度
#define  XML_LISTCTRL_ITEM_PADDING                L"itempadding" // 列表项的缩进

#define  XML_LISTCTRL_MIN_WIDTH                   L"minwidth"    //  auto size列表框的最小、大尺寸
#define  XML_LISTCTRL_MIN_HEIGHT                  L"minheight"
// #define  XML_LISTCTRL_MAX_WIDTH                   L"maxwidth"
// #define  XML_LISTCTRL_MAX_HEIGHT                  L"maxheight"

#define  XML_LISTCTRL_EDIT_PRIFIX                 L"edit."       // 编辑框的属性前缀
#define  XML_LISTCTRL_BUTTON_PRIFIX               L"button."     // 按钮的属性前缀
#define  XML_LISTCTRL_COMBOBOX_PRIFIX             L"combobox."   // 下拉列表的属性前缀

// combobox
#define  XML_COMBOBOX_EDIT_PRIFIX                 L"edit."       // 编辑框的属性前缀
#define  XML_COMBOBOX_BUTTON_PRIFIX               L"button."     // 按钮的属性前缀
#define  XML_COMBOBOX_LISTBOX_PRIFIX              L"listbox."    // 下拉列表的属性前缀
#define  XML_COMBOBOX_DROPWNDID                   L"dropwnd.id"  // 下拉窗口id

// ledctrl
#define  XML_LEDCTRL_INDEXMAP                     L"indexmap"    // 图片索引映射关系
#define  XML_LEDCTRL_ITEMWIDTH                    L"itemwidth"   // 配置每一项的显示宽度（默认为图片宽度）
#define  XML_LEDCTRL_ITEMHEIGHT                   L"itemheight"  // 配置每一项的显示高度（默认为图片高度）
#define  XML_LEDEXCTRL_SAMEWIDTH                  L"samewidth"   // bool，如果大小相同的话，就不用配置posmap属性了
#define  XML_LEDEXCTRL_POSMAP                     L"posmap"      // 图片项位置映射关系  0,10;10,16;16,6;22,6;28,6;34,6;40,6;46,6;52,6;58,6;64,6 ...

// menu
#define  XML_MENU                 L"Menu"      // layout.xml中菜单结点的tag name
#define  XML_MENU_STRINGITEM      L"String"    //
#define  XML_MENU_POPUPITEM       L"Popup"     //
#define  XML_MENU_SEPARATORITEM   L"Separator" // 
#define  XML_MENU_DISABLE         L"disable"   // menu item是否可用，为1或者true表示被禁用
#define  XML_MENU_POPUP_WNDID     L"wnd.id"    // popup子菜单皮肤资源窗口id
#define  XML_MENU_POPUP_MENUID    L"menu.id"   // popup子菜单在窗口中的控件id

#define  XML_MENU_ICONGUTTERWIDTH            L"icongutterwidth"  // 菜单图标列的宽度
#define  XML_MENU_POPUPTRIANGLEWIDTH         L"trianglewidth"    // 右侧三角形的占用宽度
#define  XML_MENU_SEPARATORHEIGHT            L"separatorheight"  // 分隔条的高度
#define  XML_MENU_LEFT_TEXT_INDENT           L"textleftindent"   // 左侧文本缩进
#define  XML_MENU_RIGHT_TEXT_INDENT          L"textrightindent"  // 左侧文本缩进
#define  XML_MENU_SUBMENU_GAP                L"submenugap"       // 子菜单与父菜单之间的间隙，默认为-2

#define  XML_MENU_RADIO_ICON_RENDER_PREFIX   L"radioicon."       // 选中图标绘制render前缀
#define  XML_MENU_CHECK_ICON_RENDER_PREFIX   L"checkicon."       // 选中图标绘制render前缀
#define  XML_MENU_RADIOCHECK_ICONBK_RENDER_PREFIX   L"radiocheckiconbk." // 选中图标背景绘制render前缀
#define  XML_MENU_SEPARATOR_RENDEER_PREFIX   L"separator."       // 分隔条绘制render前缀
#define  XML_MENU_TRAIANGLE_RENDER_PREFIX    L"triangle."        // 弹出箭头绘制render前缀

#define  XML_MENU_ITEM_ICON_RENDER_PREFIX    L"icon."            // 菜单项的图标

#define  MENU_SEPARATOR_ID    -1
#define  MENU_POPUP_ID        0

// header ctrl
#define  XML_HEADERCTRL_RIGHT_BLANK_RENDER_PRIFIX  L"rightblank." // 右侧空白区域的背景绘制前缘


// listview ctrl
#define  XML_LISTVIEWCTRL_HEADERCTRL_PREFIX   L"headerctrl."  // 列表控件中的header属性前缀

// Flash Wrap
#define  XML_FLASH_URI                        L"uri"          // flash文件地址
#define  XML_FLASH_WMODE                      L"wmode"        // flash窗口模式
#define  XML_FLASH_WMODE_WINDOW               L"window"
#define  XML_FLASH_WMODE_OPAQUE               L"opaque"
#define  XML_FLASH_WMODE_TRANSPARENT          L"transparent"
enum FLASH_WMODE
{
    FLASH_WMODE_WINDOW,
    FLASH_WMODE_OPAQUE,
    FLASH_WMODE_TRANSPARENT,
};

// Layer -- 过期
// #define  XML_LAYER_TYPE             L"type"    // render chain中的layer类型
// #define  XML_LAYER_TYPE_DIRECT      L"direct"  // 直接渲染，无需缓存。renderbase类型
// #define  XML_LAYER_TYPE_CONTROL     L"control" // 内嵌一个控件，例如flash

//#define  XML_LAYER_TYPE_FLASH       L"flash"   // flash层


// TabCtrl
#define   XML_TABCTRL_HEAD_HEIGHT               L"head.height"  // 头部的高度
#define   XML_TABCTRL_HEAD_LAYOUT_TYPE          L"head.layout"  // 头部布局方式
#define   XML_TABCTRL_HEAD_LAYOUT_TYPE_Win32    L"win32"
#define   XML_TABCTRL_HEAD_LAYOUT_TYPE_AVERAGE  L"average"
#define   XML_TABCTRL_HEAD_LAYOUT_TYPE_FIX      L"fix"
#define   XML_TABCTRL_CONTENTPANEL_PREFIX       L"contentpanel."

enum TABCTRL_HEAD_LAYOUT_TYPE
{
    TABCTRL_HEAD_LAYOUT_TYPE_FIX = 0,     // 固定大小排列在左侧
    TABCTRL_HEAD_LAYOUT_TYPE_AVERAGE = 1, // 按照TABCTRL的大小平均设置每个按钮的宽度
    TABCTRL_HEAD_LAYOUT_TYPE_Win32 = 2,   // 操作系统类型
};

// TreeView
#define   XML_EXPANDICON_RENDER_TYPE_PREFIX   L"expandicon."   // 展开/收缩图标render前缀


}

#endif 