/*
 * information view
 * (C) 2022 tomo3136a
 */

#include <windows.h>

#ifdef __cplusplus
extern "C"
{
#endif

    VOID InitInfoView();
    BOOL IsOpenInfoView();
    VOID OpenInfoView(HANDLE hInst, HWND hWin, PTSTR szTitle);
    VOID CloseInfoView();
    VOID StopInfoView();
    VOID SetInfoViewTitle(PTSTR szTitle);
    VOID SetInfoViewMessage(PTSTR szText);
    VOID UpdateInfoView();
    VOID ClearInfoView();
    VOID WriteInfoView(LPTSTR buf);
    VOID WriteInfoViewEvent(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

#ifdef __cplusplus
}
#endif
