#pragma once
#include "zip.h"
#include "unzip.h"

bool Unzip2Dir(LPCTSTR szZipFile, LPCTSTR szDir);
bool ZipDir(LPCTSTR szDir, LPCTSTR szZipPath);