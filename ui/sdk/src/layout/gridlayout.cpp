#include "include/inc.h"
#include "gridlayout.h"
#include "src/object/object.h"
#include "src/attribute/attribute.h"
#include "src/attribute/long_attribute.h"
#include "src/attribute/flags_attribute.h"

using namespace ui;

GridWH::GridWH()
{
	this->last = 0;
	this->xml = 0;
	this->type = GWHT_AUTO;
}

GridLayout::GridLayout()
{
}
GridLayout::~GridLayout()
{
	this->widths.clear();
	this->heights.clear();
}

void  GridLayout::LoadGridWidth(const wchar_t* szWidth)
{
    if (!szWidth)
        return;

    util::ISplitStringEnum*  pEnum = nullptr;
    int nCount = util::SplitString(szWidth, XML_SEPARATOR, &pEnum);
    for (int i = 0; i < nCount; i++)
    {
        String  str = pEnum->GetText(i);

        // 判断是否为 AUTO 类型
        if (XML_AUTO == str)
        {
            GridWH  wh;
            wh.type = GWHT_AUTO;

            this->widths.push_back( wh );
            continue;
        }

        // 判读是否为 * 类型
        String::size_type nIndex = str.find(XML_ASTERISK);
        if (String::npos != nIndex)
        {
            GridWH  wh;
            wh.type = GWHT_ASTERISK;

            int nXml = util::wtoi(str.c_str());
            if (nXml == 0)  // 如果只指定 "*"，那么表示"1*"
                nXml = 1;

            wh.xml = nXml;
            this->widths.push_back( wh );
            continue;
        }

        // 判断其他情况
        GridWH  wh;
        wh.type = GWHT_VALUE;
        wh.last = util::wtoi( str.c_str() );
        this->widths.push_back( wh );

        continue;
    }
    SAFE_RELEASE(pEnum);
}
const wchar_t*  GridLayout::SaveGridWidth()
{
    String&  strBuffer = GetTempBufferString();
    wchar_t*  szBuffer = GetTempBuffer();

    int nCount = (int)widths.size();
    for (int i = 0; i < nCount; i++)
    {
        if (!strBuffer.empty())
            strBuffer.push_back(XML_SEPARATOR);

        switch (widths[i].type)
        {
        case GWHT_ASTERISK:
            {
                if (widths[i].xml <= 1)
                {
                    strBuffer.append(XML_ASTERISK);
                }
                else
                {
                    wprintf(szBuffer, TEXT("%d%s"), widths[i].xml, XML_ASTERISK);
                    strBuffer.append(szBuffer);
                }
            }
            break;

        case GWHT_AUTO:
            {
                strBuffer.append(XML_AUTO);
            }
            break;

        default://  GWHT_VALUE:
            {
                wprintf(szBuffer, TEXT("%d"), widths[i].last);
                strBuffer.append(szBuffer);
            }
            break;
        }
    }

    return strBuffer.c_str();
}

