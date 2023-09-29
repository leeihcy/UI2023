#include "stdafx.h"
#include "GifImage.h"
#include "include/interface/imessage.h"
#include "include/interface/iskindatasource.h"
#include "src/SkinParse\datasource\skindatasource.h"
#include "Inc\Util\iimage.h"
#include "src/application/uiapplication.h"
#include "giftimermanager.h"

//////////////////////////////////////////////////////////////////////////
#pragma region
GIF_Frame::GIF_Frame()
{
	nRealDelayTime = 100;   // 居然还有的gif没有control块..
	pImageWrap = nullptr;
}

GIF_Frame::~GIF_Frame()
{
	SAFE_DELETE(pImageWrap);
}

UI::ImageWrap*  GIF_Frame::GetIImage()
{
	if (pImageWrap)
		return pImageWrap;

	pImageWrap = new UI::ImageWrap(&image);
	return pImageWrap;
}
#pragma endregion

//////////////////////////////////////////////////////////////////////////
#pragma region

GifImageRender::GifImageRender(GifImageBase* pGifImage, Gif_Timer_Notify* pNotify, Application*  pUIApp)
{
	m_pGifImage = pGifImage;
	m_nCurFrameIndex = 0;
	m_nDrawStatus = GIF_DRAW_STATUS_STOP;

	m_pUIApp = pUIApp;

	m_hMemCanvasDC = nullptr;
	m_hMemCanvasBitmap = nullptr;

	m_hMemPrevSaveDC = nullptr;
	m_hMemPrevSaveBitmap = nullptr;

	m_notify.eType = Gif_Timer_Notify_Direct_Hwnd;
	m_notify.notify_hwnd.hDC = nullptr;
	m_notify.notify_hwnd.hWnd = nullptr;
	m_notify.notify_hwnd.x = m_notify.notify_hwnd.y = 0;
	m_notify.notify_ui_msg.pNotifyMsgObj = nullptr;
	m_notify.notify_ui_msg.lParam = 0;

	if (pNotify)
	{
		::memcpy(&m_notify, pNotify, sizeof(Gif_Timer_Notify));

		if (m_notify.eType == Gif_Timer_Notify_Direct_Hwnd &&
            m_notify.notify_hwnd.hWnd)
        {
			m_notify.notify_hwnd.hDC = ::GetDC(m_notify.notify_hwnd.hWnd);
	    }
    }

	m_hMemCanvasDC = ::CreateCompatibleDC(nullptr);
	Image image; 
	image.Create(m_pGifImage->GetWidth(),m_pGifImage->GetHeight(), 32, Image::createAlphaChannel);
	m_hMemCanvasBitmap = image.Detach();

	/*HBITMAP hOldBmp = */(HBITMAP)::SelectObject(m_hMemCanvasDC, m_hMemCanvasBitmap);

	RECT rcBack = {0,0, m_pGifImage->GetWidth(),m_pGifImage->GetHeight()};
    if (m_pGifImage->m_hBrushTransparent)
	    ::FillRect( m_hMemCanvasDC,&rcBack,m_pGifImage->m_hBrushTransparent);
}

GifImageRender::~GifImageRender()
{
	if (m_nDrawStatus != GIF_DRAW_STATUS_STOP)
		this->Stop();

// 	if (Gif_Timer_Factory::GetGifTimerEngine(false))
// 		Gif_Timer_Factory::GetGifTimerEngine(false)->kill_timer((int)this, true);
	GifTimerManager*  pTimerMgr = m_pUIApp->GetGifTimerMgr();
	pTimerMgr->RemoveItem((int)this);

	if (m_hMemCanvasBitmap)
	{
		DeleteObject(m_hMemCanvasBitmap);
		m_hMemCanvasBitmap = nullptr;
	}
	if (m_hMemCanvasDC)
	{
		::DeleteDC(m_hMemCanvasDC);
		m_hMemCanvasDC = nullptr;
	}
	if (m_hMemPrevSaveDC)
	{
		::DeleteDC(m_hMemPrevSaveDC);
		m_hMemPrevSaveDC = nullptr;
	}
	if (m_hMemPrevSaveBitmap)
	{
		::DeleteObject(m_hMemPrevSaveBitmap);
		m_hMemPrevSaveBitmap = nullptr;
	}
	if (m_notify.notify_hwnd.hDC)
	{
		::ReleaseDC(m_notify.notify_hwnd.hWnd, m_notify.notify_hwnd.hDC);
		m_notify.notify_hwnd.hDC = nullptr;
	}
}

bool GifImageRender::ModifyRenderParam(Gif_Timer_Notify* pNotify)
{
	if (nullptr == pNotify)
		return false;

	if (m_notify.notify_hwnd.hDC)
	{
		::ReleaseDC(m_notify.notify_hwnd.hWnd, m_notify.notify_hwnd.hDC);
		m_notify.notify_hwnd.hDC = nullptr;
	}

	::memcpy(&m_notify, pNotify, sizeof(Gif_Timer_Notify));
	if (m_notify.notify_hwnd.hWnd)
		m_notify.notify_hwnd.hDC = ::GetDC(m_notify.notify_hwnd.hWnd);

	return true;
}

IGifImage*  GifImageRender::GetIGifImage()
{
	return m_pGifImage->GetIGifImage();
}

void GifImageRender::Release()
{
	if (nullptr == m_pGifImage)
	{
		delete this;
		return;
	}

	m_pGifImage->DeleteRender(this);  // 注：该调用会触发delete this.因此后面不准再添加其它代码了
	return;
}

#pragma endregion
//////////////////////////////////////////////////////////////////////////

GifImageBase::GifImageBase()
{
	m_hBrushTransparent = nullptr;
	m_nNextRenderItemIndex = 0;
	m_nImageWidth = 0;
	m_nImageHeight = 0;

    m_pIGifImage = nullptr;
    m_bCreateIGifImage = false;
}
GifImageBase::~GifImageBase()
{
    if (m_bCreateIGifImage)
    {
        SAFE_DELETE(m_pIGifImage);
    }
}

void  GifImageBase::SetIGifImage(IGifImage* p)
{
    m_pIGifImage = p;
}
IGifImage*  GifImageBase::GetIGifImage()
{
    if (m_pIGifImage)
        return m_pIGifImage;

    m_pIGifImage = new IGifImage(this);
    m_bCreateIGifImage = true;
    return m_pIGifImage;
}

GifImage::GifImage()
{
    
}
GifImage::~GifImage()
{
   this->Destroy();
}

bool GifImage::Destroy()
{
	int nSize = (int) m_vFrame.size();
	for (int i = 0; i < nSize; i++)
	{
		m_vFrame[i]->image.Destroy();
		delete m_vFrame[i];
		m_vFrame[i] = nullptr;
	}
	m_vFrame.clear();

	GifImageRenderItemMap::iterator iter = m_mapRenderItem.begin();
	GifImageRenderItemMap::iterator iterEnd = m_mapRenderItem.end();
	for (; iter != iterEnd; iter++)
	{
		GifImageRender* pItem = iter->second;
		SAFE_DELETE(pItem);
	}
	m_mapRenderItem.clear();

	if (m_hBrushTransparent)
	{
		::DeleteObject(m_hBrushTransparent);
		m_hBrushTransparent = nullptr;
	}
	return true;
}

