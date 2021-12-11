/*
 * TTX debug support
 * (C) 2021 tomo3136a
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ttxdebug.h"

VOID MsgBoxInt(UINT uflg)
{
	char tmp[256];
	snprintf(tmp, sizeof(tmp) / sizeof(tmp[0]), "%08x", uflg);
	MessageBox(0, tmp, "", MB_OK);
}

VOID SetDbgTitle(HWND hWnd, PCHAR msg, UINT *pCnt)
{
	char tmp[256];
	snprintf(tmp, sizeof(tmp) / sizeof(tmp[0]), "%s: %08x", msg, *pCnt);
	*pCnt = *pCnt + 1;
	SetWindowText(hWnd, tmp);
}

static HANDLE dbg_hFile = INVALID_HANDLE_VALUE;

VOID OpenDebugLog(PCHAR szFile)
{
	if (dbg_hFile != INVALID_HANDLE_VALUE)
		return;
	dbg_hFile = CreateFile(szFile, GENERIC_WRITE, 0, NULL,
						   OPEN_ALWAYS, 0, NULL);
	if (dbg_hFile == INVALID_HANDLE_VALUE)
		return;

	SetFilePointer(dbg_hFile, 0, NULL, FILE_END);
}

VOID CloseDebugLog()
{
	if (dbg_hFile == INVALID_HANDLE_VALUE)
		return;
	CloseHandle(dbg_hFile);
	dbg_hFile = INVALID_HANDLE_VALUE;
}

VOID WriteDebugLog(PCHAR msg)
{
	DWORD dwSize;
	WriteFile(dbg_hFile, msg, strnlen_s(msg, _TRUNCATE), &dwSize, NULL);
	FlushFileBuffers(dbg_hFile);
}

VOID WriteDebugLogEvent(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	char tmp[256];
	snprintf(tmp, sizeof(tmp) / sizeof(tmp[0]), "%08x %08x %08x %08x\n", (UINT)hWnd, msg, wp, lp);
	WriteDebugLog(tmp);
}
