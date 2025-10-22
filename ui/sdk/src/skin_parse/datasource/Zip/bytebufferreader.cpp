#include "include/inc.h"
#include "bytebufferreader.h"
#include <fstream>

ByteBufferReader::ByteBufferReader()
{
    m_bdeletedata = false;
    m_pdata = nullptr;
    m_pcur = nullptr;
    m_nsize = 0;
}   
ByteBufferReader::~ByteBufferReader()
{
    destroy();
}
void  ByteBufferReader::destroy()
{
    if (m_bdeletedata)
    {
        SAFE_ARRAY_DELETE(m_pdata);
    }
    m_bdeletedata = false;
    m_pcur = nullptr;
    m_pend = nullptr;
    m_nsize = 0;
}

ByteBufferReader*  ByteBufferReader::Create()
{
	return new ByteBufferReader();
}
void  ByteBufferReader::Release()
{
    delete this;
}

int   ByteBufferReader::read(char* pread, int nread)
{
    int ncanread = (int)(m_pend - m_pcur);
    if (ncanread > nread)
        ncanread = nread;

    memcpy(pread, m_pcur, ncanread);
    m_pcur += ncanread;
    return ncanread;
}
void  ByteBufferReader::seek(int npos, std::ios::seekdir dir)
{
    switch (dir)
    {
    case std::ios::seekdir::cur:
        m_pcur += npos;
        break;

    case std::ios::seekdir::beg:
        m_pcur = m_pdata + npos;
        break;

    case std::ios::seekdir::end:
        m_pcur = m_pend - npos;
        break;
    }

    if (m_pcur < m_pdata)
        m_pcur = m_pdata;
    else if (m_pcur > m_pend)
        m_pcur = m_pend;
}

int  ByteBufferReader::tell()
{
    return (int)(m_pcur - m_pdata);
}

bool  ByteBufferReader::load(const byte* pdata, int nsize, bool bdeletedata)
{
    destroy();
    m_pcur = m_pdata = (byte*)pdata;
    m_nsize = nsize;
    m_bdeletedata = bdeletedata;
    m_pend = m_pdata + nsize;
    return true;
}

bool  ByteBufferReader::save(const char* szPath)
{
    if (!szPath)
        return false;

    std::fstream f;
    f.open(szPath, std::ios::out|std::ios::binary);
    if (f.fail())
        return false;

    f.write((const char*)m_pdata, m_nsize);
    if (f.fail())
        return false;

    f.close();
    return true;
}