#include "StdAfx.h"
#include "xmlparse.h"
#include <atlconv.h>

#include "UISDK\Kernel\Src\Atl\image.h"
#include "UISDK\Kernel\Src\Util\MapAttr\mapattr.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\Window\windowbase.h"

#if 0

project.ui <- ProjectXmlParse
<?xml version="1.0" encoding="utf-8" ?>
<ui>
	<proj  select="mediaplayer">                                        // 当前工程使用哪个界面皮肤
		<skin  name="mediaplayer" path="skin\mediaplayer\ui.xml"/>      // 一项皮肤配置信息，名称+路径
		<skin  name="orange" path="skin\orange\ui.xml"/>
	</proj>
	
	<log  swtich="on/off" >                                             // 当前工程中的日志配置信息, 全局日志开关，默认为off
		<item 
			name=""                                                     // 日志名称
			path=""                                                     // 日志文件输出路径
			flag="0x "                                                  // 日志打印方式
			fileflag="0x "                                              // 文件生成方式
			level=""                                                    // 日志输出级别
			switch="on/off"                                             // 该项日志开关，默认为off
		/>		
	</log>
</ui>

ui.xml  <- UIXmlParse

<?xml version="1.0" encoding="utf-8" ?>
<ui>
	<log>
		<item name="ui" path="log\ui.log" level="debug" flag="0x3" flag="0x3" fileflag="0x0A000008"/>
	</log>

	<xml>
		<!-- 图片资源配置文件路径 -->
		<img>img.xml</img>

		<!-- 颜色资源配置文件路径 -->
		<color>col.xml</color>

		<!-- 字体资源配置文件路径 -->
		<font>font.xml</font>

		<!-- 国际化配置文件路径 -->
		<i18n></i18n>

		<!-- 控件样式配置文件路径 -->
		<style></style>

		<!-- 控件位置配置文件路径 -->
		<pos></pos>
	</xml>
</ui>



img.xml <- ImgXmlParse 

<?xml version="1.0" encoding="utf-8" ?>
<...>
	<img>
		<item
			id="btn_4.png"     // id: 图片id，可起为图片名称
			type=""            // 图片类型:icon, imagelist, gif
			useskinhls=""      // 是否参与色调改变: 0/1
			count=""           // imagelist类型中图片项数量
			width="" height="" // icon类型加载时指定的图片大小
		>img\btn.png</item>	   // node data:图片路径，程序可自己转化成绝对路径。或者写成path=""属性
	</img>
</...>



font.xml <- FontXmlParse 

<?xml version="1.0" encoding="utf-8" ?>
<...>
	<font>
		<item id="songti"         // id
			fontface="宋体"       // 字体名称       (默认SYSTEM)
			height="9"            // 字体大小       (默认9)
			orientation="0"       // 字体角度       (默认0)
			bold="false"          // 是否粗体显示    0/1(默认0)
			italic="false"        // 是否斜体显示    0/1(默认0)
			underline="false"     // 是否下划线显示  0/1(默认0)
			strikeout="false"     // 是否删除线显示  0/1(默认0)
		></item>
	 </font>
 </...>


 color.xml <- ColorXmlParse

 <?xml version="1.0" encoding="utf-8" ?>
 <...>
	 <color>
		<item id="black" useskinhls="">255,255,255</item>   // 是否参与色调改变: 0/1
	 </color>
 </...>


 layout.xml <- LayoutXmlParse
 <?xml version="1.0" encoding="utf-8" ?>
 <...>
	 <layout>
		 <classname                         // object对应的xml名称
			 id=""                          // object的id，全局唯一，自动匹配style中的id selector
			 styleclass=""                  // object的样式类型，自动匹配style中的class selector，多个之间使用,分隔
		 >
		 </classname>
	 </layout>
 </...>


 style.xml <- CXmlStyleParse
 <?xml version="1.0" encoding="utf-8" ?>
 <...>
	 <style>
		 <class                         // CLASS选择器，具有通用性
			 id=""                      // CLASS选择器ID，用于外部引用时指定
			 inherit="a,b,c"            // 继承属性，多个继承关系用,分开
			 key1="value1"              // 属性1
			 key2="value2"              // 属性2
		 />
		 <id                            // ID选择器，具有唯一性
			 id=""                      // ID选择器ID，用于外部引用时指定
			 inherit="a,b,c"            // 继承属性，多个继承关系用,分开
			 key1="value1"              // 属性1
			 key2="value2"              // 属性2
		 /> 
		 <~tag~                         // 标签选择器，可自动匹配对应类型的标签
			 inherit="a,b,c"            // 继承属性，多个继承关系用,分开
			 key1="value1"              // 属性1
			 key2="value2"              // 属性2
		 />

	 </style>
 </...>

 layout.xml <- LayoutXmlParse
 <?xml version="1.0" encoding="utf-8" ?>
 <...>
	 <layout>
		 <classname                         // object对应的xml名称，不区分大小写

			 /* object 对象需要的属性列表 */

			 id=""                      // 对象id
			 width=""                   // 对象宽度，可以为 auto | 数值，（默认 auto）
			 height=""                  // 对象的高度，可以为 auto | 数值，（默认 auto）
			 margin=""                  // 对象的外边距，（默认为全0)
			 padding=""                 // 对象的内边距，（默认为全0)
			 minwidht=""                // 对象的最小宽度，（未实现)
			 minheight=""               // 对象的最小高度，（未实现)
			 maxwidth=""                // 对象的最大宽度，（未实现)
			 maxheight=""               // 对象的最大高度，（未实现)

			 erasebkgnd.type=""         // 对象背景绘制方式：color | gradienth | gradientv | imagesimple | 
			 erasebkgnd.color=""
			 erasebkgnd.colorfrom=""
			 erasebkgnd.colorto=""
			 erasebkgnd.image=""
			 erasebkgnd.bkcolor=""

			 cursor=""
			 font=""

			 /* panel 对象需要的属性列表 */

			 layout=""                  // 布局类型，可取值：stack，grid，dock，canvas，card，默认为canvas

			 /* 各种布局类型需要的属性列表 */

			 /* stack布局 */
			 layout.direction=""        // 方向，可以为lefttoright｜righttoleft｜leftandright｜toptobottom｜bottomtotop｜topandbottom，默认为lefttoright
			 layout.dock=""             // 如果direction取为leftandright/topandbottom时，依靠在哪边，可以为 left|right|top|bottom

			 /* canvas布局 */
			 layout.left=""             // 距离左边缘的大小，（默认为0）
			 layout.right=""            // 距离右边缘的大小，当存在layout.left时，优先取layout.left
			 layout.top=""              // 距离上边缘的大小，（默认为0）
			 layout.bottom=""           // 距离下边缘的大小，当存在layout.top时，优先取layout.top

			 /* grid布局 */
			 layout.width=""            // grid布局的各列的宽度定义，可取为：100,*,*,auto。逗号的数量决定了列数（默认为*）
			 layout.height=""           // grid布局的各列的高度定义，可取为：100,*,*,auto。逗号的数量决定了行数（默认为*）
			 layout.row=""              // 对象所在行，（默认为0）
			 layout.col=""              // 对象所在列，（默认为0）
			 layout.rowspan=""          // 对象占据的行数量，（默认为1）
			 layout.colspan=""          // 对象占据的列数量，（默认为1）

			 /* dock布局 */
			 layout.dock=""             // 对象停靠的方向，可取为：left,right,top,bottom,center
		 >
		 </classname>
	 </layout>
 </...>


 窗口类
 <classname
	
	 /* window对象需要的属性列表 */

	 font=""                    // 窗体使用的字体

	 /*  背景透明属性列表*/

	 transparentmaskcolor=""    // FrameWindowBackgroundRender特有，窗口透明色（默认粉色255,0,255) [默认]
	 transparentmaskalpha=""    // FrameWindowBackgroundRender特有，指定将小于多少alpha值的区域抠除掉 
	>
</classname

 控件基类control属性
 <classname
	 transparentbk="0/1"               // 控件的背景是否是透明的，透明的控件刷新时需要重新刷新整个窗口
 </classname>

 windows控件封装
 <HwndHost 
     controlid=""                       // 要封装的控件ID值，10进制
 >
 </HwndHost>

 控件属性，不包括基本的object属性
 <Button 
	 btnbkrender=""                     // 按钮渲染类型 color,theme,image
	 drawfocus=""                       // 是否绘制焦点状态 0,1
	 autosize=""                        // 按钮大小自动类型 content(默认),bkimage,bkimagehcontentw
	
	 bknormalcol=""                     // color bkrender类型的背景色 -- color id
	 bkhovercol=""
	 bkpresscol=""
	 bkdisablecol=""
	 bordernormalcol=""                 // color bkrender类型的边框色 -- color id
	 borderpresscol=""
	 borderhovercol=""
	 borderdisablecol=""
	 bkrenderflag=""                    // 需要绘制哪些状态的color 1|2|4|8，默认为6，绘制hover和press状态

	 bkimage=""                         // 按钮四状态图片
	 imagedirection=""                  // bkimage的方向 x,y

	 icon=""                            // 按钮图标--image id
	 icontype=""                        // 图标绘制类型，目前仅支持 static，8state还没有扩展 gif, 4state

	 text=""                            // 按钮标题
	 texttype=""                        // 文本绘制类型 singleline（默认）
	 >
 </Button>

 静态文本控件
 <Lable>
	 text=""                            // 文本内容
	 font=""                            // 文本使用的字体
	 color=""                           // 文本使用的颜色
	 halign=""                          // 横向对齐方式 left right center
	 valign=""                          // 纵向对齐方式 top bottom center
 </Lable>

 <ImageProgressCtrl
	 fore_image=""
	 bkgnd_image=""
	 btn.*=""
 >
 </ImageProgressCtrl>

 groupbox
 <GroupBox
	 text=""                            // 文本内容
	 font=""                            // 文本使用的字体
	 color=""                           // 文本使用的颜色
	 halign=""                          // 横向对齐方式 left right center
	 valign=""                          // 纵向对齐方式 top bottom center
	 borderimage=""                     // 边框图片
 >
 </GroupBox>

 // 编辑框
 <Edit
	color=""
	selectcolor=""
	selectbkcolor=""
	maxlength=""
 >
 </Edit>
