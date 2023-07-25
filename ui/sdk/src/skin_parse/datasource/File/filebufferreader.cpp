#include "filebufferreader.h"

FileBufferReader::FileBufferReader()
{

}
FileBufferReader::~FileBufferReader()
{

}
FileBufferReader*  FileBufferReader::Create()
{
	return new FileBufferReader();
}

void  FileBufferReader::Release()
{
    delete this;
}

int   FileBufferReader::read(char* pread, int nread)
{
    return (int)f.read(pread, nread).gcount();
}
void  FileBufferReader::seek(int npos, std::ios_base::seekdir dir)
{
    f.seekg(npos, dir);
}
int  FileBufferReader::tell()
{
    return (int)f.tellg();
}
bool  FileBufferReader::load(const wchar_t* szPath)
{
    if (f.is_open())
        f.close();
    f.open(szPath, std::ios_base::in|std::ios_base::binary);

    if (f.fail())
        return false;

    return true;
}

bool  FileBufferReader::save(const wchar_t* szPath)
{
    UIASSERT(0 && _T("not implement"));
    return false;  // not implement;
}