void  GridLayout::LoadGridHeight(const wchar_t* szHeight)
{
    util::ISplitStringEnum*  pEnum = nullptr;
    int nCount = util::SplitString(szHeight, XML_SEPARATOR, &pEnum);
    for (int i = 0; i < nCount; i++)
    {
        String str = pEnum->GetText(i);

        // 判断是否为 AUTO 类型
        if (XML_AUTO == str)
        {
            GridWH  wh;
            wh.type = GWHT_AUTO;

            this->heights.push_back( wh );
            continue;
        }

        // 判读是否为 * 类型
        String::size_type nIndex = str.find(XML_ASTERISK);
        if (String::npos != nIndex)
        {
            GridWH  wh;
            wh.type = GWHT_ASTERISK;

            int nXml = util::wtoi(str.c_str());
            if( nXml == 0 )  // 如果只指定 "*"，那么表示"1*"
                nXml = 1;

            wh.xml = nXml;

            this->heights.push_back( wh );
            continue;
        }

        // 判断其他情况
        GridWH  wh;
        wh.type = GWHT_VALUE;
        wh.last = util::wtoi( str.c_str() );
        this->heights.push_back( wh );

        continue;
    }
    SAFE_RELEASE(pEnum);
}
const wchar_t*  GridLayout::SaveGridHeight()
{
    String&  strBuffer = GetTempBufferString();
    wchar_t*  szBuffer = GetTempBuffer();

    int nCount = (int)heights.size();
    for (int i = 0; i < nCount; i++)
    {
        if (!strBuffer.empty())
            strBuffer.push_back(XML_SEPARATOR);

        switch (heights[i].type)
        {
        case GWHT_ASTERISK:
            {
                if (heights[i].xml <= 1)
                {
                    strBuffer.append(XML_ASTERISK);
                }
                else
                {
                    wprintf(szBuffer, TEXT("%d%s"), heights[i].xml, XML_ASTERISK);
                    strBuffer.append(szBuffer);
                }
            }
            break;

        case GWHT_AUTO:
            {
                strBuffer.append(XML_AUTO);
            }
            break;

        default://  GWHT_VALUE:
            {
                wprintf(szBuffer, TEXT("%d"), heights[i].last);
                strBuffer.append(szBuffer);
            }
            break;
        }
    }

    return strBuffer.c_str();
}
void  GridLayout::Serialize(SERIALIZEDATA* pData)
{
    {
        AttributeSerializer s(pData, TEXT("GridLayout"));

		// 解析Grid的每一列的宽度
        s.AddString(XML_LAYOUT_GRID_WIDTH, 
            Slot(&GridLayout::LoadGridWidth, this),
            Slot(&GridLayout::SaveGridWidth, this));

        s.AddString(XML_LAYOUT_GRID_HEIGHT, 
            Slot(&GridLayout::LoadGridHeight, this),
            Slot(&GridLayout::SaveGridHeight, this));
    }

    if (pData->IsLoad())
    {
        // 如果对于gridlayout没有定义layout.width，则默认layout.width="*"
        if (widths.empty())
        {
            GridWH  wh;
            wh.type = GWHT_ASTERISK;
            wh.xml = 1;
            this->widths.push_back( wh );
        }

        // 如果对于gridlayout没有定义layout.height，则默认layout.height="*"
        if (heights.empty())
        {
            GridWH  wh;
            wh.type = GWHT_ASTERISK;
            wh.xml = 1;
            this->widths.push_back( wh );
        }
    }
}