#endif

//////////////////////////////////////////////////////////////////////////
//	
//	全局函数定义

//
// 加载一个xml，(并将内容全部转换成小写[废弃])
//
#if 0
bool LoadXml_ToLower( CMarkup* pXml, const String& strDataSource )
{
	if (NULL == pXml )
		return false;

	bool bRet = pXml->Load( strDataSource );
	if (false == bRet )
	{
		UI_LOG_ERROR( _T("XmlParse::LoadXml, failed to load xml:%s"), strDataSource.c_str() );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 统一切换为小写字母，防止配置大小写错误 注：由于涉及到一些英文文本内容配置，如text,tooltip
	// 因此不能随便就能内容全部转成小写

	//String content = pXml->GetDoc();
	//transform(content.begin(),content.end(),content.begin(),tolower);
	//return pXml->SetDoc(content);
	return bRet;
}

//
//	获取一个路径对应的临时文件路径
//
String Get_TempFile( const String& strDataSource )
{
	return strDataSource + TEMP_FILE_EXT;
}

// 将CMarkup中的内容保存到一个temp文件中，用于与外界共享
bool Commit_2_TempFile(CMarkup& markup, const String& strDataSource)
{
	String strTempFilePath = Get_TempFile(strDataSource);

// #ifdef _UNICODE
// 	wofstream f;
// 	//*setlocale(LC_ALL,"chs");*/
// 	locale loc("chs");
// 	f.imbue(loc);
// #else
// 	ofstream f;
// #endif
// 
// 	f.open(strTempFilePath.c_str(), ios_base::out );
// 	if (f.fail() )
// 		return false;
// 
// 	f << markup.GetDoc().c_str();
// 	UIASSERT(f.good());
// 
// 	f.close();

	markup.Save(strTempFilePath);
#ifdef _DEBUG
#else
	::SetFileAttributes(strTempFilePath.c_str(), FILE_ATTRIBUTE_HIDDEN );
#endif

	return true;
 }
// 从一个temp文件中重新加载数据到CMarkup中
bool Update_From_TempFile(CMarkup& markup, const String& strDataSource)
{
	return false;
}

bool Delete_TempFile(const String& strDataSource)
{
	String strTempFilePath = Get_TempFile(strDataSource);
	if (PathFileExists(strTempFilePath.c_str() ))
	{
		::DeleteFile(strTempFilePath.c_str());
	}
	return true;
}
#endif
//////////////////////////////////////////////////////////////////////////

#if 0
CXmlImageParse::CXmlImageParse()
{
}
CXmlImageParse::~CXmlImageParse()
{
	if (m_pUIApp->IsDesignMode() )
		::Delete_TempFile(m_strDataSource);;
}
bool CXmlImageParse::Create()
{
	::DeleteFile( m_strDataSource.c_str() );
	if (FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		if (false == Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI, XML_IMG) )
			return false;
	}

	if (false == m_xml.Load(m_strDataSource) )
		return false;

	::Commit_2_TempFile(m_xml, m_strDataSource);
	return true;
}
bool CXmlImageParse::Save( ImageRes*  pImageInfo )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return m_xml.Save(m_strDataSource);
}


bool CXmlImageParse::Load(ImageRes*  pImageInfo, CursorRes* pCursorInfo, GifRes* pGifInfo)
{
	if (NULL == pImageInfo )
		return false;

	CMarkup* pXml = NULL;
	CMarkup  tempXml;
	if (m_pUIApp->IsDesignMode() )  // 非design模式下，不需要保存xml的内容
		pXml = &m_xml;
	else
		pXml = &tempXml;

	bool bRet = this->load_from_file(pXml, m_strDataSource, pImageInfo, pCursorInfo, pGifInfo);
	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlImageParse::Load  failed!"));
	}
	else
	{
		if (m_pUIApp->IsDesignMode() )
		{
			::Commit_2_TempFile(m_xml, m_strDataSource);  // 将读取到的数据写入temp文件，用于外部编辑
		}
	}
	return bRet;

}

// 从temp文件
bool CXmlImageParse::Reload(ImageRes* pImageInfo, CursorRes* pCursorInfo, GifRes* pGifInfo)
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return this->load_from_file(&m_xml, m_strDataSource + TEMP_FILE_EXT, pImageInfo, pCursorInfo, pGifInfo);
}
bool CXmlImageParse::InsertImage(  ImageResItem *pImageItemInfo )
{
	if (NULL == pImageItemInfo )
		return false;

	if (false == m_pUIApp->IsDesignMode())
		return false;

	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_IMG) )            
		{
			if (false == m_xml.InsertElem( XML_IMG ))         break;
		}
		if (false == m_xml.IntoElem() )                       break;
		if (false == insert_image_2_xml(pImageItemInfo) )     break;

		bRet = true;
	}
	while(0);

	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}

bool CXmlImageParse::ModifyImage( ImageResItem *pImageItemInfo )
{
	if (NULL == pImageItemInfo )
		return false;

	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_IMG) )                break;
		if (false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if (false == m_xml.FindElem(XML_ITEM) )
				break;;
			if (m_xml.GetAttrib(XML_ID) == pImageItemInfo->GetIDRef())
			{
				TCHAR szRelative[MAX_PATH] = _T("");
				Util::CalcRelativePathToFile(m_strDataSource.c_str(), pImageItemInfo->GetPathRef().c_str(), szRelative );
				if (m_xml.SetData( pImageItemInfo->GetPathRef()) )  
				{
					bLoopRet = true;
				}
				break;
			}
		}
		if (!bLoopRet ) break;

		bRet = true;
	}
	while(0);


	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}

bool CXmlImageParse::RemoveImage( const String& strID )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_IMG) )                break;
		if (false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if (false == m_xml.FindElem(XML_ITEM) )
				break;;
			if (m_xml.GetAttrib(XML_ID) == strID )
			{
				if (m_xml.RemoveElem() )
				{
					bLoopRet = true;
				}
				break;
			}
		}
		if (!bLoopRet ) break;

		bRet = true;
	}
	while(0);

	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}


//
//	忽略原有xml内容进行重写(暂时没有用到)
//
bool CXmlImageParse::clear_save( ImageRes*  pImageInfo )
{
	if (NULL == pImageInfo )
		return false;

	CMarkup  xml;
	if (false == xml.Load(m_strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlImageParse::Save  load xml failed, datasource=%s"), m_strDataSource.c_str() );
		return false;
	}

	xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == xml.IntoElem() )                       break;

		if (xml.FindElem(XML_IMG) )            
		{
			xml.RemoveElem() ;
		}

		if (false == xml.InsertElem( XML_IMG ))             break;
		if (false == xml.IntoElem() )                       break;

		// 添加元素
		int  nCount   = (int)pImageInfo->GetImageCount();
		bool bLoopRet = true;  

		for ( int i = nCount-1; i >= 0; i-- )
		{
			ImageResItem*  pItem = pImageInfo->GetImageItem(i);

			if (false == insert_image_2_xml(pItem) ){ bLoopRet = false; break; }
		}
		if (!bLoopRet )  break;

		// 保存文件
		if (false == xml.Save(m_strDataSource))              break;

		bRet = true;
	} 
	while (0);

	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlImageParse::Save  failed!"));
	}
	return bRet;
}

bool CXmlImageParse::load_from_file( CMarkup* pXml, const String& strDataSource, ImageRes *pImageInfo, CursorRes* pCursorInfo, GifRes* pGifInfo )
{
#if 0 // TODO: 删除，重新架构了
	if (NULL == pXml || NULL == pImageInfo || NULL == pCursorInfo || NULL == pGifInfo)
		return false;

	if (false == ::LoadXml_ToLower(pXml, strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlImageParse::load_from_file  load xml failed, datasource=%s"), strDataSource.c_str() );
		return false;
	}

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == pXml->FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == pXml->IntoElem() )                       break;

		if (false == pXml->FindElem( XML_IMG ))               break;
		if (false == pXml->IntoElem() )                       break;

		bool bLoopRet = true;
		for ( ;; )
		{
//			if (false == pXml->FindElem( XML_ITEM ))        { break; }
			if (false == pXml->FindElem())                  { break; }

			String strTagName = pXml->GetTagName();

			//	加载所有属性
			ATTRMAP  mapAttrib;
			for( int j = 0; ; j++ )
			{
				String key = pXml->GetAttribName(j);
				if (_T("") == key )
					break;

				String value = pXml->GetAttrib( key );
				mapAttrib[key]= value;
			}

			String  strPath = pXml->GetData();
			if (strPath.empty())
			{
				strPath = pXml->GetAttrib(XML_PATH);   // 如果没有配置在内容中，就检查一下是否是配置成属性了
			}
			String  strFullPath;
			if (Util::IsFullPath(strPath.c_str()) )
			{
				strFullPath = strPath;
			}
			else
			{
				TCHAR szProjDir[MAX_PATH] = _T("");
				TCHAR szFullPath[MAX_PATH] = _T("");
				Util::GetPathDir(m_strDataSource.c_str(), szProjDir);
				Util::CalcFullPathByRelative( szProjDir, strPath.c_str(), szFullPath );

				strFullPath = szFullPath;
			}

			if (strTagName == XML_IMAGE_ITEM_CURSOR)
			{
				if (false == pCursorInfo->LoadItem(mapAttrib, strFullPath))
					UI_LOG_WARN(_T("%s insert cursor failed. path=%s"), FUNC_NAME, strFullPath.c_str());
			}
			else if (strTagName == XML_IMAGE_ITEM_GIF)
			{
				if (false == pGifInfo->LoadItem(mapAttrib, strFullPath))
					UI_LOG_WARN(_T("%s insert gif failed. path=%s"), FUNC_NAME, strFullPath.c_str());
			}
			else
			{
				if (false == pImageInfo->LoadItem(strTagName, mapAttrib, strFullPath))
					UI_LOG_WARN(_T("%s insert image failed. path=%s"), FUNC_NAME, strFullPath.c_str());
			}
		}
		bRet = true;
	} 
	while (0);

	return bRet;