//
// 跳过gif文件中的数据部分
//
int GifImage::skip_data_block(IStreamBufferReader*  pBuffer, byte* pBits)
{
	int nDataLength = 0;              // 统计数据的总大小
	unsigned char cNextBlockLen = 0;  // 读取下一段的数据大小
	do 
	{
		pBuffer->read((char*)&cNextBlockLen, 1);  // 本段数据长度 最大为FF，如果为0表示数据段结束
		if( 0 == cNextBlockLen )
			break;

		if (pBits)
		{
			pBuffer->read(((char*)pBits+nDataLength), cNextBlockLen);
		}
		else
		{
			pBuffer->seek(cNextBlockLen, ios_base::cur);
		}
		nDataLength += cNextBlockLen;
	} 
	while (1);
	return nDataLength;
}

//
//	组建一帧gif数据文件
//
void GifImage::build_one_frame_data(
			GIF_FileMark* pFileMark,                  // 
			GIF_LogicalScreenDescriptor* pDesc,       // 
			void* pColorTable, int nColorTableSize,   // 色彩表数据，当存在local color table时，使用local color table数据
			void* pImageData,  int nImageDataSize,    // 单帧数据
			void** ppOut, int* pOutSize               // 返回值,ppOut需要delete[]进行释放
		)
{
	assert( nullptr != pFileMark );
	assert( nullptr != pDesc );
	assert( nullptr != pImageData );
	assert( nImageDataSize > 0 );
	assert( nullptr != ppOut);
	assert( nullptr != pOutSize) ;

	int nTotalSize = sizeof(GIF_FileMark) + sizeof(GIF_LogicalScreenDescriptor) + nColorTableSize + nImageDataSize + 1;
	char* one_frame_data = new char[nTotalSize];

	int pos = 0;
	memcpy(one_frame_data+pos, pFileMark, sizeof(GIF_FileMark));
	pos += sizeof(GIF_FileMark);

	memcpy(one_frame_data+pos, pDesc, sizeof(GIF_LogicalScreenDescriptor));
	pos += sizeof(GIF_LogicalScreenDescriptor);

	if( pColorTable > 0 )
	{
		memcpy(one_frame_data+pos, pColorTable, nColorTableSize );
		pos += nColorTableSize;
	}

	memcpy(one_frame_data+pos, pImageData, nImageDataSize);
	pos += nImageDataSize;

	byte byteTrailer = GIF_BLOCK_FLAG_TRAILER;
	memcpy(one_frame_data+pos, &byteTrailer, 1 );
	pos += 1;

	assert(pos == nTotalSize);

	*pOutSize = nTotalSize;
	*ppOut = one_frame_data;
}

//
//	使用路径进行文件GIF加载（只有外部请求一人render时，才真正调用load加载文件）
//
bool GifImage::Load(const TCHAR* szPath, IMapAttribute* pMapAttrib)
{
    IStreamBufferReader*  pStreamBuffer = nullptr;

    CreateStreamBuffer(STREAM_TYPE_FILE, &pStreamBuffer);
    bool b = ((IFileBufferReader*)pStreamBuffer)->load(szPath);
    if (!b)
        return false;

    b = Load(pStreamBuffer, pMapAttrib);
    SAFE_RELEASE(pStreamBuffer);

	return b;
}

bool  GifImage::Load(IStreamBufferReader*  pBuffer, IMapAttribute* pMapAttrib)
{
#ifdef _DEBUG
    // 加载速度测试
    LARGE_INTEGER liPerFreq = {0};
    ::QueryPerformanceFrequency(&liPerFreq);
    LARGE_INTEGER liStart = {0};
    ::QueryPerformanceCounter(&liStart);

    bool bRet = RealLoad(pBuffer);

    LARGE_INTEGER liEnd = {0};
    ::QueryPerformanceCounter(&liEnd);

    int time = (int)((liEnd.QuadPart - liStart.QuadPart)*1000/liPerFreq.QuadPart);
    TCHAR szText[128] = {0};
    wprintf(szText, _T("Load gif Cost: %d ms\r\n"), time);
    ::OutputDebugString(szText);

    return bRet;
#else
    return RealLoad(pBuffer);
#endif
}

bool GifImageBase::IsLoaded()
{
	if (m_vFrame.empty())
		return false;

	return true;
}

