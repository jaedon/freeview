#ifndef __CALLYPSO_VERSION_H__
#define __CALLYPSO_VERSION_H__

/********* [script reference area - START] ***********/
#define CALYPSO_VERSION_MAJOR 1
#define CALYPSO_VERSION_MINOR 2
#define CALYPSO_VERSION_MICRO 0
#define CALYPSO_VERSION_HOTFIX 3

#define CALYPSO_VERSION_BCODE 1
#define CALYPSO_VERSION_BREVS 1

/*
	[Branch code look-up table]
	Ref##1 : release
	Ref##2 : candidate
	Ref##3 : branch-type
	Ref##10 : pre-distrubute-for-ligada
	[ Free to add here for calypso members ]
*/

/********* [script reference area - END ] ***********/

/* Each version range is 0~999 */
#define CALYPSO_VERSION_FORMULA(MJ,MN,MC,HF,BC) ( (MJ * 1000000000000) + (MN * 1000000000) + (MC * 1000000) + (HF * 1000) + (BC) )
#define CALYPSO_LIB_VERSION (CALYPSO_VERSION_FORMULA(CALYPSO_VERSION_MAJOR, CALYPSO_VERSION_MINOR, CALYPSO_VERSION_MICRO, CALYPSO_VERSION_HOTFIX, CALYPSO_VERSION_BCODE))
#define CALYPSO_REQUIRED_VERSION(MJ,MN,MC,HF,BC) (CALYPSO_LIB_VERSION >= CALYPSO_VERSION_FORMULA(MJ,MN,MC,HF,BC))

#endif
