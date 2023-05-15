/*
	apiask.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/



#ifndef APIASK_H
#define APIASK_H


/* apiask.c 의 디버깅을 위한 테스트 함수. */
void apiask_test(int nTestNumber);

/* 아래의 함수들의 사양은 API68K.c 의 대응하는 FP 콜을 참조할 것. */

/* 로마자 변환(카타카나) */
int apiask_15_RomaToKata(char *src, char *dst);

/* 로마자 변환(히라가나) */
int apiask_16_RomaToHira(char *src, char *dst);

/* 변환 (최초의 후보군 작성) */
int apiask_19_Henkan(char *buf, char *kouho, char *kouzoku);

/* 후보군 블록 안에서의 이전 후보 */
int apiask_20_PrevKouho(char *kouho, char *kouzoku);

/* 후보군 블록 안에서의 다음 후보 */
int apiask_21_NextKouho(char *kouho, char *kouzoku);

/* 이전 후보 블록의 작성 */
int apiask_22_PrevKouhoBlock(char *kouho, char *kouzoku);

/* 다음 후보 블록의 작성 */
int apiask_23_NextKouhoBlock(char *kouho, char *kouzoku);

/* 전체 확정 */
int apiask_24_AllKakutei(char *buf);

/* 선두 문절 확정  */
int apiask_25_FirstKakutei(int no, char *buf, char *kouho, char *kouzoku, short *nkouho);

/* 사전 오픈 */
int apiask_28_OpenDic(void);

/* 사전 클로즈 */
int apiask_29_CloseDic(void);

/* 사전 단어 등록  */
int apiask_30_TourokuAdd(int ndic, char *yomi, char *tango, int syn);

/* 사전 단어 삭제 */
int apiask_31_TourokuDelete(int ndic, char *yomi, char *tango, int syn);

/* 사전 파일 설정 */
int apiask_32_SetDicName(char *maindic, char *subdic);

/* 선두 문절을 1문자 길게 해서 다시 변환 */
int apiask_33_LongerKouho(char *kouho, char *kouzoku);

/* 선두 문절을 1문자 짧게 해서 다시 변환 */
int apiask_34_ShorterKouho(char *kouho, char *kouzoku);

/* 이전 문절에의 이동 */
int apiask_35_PrevBunsetu(char *kouho, char *kouzoku);

/* 다음 문절에의 이동 */
int apiask_36_NextBunsetu(char *kouho, char *kouzoku);

/* 선두 문절의 문자종 변환 (카타카나) */
int apiask_37_FirstHenkanToKata(char *kouho, char *kouzoku);

/* 선두 문절의 문자종 변환 (히라가나) */
int apiask_38_FirstHenkanToHira(char *kouho, char *kouzoku);

/* 선두 문절의 문자종 변환 (반각)  */
int apiask_39_FirstHenkanToHan(char *kouho, char *kouzoku);

/* 선두 문절의 문자종 변환 (전각) */
int apiask_40_FirstHenkanToZen(char *kouho, char *kouzoku);

#endif