bool GifImage::RealLoad(IStreamBufferReader*  pBuffer)
{
	UIASSERT(0 == m_vFrame.size());
    if (nullptr == pBuffer)
        return false;    

	bool bRet = false;
    int nRead = 0;
	do 
	{
		// 文件标志
		GIF_FileMark header;
		nRead = pBuffer->read((char*)&header, sizeof(GIF_FileMark));
		if (nRead < sizeof(GIF_FileMark) || !header.Valid()) 
            break;

		// 文件全局信息
		GIF_LogicalScreenDescriptor logicScreenDesc;
		nRead = pBuffer->read((char*)&logicScreenDesc, sizeof(GIF_LogicalScreenDescriptor));
        if (nRead < sizeof(GIF_LogicalScreenDescriptor))
            break;

		this->m_nImageWidth = logicScreenDesc.logical_screen_width;
		this->m_nImageHeight = logicScreenDesc.logical_screen_height;

		// 全局颜色表
		byte* pGlobalColorTable = nullptr;
		int   nGlobalColorTableSize = 0;

		if (logicScreenDesc.global_color_table_flag)
			nGlobalColorTableSize = static_cast<int>(3*pow((double)2,(int)(logicScreenDesc.size_of_global_color_table+1)));

		if (0 != nGlobalColorTableSize)
		{
			pGlobalColorTable = new byte[nGlobalColorTableSize];
			nRead = pBuffer->read((char*)pGlobalColorTable, nGlobalColorTableSize);

			// 获取背景画刷
// 			int R = pGlobalColorTable[logicScreenDesc.background_color_index*3];
// 			int G = pGlobalColorTable[logicScreenDesc.background_color_index*3+1];
// 			int B = pGlobalColorTable[logicScreenDesc.background_color_index*3+2];
//			m_hBrushTransparent = CreateSolidBrush(RGB(R,G,B));
		}

		// 循环读取每一帧的数据，重组成一幅幅单帧的gif文件，让image加载。
		GIF_Frame* pFrame = nullptr;
		int nFrameStartPos = 0;
	
		do 
		{
			BYTE bNextBlockFlag = 0;
			nRead = pBuffer->read((char*)&bNextBlockFlag, 1);
            if (nRead < 1)
                break;
            
			if (bNextBlockFlag == GIF_BLOCK_FLAG_TRAILER)
			{
				bRet = true;
				break;   // 文件结束
			}

			if (nullptr == pFrame)
			{
				pFrame = new GIF_Frame;
				nFrameStartPos = pBuffer->tell();
				nFrameStartPos -= 1;
			}

			switch (bNextBlockFlag)
			{
			case GIF_BLOCK_FLAG_EXTENSION:
				{
					BYTE bNextExtFlag = 0;
					pBuffer->read ((char*)&bNextExtFlag, 1);

					switch (bNextExtFlag)
					{
					case GIF_EXT_FLAG_PLAIN:
						{
							// 跳过 
							pBuffer->seek(13,ios_base::cur);
							skip_data_block(pBuffer);
						}
						break;
					case GIF_EXT_FLAG_CONTROL:
						{
							// 跳过
							pBuffer->read((char*)&pFrame->control, sizeof(GIF_GraphicControlExtension));
							pFrame->nRealDelayTime = pFrame->control.delay_time*10;
							if ( 0 == pFrame->nRealDelayTime )    // 有些gif该值为0
								pFrame->nRealDelayTime = 100;

							assert( pFrame->control.disposal_methold != 3 );// 这种GIF没有经没测试，没找到这样素材
						}
						break;
					case GIF_EXT_FLAG_COMMENT:
						{
							// 跳过
							skip_data_block(pBuffer);
						}
						break;
					case GIF_EXT_FLAG_APP:
						{
							// 跳过 
							pBuffer->seek(12,ios_base::cur);
							skip_data_block(pBuffer);
						}
						break;
					};
				}
				break;
			case GIF_BLOCK_FLAG_IMAGE:
				{
					pBuffer->read((char*)&pFrame->descriptor, sizeof(GIF_ImageDescriptor));

					//////////////////////////////////////////////////////////////////////////
					// 本地色彩表

					byte* pLocalColorTable = nullptr;
					int   nLocalColorTableSize = 0;

					if( pFrame->descriptor.local_color_table_flag )
						nLocalColorTableSize = static_cast<int>(3*pow((double)2,pFrame->descriptor.size_of_local_color_table+1));

					if( 0 != nLocalColorTableSize )
					{
						pLocalColorTable = new byte[nLocalColorTableSize];
						pBuffer->read((char*)pLocalColorTable, nLocalColorTableSize);
					}

					//////////////////////////////////////////////////////////////////////////
					// 解码

					byte* pColorTableForThisFrame     = pLocalColorTable==nullptr? pGlobalColorTable:pLocalColorTable;
					int   nColorTableSizeForThisFrame = pLocalColorTable==nullptr? nGlobalColorTableSize:nLocalColorTableSize;

					BYTE byte_LZW_codesize = 0;
					pBuffer->read((char*)&byte_LZW_codesize,1);   // 这个值是干什么的？  <-- 是LZW算法的初始位长度LZW code size 

					// 先尝试使用自己的解码算法
					bool bDecodeRet = this->decode_by_lzw(pBuffer, pFrame, byte_LZW_codesize, pColorTableForThisFrame, nColorTableSizeForThisFrame);
// 					if (false == bDecodeRet)
// 					{
// 						// 失败了再尝试用gdiplus的方法解码
// 						bDecodeRet = this->decode_by_gdiplus(pBuffer, pFrame, nFrameStartPos, header, logicScreenDesc, pColorTableForThisFrame, nColorTableSizeForThisFrame);
// 					}

					if( nullptr != pLocalColorTable )
					{
						delete[] pLocalColorTable;
						pLocalColorTable = nullptr;
						nLocalColorTableSize = 0;
					}

					if (false == bDecodeRet)
					{
						this->Destroy();
						goto PARSE_ERROR;
					}

					// 保存帧
					m_vFrame.push_back(pFrame);
					pFrame = nullptr;
				}
				break;

			};  // switch(bNextBlockFlag)

		} while (1);  // do

PARSE_ERROR:
		if (pFrame)  // 剩余部分没有image data了，导致还new了一次，在这里释放掉
		{
			delete pFrame;
			pFrame = nullptr;
		}

		delete[] pGlobalColorTable;
		pGlobalColorTable = nullptr;
	} 
	while (0);

	return bRet;
}

