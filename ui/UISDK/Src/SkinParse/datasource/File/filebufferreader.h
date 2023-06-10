#pragma once
#include "Inc\Interface\iskindatasource.h"
#include <fstream>

class FileBufferReader : public IFileBufferReader
{
public:
    FileBufferReader();
    ~FileBufferReader();

public:
	static FileBufferReader* Create();
    virtual void  Release();
    virtual int   read(char* pread, int nread);
    virtual void  seek(int npos, ios_base::seek_dir dir);
    virtual int   tell();
    virtual bool  load(LPCTSTR szPath);
    virtual bool  save(LPCTSTR szPath);

private:
    fstream  f;
};