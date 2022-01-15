/*
 * TTX version support
 * (C) 2021 tomo3136a
 */

#include "tt-version.h"

#if ((TT_VERSION_MAJOR * 10000 + TT_VERSION_MINOR) < 40105)
#error "not support build version"
#endif

/* old version PTTSet structure support */
/* build test: 4.105 */
/* running test: 4.105, 4.104, 4.98, 4.96, 4.80 */

#define TT_ADJ(pts, nm, ver, rev, ref, v)          \
	(((tt_version < (ver * 10000 + rev)) &&        \
	  ((void *)&(pts->ref) <= (void *)&(pts->nm))) \
		 ? (-v)                                    \
		 : 0)

#define TS(pts, nm)                                             \
	(((PTTSet)((char *)(pts) +                                  \
			   TT_ADJ((pts), nm, 4, 105, UseNormalBGColor, 4))) \
		 ->nm)

#ifdef __cplusplus
extern "C"
{
#endif

	extern WORD tt_version;

	void TTXInitVersion();

#ifdef __cplusplus
}
#endif