#endif 
    return 0;
}
bool CXmlImageParse::insert_image_2_xml( ImageResItem *pImageItemInfo )
{
	TCHAR szRelative[MAX_PATH] = _T("");
	Util::CalcRelativePathToFile(m_strDataSource.c_str(), pImageItemInfo->GetPathRef().c_str(), szRelative );

	if (false == m_xml.AddElem( XML_ITEM, String(szRelative)) )    
		return false;
	if (false == m_xml.SetAttrib( XML_ID, pImageItemInfo->GetIDRef()) )  
		return false;

	return true;
}


//////////////////////////////////////////////////////////////////////////


CXmlColorParse::CXmlColorParse()
{
}
CXmlColorParse::~CXmlColorParse()
{
	if (m_pUIApp->IsDesignMode() )
		::Delete_TempFile(m_strDataSource);
}
bool CXmlColorParse::Create()
{
	::DeleteFile( m_strDataSource.c_str() );
	if (FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		if (false == Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI, XML_COLOR) )
			return false;
	}

	if (false == m_xml.Load(m_strDataSource) )
		return false;

	::Commit_2_TempFile(m_xml, m_strDataSource);
	return true;
}
bool CXmlColorParse::Save( ColorRes*  pColorInfo )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return m_xml.Save(m_strDataSource);
}
bool CXmlColorParse::clear_save(ColorRes* pColorInfo )
{
	if (NULL == pColorInfo )
		return false;

	CMarkup  xml;
	if (false == xml.Load(m_strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlColorParse::Save  load xml failed, datasource=%s"), m_strDataSource.c_str() );
		return false;
	}

	xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == xml.IntoElem() )                       break;

		if (xml.FindElem(XML_COLOR) )            
		{
			xml.RemoveElem() ;
		}

		if (false == xml.InsertElem( XML_COLOR ))             break;
		if (false == xml.IntoElem() )                       break;

		// 添加元素
		int  nCount   = (int)pColorInfo->GetColorCount();
		bool bLoopRet = true;  

		for ( int i = nCount-1; i >= 0; i-- )
		{
			ColorResItem* pItem = pColorInfo->GetColorItem(i);
			if (false == insert_color_2_xml(pItem) ){ bLoopRet = false; break; }
			
		}
		if (!bLoopRet )  break;

		// 保存文件
		if (false == xml.Save(m_strDataSource))              break;

		bRet = true;
	} 
	while (0);

	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlColorParse::Save  failed!"));
	}
	return bRet;
}

bool CXmlColorParse::Load( ColorRes*  pColorInfo )
{
	if (NULL == pColorInfo )
		return false;

	CMarkup* pXml = NULL;
	CMarkup  tempXml;
	if (m_pUIApp->IsDesignMode() )  // 非design模式下，不需要保存xml的内容
		pXml = &m_xml;
	else
		pXml = &tempXml;

	bool bRet = this->load_from_file(pXml, m_strDataSource, pColorInfo);
	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlColorParse::Load  failed!"));
	}
	else
	{
		if (m_pUIApp->IsDesignMode() )
		{
			::Commit_2_TempFile(m_xml, m_strDataSource);  // 将读取到的数据写入temp文件，用于外部编辑
		}
	}
	return bRet;
}
bool CXmlColorParse::load_from_file( CMarkup* pXml, const String& strDataSource, ColorRes *pColorInfo )
{
#if 0
	if(NULL == pXml || NULL == pColorInfo)
		return false;

	if (false == ::LoadXml_ToLower(pXml, strDataSource))
	{
		UI_LOG_ERROR( _T("CXmlColorParse::Load  load xml failed, datasource=%s"), m_strDataSource.c_str() );
		return false;
	}
	pXml->ResetPos();

	bool bRet = false;
	do 
	{
		if (false == pXml->FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == pXml->IntoElem() )                       break;

		if (false == pXml->FindElem( XML_COLOR ))             break;
		if (false == pXml->IntoElem() )                       break;

		bool bLoopRet = true;
		for ( ;; )
		{
			if (false == pXml->FindElem(XML_ITEM) )           break;

			//	加载所有属性
			ATTRMAP  mapAttrib;
			for( int j = 0; ; j++ )
			{
				String key = pXml->GetAttribName(j);
				if (_T("") == key )
					break;

				String value = pXml->GetAttrib( key );
				mapAttrib[key]= value;
			}

			String strValue = pXml->GetData();
			if (false == pColorInfo->LoadItem(mapAttrib, strValue))
						UI_LOG_WARN(_T("%s insert color failed."), FUNC_NAME);
		}
		if (!bLoopRet )  break;

		bRet = true;
	} 
	while (0);

	if (false == bRet )
	{
		UI_LOG_ERROR(_T("CXmlColorParse::load_from_file  failed!"));
	}
    return bRet;
#endif 
    return 0;
}

bool CXmlColorParse::Reload( ColorRes* pColorInfo )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return this->load_from_file( &m_xml, m_strDataSource + TEMP_FILE_EXT, pColorInfo );
}
bool CXmlColorParse::InsertColor( ColorResItem *pColorItemInfo )
{
	if (NULL == pColorItemInfo )
		return false;

	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_COLOR) )            
		{
			if (false == m_xml.InsertElem( XML_COLOR ))       break;
		}
		if (false == m_xml.IntoElem() )                       break;
		if (false == insert_color_2_xml(pColorItemInfo) )     break;

		bRet = true;
	}
	while(0);

	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlColorParse::ModifyColor( ColorResItem *pColorItemInfo )
{
	if (NULL == pColorItemInfo )
		return false;

	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_COLOR) )                break;
		if (false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if (false == m_xml.FindElem(XML_ITEM) )
				break;;
			if (m_xml.GetAttrib(XML_ID) == pColorItemInfo->GetIDRef() )
			{
				if (m_xml.SetData( pColorItemInfo->GetColorStringRef() ) )  
				{
					bLoopRet = true;
				}
				break;
			}
		}
		if (!bLoopRet ) break;

		bRet = true;
	}
	while(0);


	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlColorParse::RemoveColor( const String& strID )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_COLOR) )              break;
		if (false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if (false == m_xml.FindElem(XML_ITEM) )
				break;;
			if (m_xml.GetAttrib(XML_ID) == strID )
			{
				if (m_xml.RemoveElem() )
				{
					bLoopRet = true;
				}
				break;
			}
		}
		if (!bLoopRet ) break;

		bRet = true;
	}
	while(0);

	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlColorParse::insert_color_2_xml( ColorResItem *pColorItemInfo )
{
	if (NULL == pColorItemInfo)
		return false;

	if (false == m_xml.AddElem( XML_ITEM, pColorItemInfo->GetColorStringRef().c_str() ) )   
		return false;
	if (false == m_xml.SetAttrib( XML_ID, pColorItemInfo->GetIDRef().c_str())) 
		return false;

	return true;
}
//////////////////////////////////////////////////////////////////////////

CXmlFontParse::CXmlFontParse()
{
}
CXmlFontParse::~CXmlFontParse()
{
	if (m_pUIApp->IsDesignMode() )
		::Delete_TempFile(m_strDataSource);
}
bool CXmlFontParse::Create()
{
	::DeleteFile( m_strDataSource.c_str() );
	if (FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		if (false == Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI, XML_FONT) )
			return false;
	}

	if (false == m_xml.Load(m_strDataSource) )
		return false;

	::Commit_2_TempFile(m_xml, m_strDataSource);
	return true;
}
bool CXmlFontParse::Save( FontRes*  pFontInfo )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return m_xml.Save(m_strDataSource);
}
bool CXmlFontParse::clear_save( FontRes*  pFontInfo )
{
	if (NULL == pFontInfo )
		return false;

	CMarkup  xml;
	if (false == xml.Load(m_strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlFontParse::Save  load xml failed, datasource=%s"), m_strDataSource.c_str() );
		return false;
	}

	xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == xml.IntoElem() )                       break;

		if (xml.FindElem(XML_FONT) )            
		{
			xml.RemoveElem() ;
		}

		if (false == xml.InsertElem( XML_FONT ))             break;
		if (false == xml.IntoElem() )                       break;

		// 添加元素
		int  nCount   = (int)pFontInfo->GetFontCount();
		bool bLoopRet = true;  

		for ( int i = nCount-1; i >= 0; i-- )
		{
			FontResItem* pItem = NULL;
			pItem = pFontInfo->GetFontItem(i);
			if (NULL == pItem)
			{
				UI_LOG_WARN( _T("CXmlFontParse::Save GetFontItem failed, nIndex=%d"), i );
				continue;
			}

			if (false == this->insert_font_2_xml(pItem) ) 
			{ 
				bLoopRet = false; 
				break; 
			}

		}
		if (!bLoopRet )  break;

		// 保存文件
		if (false == xml.Save(m_strDataSource))              break;

		bRet = true;
	} 
	while (0);

	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlFontParse::Save  failed!"));
	}
	return bRet;
}

