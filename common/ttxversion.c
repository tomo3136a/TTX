/*
 * TTX version support
 * (C) 2021 tomo3136a
 */

#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>

#include "ttxversion.h"

WORD tt_version = 0;

//same function with ttset::RunningVersion
static WORD TTRunningVersion()
{
#pragma comment(lib, "version.lib")
	TCHAR szPath[MAX_PATH];
	DWORD dwSize;
	DWORD dwHandle;
	LPVOID lpBuf;
	UINT uLen;
	VS_FIXEDFILEINFO *pFileInfo;
	int major, minor;

	GetModuleFileName(NULL, szPath, sizeof(szPath)/sizeof(szPath[0]) - 1);

	dwSize = GetFileVersionInfoSize(szPath, &dwHandle);
	if (dwSize == 0)
	{
		return 0;
	}

	lpBuf = malloc(dwSize*sizeof(TCHAR));
	if (!GetFileVersionInfo(szPath, dwHandle, dwSize, lpBuf))
	{
		free(lpBuf);
		return 0;
	}

	if (!VerQueryValue(lpBuf, _T("\\"), (LPVOID *)&pFileInfo, &uLen))
	{
		free(lpBuf);
		return 0;
	}

	major = HIWORD(pFileInfo->dwFileVersionMS);
	minor = LOWORD(pFileInfo->dwFileVersionMS);

	free(lpBuf);

	return major * 1000 + minor;
}

void TTXInitVersion(WORD version)
{
	tt_version = (version) ? version : TTRunningVersion();
}
