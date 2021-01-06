/*
 * TTX debug support
 * (C) 2021 tomo3136a
 */

#include <windows.h>

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
