/*
 * TTX common UI library
 * (C) 2022 tomo3136a
 */

#ifndef _TTXCMN_UI_H
#define _TTXCMN_UI_H

#include <windows.h>

/* auto resize for dialog compornent by window resize */
#define RB_LEFT 0x0001
#define RB_TOP 0x0002
#define RB_RIGHT 0x0004
#define RB_BOTTOM 0x0008

#ifdef __cplusplus
extern "C" {
#endif

/* window control */
/// get right-bottom point from window item
VOID GetPointRB(HWND hWnd, UINT uItem, POINT *pt);

/// move right-bottom point within window item
VOID MovePointRB(HWND hWnd, UINT uItem, POINT *ptRB, UINT uFlag);

/// get window size to point structure
VOID GetWindowSize(HWND hWnd, POINT *pt);

/// set window size from point structure
VOID SetWindowSize(HWND hWnd, POINT *pt);

/// move window relative direction
VOID SetHomePosition(HWND hWnd, HWND hWndBase, UINT uPos);

/// adjust window position to center of parent window
VOID MoveParentCenter(HWND hWnd);

/// create dialog font and set to phFont (require to delete item after)
VOID SetDlgFont(HWND hWnd, UINT uItem, HFONT *phFont, LONG uH, LPTSTR szFont);

/* dialog */
enum {
	PTF_CONTRACT = 16,
	PTF_SETTPATH = 32,
	PTF_GETPATH = 64
};
/// open to file select dialog
BOOL OpenFileDlg(HWND hWnd, UINT editCtl, LPTSTR szTitle, LPTSTR szFilter, LPTSTR szPath, UINT uFlag);

/// open to folder select dialog
BOOL OpenFolderDlg(HWND hWnd, UINT editCtl, LPTSTR szTitle, LPTSTR szPath, UINT uFlag);

#ifdef __cplusplus
}
#endif

#endif /* _TTXCMN_UI_H */
