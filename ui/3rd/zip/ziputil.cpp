#include "stdafx.h"
#include "ziputil.h"
#include <shlobj.h>

bool Unzip2Dir(LPCTSTR szZipFile, LPCTSTR szDir)
{
	if (!PathIsDirectory(szDir))
		return false;

    HZIP hz = OpenZip(szZipFile, 0);
	if (!hz)
		return false;
	
	ZIPENTRY ze;
	GetZipItem(hz, -1, &ze);
	int numitems = ze.index;

	TCHAR szSafeDir[MAX_PATH] = _T("");
	_tcscpy(szSafeDir, szDir);

	int nLength = (int)_tcslen(szSafeDir);
	if (szSafeDir[nLength-1] != _T('\\'))
	{
		_tcscat(szSafeDir, _T("\\"));
		nLength++;
	}

	for (int zi=0; zi<numitems; zi++)
	{  
		ZIPENTRY ze; 
		GetZipItem(hz,zi,&ze);  // fetch individual details

		_tcscpy(szSafeDir+nLength, ze.name);
		UnzipItem(hz, zi, szSafeDir);  // e.g. the item's name.
	}
	CloseZip(hz); 
	return true;
}


void zipDir(String strDir, String strDirZip, HZIP hzip)
{
	WIN32_FIND_DATA  finddata;

	String strFind = strDir;
	strFind.append(_T("*.*"));
	HANDLE hFind=::FindFirstFile(strFind.c_str(), &finddata);
	if (INVALID_HANDLE_VALUE == hFind)
		return;

	do
	{
		String strFile = finddata.cFileName;

		String strZipFile(strDirZip);
		strZipFile.append(strFile);
		String strLocalFile(strDir);
		strLocalFile.append(strFile);

		if(finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strFile.length() > 0 && strFile[0] == TEXT('.'))
				continue;

			String strDirZip2(strDirZip);
			strDirZip2.append(strFile);
			strDirZip2.append(TEXT("\\"));

			String strDir2(strDir);
			strDir2.append(strFile);
			strDir2.append(TEXT("\\"));

			zipDir(strDir2, strDirZip2, hzip);
			continue;
		}

		ZipAdd(hzip, strZipFile.c_str(), strLocalFile.c_str());
	}
	while(FindNextFile(hFind,&finddata));

	FindClose(hFind);
}

bool ZipDir(LPCTSTR szDir, LPCTSTR szZipPath)
{
	if (!szDir)
		return false;

	String strDir(szDir);
	if (strDir.empty())
		return false;;

	if (strDir[strDir.length()-1] != TEXT('\\'))
		strDir.append(TEXT("\\"));

	if (!PathFileExists(strDir.c_str()))
		return false;

	// 确保目录存在
	{
		LPCTSTR szZipDir = _tcsrchr(szZipPath, TEXT('\\'));
		if (szZipDir)
		{
			String strZipDir;
			strZipDir.assign(szZipPath, szZipDir-szZipPath);
			SHCreateDirectory(nullptr, strZipDir.c_str());
		}
	}
	HZIP hzip = CreateZip(szZipPath,0);
	if (!hzip)
		return false;

	zipDir(strDir, String(), hzip);

	CloseZip(hzip);
	return true;
}

// void CZIPUtil::GetAllZipPath(CString strSourcePath, CStringList& strZipList)
// {
// 	DWORD dwAttributes = ::GetFileAttributes(strSourcePath);
// 	BOOL bIsADreictory =(dwAttributes == FILE_ATTRIBUTE_DIRECTORY);
// 	CFileFind fileFind;
// 	CString fileName = "";
// 
// 	//不是一个目录，返回
// 	if (bIsADreictory == FALSE)
// 	{
// 		return ;
// 	}
// 
// 	//查找文件
// 	fileName.Format("%s%s",strSourcePath,"*.zip");
// 	BOOL bFind = fileFind.FindFile(fileName);
// 	while (bFind)
// 	{  
// 		CString strName = "";
// 		CString strPath = "";
// 		CString strAimPath = "";
// 		bFind = fileFind.FindNextFile();
// 		if (fileFind.IsDots())
// 		{ 
// 			continue;
// 		}
// 		
// 		if (fileFind.IsDirectory())
// 		{
// 			continue;
// 		}
// 		strName = fileFind.GetFileName();
// 		strPath = fileFind.GetFilePath();
//         strZipList.AddHead(strPath);
//    }
// }