bool GifImage::decode_by_lzw(IStreamBufferReader*  pBuffer, GIF_Frame* pFrame, byte byte_LZW_codesize, byte* pColorTable, int nColorTableSize)
{
	byte*   pGifFrameImageData = nullptr;
	byte*   pOutputData = nullptr;
	int     nOutputDataSize = pFrame->descriptor.image_width*pFrame->descriptor.image_height; 

	//////////////////////////////////////////////////////////////////////////
	// 读取图像数据用于解码（先读取数据大小）

	int nDataBeginPos = pBuffer->tell();
	int nDataLength = skip_data_block(pBuffer);
    pBuffer->seek(nDataBeginPos, ios_base::beg);

	pGifFrameImageData = new byte[nDataLength];
	skip_data_block(pBuffer, pGifFrameImageData);

	//////////////////////////////////////////////////////////////////////////
	// LZW解码

	pOutputData = new byte[nOutputDataSize];
	memset(pOutputData, 0, nOutputDataSize);

	GifLZWDecoder decoder(
		byte_LZW_codesize, 
		pOutputData, 
		nOutputDataSize, 
		pFrame->descriptor.image_width,
		pFrame->descriptor.image_height);
	if (false == decoder.Decode(pGifFrameImageData, nDataLength))
	{
		SAFE_ARRAY_DELETE(pGifFrameImageData);
		SAFE_ARRAY_DELETE(pOutputData);

        pBuffer->seek(nDataBeginPos, ios_base::beg);   // 重新让gdiplus解码的位置
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 解析交叉GIF类型的数据
	
	if (pFrame->descriptor.interlace_flag)
	{
		byte*   pInterlaceData = new byte[nOutputDataSize];
		int     nWriteCursor = 0;
		int     nReadCursor = 0;
		int     nRowDataSize = pFrame->descriptor.image_width;

		// Group 1，每隔8行取一次，从第0行开始写回
		for (int row = 0; row < pFrame->descriptor.image_height; row+=8)
		{
			nWriteCursor = row*nRowDataSize;
			memcpy(pInterlaceData+nWriteCursor, pOutputData+nReadCursor, nRowDataSize);
			nReadCursor += nRowDataSize;
		}

		// Group 2，每隔8行取一次，从第4行开始写回
		for (int row = 4; row < pFrame->descriptor.image_height; row+=8)
		{
			nWriteCursor = row*nRowDataSize;
			memcpy(pInterlaceData+nWriteCursor, pOutputData+nReadCursor, nRowDataSize);
			nReadCursor += nRowDataSize;
		}

		// Group 3，每隔4行取一次，从第2行开始写回
		for (int row = 2; row < pFrame->descriptor.image_height; row+=4)
		{
			nWriteCursor = row*nRowDataSize;
			memcpy(pInterlaceData+nWriteCursor, pOutputData+nReadCursor, nRowDataSize);
			nReadCursor += nRowDataSize;
		}

		// Group 4，每隔2行取一次，从第1行开始写回
		for (int row = 1; row < pFrame->descriptor.image_height; row+=2)
		{
			nWriteCursor = row*nRowDataSize;
			memcpy(pInterlaceData+nWriteCursor, pOutputData+nReadCursor, nRowDataSize);
			nReadCursor += nRowDataSize;
		}
		SAFE_ARRAY_DELETE(pOutputData);
		pOutputData = pInterlaceData;
	}

	//////////////////////////////////////////////////////////////////////////
	// 解析调色板和透明索引

	pFrame->image.Create(pFrame->descriptor.image_width,pFrame->descriptor.image_height, 32, Image::createAlphaChannel);

	BYTE*   pBits = (BYTE*)pFrame->image.GetBits();
	int     bytesperline = 4*pFrame->descriptor.image_width;  
	int     npxIndex = 0;

	for (int row = 0; row < pFrame->descriptor.image_height; row ++ )
	{
		for( int i = 0; i < bytesperline; i += 4 )
		{
			int nColorTableIndex = pOutputData[npxIndex++];    // 该像素对应的调色板的值
			int nColorTableIndexAddr = 3*nColorTableIndex;

			if (pFrame->control.transparent_color_flag && 
				pFrame->control.transparent_color_index == nColorTableIndex)
			{

				pBits[i]   = 0;          // 注：这里必须用0，相当于alpha通道的预乘，否则image.draw将变成白底，而不是透明
				pBits[i+1] = 0;
				pBits[i+2] = 0;

				pBits[i+3] = 0;          // 该像素透明
			}
			else
			{
				pBits[i]   = pColorTable[nColorTableIndexAddr+2]; // B
				pBits[i+1] = pColorTable[nColorTableIndexAddr+1]; // G
				pBits[i+2] = pColorTable[nColorTableIndexAddr];   // R

				pBits[i+3] = 255;
			}
		}

		pBits += pFrame->image.GetPitch();
	}

#ifdef _DEBUGx // <-- 将每一帧保存为一个文件
	static int n = 0;
	TCHAR szPath[MAX_PATH] = _T("");
	wprintf(szPath, _T("C:\\one_frame\\%d.png"),n++ );
	pFrame->image.Save(szPath, Gdiplus::ImageFormatPNG);
#endif

	SAFE_ARRAY_DELETE(pGifFrameImageData);
	SAFE_ARRAY_DELETE(pOutputData);
	return true;
}

bool GifImage::decode_by_gdiplus(IStreamBufferReader*  pBuffer, 
								 GIF_Frame* pFrame, 
								 int  nFrameStartPos,
								 GIF_FileMark& header, 
								 GIF_LogicalScreenDescriptor& logicScreenDesc,
								 byte* pColorTable, 
								 int nColorTableSize )
{
	skip_data_block(pBuffer);
	int nFrameEndPos = pBuffer->tell();

	// 单帧图像数据
	int   nImageDataSize = nFrameEndPos-nFrameStartPos;
	void* pImageData = (void*)new char[nImageDataSize];

	pBuffer->seek(nFrameStartPos, ios_base::beg);
	pBuffer->read((char*)pImageData, nImageDataSize);

	// 组装
	void* one_frame_gif_file_data = nullptr;
	int   one_frame_gif_file_data_size = 0;

	// TODO: 真的有必要吗？Gdiplus内部应该会判断选哪个颜色表吧
// 	if( nullptr == pLocalColorTable )
// 		this->build_one_frame_data(&header, &logicScreenDesc, pGlobalColorTable, nGlobalColorTableSize, pImageData, nImageDataSize, &one_frame_gif_file_data, &one_frame_gif_file_data_size );
// 	else
// 		this->build_one_frame_data(&header, &logicScreenDesc, pLocalColorTable, nLocalColorTableSize, pImageData, nImageDataSize, &one_frame_gif_file_data, &one_frame_gif_file_data_size );

	this->build_one_frame_data(&header, &logicScreenDesc, pColorTable, nColorTableSize, pImageData, nImageDataSize, &one_frame_gif_file_data, &one_frame_gif_file_data_size);

	if ( nullptr != pImageData )
	{
		delete[] pImageData;
		pImageData = nullptr;
		nImageDataSize = 0;
	}

#ifdef _DEBUGx // <-- 将每一帧保存为一个文件
	static int n = 0;
	TCHAR szPath[MAX_PATH] = _T("");
	wprintf(szPath, _T("C:\\one_frame\\%d.png"),n++ );
	fstream temp;
	temp.open(szPath,ios_base::out|ios_base::binary);
	if( temp.good() )
	{
		temp.write((const char*)one_frame_gif_file_data, one_frame_gif_file_data_size);
		temp.close();
	}
#endif

	// 加载图片
	if (false == pFrame->image.LoadFromData(one_frame_gif_file_data, one_frame_gif_file_data_size))
	{
		delete [] one_frame_gif_file_data;
		one_frame_gif_file_data = nullptr;

		return false;
	}
	delete [] one_frame_gif_file_data;
	one_frame_gif_file_data = nullptr;


	// 设置背景透明色
	if (pFrame->control.transparent_color_flag)
	{
		if (false == this->decode_gif_image_transparent(pFrame, (LONG)pFrame->control.transparent_color_index))
			return false;

		//pFrame->image.SetTransparentColor((LONG)pFrame->control.transparent_color_index);
	}
	return true;
}

int GifImageRender::get_next_frame_index()
{
	int nIndex = m_nCurFrameIndex;
	m_nCurFrameIndex++;
	if (m_nCurFrameIndex >= (int)m_pGifImage->m_vFrame.size())
		m_nCurFrameIndex = 0;

	return nIndex;
}
GIF_Frame* GifImageBase::GetFrame(int nIndex)
{
	if( nIndex < 0 || nIndex >= (int)m_vFrame.size() )
		return nullptr;

	return m_vFrame[nIndex];
}

Image*  GifImageBase::GetFrameImage( int nIndex )
{
	GIF_Frame* pFrame = GetFrame(nIndex);
	if (!pFrame)
		return nullptr;

	return &pFrame->image;
}

ImageWrap*  GifImageBase::GetFrameIImage( int nIndex )
{
	GIF_Frame* pFrame = GetFrame(nIndex);
	if (!pFrame)
		return nullptr;

	return pFrame->GetIImage();
}

//
//	设置绘制参数。在显示前必须调用该函数 
//
//	Return
//		成功返回TRUE，失败返回FALSE
//
GifImageRender* GifImageBase::AddRender(Gif_Timer_Notify* pNotify, Application*  pUIApp, int* pIndex)
{
	if (nullptr == pNotify)
		return nullptr;

	if (0 == m_mapRenderItem.size())
	{
        // 		if (false == this->RealLoad())   // 只有在真正有人请求一个Render时，才创建
        // 			return nullptr;
        // 

        if (nullptr == m_hBrushTransparent)
            this->SetTransparentColor();
    }

	if (m_vFrame.size() <= 0)
		return nullptr;

	GifImageRender* pDrawItem = new GifImageRender(this, pNotify, pUIApp);
	m_mapRenderItem.insert(make_pair(m_nNextRenderItemIndex, pDrawItem));
	
	if (pIndex)
	{
		*pIndex = m_nNextRenderItemIndex;
	}
	m_nNextRenderItemIndex++;

	return pDrawItem;
}
bool GifImageBase::ModifyRender(Gif_Timer_Notify* pNotify, int nIndex)
{
	if (m_vFrame.size() <= 0 || nullptr == pNotify)
		return false;

	GifImageRender* pItem = this->GetDrawItemByIndex(nIndex);
	if (nullptr == pItem)
		return false;

	return pItem->ModifyRenderParam(pNotify);
}

bool GifImageBase::DeleteRender(int nIndex)
{
	GifImageRender* pItem = this->GetDrawItemByIndex(nIndex);
	if (nullptr == pItem)
		return false;

	m_mapRenderItem.erase(nIndex);
	SAFE_DELETE(pItem);

	if (m_mapRenderItem.size() == 0)
	{
		this->Destroy();
	}
	return true;
}
void GifImageBase::DeleteRender(GifImageRender*  pItem)
{
	GifImageRenderItemMap::iterator iter = m_mapRenderItem.begin();	
	GifImageRenderItemMap::iterator iterEnd = m_mapRenderItem.end();

	for (; iter != iterEnd; iter++)
	{
		if (iter->second == pItem)
		{
			m_mapRenderItem.erase(iter);
			break;
		}
	}
	SAFE_DELETE(pItem);

	if (m_mapRenderItem.size() == 0)
	{
		this->Destroy();
	}
}

bool GifImageBase::SetTransparentColor(COLORREF colTransparent)
{
	SAFE_DELETE_GDIOBJECT(m_hBrushTransparent);
	m_hBrushTransparent = ::CreateSolidBrush(colTransparent);

	return true;
}


GifImageRender*  GifImageBase::GetDrawItemByIndex(int nIndex)
{
	if (-1 == nIndex)
		nIndex = 0;

	GifImageRenderItemMap::iterator iter= m_mapRenderItem.find(nIndex);
	if (iter == m_mapRenderItem.end())
		return nullptr;
	else
		return iter->second;
}

int GifImageRender::GetWidth()
{
	if (nullptr == m_pGifImage)
		return 0;

	return m_pGifImage->GetWidth();
}
int GifImageRender::GetHeight()
{
	if (nullptr == m_pGifImage)
		return 0;

	return m_pGifImage->GetHeight();
}

//
//	TODO: 如何实现立即显示第一帧数据
//
void GifImageBase::Start(int nIndex)
{
	GifImageRender* pItem = this->GetDrawItemByIndex(nIndex);
	if (nullptr == pItem)
		return;
	
	pItem->Start();
}
void GifImageRender::Start()
{
	int nSize = (int)m_pGifImage->m_vFrame.size();
	if (nSize <= 0)
		return ;

	if (m_nDrawStatus == GIF_DRAW_STATUS_START)
		return;

	m_nDrawStatus = GIF_DRAW_STATUS_START;
	if (nSize == 1)    // 单帧gif图片不需要使用计时器
	{
		handle_disposal(nullptr);                           // 刷背景
		draw_frame( m_pGifImage->GetFrame(m_nCurFrameIndex));   // 绘制第一帧到m_hMemCanvasDC中
		commit(m_notify.notify_hwnd.hDC, m_notify.notify_hwnd.x,m_notify.notify_hwnd.y);
	}
	else
	{
		Gif_TimerItem timer_item = { (int)this, ::GetTickCount(), 0, -1, this };  // -1无限循环
		GifTimerManager*  pTimerMgr = m_pUIApp->GetGifTimerMgr ();
		pTimerMgr->AddItem(&timer_item);
	//	Gif_Timer_Factory::CreateGifTimerEngine()->set_timer(timer_item);

	}
}

void GifImageBase::Pause(int nIndex)
{
	if (m_vFrame.size() <= 0)
		return ;

	GifImageRender* pItem = this->GetDrawItemByIndex(nIndex);
	if (nullptr == pItem)
		return;

	pItem->Pause();
}
void GifImageRender::Pause()
{
	m_nDrawStatus = GIF_DRAW_STATUS_PAUSE;
}
void GifImageBase::Stop(int nIndex)
{
	if (m_vFrame.size() <= 0)
		return ;

	GifImageRender* pItem = this->GetDrawItemByIndex(nIndex);
	if (nullptr == pItem)
		return;

	pItem->Stop();
}
void GifImageRender::Stop()
{
	// 刷新父窗口，这里需要注意进行同步，线程中的最后一次on_timer可能将下面的刷背景又覆盖了
	m_nCurFrameIndex = 0;
	m_nDrawStatus = GIF_DRAW_STATUS_STOP;

	handle_disposal(nullptr);  // 刷背景
	this->commit(m_notify.notify_hwnd.hDC, m_notify.notify_hwnd.x,m_notify.notify_hwnd.y);
}

//
//	外部窗口调用
//
void GifImageBase::OnPaint(HDC hDC, int nIndex)
{
	GifImageRender* pItem = this->GetDrawItemByIndex(nIndex);
	if (nullptr == pItem)
		return;

	pItem->OnPaint(hDC, 0, 0);
}

//
//	UI控件调用，x,y为控件内部坐标，如(0,0)
//
void GifImageBase::OnPaint(HDC hDC, int x, int y, int nIndex)
{
	GifImageRender* pItem = this->GetDrawItemByIndex(nIndex);
	if (nullptr == pItem)
		return;

	pItem->OnPaint(hDC, x, y);
}
void GifImageRender::OnPaint(HDC hDC, int x, int y)
{
	if (GIF_DRAW_STATUS_STOP == m_nDrawStatus)
		return;

	::BitBlt(hDC, x, y, m_pGifImage->GetWidth(), m_pGifImage->GetHeight(),m_hMemCanvasDC,0,0,SRCCOPY);
}

void GifImageRender::OnAlphaPaint(HDC hDC, int x, int y)
{
    if (GIF_DRAW_STATUS_STOP == m_nDrawStatus)
        return;

    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    ::AlphaBlend( hDC, 
        x, y, m_pGifImage->GetWidth(), m_pGifImage->GetHeight(),
        m_hMemCanvasDC, 0, 0, m_pGifImage->GetWidth(), m_pGifImage->GetHeight(), bf);
}
void GifImageRender::OnAlphaPaint(HDC hDC, int x, int y, int cx, int cy)
{
	if (GIF_DRAW_STATUS_STOP == m_nDrawStatus)
		return;

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	::AlphaBlend( hDC, 
		x, y, cx, cy,
		m_hMemCanvasDC, 
		0, 0, m_pGifImage->GetWidth(), m_pGifImage->GetHeight(), bf);
}

// 该函数可能在gif timer线程被调用
void GifImageRender::commit(HDC hDC, int x, int y)
{
	switch (m_notify.eType)
	{
	case Gif_Timer_Notify_Direct_Hwnd:
		if (hDC)
		{
			::BitBlt(hDC, x, y, 
				m_pGifImage->GetWidth(), 
				m_pGifImage->GetHeight(),
				m_hMemCanvasDC,
				0,0,SRCCOPY);
		}
		break;
	case Gif_Timer_Notify_Send_Msg:
		{		
			UIMSG msg;
			msg.message = UI_MSG_GIFFRAME_TICK;
			msg.wParam = (WPARAM)this;
			msg.lParam = m_notify.notify_ui_msg.lParam;
			msg.pMsgTo = m_notify.notify_ui_msg.pNotifyMsgObj;
			UISendMessage(&msg);
		}
		break;
	case Gif_Timer_Notify_Function:
		{
			if (m_notify.notify_function.pfnCallback)
			{
				m_notify.notify_function.pfnCallback(
					m_notify.notify_function.lParam);
			}
		}
		break;
	}
}

GIF_DRAW_STATUS GifImageBase::GetStatus(int nIndex)
{
	GifImageRender* pItem = this->GetDrawItemByIndex(nIndex);
	if (nullptr == pItem)
		return GIF_DRAW_STATUS_STOP;

	return pItem->GetStatus();
}
GIF_DRAW_STATUS GifImageRender::GetStatus()
{
	return m_nDrawStatus; 
}

//
//	当一帧绘制完成后，并到达delay时间，将要绘制下一帧时，处理它的disposal
//
void GifImageRender::handle_disposal(GIF_Frame* pFrame)
{
	// 当pFrame为空时，默认就刷新整个图片的背景
	int  nDisposal = GIF_DISPOSAL_RESTORE_BACKGROUND;
	RECT rcBack = {0,0, m_pGifImage->GetWidth(),m_pGifImage->GetHeight()};

	if (pFrame)
	{
		nDisposal = pFrame->control.disposal_methold;
		::SetRect(&rcBack, 
			pFrame->descriptor.image_left_position,
			pFrame->descriptor.image_top_position,
			pFrame->descriptor.image_left_position+pFrame->descriptor.image_width,
			pFrame->descriptor.image_top_position+pFrame->descriptor.image_height);
	}

	switch (nDisposal)
	{
	case GIF_DISPOSAL_NULL:
	case GIF_DISPOSAL_LEFT:
		break;

	case GIF_DISPOSAL_RESTORE_BACKGROUND:
		{
            if (m_pGifImage->m_hBrushTransparent)
			    ::FillRect(m_hMemCanvasDC, &rcBack, m_pGifImage->m_hBrushTransparent);
		}
		break;
	case GIF_DISPOSAL_RESTORE_PREVIOUS:
		{
			// 还原
			if (m_hMemPrevSaveDC)
				::BitBlt(m_hMemCanvasDC,0,0,m_pGifImage->GetWidth(),m_pGifImage->GetHeight(),m_hMemPrevSaveDC,0,0,SRCCOPY);
		}
		break;
	}
}
//
//	绘制一帧图像
//
//	[parameter]
//		pFrame
//			[in]	要绘制的当前帧
//
void GifImageRender::draw_frame(GIF_Frame* pFrame)
{
	if (nullptr == pFrame)
		return;

	switch (pFrame->control.disposal_methold)
	{
	case GIF_DISPOSAL_RESTORE_PREVIOUS:
		if (nullptr == m_hMemPrevSaveDC)  // 在这里进行m_hMemPrevSaveBitmap的创建
		{
			m_hMemPrevSaveDC =::CreateCompatibleDC(nullptr);
			Image image;
			image.Create(m_pGifImage->GetWidth(),m_pGifImage->GetHeight(), 32, Image::createAlphaChannel);
			m_hMemPrevSaveBitmap = image.Detach();

			/*HBITMAP hOldBmp = */(HBITMAP)::SelectObject(m_hMemPrevSaveDC, m_hMemPrevSaveBitmap);
		}
		// 备份
		::BitBlt(m_hMemPrevSaveDC,0,0,m_pGifImage->GetWidth(),m_pGifImage->GetHeight(),m_hMemCanvasDC,0,0,SRCCOPY);
		break;

	case GIF_DISPOSAL_LEFT: 
		break;
	}
#ifdef _DEBUGx // <-- 将每一帧保存为一个文件
	static int n = 0;
	TCHAR szPath[MAX_PATH] = _T("");
	wprintf(szPath, _T("C:\\one_frame\\%d_a.png"),++n );
	 
	ImageWrap image;
	image.Attach(m_hMemCanvasBitmap, true);
	image.SaveAsPng(szPath);
	image.Detach();
	SelectObject(m_hMemCanvasDC, m_hMemCanvasBitmap);
#endif
 	pFrame->image.Draw( m_hMemCanvasDC, 
		pFrame->descriptor.image_left_position,
 		pFrame->descriptor.image_top_position,
		pFrame->descriptor.image_width,
		pFrame->descriptor.image_height,
// 		pFrame->descriptor.image_left_position,
// 		pFrame->descriptor.image_top_position,
		0,0,
		pFrame->descriptor.image_width,
		pFrame->descriptor.image_height);

#ifdef _DEBUGx // <-- 将每一帧保存为一个文件
	wprintf(szPath, _T("C:\\one_frame\\%d_b.png"),n );
	image.Attach(m_hMemCanvasBitmap, true);
	image.SaveAsPng(szPath);
	image.Detach();
	SelectObject(m_hMemCanvasDC, m_hMemCanvasBitmap);

#endif
}

void GifImageRender::on_tick(Gif_TimerItem* pTimerItem)  
{
	if (m_nDrawStatus != GIF_DRAW_STATUS_START)  
	{
		pTimerItem->nRepeat = 1;  // 暂停或停止状态下，在check_timer中将自动删除
		return;
	}
	if (m_nDrawStatus != GIF_DRAW_STATUS_START)
	{
		GifTimerManager*  pTimerMgr = m_pUIApp->GetGifTimerMgr ();
		pTimerMgr->RemoveItem((int)this);
		return;
	}
	GIF_Frame* pPrevFrame = m_pGifImage->GetFrame(m_nCurFrameIndex-1);  // 当0 == m_nCurFrameIndex时将返回nullptr，这时调用handle_disposal即可刷新整个背景
	this->handle_disposal(pPrevFrame);

	GIF_Frame* pFrame = m_pGifImage->GetFrame(m_nCurFrameIndex);
	draw_frame(pFrame);
	this->commit(m_notify.notify_hwnd.hDC, m_notify.notify_hwnd.x,m_notify.notify_hwnd.y);

	// 为了避免频繁的更新列表，在这里每次仅更新Gif_timer_item里面的数据，而不是删除再添加一个
	int nNextFrameIndex = get_next_frame_index();
	if (nNextFrameIndex < (int) m_pGifImage->m_vFrame.size())
	{
		GIF_Frame* pFrame = m_pGifImage->m_vFrame[nNextFrameIndex];

		if (pFrame)
		{
			pTimerItem->update_repeat(); 
			pTimerItem->nWait = pFrame->nRealDelayTime;
		}
	}
}

// <<--- 由于gdiplus加载单帧GIF出现透明色的值被改变的问题，现在已改用自己的LZW算法来解码
//
//	解析gif image中的透明信息
//
//	由于gif的透明是通过指定调色板中的一个索引来实现，而不能通过判断和透明色相等来实现
//	因为背景色可能和透明色为相同的颜色
//
//	备注:
//		image.GetWidth和image.GetHeight并不真实。真实的数据宽和高要读取LogicalScreenDescriptor中的width/height
//
bool GifImage::decode_gif_image_transparent(GIF_Frame* pFrame, int nTransparentIndex)
{
	if (nullptr == pFrame)
		return false;

	int nDestRowBytes = 4*pFrame->descriptor.image_width;
	int nDestWidth = pFrame->descriptor.image_width;
	int nDestHeight= pFrame->descriptor.image_height;

	BITMAPINFOHEADER bmih;
	::ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biBitCount = 32;
	bmih.biWidth =  nDestWidth;
	bmih.biHeight = nDestHeight * -1;
	bmih.biCompression = BI_RGB;
	bmih.biPlanes = 1;
	BYTE* lpBits = nullptr;
	
	HBITMAP hBitmap = ::CreateDIBSection(nullptr,(BITMAPINFO*)&bmih, DIB_RGB_COLORS, (void**)&lpBits, nullptr, 0 );
	if (nullptr == hBitmap || nullptr == lpBits)
		return false;

	RGBQUAD palettes[255];  // 取最大可能的大小
	pFrame->image.GetColorTable(0,255, palettes);

	BYTE*       pDestBits = lpBits;// + (pFrame->descriptor.image_height-1)*nDestRowBytes;  // 切换到最后一行，因为DIB是由下到上的（坐标顺序）
	const BYTE* pSrcBits  = ((const BYTE*)pFrame->image.GetBits()) + pFrame->descriptor.image_top_position*pFrame->image.GetPitch();

	for (int row = 0; row < nDestHeight; row ++ )
	{
		for (int i=0,j=0; i<nDestRowBytes; i+=4,j++)
		{
			BYTE index = pSrcBits[j+pFrame->descriptor.image_left_position];  // 取出这个位置的调色板索引值
			if (index >= nTransparentIndex)
			{
				pDestBits[i] = pDestBits[i+1] = pDestBits[i+2] = pDestBits[i+3]	= 0; // 将alpha置为0，透明掉
			}
			else
			{
				RGBQUAD& p = palettes[index];
				pDestBits[i]   = p.rgbBlue;
				pDestBits[i+1] = p.rgbGreen;
				pDestBits[i+2] = p.rgbRed;
				pDestBits[i+3] = 255;
			}
		}
		pDestBits += nDestRowBytes;
		pSrcBits  += pFrame->image.GetPitch();
	}

	pFrame->image.Destroy();
	pFrame->image.Attach(hBitmap);
	pFrame->image.ForceUseAlpha();
	
	return !pFrame->image.IsNull();
}


//////////////////////////////////////////////////////////////////////////
//
//  LZW 解码 GIF 数据
//
//////////////////////////////////////////////////////////////////////////

GifLZWDecoder::GifLZWDecoder(byte nInitBitLength, byte* pDecodeResultData, int nDecodeResultSize, int nwidth, int nheight)
{
	memset(m_dict, 0, sizeof(DictItem)*4096);
	m_nInitBitLength   = nInitBitLength; 
	m_nCurBitLength    =  m_nInitBitLength+1;

	GIF_LZW_CLEAN_TAG = 1 << m_nInitBitLength;
	GIF_LZW_END_TAG   = GIF_LZW_CLEAN_TAG+1;

	m_pResultData = pDecodeResultData;
	m_nResultDataSize = nDecodeResultSize;

	m_nWidth = nwidth;
	m_nHeight = nheight;
}


//
// 从当前数据流中读取下一个要进行运行的数字。
//
// 备注：
//	1. LZW是基于位的，不是基于字节的
//  2. LZW的位是变长的，不固定，由 lzw code size ~ 12，每当字典中的项上一个数量级时，取的位数加1，加到12后返回lzw code size
//  3. 例如  58         C1         05         D3         ， 当前nReadBitPosInByte=3，即01011,000这里，m_nCurBitLength=11
//          01011000   11000001   00000101   11010011
//
//     则下一个取的数为 58中的前5位加上C1中的后6位，000001 01011 ->43, nReadBitPosInByte=6
//     然后下一个取的数为 C1的前2两加上整个8位的05，还要加上D3的最低位， 1 00000101 11 -> 1047，整个跨度了3个字节才取到了需要的数字
//     因此下面代码中的 (*((int*)pDataCur) 就是先取出四个字节来进行移位/与操作。这里不能使用 byte或者WORD来代替
//
#define GET_NEXT_VALUE() \
	((*((int*)pDataCur))>>nReadBitPosInByte) & ((1<<m_nCurBitLength)-1); \
	nReadBitPosInByte += m_nCurBitLength;              \
	pDataCur += nReadBitPosInByte>>3;                  \
	nReadBitPosInByte %= 8;

// LZW算法解码
bool  GifLZWDecoder::Decode(const byte* pSrcData, int nSrcDataSize)
{
#if 0 // 这块算法有问题，某些图片解析不出来。2018/3/2，不再使用.
	// http://rsb.info.nih.gov/ij/plugins/download/agr/Animated_Gif_Reader.java
	int  code = 0;
	int  incode = 0;
	
	const byte* p = nullptr;
	int  datum = 0;
	int  bits = 0;
	int  avail = GIF_LZW_CLEAN_TAG + 2;
	int  oldcode = -1;
	int  codesize = m_nInitBitLength + 1;
	int  codemask = (1 << codesize) - 1;
	byte firstchar = 0;
	int  stackp = 0;

	int  nResultDataIndex = 0;

    for (int i = 0; i < GIF_LZW_CLEAN_TAG; i++)
    {
        m_dict[i].suffix = (byte)i;
    }

 	for (p = pSrcData-1; nResultDataIndex < m_nResultDataSize; )
 	{
		if (stackp == 0)
		{
			p++; 
 			if (bits < codesize)
 			{
				datum += ((int)*p) << bits;
				bits += 8;
				
				if (bits < codesize)
					continue;
			}
			code = datum & codemask;
			datum >>= codesize;
			bits -= codesize;
 
			if (code > avail || code == GIF_LZW_END_TAG)
			{
				goto out_of_for;
			}

			if (code == GIF_LZW_CLEAN_TAG)
			{
				codesize = m_nInitBitLength + 1;
				codemask = (1 << codesize) - 1;
				avail = GIF_LZW_CLEAN_TAG + 2;
				oldcode = -1;
				continue;
			}
 
			if (oldcode == -1)
			{
				//m_dict[stackp++].stack = m_dict[code].suffix;
				m_pResultData[nResultDataIndex++] = m_dict[code].suffix;
				oldcode = code;
				firstchar = (byte)code;
				continue;
			}
 
			incode = code;
			if (code == avail)
			{
				m_dict[stackp++].stack = firstchar;
				code = oldcode;
			}
			while (code > GIF_LZW_CLEAN_TAG)
			{
				m_dict[stackp++].stack = m_dict[code].suffix;
				code = m_dict[code].prefix;
			}
			firstchar = ((int)m_dict[code].suffix) & 0xff;

			if (avail >= 4097)
				break;

			m_dict[stackp++].stack = firstchar;
			m_dict[avail].prefix = (WORD)oldcode;
			m_dict[avail].suffix = firstchar;
			avail++;

			if (((avail & codemask) == 0) && (avail < 4097))
			{
				codesize++;
				codemask += avail;
			}
			oldcode = incode;
 		}
		stackp--;
		m_pResultData[nResultDataIndex++] = m_dict[stackp].stack;
 	}

out_of_for:
	return true;

#else

	int nRetSize = 0;

	// 注意：LZW是按位读取的数据，并不是按字节读取的
	const byte*  pDataEnd = pSrcData+nSrcDataSize;
	const byte*  pDataCur = pSrcData;
	byte  nReadBitPosInByte = 0;      // 当前读取pData字节的哪一位了

	WORD  wPrefix = 0, wSuffix = 0;

	wPrefix = GET_NEXT_VALUE();
	do 
	{
		if (wPrefix == GIF_LZW_END_TAG)   // 结束
			break;

		// 在实际的解码中, 第一个数据往往就是CLEAN标志, 这是为了算法而优化的设计,这样我们能直接进入解码循环,而不必在循环外部初始化.
		// : 的确，对于一个7位长的LZW，读取到的第一个数据真的是0x80 = 128
		if (wPrefix == GIF_LZW_CLEAN_TAG) // 重新初始化，这也是数据的第一个字节
		{
			memset(m_dict, 0, sizeof(DictItem)*4096);
			m_nDictLower      = (1<<m_nInitBitLength)+2;
			m_nDictUpper      =  m_nDictLower;
			m_nCurBitLength   =  m_nInitBitLength+1;
			m_nCurBitLengthEx = (1<<m_nCurBitLength)-1;

			wPrefix = GET_NEXT_VALUE();
			continue;
		}

		wSuffix = GET_NEXT_VALUE();

		if (wSuffix>m_nDictUpper)
		{
			UI_LOG_ERROR(_T("gif decode failed. wSuffix=%d, m_nDictUpper=%d"), wSuffix, m_nDictUpper);
			return false;
		}

		// 为什么在这里要先存入一个前缀？这是为了解决如当第一个要push字典项为 dict[82] = {7F,82}时，
		// 82这个字典项根本就不存在，不能用自己来定义自己。 因此先写入 dict[82]={7F, }; suffix先不写入。
		// 这个时候能够获取到dict[82].prefix了。
		m_dict[m_nDictUpper].prefix = wPrefix;

		// 后缀必须是一个单独的字符
		WORD wSingleSuffix = wSuffix;
		while (wSingleSuffix > GIF_LZW_CLEAN_TAG)
			wSingleSuffix = m_dict[wSingleSuffix].prefix;
		
		Output(wPrefix);

		if (m_nDictUpper == wSuffix ||          // <--这是什么意思？当出现dict[82]={7F,82}这种自己定义自己时，必须直接将这项放入字典，而不是再检查一下CheckExist{7F,7F}是否存在。如果不存在则侥幸过关，如果存在，则82这一项则被丢掉了，导致下一次循环时dict[82].prefix=82, output(82)将死循环。
			false==CheckExist(wPrefix,wSingleSuffix))
		{
			PushDict(wPrefix, (byte)wSingleSuffix);   // 存入字典
		}
		wPrefix = wSuffix;

	} while (pDataEnd>pDataCur);

	UIASSERT(m_nResultDataSize==0);   // 保证将压缩的LZW数据解压缩为图片大小
	return m_nResultDataSize == 0;

#endif
}

// 检查prefix suffix是否在字典中存在
inline bool GifLZWDecoder::CheckExist(WORD wValue1, WORD wValue2)
{
	for (int i = m_nDictLower; i < m_nDictUpper; i++)
	{
		if (m_dict[i].prefix == wValue1 && m_dict[i].suffix == wValue2)
		{
			return true;
		}
	}
	return false;
}

// 输出一个有效结果。如果w值仍然是一个字典项，继续搜索字典。
// TODO: 将递归调用优化成循环调用
inline void GifLZWDecoder::Output(WORD w)
{
	if(w > GIF_LZW_CLEAN_TAG)
	{
		Output(m_dict[w].prefix);
		Output(m_dict[w].suffix);
	}
	else
	{
		*m_pResultData = (byte)w;
		m_pResultData++;
		m_nResultDataSize--;

#ifdef _DEBUG
// 		 	TCHAR szInfo[16];
// 		 	wprintf(szInfo, _T("%d(%02X) "),w,w);
// 		 	::OutputDebugString(szInfo);
#endif
	}
}

// 向字典中添加一项
inline void GifLZWDecoder::PushDict(WORD wPrefix, byte wSuffix)
{
	m_dict[m_nDictUpper].suffix = wSuffix;

	// GIF规范规定的是12位，超过12位的表达范围就推倒重来，并且GIF为了提高压缩率，采用的是变长的字长。
	// 比如说原始数据是8位，那么一开始，先加上一位再说，开始的字长就成了9位，然后开始加标号，当标号加到512时，
	// 也就是超过9为所能表达的最大数据时，也就意味着后面的标号要用10位字长才能表示了，那么从这里开始，
	// 后面的字长就是10位了。依此类推，到了2^12也就是4096时，在这里插一个清除标志，从后面开始，从9位再来。

	if (m_nDictUpper >= m_nCurBitLengthEx)
	{
		m_nCurBitLength++;
		if (m_nCurBitLength>12)
			m_nCurBitLength = 12;
		else
			m_nCurBitLengthEx = (1<<m_nCurBitLength)-1;
	}

	m_nDictUpper++;
}

