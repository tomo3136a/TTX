/*
 * Tera Term Estimate of tttset structure mismatch Extension
 * (C) 2020 tomo3136a
 */

#include "teraterm.h"
#include "tttypes.h"
#include "ttplugin.h"
#include "tt-version.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ttxcommon.h"

#define BUF_SZ 65536
#define LINE(buf, nm, s)        \
	strcat_s(buf, BUF_SZ, nm);  \
	strcat_s(buf, BUF_SZ, "="); \
	strcat_s(buf, BUF_SZ, s);   \
	strcat_s(buf, BUF_SZ, "\r")

void DisplayTTTSet(HWND hWnd, PTTSet ts, WORD tt_version)
{
	CHAR buf[128];
	PCHAR msg;

	msg = (PCHAR)malloc(BUF_SZ*sizeof(CHAR));
	msg[0] = 0;
	LINE(msg, "HomeDir", ts->HomeDir);
	LINE(msg, "SetupFName", ts->SetupFName);
	LINE(msg, "KeyCnfFN", ts->KeyCnfFN);
	LINE(msg, "LogFN", ts->LogFN);
	LINE(msg, "MacroFN", ts->MacroFN);
	LINE(msg, "HostName", ts->HostName);
	LINE(msg, "VTFont", ts->VTFont);
	LINE(msg, "PrnFont", ts->PrnFont);
	LINE(msg, "PrnDev", ts->PrnDev);
	LINE(msg, "FileDir", ts->FileDir);
	LINE(msg, "FileSendFilter", ts->FileSendFilter);
	LINE(msg, "DelimList", ts->DelimList);
	LINE(msg, "TEKFont", ts->TEKFont);
	LINE(msg, "Answerback", ts->Answerback);
	LINE(msg, "Title", ts->Title);
	LINE(msg, "TermType", ts->TermType);
	LINE(msg, "MouseCursorName", ts->MouseCursorName);
	LINE(msg, "CygwinDirectory", ts->CygwinDirectory);
//	LINE(msg, "Locale", ts->Locale);
	LINE(msg, "ViewlogEditor", ts->ViewlogEditor);
	LINE(msg, "LogDefaultName", ts->LogDefaultName);
	LINE(msg, "LogDefaultPath", ts->LogDefaultPath);
	LINE(msg, "EtermLookfeel.BGSPIPath", ts->EtermLookfeel.BGSPIPath);
	LINE(msg, "EtermLookfeel.BGThemeFile", ts->EtermLookfeel.BGThemeFile);
	LINE(msg, "UILanguageFile", ts->UILanguageFile);
	LINE(msg, "UIMsg", ts->UIMsg);
	LINE(msg, "UILanguageFile_ini", ts->UILanguageFile_ini);
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, "Build:%d.%d, Running:%d",
				TT_VERSION_MAJOR, TT_VERSION_MINOR, tt_version);
	MessageBoxA(hWnd, msg, buf, MB_OK);

	msg[0] = 0;
	LINE(msg, "XModemRcvCommand", ts->XModemRcvCommand);
	LINE(msg, "ZModemRcvCommand", ts->ZModemRcvCommand);
	LINE(msg, "YModemRcvCommand", ts->YModemRcvCommand);
	LINE(msg, "ConfirmChangePasteStringFile", ts->ConfirmChangePasteStringFile);
	strcat_s(msg, BUF_SZ, "---- v4.63 ----\r");
	strcat_s(msg, BUF_SZ, "---- v4.67 ----\r");
	LINE(msg, "TerminalUID", ts->TerminalUID);
	LINE(msg, "ClickableUrlBrowser", ts->ClickableUrlBrowser);
	LINE(msg, "ClickableUrlBrowserArg", ts->ClickableUrlBrowserArg);
	LINE(msg, "ScpSendDir", ts->ScpSendDir);
	LINE(msg, "BGImageFilePath", ts->BGImageFilePath);
	strcat_s(msg, BUF_SZ, "---- v4.80 ----\r");
	LINE(msg, "CygtermSettings.term", ts->CygtermSettings.term);
	LINE(msg, "CygtermSettings.term_type", ts->CygtermSettings.term_type);
	LINE(msg, "CygtermSettings.port_start", ts->CygtermSettings.port_start);
	LINE(msg, "CygtermSettings.port_range", ts->CygtermSettings.port_range);
	LINE(msg, "CygtermSettings.shell", ts->CygtermSettings.shell);
	LINE(msg, "CygtermSettings.env1", ts->CygtermSettings.env1);
	LINE(msg, "CygtermSettings.env2", ts->CygtermSettings.env2);
	LINE(msg, "LogTimestampFormat", ts->LogTimestampFormat);
	strcat_s(msg, BUF_SZ, "---- v4.96 ----\r");
	strcat_s(msg, BUF_SZ, "---- v4.98 ----\r");
	strcat_s(msg, BUF_SZ, "---- v4.100 ----\r");
	strcat_s(msg, BUF_SZ, "---- v4.102 ----\r");
	strcat_s(msg, BUF_SZ, "---- v4.104 ----\r");
	LINE(msg, "DialogFontName", ts->DialogFontName);
	strcat_s(msg, BUF_SZ, "\r");
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, "ConfigVersion=%d\r", ts->ConfigVersion);
	strcat_s(msg, BUF_SZ, buf);
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, "RunningVersion=%d\r", ts->RunningVersion);
	strcat_s(msg, BUF_SZ, buf);
	strcat_s(msg, BUF_SZ, "---- v4.105 ----\r");
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, "Build:%d.%d, Running:%d",
				TT_VERSION_MAJOR, TT_VERSION_MINOR, tt_version);
	MessageBoxA(hWnd, msg, buf, MB_OK);
	free(msg);
}