Size  GridLayout::Measure()
{
	Size size = {0,0};

	int nGridRows = (int)this->heights.size();
	int nGridCols = (int)this->widths.size();

	Object*   pChild = nullptr;
	while ((pChild = this->m_pPanel->EnumChildObject(pChild)))
	{
		if (pChild->IsSelfCollapsed())
        {
			continue;
        }

        GridLayoutParam*  pParam = s_GetObjectLayoutParam(pChild);
        if (!pParam)
        {
            continue;;
        }

		int nRow = pParam->GetRow();
        int nCol = pParam->GetCol();
        int nnGridRowspan = pParam->GetRowSpan();
        int nnGridColspan = pParam->GetColSpan();

		// 超出了范围
		if (nCol >= nGridCols || nRow >= nGridRows)
		{
			UI_LOG_WARN(L"GridLayout::MeasureChildObject, Object[ m_strID=\"%s\", col=%d, row=%d ] 超出grid范围 [col=%d,row=%d] )",
				pChild->GetId(), nCol, nRow, nGridCols, nGridRows );
			continue;
		}
 
		// 对于宽和高都属于合并单元格的对象，放弃对它的计算
		// 同时放弃对合并列或合并行的计算，但是对于一个合并行，如果它只占一列的话
		// 必须计算该列的大小
		if (nnGridRowspan != 1 && nnGridColspan != 1)
		{
				continue;
		}
	
		// 如果对象所在的行和列都有固定的值，那么不需要计算
		if (widths[nCol].type == GWHT_VALUE &&
			heights[nRow].type == GWHT_VALUE)
		{
			continue;
		}

		Size s = pChild->GetDesiredSize();
		
		// 设置对象所在列的宽度
		if (nnGridColspan == 1)
		{
			switch (widths[nCol].type)
			{
			case GWHT_VALUE:     // 该列的值是固定值，不需要修改
				break;

			case GWHT_ASTERISK:  // 该列是平分大小，在这里先取这一列的最大值，在Arrange里面会再具体赋值
				if( s.width > widths[nCol].last )
					widths[nCol].last = s.width;
				break;

			case GWHT_AUTO:      // 该列是自动大小，那么取这一列的最大值
				if (s.width > widths[nCol].last)
					widths[nCol].last = s.width;
				break;

			default:
				UIASSERT(false);
				break;
			}
		}

		//设置对象所在行的高度
		if (nnGridRowspan == 1)
		{
			switch (heights[nRow].type)
			{
			case GWHT_VALUE:     // 该行的值是固定值，不需要修改
				break;

			case GWHT_ASTERISK:  // 该行是平分大小，在这里先取这一行的最大值，在Arrange里面会再具体赋值
				if (s.height > heights[nRow].last)
					heights[nRow].last = s.height;
				break;

			case GWHT_AUTO:      // 该行是自动大小，那么去这一行的最大值
				if (s.height > heights[nRow].last)
					heights[nRow].last = s.height;
				break;

			default:
				UIASSERT(false);
				break;
			}
		}
	}// end of while( pChild != this->m_pPanel->EnumChildObject( pChild ) )

	// 必须将所有 * 类型的比例关系调整好，如 1* : 2* : 3* ，这个时候如果分别对应 5, 5, 10，
	// 那么必须安排成　5,10,15（满足最小那个）
	// 具体求法为：先分别算出比例关系：5/1 : 5/2 : 10/3 = 5 : 2.5 : 3.3
	// 取最大的那个数值，以该数值作为新的比例基数，去重新计算结果 1*5 : 2*5 : 3*5
	
	double maxRate = 0;
	// 求出比例基数
	for (int i = 0; i < nGridCols; i++)
	{
		if (this->widths[i].type == GWHT_ASTERISK)
		{
			if (widths[i].xml == 0)
				widths[i].xml = 1;

			double rate = (double)widths[i].last / (double)widths[i].xml;
			if (rate > maxRate)
				maxRate = rate;
		}
	}
	// 用求出的基数重新计算
	for (int i = 0; i < nGridCols; i++)
	{
		if (this->widths[i].type == GWHT_ASTERISK)
		{
			widths[i].last = (int)(widths[i].xml * maxRate);
		}
	}

	// 同理求出各行的高度
	maxRate = 0;
	for (int i = 0; i < nGridRows; i++)
	{
		if (this->heights[i].type == GWHT_ASTERISK)
		{
			if (heights[i].xml == 0)
				heights[i].xml = 1;

			double rate = (double)heights[i].last / (double)heights[i].xml;
			if (rate > maxRate)
				maxRate = rate;
		}
	}
	for (int i = 0; i < nGridRows; i++)
	{ 
		if (this->heights[i].type == GWHT_ASTERISK)
		{ 
			heights[i].last =(int)(heights[i].xml * maxRate);
		}
	}

	// 返回panel需要的大小
	for (int i = 0; i < nGridCols; i++)
	{
		size.width += widths[i].last;
	}
	for (int i = 0; i < nGridRows; i++)
	{
		size.height += heights[i].last;
	}

    return size;
}
void  GridLayout::DoArrage(IObject* pObjToArrage)
{
	// 调用该函数时，自己的大小已经被求出来了

	// 先求出GRID的宽度和高度，后面对各个子对象的布局都是先基于GRID的
	Rect rcClient;
	m_pPanel->GetClientRectInObject(&rcClient);
	int  nWidth  = rcClient.Width();  //m_pPanel->GetWidth();
	int  nHeight = rcClient.Height(); //m_pPanel->GetHeight();

	int nGridRows = (int)this->heights.size();  // Grid的行数
	int nGridCols = (int)this->widths.size();   // Grid的列数

	// 初始化，避免上一次的结果影响本次的布局
	for (int i = 0; i < nGridCols; i++)
	{
		if (widths[i].type == GWHT_AUTO)
		{
			widths[i].last = 0;
		}
	}
	for (int i = 0; i < nGridRows; i++)
	{
		if (heights[i].type == GWHT_AUTO)
		{
			heights[i].last = 0;
		}
	}

    // 第一步. 必须先安排auto类型
	Object*   pChild = nullptr;
	while ((pChild = this->m_pPanel->EnumChildObject(pChild)))
	{
		// 放在IsSelfCollapsed之前。editor中也需要加载隐藏对象的布局属性
		GridLayoutParam*  pParam = s_GetObjectLayoutParam(pChild);
		if (!pParam)
			continue;;

		if (pChild->IsSelfCollapsed())
			continue;

        int nRow = pParam->GetRow();
        int nCol = pParam->GetCol();
        // int nnGridRowspan = pParam->GetRowSpan();
        // int nnGridColspan = pParam->GetColSpan();

		// 超出了范围
		if (nCol >= nGridCols || nRow >= nGridRows)
		{
			UI_LOG_WARN( _T("GridLayout::ArrangeChildObject, Object[ m_strID=\"%s\", col=%d, row=%d ] 超出grid范围 [col=%d,row=%d] )"),
				pChild->GetId(), nCol, nRow, nGridCols, nGridRows );
			continue;
		}

		// 只求auto类型
		if (widths[nCol].type != GWHT_AUTO && heights[nRow].type != GWHT_AUTO)
		{
			continue ;
		}

		Size s = pChild->GetDesiredSize();

		if (widths[nCol].type == GWHT_AUTO)
		{
			if( widths[nCol].last < s.width )
				widths[nCol].last = s.width;
		}
		if (heights[nRow].type == GWHT_AUTO)
		{
			if (heights[nRow].last < s.height)
				heights[nRow].last = s.height;
		}
	}
	
	// 第二步. 计算*类型
	int nWidthRemain  = nWidth;   // 除去auto、value类型后，剩余的宽度
	int nHeightRemain = nHeight;  // 除去auto、value类型后，剩余的宽度
	int nASTERISKWidth  = 0;      // 宽度中的*的数量
	int nASTERISKHeight = 0;      // 高度中的*的数量

	for (int i = 0; i < nGridCols; i++)
	{
		if (widths[i].type != GWHT_ASTERISK)
		{
			nWidthRemain -= widths[i].last;
		}
		else
		{
			nASTERISKWidth += widths[i].xml;
		}
	}
	for (int i = 0; i < nGridRows; i++)
	{
		if (heights[i].type != GWHT_ASTERISK)
		{
			nHeightRemain -= heights[i].last;
		}
		else
		{
			nASTERISKHeight += heights[i].xml;
		}
	}

	if (nASTERISKWidth == 0)
		nASTERISKWidth = 1;
	if (nASTERISKHeight == 0)
		nASTERISKHeight = 1;

	// 开始平分(TODO. 由于整数的除法，这里可能造成几个像素的误差）
	for (int i = 0; i < nGridCols; i++)
	{
		if (widths[i].type == GWHT_ASTERISK)
		{
			widths[i].last = nWidthRemain / nASTERISKWidth * widths[i].xml;
		}
	}
	for (int i = 0; i < nGridRows; i++)
	{
		if (heights[i].type == GWHT_ASTERISK)
		{
			heights[i].last = nHeightRemain / nASTERISKHeight * heights[i].xml;
		}
	}


	// 第三步. 安排各个控件的位置
	pChild = nullptr;
	while ((pChild = this->m_pPanel->EnumChildObject(pChild)))
	{
        GridLayoutParam*  pParam = s_GetObjectLayoutParam(pChild);
        if (!pParam)
        {
            continue;;
        }

        int nRow = pParam->GetRow();
        int nCol = pParam->GetCol();
        int nnGridRowspan = pParam->GetRowSpan();
        int nnGridColspan = pParam->GetColSpan();


		// (row,col) -> ( rect.left, rect.top, rect.right, rect.bottom )
		// (0,0)   -> (0,0,widths[0],heights[0])
		// (1,1)   -> (widths[0],heights[0],widths[0]+widths[1],heights[0]+heights[1] )

		Rect  rcObjectInGrid;
		rcObjectInGrid.left   = this->getColPos( nCol );
		rcObjectInGrid.right  = this->getColPos( nCol + nnGridColspan );
		rcObjectInGrid.top    = this->getRowPos( nRow );
		rcObjectInGrid.bottom = this->getRowPos( nRow + nnGridRowspan );

		Rect rc;
		rc.CopyFrom(rcObjectInGrid);
		rc.Offset(this->m_pPanel->GetPaddingL(), this->m_pPanel->GetPaddingT());

 		Rect rcMargin;
 		pChild->GetMarginRegion(&rcMargin);
 		util::DeflatRect(&rc, &rcMargin);

        int nConfigW = pParam->GetConfigWidth();
        int nConfigH = pParam->GetConfigHeight();
        if (nConfigW >= 0)
            rc.right = rc.left + nConfigW;
        if (nConfigH >= 0)
            rc.bottom = rc.top + nConfigH;
	
		pChild->SetObjectPos(&rc, SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS|SWP_FORCESENDSIZEMSG);
	}
}

