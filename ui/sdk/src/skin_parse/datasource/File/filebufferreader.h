#pragma once
#include "include/interface/iskindatasource.h"
#include <fstream>

class FileBufferReader : public ui::IFileBufferReader
{
public:
    FileBufferReader();
    ~FileBufferReader();

public:
	static FileBufferReader* Create();
    virtual void  Release();
    virtual int   read(char* pread, int nread);
    virtual void  seek(int npos, std::ios_base::seekdir dir);
    virtual int   tell();
    virtual bool  load(const char* szPath);
    virtual bool  save(const char* szPath);

private:
    std::fstream  f;
};