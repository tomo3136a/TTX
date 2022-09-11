/*
 * TTX debug support
 * (C) 2021 tomo3136a
 */

#include <windows.h>

#ifdef TT4
#define __FUNCTIONT__ __FUNCTION__
#else /* TT4 */
#define __FUNCTIONT__ __FUNCTIONW__
#endif /* TT4 */

#include "infoview.h"

#define DBG_VIEW(fn, fmt, ...)                                                      \
	{                                                                               \
		TCHAR *buf = (TCHAR *)malloc(sizeof(TCHAR)*1024);                           \
		_sntprintf_s(buf, 1024, 1024, fn _T(": ") fmt _T("\r\n") , __VA_ARGS__);    \
        WriteInfoView(buf);                                                         \
        free(buf);                                                                  \
	}

#ifdef __cplusplus
extern "C"
{
#endif

    VOID MsgBoxInt(UINT uflg);
    VOID SetDbgTitle(HWND hWnd, PCHAR msg, UINT *pCnt);

    VOID OpenDebugLog(PCHAR szFile);
    VOID CloseDebugLog();
    VOID WriteDebugLog(PCHAR buf);
    VOID WriteDebugLogEvent(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

#ifdef __cplusplus
}
#endif