// 获取第nCol列离GRID最左侧的距离
// 注意：这里的参数可以是对象的col+colspan，因此该值可能会超出grid的最大列
int GridLayout::getColPos(unsigned int nCol)
{
	unsigned int nGridCols = (uint)widths.size();
	if (nCol >= nGridCols)
	{
	//	UI_LOG_WARN( _T("GridLayout::getColPos, nCol[%d] > widths.size[%d]"), nCol, nGridCols );
		nCol = nGridCols;
	}

	int nRet = 0;
	for (unsigned int i = 0; i < nCol; i++)
	{
		nRet += widths[i].last;
	}
	return nRet;
}
// 获取第nRow行离GRID最上面的距离
int GridLayout::getRowPos(unsigned int nRow )
{
	unsigned int nGridRows = (uint)heights.size();
	if (nRow >= nGridRows)
	{
	//	UI_LOG_WARN( _T("GridLayout::getRowPos, nCol[%d] > heights.size[%d]"), nRow, nGridRows );
		nRow = nGridRows;
	}

	int nRet = 0;
	for (unsigned int i = 0; i < nRow; i++)
	{
		nRet += heights[i].last;
	}
	return nRet;
}

GridWH*  GridLayout::GetWidth(unsigned int nIndex)
{
    if (nIndex >= widths.size())
        return nullptr;

    return &widths[nIndex];
}
GridWH*  GridLayout::GetHeight(unsigned int nIndex)
{
    if (nIndex >= heights.size())
        return nullptr;

    return &heights[nIndex];
}


