#pragma once
#include "include/interface/iskindatasource.h"

class ByteBufferReader : public ui::IByteBufferReader
{
public:
    ByteBufferReader();
    ~ByteBufferReader();

public:
	static ByteBufferReader*  Create();
    virtual void  Release();

    virtual int   read(char* pread, int nread);
    virtual void  seek(int npos, ios_base::seek_dir dir);
    virtual int   tell();
    virtual bool  load(const byte* pdata, int nsize, bool bdeletedata);
    virtual bool  save(const wchar_t* szPath);

private:
    void  destroy();

private:
    byte*  m_pdata;
    byte*  m_pcur;
    byte*  m_pend;
    int    m_nsize;
    bool   m_bdeletedata;
};