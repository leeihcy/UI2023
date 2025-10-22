#pragma once
#include "include/interface/iskindatasource.h"
#include <ios>

class ByteBufferReader : public ui::IByteBufferReader
{
  using byte = unsigned char;

public:
    ByteBufferReader();
    ~ByteBufferReader();

public:
	static ByteBufferReader*  Create();
    virtual void  Release();

    virtual int   read(char* pread, int nread);
    virtual void  seek(int npos, std::ios::seekdir dir);
    virtual int   tell();
    virtual bool  load(const byte* pdata, int nsize, bool bdeletedata);
    virtual bool  save(const char* szPath);

private:
    void  destroy();

private:
    byte*  m_pdata;
    byte*  m_pcur;
    byte*  m_pend;
    int    m_nsize;
    bool   m_bdeletedata;
};