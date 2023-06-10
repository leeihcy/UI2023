#pragma once
#include "Inc\Interface\iskindatasource.h"

class ByteBufferReader : public IByteBufferReader
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
    virtual bool  save(const TCHAR* szPath);

private:
    void  destroy();

private:
    byte*  m_pdata;
    byte*  m_pcur;
    byte*  m_pend;
    int    m_nsize;
    bool   m_bdeletedata;
};