void  GridLayout::ChildObjectVisibleChanged(IObject* pObj)
{
	UIASSERT(pObj);
	UIASSERT(pObj->GetParentObject());
	UIASSERT(pObj->GetParentObject()->GetImpl() == m_pPanel);

    pObj->Invalidate();
}


//////////////////////////////////////////////////////////////////////////

GridLayoutParam::GridLayoutParam()
{
    m_nConfigWidth = AUTO;
    m_nConfigHeight = AUTO;
    m_nConfigLayoutFlags = 0;

    m_nCol = m_nRow = 0;
    m_nColSpan = m_nRowSpan = 1;
}
GridLayoutParam::~GridLayoutParam()
{

}

void  GridLayoutParam::UpdateByRect()
{
    Rect  rcParent;
    m_pObj->GetParentRect(&rcParent);

    if (m_nConfigWidth >= 0)
    {
        m_nConfigWidth = rcParent.Width();
    }
    if (m_nConfigHeight >= 0)
    {
        m_nConfigHeight = rcParent.Height();
    }
}
void  GridLayoutParam::Serialize(SERIALIZEDATA* pData)
{
    AttributeSerializer s(pData, TEXT("GridLayoutParam"));
    s.AddLong(XML_WIDTH, m_nConfigWidth)->AddAlias(AUTO, XML_AUTO)->SetDefault(AUTO);
    s.AddLong(XML_HEIGHT, m_nConfigHeight)->AddAlias(AUTO, XML_AUTO)->SetDefault(AUTO);
    s.AddLong(XML_LAYOUT_GRID_COL, m_nCol);
    s.AddLong(XML_LAYOUT_GRID_ROW, m_nRow);
    s.AddLong(XML_LAYOUT_GRID_COLSPAN, m_nColSpan)->SetDefault(1);
    s.AddLong(XML_LAYOUT_GRID_ROWSPAN, m_nRowSpan)->SetDefault(1);

    s.AddFlags(XML_LAYOUT_ITEM_ALIGN, m_nConfigLayoutFlags)
        ->AddFlag(LAYOUT_ITEM_ALIGN_LEFT, XML_LAYOUT_ITEM_ALIGN_LEFT)
        ->AddFlag(LAYOUT_ITEM_ALIGN_RIGHT, XML_LAYOUT_ITEM_ALIGN_RIGHT)
        ->AddFlag(LAYOUT_ITEM_ALIGN_TOP, XML_LAYOUT_ITEM_ALIGN_TOP)
        ->AddFlag(LAYOUT_ITEM_ALIGN_BOTTOM, XML_LAYOUT_ITEM_ALIGN_BOTTOM)
        ->AddFlag(LAYOUT_ITEM_ALIGN_CENTER, XML_LAYOUT_ITEM_ALIGN_CENTER)
        ->AddFlag(LAYOUT_ITEM_ALIGN_VCENTER, XML_LAYOUT_ITEM_ALIGN_VCENTER);
}