bool CXmlFontParse::Reload( FontRes* pFontInfo )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return this->load_from_file( &m_xml, m_strDataSource + TEMP_FILE_EXT, pFontInfo );	
}

bool CXmlFontParse::Load( FontRes* pFontInfo )
{
	if (NULL == pFontInfo )
		return false;

	CMarkup* pXml = NULL;
	CMarkup  tempXml;
	if (m_pUIApp->IsDesignMode() )  // 非design模式下，不需要保存xml的内容
		pXml = &m_xml;
	else
		pXml = &tempXml;

	bool bRet = this->load_from_file(pXml, m_strDataSource, pFontInfo);
	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlFontParse::Load  failed!"));
	}
	else
	{
		if (m_pUIApp->IsDesignMode() )
		{
			::Commit_2_TempFile(m_xml, m_strDataSource);  // 将读取到的数据写入temp文件，用于外部编辑
		}
	}
	return bRet;
}

bool CXmlFontParse::load_from_file( CMarkup* pXml, const String& strDataSource, FontRes *pFontInfo )
{
	if(NULL == pXml || NULL == pFontInfo )
		return false;

	if (false == ::LoadXml_ToLower(pXml, strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlFontParse::load_from_file  load xml failed, datasource=%s"), strDataSource.c_str() );
		return false;
	}

	pXml->ResetPos();

	bool bRet = false;
	do 
	{
		if (false == pXml->FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == pXml->IntoElem() )                       break;

		if (false == pXml->FindElem( XML_FONT ))              break;
		if (false == pXml->IntoElem() )                       break;

		bool bLoopRet = true;
		for ( ;; )
		{
			if (false == pXml->FindElem( XML_ITEM ) )        { break; }

			String strID = pXml->GetAttrib( XML_ID );
			String strFaceName = pXml->GetAttrib( XML_FONT_FACENAME );
			String strFontHeight = pXml->GetAttrib( XML_FONT_HEIGHT );
			String strOrientation = pXml->GetAttrib( XML_FONT_ORIENTATION );
			String strBold = pXml->GetAttrib( XML_FONT_BOLD );
			String strItalic = pXml->GetAttrib( XML_FONT_ITALIC );
			String strUnderline = pXml->GetAttrib( XML_FONT_UNDERLINE );
			String strStrikeout = pXml->GetAttrib( XML_FONT_STRIKEOUT );

			if (strFaceName.empty() )
			{
				strFaceName = _T("SYSTEM");
			}
			int nFontHeight = 9;
			int lfOrientation = 0;
			int lfEscapement = 0;
			int lfWeight = FW_NORMAL;
			int lfItalic = 0;
			int lfUnderline = 0;
			int lfStrikeOut = 0;

			if(!strFontHeight.empty())
			{
				nFontHeight = _ttoi(strFontHeight.c_str());
			}
			if(!strOrientation.empty())
			{
				lfOrientation = _ttoi(strOrientation.c_str());
			}
			if(!strBold.empty())
			{
				lfWeight = (_ttoi(strBold.c_str())==1)?FW_BOLD:FW_NORMAL;
			}
			if (!strItalic.empty())
			{
				lfItalic = _ttoi(strItalic.c_str());
			}
			if (!strUnderline.empty())
			{
				lfUnderline = _ttoi(strUnderline.c_str());
			}
			if (!strStrikeout.empty())
			{
				lfStrikeOut = _ttoi(strStrikeout.c_str());
			}


			LOGFONT lf;
			::ZeroMemory( &lf, sizeof(lf) );
			_tcsncpy( lf.lfFaceName, strFaceName.c_str(), 31 );
			lf.lfHeight = Util::FontSize2Height( nFontHeight );
			lf.lfOrientation = lfOrientation;
			lf.lfEscapement = lf.lfOrientation;
			lf.lfWeight = lfWeight;
			lf.lfItalic = lfItalic;
			lf.lfUnderline = lfUnderline;
			lf.lfStrikeOut = lfStrikeOut;

			if (false == pFontInfo->InsertFont( strID, &lf ) )
			{
				UI_LOG_WARN( _T("CXmlFontParse::load_from_file insert font failed. m_strID=%s, facename=%s"), strID.c_str(), strFaceName.c_str() );
			}
		}
		if (!bLoopRet )  break;

		bRet = true;
	} 
	while (0);

	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlFontParse::load_from_file  failed!"));
	}
	return bRet;
}

bool CXmlFontParse::InsertFont( FontResItem *pFontItemInfo )
{
	if (NULL == pFontItemInfo )
		return false;

	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_FONT) )            
		{
			if (false == m_xml.InsertElem( XML_FONT ))        break;
		}
		if (false == m_xml.IntoElem() )                       break;
		if (false == insert_font_2_xml(pFontItemInfo) )     break;

		bRet = true;
	}
	while(0);

	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlFontParse::ModifyFont( FontResItem *pFontItemInfo )
{
	if (NULL == pFontItemInfo)
		return false;

	if (!m_pUIApp->IsDesignMode())
		return true;

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_FONT) )                break;
		if (false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if (false == m_xml.FindElem(XML_ITEM) )
				break;;
			if (m_xml.GetAttrib(XML_ID) == pFontItemInfo->GetIDRef() )
			{
				TCHAR szFontFace[32] = _T("");
				pFontItemInfo->GetFaceName(szFontFace);
				if (false == m_xml.SetAttrib( XML_ID,    pFontItemInfo->GetIDRef() ))                      { bLoopRet = false; break; }
				if (false == m_xml.SetAttrib( XML_FONT_FACENAME,    szFontFace ))                          { bLoopRet = false; break; }
				if (false == m_xml.SetAttrib( XML_FONT_HEIGHT,      pFontItemInfo->GetFontSize() ))        { bLoopRet = false; break; }
				if (false == m_xml.SetAttrib( XML_FONT_ORIENTATION, pFontItemInfo->GetFontOrientation() )) { bLoopRet = false; break; }
				if (false == m_xml.SetAttrib( XML_FONT_BOLD,      (int)pFontItemInfo->GetFontBold() ))     { bLoopRet = false; break; }
				if (false == m_xml.SetAttrib( XML_FONT_ITALIC,    (int)pFontItemInfo->GetFontItalic() ))   { bLoopRet = false; break; }
				if (false == m_xml.SetAttrib( XML_FONT_UNDERLINE, (int)pFontItemInfo->GetFontUnderline() )){ bLoopRet = false; break; }
				if (false == m_xml.SetAttrib( XML_FONT_STRIKEOUT, (int)pFontItemInfo->GetFontStrikeout() )){ bLoopRet = false; break; }

				bLoopRet = true;
				break;
			}
		}
		if (!bLoopRet ) break;

		bRet = true;
	}
	while(0);


	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlFontParse::RemoveFont( const String& strID )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_FONT) )                break;
		if (false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if (false == m_xml.FindElem(XML_ITEM) )
				break;;
			if (m_xml.GetAttrib(XML_ID) == strID )
			{
				if (m_xml.RemoveElem() )
				{
					bLoopRet = true;
				}
				break;
			}
		}
		if (!bLoopRet ) break;

		bRet = true;
	}
	while(0);

	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}

bool CXmlFontParse::insert_font_2_xml( FontResItem *pFontItemInfo )
{
	if (NULL == pFontItemInfo)
		return false;

	if (false == m_xml.AddElem( XML_ITEM ) )   { return false; }

	TCHAR szFontFace[32] = _T("");
	pFontItemInfo->GetFaceName(szFontFace);
	if (false == m_xml.SetAttrib( XML_ID,    pFontItemInfo->GetIDRef() ))                      { return false; }
	if (false == m_xml.SetAttrib( XML_FONT_FACENAME,    szFontFace ))                          { return false; }
	if (false == m_xml.SetAttrib( XML_FONT_HEIGHT,      pFontItemInfo->GetFontSize() ))        { return false; }
	if (false == m_xml.SetAttrib( XML_FONT_ORIENTATION, pFontItemInfo->GetFontOrientation() )) { return false; }
	if (false == m_xml.SetAttrib( XML_FONT_BOLD,      (int)pFontItemInfo->GetFontBold() ))     { return false; }
	if (false == m_xml.SetAttrib( XML_FONT_ITALIC,    (int)pFontItemInfo->GetFontItalic() ))   { return false; }
	if (false == m_xml.SetAttrib( XML_FONT_UNDERLINE, (int)pFontItemInfo->GetFontUnderline() )){ return false; }
	if (false == m_xml.SetAttrib( XML_FONT_STRIKEOUT, (int)pFontItemInfo->GetFontStrikeout() )){ return false; }

	return true;
}
//////////////////////////////////////////////////////////////////////////


