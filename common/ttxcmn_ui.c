/*
 * TTX common UI library
 * (C) 2022 tomo3136a
 */

#include "teraterm.h"
#include "tttypes.h"
#include <windows.h>

#include <shlobj.h>
#include <tchar.h>

#include "ttxcommon.h"
#include "ttxcmn_ui.h"

///////////////////////////////////////////////////////////////////////////

// get right-bottom point from window item
//ウインドウ内のUIコンポーネントの右下のポイントを pt に取得
VOID GetPointRB(HWND hWnd, UINT uItem, POINT *pt)
{
	HWND hItem;
	RECT rect;
	hItem = GetDlgItem(hWnd, uItem);
	GetWindowRect(hItem, &rect);
	pt->x = rect.right;
	pt->y = rect.bottom;
	ScreenToClient(hWnd, pt);
	GetClientRect(hWnd, &rect);
	pt->x = rect.right - pt->x;
	pt->y = rect.bottom - pt->y;
}

// move right-bottom point within window item
//ウインドウのサイズ変更時、ウインドウ内のUIコンポーネントの位置・サイズを調整
// uFlag==RB_LEFT の場合、ウインドウの右側とUIコンポーネントの左側の距離を保つ
// uFlag==RB_TOP の場合、ウインドウの下側とUIコンポーネントの上側の距離を保つ
// uFlag==RB_RIGHT の場合、ウインドウの右側とUIコンポーネントの右側の距離を保つ
// uFlag==RB_BOTTOM の場合、ウインドウの下側とUIコンポーネントの下側の距離を保つ
VOID MovePointRB(HWND hWnd, UINT uItem, POINT *ptRB, UINT uFlag)
{
	HWND hItem = GetDlgItem(hWnd, uItem);
	RECT rect;
	POINT pt;
	LONG x, y, dx, dy, xx, yy;
	GetWindowRect(hItem, &rect);
	pt.x = rect.left;
	pt.y = rect.top;
	dx = rect.right - rect.left;
	dy = rect.bottom - rect.top;
	ScreenToClient(hWnd, &pt);
	GetClientRect(hWnd, &rect);
	x = pt.x;
	y = pt.y;
	xx = rect.right - dx - pt.x - ptRB->x;
	yy = rect.bottom - dy - pt.y - ptRB->y;
	if (uFlag & RB_LEFT) {
		x += xx;
		dx -= xx;
	}
	if (uFlag & RB_TOP) {
		y += yy;
		dy -= yy;
	}
	if (uFlag & RB_RIGHT) {
		dx += xx;
	}
	if (uFlag & RB_BOTTOM) {
		dy += yy;
	}
	MoveWindow(hItem, x, y, dx, dy, 1);
}

// get window size to point structure
//ウインドウのサイズをpoint型で取得
VOID GetWindowSize(HWND hWnd, POINT *pt)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	if (pt->x)
		pt->x = rect.right - rect.left;
	if (pt->y)
		pt->y = rect.bottom - rect.top;
}

// set window size from point structure
//ウインドウのサイズをpoint型で設定
VOID SetWindowSize(HWND hWnd, POINT *pt)
{
	if (pt->x || pt->y) {
		RECT rect;
		LONG w, h;
		GetWindowRect(hWnd, &rect);
		w = (pt->x) ? pt->x : (rect.right - rect.left);
		h = (pt->y) ? pt->y : (rect.bottom - rect.top);
		MoveWindow(hWnd, rect.left, rect.top, w, h, TRUE);
		// SendMessage(hWnd, WM_SIZING, 0, MAKELPARAM(TRUE, 0));
	}
}

// move window relative direction
//基準となるウインドウ hWndBase にたいして隣接する位置のウインドウ hWnd を移動
//ウインドウの位置は、 uPos 方向で指定
// uPos==0 の場合、右側
// uPos==1 の場合、下側
// uPos==2 の場合、左側
// uPos==3 の場合、上側
VOID SetHomePosition(HWND hWnd, HWND hWndBase, UINT uPos)
{
	RECT rect;
	LONG x, y, w, h;

	GetWindowRect(hWnd, &rect);
	w = rect.right - rect.left;
	h = rect.bottom - rect.top;

	GetWindowRect(hWndBase, &rect);
	x = rect.left;
	y = rect.top;
	switch (uPos) {
		case 0:
			x = rect.right;
			y = rect.top;
			break;
		case 1:
			x = rect.left - w;
			y = rect.top;
			break;
		case 2:
			x = rect.left;
			y = rect.top - h;
			break;
		case 3:
			x = rect.left;
			y = rect.bottom;
			break;
		default:
			x = rect.left;
			y = rect.top;
			break;
	}
	MoveWindow(hWnd, x, y, w, h, TRUE);
}

#if 0
///test to exist cursor position into window item
BOOL TestCursorPos(HWND hWnd, UINT uItem)
{
	HWND hItem;
	RECT rect;
	POINT pt;
	hItem = GetDlgItem(hWnd, uItem);
	GetWindowRect(hItem, &rect);
	GetCursorPos(&pt);
	return (pt.x < rect.left || pt.x > rect.right || pt.y < rect.top || pt.y > rect.bottom);
}
#endif

