#include "filebufferreader.h"
#include "sdk/include/macro/helper.h"

namespace ui {

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
void  FileBufferReader::seek(int npos, eSeekDir dir)
{
#if defined(OS_WIN)
    f.seekg(npos, (int)dir);
#else
    f.seekg(npos, (std::ios_base::seekdir)dir);
#endif
}
int  FileBufferReader::tell()
{
    return (int)f.tellg();
}
bool  FileBufferReader::load(const char* szPath)
{
    if (f.is_open())
        f.close();
    f.open(szPath, std::ios_base::in|std::ios_base::binary);

    if (f.fail())
        return false;

    return true;
}

bool  FileBufferReader::save(const char* szPath)
{
    UIASSERT(0 && "not implement");
    return false;  // not implement;
}

}