CXmlStyleParse::CXmlStyleParse()
{
}
CXmlStyleParse::~CXmlStyleParse()
{
	if (m_pUIApp->IsDesignMode() )
		::Delete_TempFile(m_strDataSource);
}
bool CXmlStyleParse::Create()
{
	::DeleteFile( m_strDataSource.c_str() );
	if (FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		if (false == Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI, XML_STYLE) )
			return false;
	}
	if (false == m_xml.Load(m_strDataSource) )
		return false;

	::Commit_2_TempFile(m_xml, m_strDataSource);
	return true;
}

bool CXmlStyleParse::Save( StyleRes* pStyleInfo )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return m_xml.Save(m_strDataSource);
}
bool CXmlStyleParse::Load( StyleRes* pStyleInfo )
{
	if (NULL == pStyleInfo )
		return false;

	CMarkup* pXml = NULL;
	CMarkup  tempXml;
	if (m_pUIApp->IsDesignMode() )  // 非design模式下，不需要保存xml的内容
		pXml = &m_xml;
	else
		pXml = &tempXml;

	bool bRet = this->load_from_file(pXml, m_strDataSource, pStyleInfo);
	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlStyleParse::Load  failed!"));
	}
	else
	{
		if (m_pUIApp->IsDesignMode() )
		{
			::Commit_2_TempFile(m_xml, m_strDataSource);  // 将读取到的数据写入temp文件，用于外部编辑
		}
	}
	return bRet;
}
bool CXmlStyleParse::Reload( StyleRes* pStyleInfo )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return this->load_from_file( &m_xml, m_strDataSource + TEMP_FILE_EXT, pStyleInfo );
}



//
// 解析一个继承字符串所属的样式类型，如将#button解析为 id选择类型，id=button
//
/*static*/ bool ParseInheritString(const String& strInherit, STYLE_SELECTOR_TYPE& eStyletype, TCHAR* szStyleName )
{
	if (strInherit.length() <= 0 || NULL == szStyleName )
		return false;

	if (strInherit[0] == STYLE_ID_PREFIX )
	{
		eStyletype = STYLE_SELECTOR_TYPE_ID;
		_tcsncpy( szStyleName, strInherit.substr( 1, strInherit.length()-1 ).c_str(), MAX_STRING_LEN-1 );
	}
	else if (strInherit[0] == STYLE_CLASS_PREFIX )
	{
		eStyletype = STYLE_SELECTOR_TYPE_CLASS;
		_tcsncpy( szStyleName, strInherit.substr( 1, strInherit.length()-1 ).c_str(), MAX_STRING_LEN-1 );
	}
	else
	{
		eStyletype = STYLE_SELECTOR_TYPE_TAG;
		_tcsncpy( szStyleName, strInherit.c_str(), MAX_STRING_LEN-1 );
	}
	return true;
}
/*static*/ bool MakeInheritString(const STYLE_SELECTOR_TYPE& eStyletype, const String& strStypeName, TCHAR* szInherit )
{
	if (strStypeName.length() <= 0 || NULL == szInherit )
		return false;

	if (eStyletype == STYLE_SELECTOR_TYPE_ID )
	{
		_tcscpy( szInherit, _T(" ") );
		szInherit[0] = STYLE_ID_PREFIX ;
		_tcscat( szInherit, strStypeName.c_str() );
	}
	else if (eStyletype == STYLE_SELECTOR_TYPE_CLASS )
	{
		_tcscpy( szInherit, _T(" ") );
		szInherit[0] = STYLE_CLASS_PREFIX ;
		_tcscat( szInherit, strStypeName.c_str() );
	}
	else
	{
		_tcscpy( szInherit, strStypeName.c_str() );
	}
	return true;
}


//
//	递归解析一个CPojo_StyleItem的继承数据
//
//	Parameter
//		pTreeItem
//			[in]	要解析的CPojo_StyleItem，pTreeItem代表它的继承依赖树，是树的叶子结点。
//					如果自己重复出现在这个树中，表示出现了死锁继承，无法解析。
//
//		pStyleRes
//			[in]	当前所有的StyleItem列表
//
//	Return
//		解析成功返回TRUE，失败返回FALSE。只要树中有一个返回FALSE，这棵树全部返回FALSE。
//
bool CXmlStyleParse::parse_inherit( tree<StyleResItem*>* pTreeItem, StyleRes* pStyleRes )
{
	if (NULL == pTreeItem || NULL == pTreeItem->data )
		return false;

	StyleResItem* pStyleItem = pTreeItem->data;

	// 判断pTreeItem是否有继承关系
	int nSize = pStyleItem->GetInheritCount();
	if (0 == nSize )
		return true;

	// 判断当前这个pTreeItem中整棵树中是不是重复出现了，如果重复出现了则表示死循环了
	{
		tree<StyleResItem*>* pParentItem = pTreeItem;
		do
		{
			pParentItem = pParentItem->parent;
			if (pParentItem && pParentItem->data == pStyleItem )
				return false;
		}
		while(pParentItem != NULL);
	}


	for( int i = nSize-1; i >= 0; i-- )
	{
		String strInherit;
		if (false == pStyleItem->GetInheritItem(i, strInherit) )
			continue;

		STYLE_SELECTOR_TYPE type = STYLE_SELECTOR_TYPE_TAG;
		TCHAR szStyleName[MAX_STRING_LEN] = _T("");
		ParseInheritString(strInherit, type, szStyleName);

		StyleResItem* pInheritItem = pStyleRes->GetStyleItem(type, szStyleName);
		if (NULL == pInheritItem )
			continue;

		tree<StyleResItem*> t;
		t.parent = pTreeItem;
		t.data   = pInheritItem;

		if (false == this->parse_inherit( &t, pStyleRes) )
		{
			return false;
		}

		// 解析成功，继承它的属性
		pInheritItem->InheritMyAttributesToAnother(pStyleItem);
		pStyleItem->RemoveInheritItem( strInherit );
	}

	return true;
}

bool CXmlStyleParse::load_from_file( CMarkup* pXml, const String& strDataSource, StyleRes *pStyleInfo )
{
	if (NULL == pXml || NULL == pStyleInfo )
		return false;

	if (false == ::LoadXml_ToLower(pXml, strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlStyleParse::load_from_file  load xml failed, datasource=%s"), strDataSource.c_str() );
		return false;
	}

	pXml->ResetPos();

	bool bRet = false;
	do 
	{
		if (false == pXml->FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == pXml->IntoElem() )                       break;

		if (false == pXml->FindElem( XML_STYLE ))             break;
		if (false == pXml->IntoElem() )                       break;

		bool bLoopRet = true;
		for ( ;; )
		{
			if (false == pXml->FindElem( ) )                  { break; }
			
			StyleResItem* pStyleItem = new StyleResItem;

			// 加载所有属性
			for( int j = 0; ; j++ )
			{
				String key = pXml->GetAttribName(j);
				if (_T("") == key )
					break;

				String value = pXml->GetAttrib( key );
				pStyleItem->SetAttribute(key,value);
			}

			// 解析  SELECTOR_TPYE, ID
			String strTagName = pXml->GetTagName();
			if (XML_STYLE_SELECTOR_TYPE_ID == strTagName )
			{
				pStyleItem->SetSelectorType( STYLE_SELECTOR_TYPE_ID );
				
				String strID;
				if (false == pStyleItem->GetAttribute( XML_ID, strID ) )
				{
					UI_LOG_WARN( _T("CXmlStyleParse::load_from_file Can't find the %s attribute of %s"), XML_ID, strTagName.c_str() );
					delete pStyleItem;
					continue;
				}
				pStyleItem->SetID( strID );
				pStyleItem->RemoveAttribute( XML_ID );
			}
			else if (XML_STYLE_SELECTOR_TYPE_CLASS == strTagName )
			{
				pStyleItem->SetSelectorType( STYLE_SELECTOR_TYPE_CLASS );

				String strID;
				if (false == pStyleItem->GetAttribute( XML_ID, strID ) )
				{
					UI_LOG_WARN( _T("CXmlStyleParse::load_from_file Can't find the %s attribute of %s"), XML_ID, strTagName.c_str() );
					delete pStyleItem;
					continue;
				}
				pStyleItem->SetID( strID );
				pStyleItem->RemoveAttribute( XML_ID );
			}
			else
			{
				pStyleItem->SetSelectorType( STYLE_SELECTOR_TYPE_TAG );
				pStyleItem->SetID(strTagName);
			}

			// 解析 inherit 属性
			String strInherit;
			if (pStyleItem->GetAttribute(XML_STYLE_INHERIT, strInherit) )
			{
				pStyleItem->RemoveAttribute(XML_STYLE_INHERIT);  // 不是一个属性，它是需要被扩展的
				pStyleItem->SetInherits( strInherit );
			}

			if (false == pStyleInfo->InsertStyle(pStyleItem) )
			{
				delete pStyleItem;
				pStyleItem = NULL;
			}
		}
		if (!bLoopRet )  break;

		// 在所有的ITEM加载完后，开始解析inherit关系
		int nSize = pStyleInfo->GetStyleCount();
		for ( int i = 0; i < nSize; i++ )
		{
			StyleResItem* pItem = pStyleInfo->GetStyleItem(i);
			tree<StyleResItem*>  t;
			t.data = pItem;

			if (false == this->parse_inherit(&t, pStyleInfo) )
			{
				UI_LOG_WARN(_T("CXmlStyleParse::load_from_file parse_inherit failed. style item=%s"), pItem->GetIDRef().c_str() );
			}
		}

		bRet = true;
	} 
	while (0);

	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlStyleParse::load_from_file  failed."));
	}
	return bRet;
}

