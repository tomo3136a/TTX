/*
 * TTX version support
 * (C) 2021 tomo3136a
 */

#include "ttxversion_db.h"

#define IS_TT4() (tt_version<5000)
#define BEGIN_TTX_STR(m) LPTSTR m##W = (IS_TT4()) ? toTC((PCHAR)m) : m;
#define END_TTX_STR(m) if (IS_TT4()) { TTXFree(&m##W); }
#define BEGIN_TTX_STR2(m1,m2) BEGIN_TTX_STR(m1) BEGIN_TTX_STR(m2)
#define END_TTX_STR2(m1,m2) END_TTX_STR(m1) END_TTX_STR(m2)

#define TS2(pts,nm,c) TTTSET(tt_version,pts,nm,c)
#define TS(pts,nm) TS2(pts,nm,TTTSET_VER_##nm)
#define TEST_TS(nm) TEST_TTTSET(tt_version,nm)
#define TEST_TS2(nm1,nm2) (TEST_TS(nm1)&&TEST_TS(nm2))
#define TEST_TS3(nm1,nm2,nm3) (TEST_TS2(nm1,nm2)&&TEST_TS(nm3))
#define TEST_TS4(nm1,nm2,nm3,nm4) (TEST_TS3(nm1,nm2,nm3)&&TEST_TS(nm4))
#define TEST_TS5(nm1,nm2,nm3,nm4,nm5) (TEST_TS4(nm1,nm2,nm3,nm4)&&TEST_TS(nm5))

#define CV(pcv,nm) TCOMVAR(tt_version,pcv,nm,TCOMVAR_VER_##nm)
#define TEST_CV(nm) TEST_TCOMVAR(tt_version,nm)
#define TEST_CV2(nm1,nm2) (TEST_CV(nm1)&&TEST_CV(nm2))
#define TEST_CV3(nm1,nm2,nm3) (TEST_CV2(nm1,nm2)&&TEST_CV(nm3))
#define TEST_CV4(nm1,nm2,nm3,nm4) (TEST_CV3(nm1,nm2,nm3)&&TEST_CV(nm4))
#define TEST_CV5(nm1,nm2,nm3,nm4,nm5) (TEST_CV4(nm1,nm2,nm3,nm4)&&TEST_CV(nm5))

// #define TTXHOOK_VER_TTXOpenFile 4063
// #define TTXHOOK_VER_TTXCloseFile 4063
// #define TTXHOOK_VER_TCreateFile 4063
// #define TTXHOOK_VER_TCloseFile 4063
// #define TTXHOOK_VER_TReadFile 4063
// #define TTXHOOK_VER_TWriteFile 4063

#define TTXHOOK_VER_Tfreeaddrinfo 4068
#define TTXHOOK_VER_TWSAAsyncGetAddrInfo 4068

#define TTXHOOK_VER_TTXFileHooks 4068
#define EXPORTS_VER_TTXSetCommandLine 5000
#define HAS_TTXHOOK(m) ((TTXHOOK_VER_##m+0)<=tt_version)

#ifdef __cplusplus
extern "C"
{
#endif

	extern WORD tt_version;

	void TTXInitVersion(WORD version);

#ifdef __cplusplus
}
#endif