// adjust window position to center of parent window
//ウインドウを親ウインドウの中心に移動
VOID MoveParentCenter(HWND hWnd)
{
	RECT prc, rc;
	GetWindowRect(GetParent(hWnd), &prc);
	GetClientRect(hWnd, &rc);
	int x = (prc.left + prc.right - rc.right) / 2;
	int y = (prc.top + prc.bottom - rc.bottom) / 2;
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

// create dialog font and set to phFont (require to delete item after)
//ダイアログにフォントを設定
//仕様が終わったら DeleteObject すること
VOID SetDlgFont(HWND hWnd, UINT uItem, HFONT *phFont, LONG uH, LPTSTR szFont)
{
	HWND hItem = GetDlgItem(hWnd, uItem);
	*phFont = CreateFont(uH, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
						 DEFAULT_QUALITY, FF_DONTCARE, (szFont[0] ? szFont : _T("ＭＳ ゴシック")));
	SendMessage(hItem, WM_SETFONT, (WPARAM)(*phFont), MAKELPARAM(TRUE, 0));
}

// open to file select dialog
//ファイル選択ダイアログを表示させる
//パスは、hWnd ウインドウの editCtrl UI コンポーネントに読み書きする
// hWnd ウインドウが無いか editCtl が -1 の場合は、 UI コンポーネントは使用せず、 szPath を読み込む
// szPath が NULL 以外の場合は szPath にも書き込む(サイズは MAX_PATH 以上であること)
//成功した場合 TRUE を返す
BOOL OpenFileDlg(HWND hWnd, UINT editCtl, LPTSTR szTitle, LPTSTR szFilter, 
				 LPTSTR szPath, UINT uFlag)
{
	TCHAR path[MAX_PATH];
	TCHAR buf[MAX_PATH];
	OPENFILENAME ofn;
	DWORD path_sz;

	path_sz = sizeof(path) / sizeof(path[0]);

	buf[0] = 0;
	if (hWnd && (editCtl != 0xffffffff))
		GetDlgItemText(hWnd, editCtl, buf, path_sz);
	if ((uFlag & PTF_SETTPATH) || (!buf[0] && szPath))
		_tcscpy_s(buf, path_sz, szPath);
	ExpandEnvironmentStrings(buf, path, path_sz);

	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = szFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = path;
	ofn.nMaxFile = path_sz;
	ofn.lpstrTitle = szTitle;
	ofn.Flags = OFN_HIDEREADONLY | OFN_NODEREFERENCELINKS;

	if (GetOpenFileName(&ofn)) {
		if (uFlag & PTF_CONTRACT) {
			GetContractPath(path, path_sz, path);
		}
		if (hWnd && editCtl != 0xffffffff) {
			SetDlgItemText(hWnd, editCtl, path);
		}
		if ((uFlag & PTF_GETPATH) && szPath) {
			_tcscpy_s(szPath, MAX_PATH, path);
		}
		return TRUE;
	}
	return FALSE;
}

// open to folder select dialog
//フォルダ選択ダイアログを表示させる
//パスは、hWnd ウインドウの editCtrl UI コンポーネント値に読み書きする
// TODO: メモリ例外になることが無いようにする
static int CALLBACK setDefaultFolder(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED) {
		SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, lpData);
		SendMessage(hwnd, BFFM_SETEXPANDED, (WPARAM)TRUE, lpData);
	}
	return 0;
}

BOOL OpenFolderDlg(HWND hWnd, UINT editCtl, LPTSTR szTitle, LPTSTR szPath, UINT uFlag)
{
	BROWSEINFO bi;
	LPITEMIDLIST pidlRoot;
	LPITEMIDLIST pidlBrowse;
	TCHAR buf[MAX_PATH];
	TCHAR buf2[MAX_PATH];
	DWORD buf_sz;
	BOOL ret = FALSE;

	buf_sz = sizeof(buf) / sizeof(buf[0]);
	if (!SUCCEEDED(SHGetSpecialFolderLocation(hWnd, CSIDL_DESKTOP, &pidlRoot))) {
		return FALSE;
	}

	if (hWnd && editCtl != 0xffffffff) {
		GetDlgItemText(hWnd, editCtl, buf, buf_sz);
	}
	else {
		_tcscpy_s(buf, buf_sz, szPath);
	}
	ExpandEnvironmentStrings(buf, buf2, buf_sz);

	bi.hwndOwner = hWnd;
	bi.pidlRoot = pidlRoot;
	bi.pszDisplayName = buf;
	bi.lpszTitle = szTitle;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_VALIDATE | BIF_NEWDIALOGSTYLE;
	bi.lpfn = setDefaultFolder;
	bi.lParam = (LPARAM)buf;
	pidlBrowse = SHBrowseForFolder(&bi);
	if (pidlBrowse != NULL) {
		if (SHGetPathFromIDList(pidlBrowse, buf2)) {
			if (uFlag & PTF_CONTRACT)
				GetContractPath(buf, buf_sz, buf2);
			else
				_tcscpy_s(buf, buf_sz, buf2);
			if (hWnd && editCtl != 0xffffffff) {
				SetDlgItemText(hWnd, editCtl, buf);
			}
			if (szPath != NULL) {
				_tcscpy_s(szPath, buf_sz, buf);
			}
		}
		CoTaskMemFree(pidlBrowse);
	}
	CoTaskMemFree(pidlRoot);

	return ret;
}