bool CXmlStyleParse::insert_style_2_xml( StyleResItem *pStyleItemInfo )
{
	if (NULL == pStyleItemInfo)
		return false;

	STYLE_SELECTOR_TYPE type = pStyleItemInfo->GetSelectorType();
	const String& strID = pStyleItemInfo->GetIDRef();
	const String& strInherit = pStyleItemInfo->GetInheritsRef();

	switch( type )
	{
	case STYLE_SELECTOR_TYPE_ID:
		m_xml.AddElem( XML_STYLE_SELECTOR_TYPE_ID );
		m_xml.AddAttrib( XML_ID, strID );
		break;

	case STYLE_SELECTOR_TYPE_CLASS:
		m_xml.AddElem( XML_STYLE_SELECTOR_TYPE_CLASS );
		m_xml.AddAttrib( XML_ID, strID );
		break;

	default:
		m_xml.AddElem( strID );
		break;
	}

	if (! strInherit.empty() )
	{
		m_xml.AddAttrib( XML_STYLE_INHERIT, strInherit );
	}
	return true;
}

//
//	查找指定的style item所在的xml位置，忽视当前所在位置，全文搜索
//
bool CXmlStyleParse::find_elem_in_xml( STYLE_SELECTOR_TYPE type, const String& strID )
{
	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_STYLE) )              break;
		if (false == m_xml.IntoElem() )                       break;

		if (false == this->find_elem_under_style(type,strID)) break;

		bRet = true;
	}
	while(0);

	return bRet;
}
//
//	查找指定的style item所在的xml位置，默认当前位置处于<style>下的最开始
//
bool CXmlStyleParse::find_elem_under_style( STYLE_SELECTOR_TYPE type, const String& strID )
{
	bool bLoopRet = false;
	switch( type )
	{
	case STYLE_SELECTOR_TYPE_ID:
		{
			while(1)    
			{
				if (false == m_xml.FindElem(XML_STYLE_SELECTOR_TYPE_ID) )
					break;
				if (m_xml.GetAttrib(XML_ID) == strID )
				{
					bLoopRet = true;
					break;
				}
			}
		}
		break;

	case STYLE_SELECTOR_TYPE_CLASS:
		{
			while(1)    
			{
				if (false == m_xml.FindElem(XML_STYLE_SELECTOR_TYPE_CLASS) )
					break;
				if (m_xml.GetAttrib(XML_ID) == strID)
				{
					bLoopRet = true;
					break;
				}
			}
		}	
		break;

	default:
		{
			if (m_xml.FindElem(strID) )
				bLoopRet = true;
		}
		break;
	}

	return bLoopRet;
}
bool CXmlStyleParse::InsertStyle( StyleResItem *pStyleItemInfo )
{
	if (NULL == pStyleItemInfo )
		return false;

	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_STYLE) )            
		{
			if (false == m_xml.InsertElem( XML_STYLE ))       break;
		}
		if (false == m_xml.IntoElem() )                       break;
		if (false == insert_style_2_xml(pStyleItemInfo) )     break;

		bRet = true;
	}
	while(0);

	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlStyleParse::ModifyStyle( StyleResItem *pStyleItemInfo )
{
	if (NULL == pStyleItemInfo )
		return false;

	UIASSERT(m_pUIApp->IsDesignMode());

	if (false == this->find_elem_in_xml(pStyleItemInfo->GetSelectorType(), pStyleItemInfo->GetIDRef()) )
	{
		UI_LOG_WARN( _T("CXmlStyleParse::ModifyStyle Cannot find element type=%d, m_strID=%s"), pStyleItemInfo->GetSelectorType(), pStyleItemInfo->GetIDRef().c_str());
		return false;
	}

	m_xml.SetAttrib( XML_STYLE_INHERIT, pStyleItemInfo->GetInheritsRef() ) ;
	Commit_2_TempFile(m_xml, m_strDataSource );
		
	return true;
}
bool CXmlStyleParse::RemoveStyle( STYLE_SELECTOR_TYPE type, const String& strID )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	if (false == this->find_elem_in_xml( type, strID ) )
	{
		UI_LOG_WARN(_T("CXmlStyleParse::InsertStyleAttribute Cannot find elem in xml. type=%d, m_strID=%s"), type, strID.c_str() );
		return false;
	}

	m_xml.RemoveElem();
	Commit_2_TempFile(m_xml, m_strDataSource );

	return true;
}

bool CXmlStyleParse::InsertStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue )
{
	UIASSERT( m_pUIApp->IsDesignMode() );

	if (false == this->find_elem_in_xml(type, strID) )
	{
		UI_LOG_WARN(_T("CXmlStyleParse::InsertStyleAttribute Cannot find elem in xml. type=%d, m_strID=%s"), type, strID.c_str() );
		return false;
	}

	m_xml.AddAttrib(strKey, strValue);
	Commit_2_TempFile(m_xml, m_strDataSource );
	return true;
}
bool CXmlStyleParse::ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue )
{
	UIASSERT( m_pUIApp->IsDesignMode() );

	if (false == this->find_elem_in_xml(type, strID) )
	{
		UI_LOG_WARN(_T("CXmlStyleParse::InsertStyleAttribute Cannot find elem in xml. type=%d, m_strID=%s"), type, strID.c_str() );
		return false;
	}

	m_xml.SetAttrib(strKey,strValue);
	Commit_2_TempFile(m_xml, m_strDataSource );
	return true;
}
bool CXmlStyleParse::RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey )
{
	UIASSERT( m_pUIApp->IsDesignMode() );

	if (false == this->find_elem_in_xml(type, strID) )
	{
		UI_LOG_WARN(_T("CXmlStyleParse::InsertStyleAttribute Cannot find elem in xml. type=%d, m_strID=%s"), type, strID.c_str() );
		return false;
	}

	UIASSERT( false && _T("TODO: 没有找到RemoveAttrib方法") );
	m_xml.SetAttrib(strKey,_T(""));  // TODO: 没有找到RemoveAttrib方法
	Commit_2_TempFile(m_xml, m_strDataSource );

	return true;
}
//////////////////////////////////////////////////////////////////////////


CXmlLayoutParse::CXmlLayoutParse()
{
}
CXmlLayoutParse::~CXmlLayoutParse()
{
	if (m_pUIApp->IsDesignMode() )
		::Delete_TempFile(m_strDataSource);
}
bool CXmlLayoutParse::Create()
{
	::DeleteFile( m_strDataSource.c_str() );
	if (FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		if (false == Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI, XML_LAYOUT) )
			return false;
	}

	if (false == m_xml.Load(m_strDataSource) )
		return false;

	::Commit_2_TempFile(m_xml, m_strDataSource);
	return true;
}

bool CXmlLayoutParse::Save( LayoutRes* pLayoutInfo )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return m_xml.Save(m_strDataSource);
}
bool CXmlLayoutParse::Load(LayoutRes* pLayoutInfo)
{
	if (NULL == pLayoutInfo)
		return false;

	CMarkup* pXml = NULL;
	CMarkup  tempXml;
//	if (m_pUIApp->IsDesignMode() )  // layout.xml比较特殊，需要全局使用，随时加载
		pXml = &m_xml;
//	else
//		pXml = &tempXml;

	bool bRet = this->load_from_file(pXml, m_strDataSource, pLayoutInfo);
	if (false == bRet)
	{
		UI_LOG_FATAL(_T("CXmlLayoutParse::Load  failed!"));
	}
	else
	{
		if (m_pUIApp->IsDesignMode())
		{
			::Commit_2_TempFile(m_xml, m_strDataSource);  // 将读取到的数据写入temp文件，用于外部编辑
		}
	}
	return bRet;
}

bool CXmlLayoutParse::load_from_file( CMarkup* pXml, const String& strDataSource, LayoutRes* pLayoutInfo )
{
	if (NULL == pXml || NULL == pLayoutInfo)
		return false;

	if (false == ::LoadXml_ToLower(pXml, strDataSource))
	{
		UI_LOG_ERROR( _T("CXmlLayoutParse::load_from_file  load xml failed, datasource=%s"), strDataSource.c_str() );
		return false;
	}
	
	return true;
}

//
//	加载指定的对象的布局数据，在xml中将根据pRootObj的object name和id来查找对象及数据
//
bool CXmlLayoutParse::LoadLayout(Object* pRootObj, LayoutRes* pResLayout)
{
	Object* pObj = NULL;
	bool     bRet = false;

	this->m_xml.ResetPos();

	do
	{
		//
		//  1. 在xml中定位到pRootOb这个对象（依据对象xml名称和对象id属性）
		//
		if (false == m_xml.FindElem())               break;     // 忽略root结点的名称
		if (false == m_xml.IntoElem())               break;

		if (false == m_xml.FindElem(XML_LAYOUT))     break;
		if (false == m_xml.IntoElem())               break;

		while (true)
		{
			if (false == m_xml.FindElem(pRootObj->GetObjectName()))
			{
				bRet = false;
				break;
			}

			if (m_xml.GetAttrib(XML_ID) != pRootObj->GetID())
			{
				continue;
			}
			else
			{
				bRet = true;
				break;
			}
		}
		if (!bRet)
		{
			UI_LOG_FATAL(_T("%s, 未找到要加载的对象：name=%s, id=%s"),
			                FUNC_NAME, pRootObj->GetObjectName(), pRootObj->GetID());
			break;
		}

		bRet = false;      // reset

		//
		//  2. 加载指定对象pRootOb的属性
		//
		this->loadAttributeForCurrentObjectInXml(pRootObj, pResLayout);

		bRet = true;
	}
	while (false);

	//
	//	3. 现在开始递归加载这个窗口的控件对象了
	//
	if (true == bRet)
	{
		this->loadObjects(pRootObj, pResLayout);

		// 发送一个通知消息用于创建其它内部对象
		UISendMessage(pRootObj, UI_WM_OBJECTLOADED);
	}

	return bRet;
}

