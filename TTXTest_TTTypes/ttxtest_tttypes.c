/*
 * (C) 2022 tomo3136a
 */

#include "teraterm.h"
#include "tttypes.h"
#include "ttplugin.h"

#include <stdio.h>
#include <tchar.h>

#include "ttxversion.h"

#define ORDER 6000

static HANDLE hInst; /* Instance handle of TTX*.DLL */

typedef struct
{
	PTTSet ts;
	PComVar cv;

} TInstVar;

static TInstVar FAR *pvar;

/* WIN32 allows multiple instances of a DLL */
static TInstVar InstVar;

///////////////////////////////////////////////////////////////

/* test 1 */
/* support version: */
/*   tttset.LogTypePlainText 4.63-5.0 over */
/*   tttset.Baud             2.3-5.0 over */
static void test_tttypes_1(WORD mjr, WORD mnr)
{
	TCHAR title[256];
	TCHAR buf[256];
	WORD v1, v2;
	DWORD v3, v4;

	v1 = pvar->ts->LogTypePlainText;
	v2 = TS(pvar->ts,LogTypePlainText);
	v3 = pvar->ts->Baud;
	v4 = TS(pvar->ts,Baud);
	_sntprintf_s(
		title, sizeof(title)/sizeof(title[0]), _TRUNCATE, 
		_T("Build:%d.%d, Running:%d.%d"), 
		TT_VERSION_MAJOR, TT_VERSION_MINOR, mjr, mnr);
	_sntprintf_s(
		buf, sizeof(buf)/sizeof(buf[0]), _TRUNCATE, 
		_T("[LogTypePlainText]\n  ->:   %d\n  TS(): %d\n\n") 
		_T("[Baud]\n  ->:   %d\n  TS(): %d"), 
		v1, v2, v3, v4);
	MessageBox(0, buf, title, MB_OK | MB_ICONINFORMATION);
}


/* test 2 */
/* support version: */
/*   tttset.LogDefaultName     4.63-5.0 over */
/*   tttset.UILanguageFile_ini 4.63-5.0 over */
static void test_tttypes_2(WORD mjr, WORD mnr)
{
	char titleA[256];
	char bufA[256];
	char *langA1;
	char *langA2;
	char *logA1;
	char *logA2;

	logA1 = pvar->ts->LogDefaultName;
	logA2 = TS(pvar->ts,LogDefaultName);
	langA1 = pvar->ts->UILanguageFile_ini;
	langA2 = TS(pvar->ts,UILanguageFile_ini);
	_snprintf_s(
		titleA, sizeof(titleA)/sizeof(titleA[0]), _TRUNCATE, 
		"Build:%d.%d, Running:%d.%d", 
		TT_VERSION_MAJOR, TT_VERSION_MINOR, mjr, mnr);
	_snprintf_s(
		bufA, sizeof(bufA)/sizeof(bufA[0]), _TRUNCATE, 
		"[LogDefaultName]\n  ->:   %s\n  TS(): %s\n\n"
		"[UILanguageFile_ini]\n  ->:   %s\n  TS(): %s", 
		logA1, logA2, langA1, langA2);
	MessageBoxA(0, bufA, titleA, MB_OK | MB_ICONINFORMATION);
}


/* test 3 */
/* support version: */
/*   tttset.UILanguageFileW_ini 5.0 over */
static void test_tttypes_3(WORD mjr, WORD mnr)
{
	wchar_t titleW[256];
	wchar_t bufW[256];
	wchar_t *langW1;
	wchar_t *langW2;

	_snwprintf_s(
	titleW, sizeof(titleW)/sizeof(titleW[0]), _TRUNCATE, 
	u"Build:%d.%d, Running:%d.%d", 
	TT_VERSION_MAJOR, TT_VERSION_MINOR, mjr, mnr);

	if(TEST_TS(UILanguageFileW)){
		langW1 = TS(pvar->ts,UILanguageFileW_ini);
#ifdef TT4
		langW2 = u"";
#else /* TT4 */
		langW2 = pvar->ts->UILanguageFileW_ini;
#endif /* TT4 */
		_snwprintf_s(
			bufW, sizeof(bufW)/sizeof(bufW[0]), _TRUNCATE, 
			u"[UILanguageFileW_ini]\n  ->:   %s\n  TS(): %s", 
			langW1, langW2);
	}
	else{
		_snwprintf_s(
			bufW, sizeof(bufW)/sizeof(bufW[0]), _TRUNCATE, 
			u"UILanguageFileW_ini not support");
	}
	MessageBoxW(0, bufW, titleW, MB_OK | MB_ICONINFORMATION);
}

///////////////////////////////////////////////////////////////

static void PASCAL TTXInit(PTTSet ts, PComVar cv)
{
	pvar->ts = ts;
	pvar->cv = cv;
}

static void PASCAL TTXGetUIHooks(TTXUIHooks *hooks)
{
	WORD mjr, mnr;

	mjr = tt_version / 1000;
	mnr = tt_version % 1000;

	test_tttypes_1(mjr, mnr);
	test_tttypes_2(mjr, mnr);
	test_tttypes_3(mjr, mnr);
}

///////////////////////////////////////////////////////////////

static TTXExports Exports = {
	/* This must contain the size of the structure. See below for its usage. */
	sizeof(TTXExports),

	ORDER,

	/* Now we just list the functions that we've implemented. */
	TTXInit,
	TTXGetUIHooks,
	NULL, //TTXGetSetupHooks,
	NULL, //TTXOpenTCP,
	NULL, //TTXCloseTCP,
	NULL, //TTXSetWinSize,
	NULL, //TTXModifyMenu,
	NULL, //TTXModifyPopupMenu,
	NULL, //TTXProcessCommand,
	NULL, //TTXEnd,
	NULL, //TTXSetCommandLine,
	NULL, //TTXOpenFile,
	NULL, //TTXCloseFile,
};

BOOL __declspec(dllexport) PASCAL FAR TTXBind(WORD Version, TTXExports *exports)
{
	int size = sizeof(Exports) - sizeof(exports->size);
	/* do version checking if necessary */
	/* if (Version!=TTVERSION) return FALSE; */

	/* initialize to tttypes test */
	TTXInitVersion(0);

	/* tset tttset menber support old version at TTXBind() */
	/* less then 4.63 (LogTypePlainText or UILanguageFile_ini) is FALSE */
	/* FALSE result is not bind TTX */
	if(!TEST_TS2(LogTypePlainText, Baud)) return FALSE;		/* test 2 entry */
	if(!TEST_TS(LogDefaultName)) return FALSE;				/* test 1 entry */
	if(!TEST_TS(UILanguageFile_ini)) return FALSE;			/* test 1 entry */

	// if (TTXIgnore(ORDER, _T(INISECTION), 0))
	// 	return TRUE;

	if (size > exports->size)
	{
		size = exports->size;
	}
	memcpy((char *)exports + sizeof(exports->size),
		   (char *)&Exports + sizeof(exports->size),
		   size);
	return TRUE;
}

BOOL WINAPI DllMain(HANDLE hInstance,
					ULONG ul_reason_for_call,
					LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_THREAD_ATTACH:
		/* do thread initialization */
		break;
	case DLL_THREAD_DETACH:
		/* do thread cleanup */
		break;
	case DLL_PROCESS_ATTACH:
		/* do process initialization */
		hInst = hInstance;
		pvar = &InstVar;
		break;
	case DLL_PROCESS_DETACH:
		/* do process cleanup */
		break;
	}
	return TRUE;
}
