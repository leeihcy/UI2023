#pragma once

namespace ui
{
    // 
	//	图片内存数据拷贝，用于皮肤改变色调时保存其原始数据，并兼容gdi/gdi+图片
	//
	class ImageData
	{
	public:
		ImageData()
		{
			m_bNeedDeletePtr = false;
			m_ptr = m_pScan0 = 0;
			m_nbpp = m_nStride = m_nWidth = m_nHeight = 0;
		}
		~ImageData()
		{
			if (m_ptr && m_bNeedDeletePtr)
				delete[] m_ptr;
			m_ptr = m_pScan0 = 0;
			m_nbpp = m_nStride = m_nWidth = m_nHeight = 0;
		}

		unsigned char*  m_ptr;         // 数据首地址，用于new/delete
		unsigned char*  m_pScan0;      // 图片第一行的数据地址
		int    m_nbpp;        // bit per pixel
		int    m_nStride;     // Offset, in bytes, between consecutive scan lines of the bitmap. If the stride is positive, the bitmap is top-down. If the stride is negative, the bitmap is bottom-up.
		int    m_nWidth;      // 图片宽度
		int    m_nHeight;     // 图片高度
		bool  m_bNeedDeletePtr;  // 是否需要释放m_ptr
	};
}