//
//	-- 换肤
//
bool  CXmlLayoutParse::ReLoadLayout(Object* pRootObj, list<Object*>& listAllChild)
{
	Object*  pObj = NULL;
	bool      bRet = false;

	this->m_xml.ResetPos();

	do
	{
		//
		//  1. 在xml中定位到pRootOb这个对象（依据对象xml名称和对象id属性）
		//
		if (false == m_xml.FindElem())              break;     // 忽略root结点的名称
		if (false == m_xml.IntoElem())              break;

		if (false == m_xml.FindElem(XML_LAYOUT))    break;
		if (false == m_xml.IntoElem())              break;

		while (true)
		{
			if (false == m_xml.FindElem( pRootObj->GetObjectName()))
			{
				bRet = false;
				break;
			}

			if (m_xml.GetAttrib(XML_ID) != pRootObj->GetID())
			{
				continue;
			}
			else
			{
				bRet = true;
				break;
			}
		}
		if (!bRet)
		{
			UI_LOG_FATAL( _T("%s, 未找到要加载的对象：name=%s, id=%s"),
				FUNC_NAME, pRootObj->GetObjectName(), pRootObj->GetID());
			break;
		}

		bRet = false;      // reset

		//
		//  2. 加载指定对象pRootOb的属性
		//
		this->loadAttributeForCurrentObjectInXml(pRootObj, NULL, true);

		bRet = true;
	}
	while(false);

	//
	//	3. 现在开始递归加载这个窗口的控件对象了
	//
	if (true == bRet)
	{
		this->reloadObjects(pRootObj, listAllChild);
	}

	return bRet;
}
#if 0
IObject*  CXmlLayoutParse::LoadRootObject(const TCHAR* szTag, const TCHAR szId)
{
    if (NULL == szTag || NULL == szId)
        return NULL;

    IObject*  pObject = NULL;
    bool  bRet = false;

    String  strTag = szTag;
    String  strId = szId;

    this->m_xml.ResetPos();

    do
    {
        //
        //  1. 在xml中定位到pRootOb这个对象（依据对象xml名称和对象id属性）
        //
        if (false == m_xml.FindElem())              break;     // 忽略root结点的名称
        if (false == m_xml.IntoElem())              break;

        if (false == m_xml.FindElem(XML_LAYOUT))    break;
        if (false == m_xml.IntoElem())              break;

        while (true)
        {
            if (false == m_xml.FindElem(strTag))
            {
                bRet = false;
                break;
            }

            if (m_xml.GetAttrib(XML_ID) != strId)
            {
                continue;
            }
            else
            {
                bRet = true;
                break;
            }
        }
        if (!bRet)
        {
            UI_LOG_FATAL( _T("%s, 未找到要加载的对象：name=%s, id=%s"),
                FUNC_NAME, XML_MENU, szId);
            break;
        }

        bRet = false;      // reset

        m_pUIApp->CreateInstanceByName(szTag, &pObject);

        //
        //  2. 
        //
        this->loadAttributeForCurrentObjectInXml(pMenu, NULL);

        bRet = true;
    }
    while(false);

    //
    //	3. 现在开始递归加载这个窗口的控件对象了
    //
    if (true == bRet )
    {
        this->loadMenuItems( pMenu );
    }

    return pMenu;
}
#endif
#if 0 // -- 架构改造
Menu* CXmlLayoutParse::LoadMenu(const String& strMenuId)
{
	Menu*    pMenu = NULL;
	bool     bRet = false;

	this->m_xml.ResetPos();

	do
	{
		//
		//  1. 在xml中定位到pRootOb这个对象（依据对象xml名称和对象id属性）
		//
		if (false == m_xml.FindElem())              break;     // 忽略root结点的名称
		if (false == m_xml.IntoElem())              break;

		if (false == m_xml.FindElem(XML_LAYOUT))    break;
		if (false == m_xml.IntoElem())              break;

		while (true)
		{
			if (false == m_xml.FindElem(XML_MENU))
			{
				bRet = false;
				break;
			}

			if (m_xml.GetAttrib(XML_ID) != strMenuId)
			{
				continue;
			}
			else
			{
				bRet = true;
				break;
			}
		}
		if (!bRet)
		{
			UI_LOG_FATAL( _T("%s, 未找到要加载的对象：name=%s, id=%s"),
				       FUNC_NAME, XML_MENU, strMenuId.c_str());
			break;
		}

		bRet = false;      // reset

		UICreateInstance(&pMenu, m_pUIApp);  // TODO:
		pMenu->ModifyStyle(MENU_STYLE_AUTO_DELETE_SUBMENU);

		//
		//  2. 
		//
		this->loadAttributeForCurrentObjectInXml(pMenu, NULL);

		bRet = true;
	}
	while(false);

	//
	//	3. 现在开始递归加载这个窗口的控件对象了
	//
	if (true == bRet )
	{
		this->loadMenuItems( pMenu );
	}

	return pMenu;
}

#endif
IStyleManager*  CXmlLayoutParse::getStyleMgr()
{
	if (NULL == m_pUIApp)
		return NULL;

	ISkinManager* pSkinMgr = NULL;
	m_pUIApp->GetSkinMgr(&pSkinMgr);
	if (NULL == pSkinMgr)
		return NULL;

	ISkinRes* pSkinRes = NULL;
	pSkinMgr->GetActiveSkin(&pSkinRes);
	if (NULL == pSkinRes)
		return NULL;

	IStyleManager* pStyleMgr = NULL;
	pSkinRes->GetStyleManager(&pStyleMgr);
	
	return pStyleMgr;
}

#if 0 // -- 架构改造
bool CXmlLayoutParse::loadMenuItems(MenuBase* pParentMenu)
{
	bool bRet = false;

	IStyleManager* pStyleMgr = this->getStyleMgr();
	if (NULL == pStyleMgr)
		return false;

	if (false == m_xml.IntoElem())
		return false;

	// 遍历所有子对象
	while(true)
	{
		bool bRet = this->m_xml.FindElem();
		if (false == bRet )     break;

		Object*  pObj = NULL;
		String   tagName = this->m_xml.GetTagName();
		if (_T("") == tagName)
		{
			UI_LOG_WARN( _T("%s, xml invalid tag name."), FUNC_NAME );
			return false;
		}

		//	加载所有属性
		ATTRMAP  mapAttrib;
		for( int j = 0; ; j++ )
		{
			String key = m_xml.GetAttribName(j);
			if (_T("") == key )
				break;

			mapAttrib[key]= m_xml.GetAttrib( key );
		}

		// 特殊处理：扩展style
		String strTagName, strStyleClass, strID;

		ATTRMAP::iterator iter = mapAttrib.find(XML_ID); // id = Menu.id，因为这里的id都是整数值
		if (mapAttrib.end() != iter)
		{
			strID = pParentMenu->GetObjectName();
			strID.push_back(XML_CHILD_SEPARATOR);
			strID.append(iter->second);
		}
		iter = mapAttrib.find(XML_STYLECLASS);
		if (mapAttrib.end() != iter)
		{
			strStyleClass = iter->second;
		}
		strTagName = pParentMenu->GetObjectName();   // tagName = Menu.String / Menu.Popup / Menu.Separator
		strTagName.push_back(XML_CHILD_SEPARATOR);
		strTagName.append(tagName);

		pStyleMgr->LoadStyle(strTagName, strStyleClass, strID, mapAttrib);

		// 加载菜单子项
		MenuItem* pItem = pParentMenu->LoadMenuItem(tagName, mapAttrib);
		if (pItem && pItem->IsPopup())
		{
			MenuBase* pSubMenu = pItem->GetSubMenu();
			this->loadAttributeForCurrentObjectInXml(pSubMenu, NULL);
			this->loadMenuItems(pSubMenu);
		}
	}

	m_xml.OutOfElem();

	return bRet;
}
#endif

Object*  CXmlLayoutParse::mapXmlToClass(const String& strXmlName)
{
	IObject* pIObject = NULL;
	m_pUIApp->CreateInstanceByName((BSTR)strXmlName.c_str(), &pIObject);
    if (NULL == pIObject)
        return NULL;

    Object* p = pIObject->GetObjectImpl();
	if (NULL == p && m_pUIApp->IsDesignMode())
	{
		TCHAR szText[128] = _T("");
		_stprintf(szText, _T("%s CreateInstance Failed, xmlName=%s. 该控件配置在保存时将丢失."), FUNC_NAME, strXmlName.c_str());
		::MessageBox(NULL, szText, _T("Error"), MB_OK|MB_ICONERROR);
	}
	return p;
}