Size  GridLayoutParam::CalcDesiredSize()
{
    Size size = {0,0};

    bool bWidthNotConfiged = m_nConfigWidth == AUTO ? true:false;
    bool bHeightNotConfiged = m_nConfigHeight == AUTO ? true:false;;

    if (bWidthNotConfiged || bHeightNotConfiged)
    {
        // 获取子对象所需要的空间
        UISendMessage(m_pObj, UI_MSG_GETDESIREDSIZE, (long)&size);

        // 如果有指定width、height的其中一个，那么忽略在上一步中得到的值
        if (this->m_nConfigWidth != AUTO)
            size.width = this->m_nConfigWidth;
        if (this->m_nConfigHeight!= AUTO)
            size.height = this->m_nConfigHeight;
    }
    else
    {
        size.width = this->m_nConfigWidth;
        size.height = this->m_nConfigHeight;
    }

    // 计算 margin 的大小
//     size.width += m_pObj->GetMarginW();
//     size.height += m_pObj->GetMarginH();

    return size;
}

bool  GridLayoutParam::IsSizedByContent()
{
    if (m_nConfigHeight != AUTO && m_nConfigWidth != AUTO)
        return false;

    // 判断该行的高是否配置了，该列的宽是否配置了
    if (!m_pObj || !m_pObj->GetParentObject())
        return true;

    ILayout* pLayout = (ILayout*)UISendMessage(m_pObj->GetParentObject(), UI_MSG_GETLAYOUT);
    if (!pLayout)
        return true;

    GridLayout*  pGridLayout = static_cast<GridLayout*>(pLayout);
    
    for (int i = 0; i < m_nRowSpan; i++)
    {
        GridWH* p = pGridLayout->GetHeight(i);
        if (!p)
            return true;

        if (p->type == GWHT_AUTO)
            return true;
    }
    for (int i = 0; i < m_nColSpan; i++)
    {
        GridWH* p = pGridLayout->GetWidth(i);
        if (!p)
            return true;

        if (p->type == GWHT_AUTO)
            return true;
    }

    return false;
}
