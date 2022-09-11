/*
 * TTX version support
 * (C) 2021 tomo3136a
 */

#include "ttxversion_db.h"

#define TS2(pts,nm,c) TTTSET(tt_version,pts,nm,c)
#define TS(pts,nm) TS2(pts,nm,TTTSET_VER_##nm)
#define TEST_TS(nm) TEST_TTTSET(tt_version,nm)
#define TEST_TS_1(nm) TEST_TTTSET(tt_version,nm)
#define TEST_TS_2(nm1,nm2) (TEST_TS_1(nm1)&&TEST_TS_1(nm2))
#define TEST_TS_3(nm1,nm2,nm3) (TEST_TS_2(nm1,nm2)&&TEST_TS_1(nm3))
#define TEST_TS_4(nm1,nm2,nm3,nm4) (TEST_TS_3(nm1,nm2,nm3)&&TEST_TS_1(nm4))
#define TEST_TS_5(nm1,nm2,nm3,nm4,nm5) (TEST_TS_4(nm1,nm2,nm3,nm4)&&TEST_TS_1(nm5))
#define VA_IDX() TEST_TS_5, TEST_TS_4, TEST_TS_3, TEST_TS_2, TEST_TS_1
#define VA_CNT(v1, v2, v3, v4, v5, N ,...) N
#define TEST_TS_N(...) VA_CNT(__VA_ARGS__, VA_IDX())(__VA_ARGS__)

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

#define TTXHOOK_VER_freeaddrinfo 4068
#define TTXHOOK_VER_WSAAsyncGetAddrInfo 4068

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