//
//	为m_mxl当前指向的对象加载它的属性
//
bool CXmlLayoutParse::loadAttributeForCurrentObjectInXml(Object* pObj, LayoutRes* pResLayout, bool bReload/*=false*/)
{
	IStyleManager* pStyleMgr = this->getStyleMgr();
	if (NULL == pStyleMgr)
		return false;

	LayoutResItem* pResItem = NULL;
	if (pResLayout)
	{
		pResItem = pResLayout->GetResItem(pObj);
		if (NULL == pResItem)
			pResItem = pResLayout->AddResItem(pObj);
	}

    IMapAttribute* pMapAttrib = NULL;
    UICreateIMapAttribute(&pMapAttrib);
    UIASSERT(pMapAttrib);

	//
	//	加载所有属性
	//
	for (int j = 0; ; j++)
	{
		String key = m_xml.GetAttribName(j);
		if (_T("") == key)
			break;

        pMapAttrib->AddAttr(key.c_str(), m_xml.GetAttrib(key).c_str());
	}

	if (m_pUIApp->IsDesignMode() && NULL != pResItem)
	{
		pResItem->SetSelfAttr(pMapAttrib);
	}

	//
	// 特殊处理：扩展style
	//

    String strID, strStyleClass;  // 避免pMapAttrib->GetAttr返回临时变量。
    {
	    const TCHAR*  szID = pMapAttrib->GetAttr(XML_ID, false);
        if (szID)
            strID = szID;

    	const TCHAR*  szStyleClass = pMapAttrib->GetAttr(XML_STYLECLASS, false);
        if (szStyleClass)
            strStyleClass = szStyleClass;
    }

	const TCHAR*  szTagName = pObj->GetObjectName();

	if (m_pUIApp->IsDesignMode() && NULL != pResItem)
	{
		//ATTRMAP attrMapStyle;
        IMapAttribute* pAttrMapStyle = NULL;
        UICreateIMapAttribute(&pAttrMapStyle);
		pStyleMgr->LoadStyle(szTagName, strStyleClass.c_str(), strID.c_str(), pAttrMapStyle);
		pResItem->SetStyleAttr(pAttrMapStyle);
        SAFE_RELEASE(pAttrMapStyle);

		pStyleMgr->LoadStyle(szTagName, strStyleClass.c_str(), strID.c_str(), pMapAttrib);
		pResItem->SetLastAttr(pMapAttrib);

#if 0 // -- 架构改造
		IUIBuilder* pUIBuilder = NULL;
		m_pUIApp->GetUIBuilderPtr(&pUIBuilder);
		if (pUIBuilder)
		{
			pUIBuilder->OnObjectAttributeLoad(pObj, static_cast<ILayoutResItem*>(pResItem));
		}
#endif
	}
	else
	{
		pStyleMgr->LoadStyle(szTagName, strStyleClass.c_str(), strID.c_str(), pMapAttrib);  // 非编辑模式下，直接使用该mapattrib
	}

	// 
	//	具体的key、value解析交由对象自己去处理
	//
    if (bReload)
    {
        UISendMessage(pObj, UI_WM_RESETATTRIBUTE);
    }
    UISendMessage(pObj, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, (LPARAM)bReload);
    SAFE_RELEASE(pMapAttrib);

// 	if (bReload)
// 	{
// 		pObj->ResetAttribute();
// 	}
// 	pObj->SetAttribute(mapAttrib, bReload);

	return true;
}

//
// 递归，加载所有子对象及子对象的属性
//
bool CXmlLayoutParse::loadObjects(Object* pObjParent, LayoutRes* pResLayout )
{
	bool bRet = false;

	if (false == m_xml.IntoElem())
		return false;

	// 遍历所有子对象
	while (true)
	{
		bool bRet = this->m_xml.FindElem();
		if (false == bRet)     break;

		Object*  pObj = NULL;
		String   tagName = this->m_xml.GetTagName();
		if (_T("") == tagName)
		{
			UI_LOG_WARN( _T("LayoutXmlParse::loadObjects, xml invalid tag name.") );
			return false;
		}
		else if (XML_RENDERCHAIN == tagName)
		{
			m_xml.IntoElem();
			this->loadRenderChain(pObjParent);
			m_xml.OutOfElem();
			continue;
		}

		// 将标签名转化为类
		pObj = this->mapXmlToClass(tagName);
		if (NULL == pObj)
		{
			continue;
		}

		// 关联
		pObjParent->AddChild(pObj);

		// 加载属性
		this->loadAttributeForCurrentObjectInXml(pObj, pResLayout);

		// 默认将该控件的添加一个notify object为窗口对象，并将msgmapid设置为0
        WindowBase* pWindowBase = pObj->GetWindowObject();
        if (pWindowBase)
        {
		    pObj->SetNotify(pWindowBase->GetIMessage(), 0);
        }

		// 递归，加载这个子对象的子对象
		this->loadObjects(pObj, pResLayout);

		// 发送一个通知消息用于创建其它内部对象
        UISendMessage(pObj, UI_WM_OBJECTLOADED);
    }

    m_xml.OutOfElem();

    return bRet;
}


bool CXmlLayoutParse::reloadObjects(Object* pObjParent, list<Object*>& listAllChild)
{

	bool bRet = false;

	if (false == m_xml.IntoElem() )
		return false;

	// 遍历所有子对象
	while (true)
	{
		bool bRet = this->m_xml.FindElem();
		if (false == bRet)     break;

		Object*  pObj = NULL;
		String   tagName = this->m_xml.GetTagName();
		if (_T("") == tagName)
		{
			UI_LOG_WARN( _T("LayoutXmlParse::loadObjects, xml invalid tag name.") );
			continue;
		}
		String   strID = this->m_xml.GetAttrib( XML_ID );
		if (strID.empty())
		{
		}
		else
		{
			// 根据 tagName + id 从listAllChild中查找该对象
			list<Object*>::iterator  iter = listAllChild.begin();
			list<Object*>::iterator  iterEnd = listAllChild.end();

			for ( ; iter != iterEnd; iter++ )
			{
				Object* pTempObj = *iter;
				if (pTempObj->GetObjectName() == tagName && pTempObj->GetID() == strID)
				{
					pObj = pTempObj;
					break;
				}
			}
		}

		if (NULL == pObj)
		{
			// 没有找到
			// 直接添加这个对象
			// 将标签名转化为类
			pObj = this->mapXmlToClass( tagName );
			if (NULL == pObj)
			{
				continue;
			}

			// 关联
			pObjParent->AddChild(pObj);

			// 加载属性
			this->loadAttributeForCurrentObjectInXml(pObj, false);

			// 默认将该控件的添加一个notify object为窗口对象，并将msgmapid设置为0
            WindowBase* pWindowBase = pObj->GetWindowObject();
            if (pWindowBase)
            {
			    pObj->SetNotify(pWindowBase->GetIMessage(), 0);
            }

			// 递归，加载这个子对象的子对象
			this->reloadObjects(pObj, listAllChild);
		}
		else
		{
			// 关联
			pObjParent->AddChild(pObj);

			// 加载属性
			this->loadAttributeForCurrentObjectInXml(pObj, NULL, true);

			// 递归，加载这个子对象的子对象
			this->reloadObjects(pObj, listAllChild);

			// 从列表中移除
			list<Object*>::iterator iter = listAllChild.begin();
			list<Object*>::iterator iterEnd = listAllChild.end();
			for (; iter != iterEnd; iter++)
			{
				if (pObj == (*iter))
				{
					listAllChild.erase(iter);
					break;
				}
			}

		}
	}

	m_xml.OutOfElem();

	return bRet;
}
bool CXmlLayoutParse::loadRenderChain(Object* pObjParent)
{
	if (NULL == pObjParent || OBJ_WINDOW != pObjParent->GetObjectType())
		return false;

	IWindowBase* pWindow = (IWindowBase*)pObjParent;
	while (m_xml.FindElem())
	{
		if (m_xml.GetTagName() != XML_RENDERCHAIN_LAYER)
			continue;

        IMapAttribute*  pMapAttrib = NULL;
        UICreateIMapAttribute(&pMapAttrib);

		for (int j = 0; ; j++)
		{
			String key = m_xml.GetAttribName(j);
			if (_T("") == key)
				break;

			String value = m_xml.GetAttrib( key );
            pMapAttrib->AddAttr(key.c_str(), m_xml.GetAttrib(key).c_str());
		}
		pWindow->InsertRenderLayer(pMapAttrib);
        SAFE_RELEASE(pMapAttrib);
	}
	return true;
}

bool CXmlLayoutParse::LoadAllWindowNodeInfo(LayoutWindowNodeInfo* pInfoArray, int* pArraySize)
{
	m_xml.ResetPos();

	int nIndex = 0;
	do
	{
		//
		//  1. 在xml中定位到pRootOb这个对象（依据对象xml名称和对象id属性）
		//
		if (false == m_xml.FindElem( ) )             break;     // 忽略root结点的名称
		if (false == m_xml.IntoElem() )              break;

		if (false == m_xml.FindElem( XML_LAYOUT) )   break;
		if (false == m_xml.IntoElem() )              break;

		while (m_xml.FindElem())
		{
			if (pInfoArray)
			{
				String strTagName = m_xml.GetTagName();
				String strNodeID = m_xml.GetAttrib(XML_ID);

				_tcsncpy(pInfoArray[nIndex].szNodeName, strTagName.c_str(), MAX_STRING_LEN-1);
				_tcsncpy(pInfoArray[nIndex].szNodeID,   strNodeID.c_str(),   MAX_STRING_LEN-1);
			}
			nIndex++;
		}
	}
	while(0);

	if (NULL == pInfoArray && NULL != pArraySize)
	{
		*pArraySize = nIndex;
	}
	
	return true;
}
#endif