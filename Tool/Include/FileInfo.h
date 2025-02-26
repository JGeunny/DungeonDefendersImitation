#pragma once

#include "Export_Function.h"

class CFileInfo
{
public:
	CFileInfo();
	~CFileInfo();

public:
	static CString ConvertRelativePath(const CString& strFullPath);
	static void ExtractDirectory(const TCHAR* pFullPath, list<Engine::PATH_INFO*>& rPathInfoLst);
	static int ExtractFileCount(const TCHAR* pFullPath);
};

