/*
 * information view
 * (C) 2022 tomo3136a
 */

#include "teraterm.h"
#include "tttypes.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>

#include "ttxcommon.h"
#include "ttxcmn_ui.h"

#include "resource.h"
#include "infoview.h"

//message buffer structure (simple link list)
typedef struct TMsgBuf
{
	LPTSTR s;
	struct TMsgBuf *n;
} TMsgBuf, *PMsgBuf, **PPMsgBuf;

typedef struct
{
	POINT win_size;
	int win_pos;
	BOOL enable;		//message write enable
	BOOL active;		//window active

	TCHAR font[LF_FACESIZE];
	POINT font_size;
	int font_charset;

	//message area
	//TCHAR title[TitleBuffSize];
	TCHAR path[MAXPATHLEN];

	PMsgBuf pbuf;			//message buffer

} TInfoView;

static TInfoView InsTInfoView;
static TInfoView *pvar;

static HWND hInfoView = NULL;

///////////////////////////////////////////////////////////////

VOID InitInfoView()
{
	memset(&InsTInfoView, 0, sizeof(InsTInfoView));
	pvar = &InsTInfoView;

	pvar->enable = TRUE;
	pvar->active = FALSE;
	pvar->win_size.x = 900;
	pvar->win_size.y = 800;
	pvar->win_pos = 0;

	pvar->font[0] = 0;
	pvar->font_size.x = 0;
	pvar->font_size.y = 0;
	pvar->font_charset = 0;

	//pvar->title[0] = 0;
	pvar->path[0] = 0;

	pvar->pbuf = NULL;
}

///////////////////////////////////////////////////////////////

static LRESULT CALLBACK Infoview_proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static POINT ptMsg, ptLog, ptBtn, ptSts;
	static HFONT hFont;
	static UINT ucnt = 0;

	switch (msg)
	{
	case WM_INITDIALOG:
		//SetDlgFont(hWnd, IDC_MSG, &hFont, pvar->font_size.y, pvar->font);
		//SetDlgFont(hWnd, IDC_LOG, &hFont, pvar->font_size.y, pvar->font);
		GetPointRB(hWnd, IDC_MSG, &ptMsg);
		GetPointRB(hWnd, IDC_LOG, &ptLog);
		GetPointRB(hWnd, IDOK, &ptBtn);
		GetPointRB(hWnd, IDC_STS, &ptSts);
		pvar->active = TRUE;
		SetWindowSize(hWnd, &(pvar->win_size));
		//SetWindowText(hWnd, pvar->title);
		SetFocus(GetDlgItem(hWnd, IDC_LOG));
		return TRUE;

	case WM_SIZE:
		MovePointRB(hWnd, IDC_MSG, &ptMsg, RB_RIGHT);
		MovePointRB(hWnd, IDC_LOG, &ptLog, RB_RIGHT | RB_BOTTOM);
		MovePointRB(hWnd, IDOK, &ptBtn, RB_TOP | RB_BOTTOM);
		MovePointRB(hWnd, IDC_STS, &ptSts, RB_RIGHT | RB_TOP | RB_BOTTOM);
		GetWindowSize(hWnd, &(pvar->win_size));
		return TRUE;

	case WM_ACTIVATE:
		if (LOWORD(wp))
			SetFocus(GetParent(hWnd));
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wp))
		{
		case IDOK:
			pvar->active = FALSE;
			ClearInfoView();
			EndDialog(hWnd, 0);
			return TRUE;

		case IDCANCEL:
			pvar->active = FALSE;
			ClearInfoView();
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;

	case WM_CLOSE:
		pvar->active = FALSE;
		ClearInfoView();
		EndDialog(hWnd, 0);
		return TRUE;

	case WM_DESTROY:
		pvar->active = FALSE;
		ClearInfoView();
		DeleteObject(hFont);
		return TRUE;
	}
	return FALSE;
}

BOOL IsOpenInfoView()
{
	return pvar->active;
}

VOID OpenInfoView(HANDLE hInst, HWND hWnd, PTSTR szTitle)
{
	if (!pvar->active)
	{
		if (hInfoView)
		{
			DestroyWindow(hInfoView);
			hInfoView = 0;
		}
		hInfoView = CreateDialog(hInst, 
			MAKEINTRESOURCE(IDD_INFOVIEW_DIALOG),
			hWnd, (DLGPROC)Infoview_proc);
		SetHomePosition(hInfoView, hWnd, pvar->win_pos);
		SetFocus(GetParent(hInfoView));
		if (szTitle)
			SetWindowText(hInfoView, szTitle);
		UpdateInfoView();
	}
}

VOID CloseInfoView()
{
	if (pvar->active)
	{
		PostMessage(hInfoView, WM_CLOSE, 0, MAKELPARAM(0, 0));
	}
}

VOID StopInfoView()
{
	pvar->enable = FALSE;
	pvar->active = FALSE;
	ClearInfoView();
}

VOID SetInfoViewTitle(PTSTR szTitle)
{
	if (pvar->active)
		SetWindowText(hInfoView, (szTitle) ? szTitle : _T(""));
}

VOID SetInfoViewMessage(PTSTR szText)
{
	if (pvar->active)
		SetDlgItemText(hInfoView, IDC_MSG, szText);
}

static void SetInfoViewText(LPTSTR szText, DWORD dwSize)
{
	HWND hWnd = GetDlgItem(hInfoView, IDC_LOG);
	int end = GetWindowTextLength(hWnd);
	int rpt = 1024;
	if (end > 30000 - rpt)
	{
		int n = 30000 + 1;
		LPTSTR buf = (LPTSTR)malloc(sizeof(TCHAR) * n);
		GetWindowText(hWnd, buf, n);
		SetWindowText(hWnd, &buf[rpt]);
		free(buf);
		end = GetWindowTextLength(hWnd);
	}
	SendMessage(hWnd, EM_SETSEL, end, end);
	SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)szText);
}

VOID UpdateInfoView()
{
	if (pvar->active)
	{
		while (pvar->pbuf != NULL) {
			PMsgBuf pbuf = pvar->pbuf;
			LPTSTR s = pbuf->s;
			pvar->pbuf = pbuf->n;
			free(pbuf);
			SetInfoViewText(s, _tcslen(s));
			TTXFree(&s);
		}
	}
}

VOID ClearInfoView()
{
	while (pvar->pbuf != NULL) {
		PMsgBuf pbuf = pvar->pbuf;
		LPTSTR s = pbuf->s;
		pvar->pbuf = pbuf->n;
		free(pbuf);
		TTXFree(&s);
	}
}

VOID WriteInfoView(LPTSTR buf)
{
	LPTSTR p = NULL;
	TTXDup(&p, 1024, buf);
	if (pvar->active)
	{
		UpdateInfoView();
		int blen = _tcslen(p);
		SetInfoViewText(p, blen);
	}
	else if (pvar->enable)
	{
		PPMsgBuf ppbuf = &(pvar->pbuf);
		while (*ppbuf != NULL) 
			ppbuf = &((*ppbuf)->n);
		PMsgBuf pbuf = (PMsgBuf)malloc(sizeof(TMsgBuf));
		pbuf->s = p;
		pbuf->n = NULL;
		*ppbuf = pbuf;
		return;
	}
	TTXFree(&p);
}

VOID WriteInfoViewEvent(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	/* T.B.D. */
}
