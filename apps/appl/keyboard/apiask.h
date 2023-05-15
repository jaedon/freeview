/*
	apiask.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/



#ifndef APIASK_H
#define APIASK_H


/* apiask.c �� ������� ���� �׽�Ʈ �Լ�. */
void apiask_test(int nTestNumber);

/* �Ʒ��� �Լ����� ����� API68K.c �� �����ϴ� FP ���� ������ ��. */

/* �θ��� ��ȯ(īŸī��) */
int apiask_15_RomaToKata(char *src, char *dst);

/* �θ��� ��ȯ(���󰡳�) */
int apiask_16_RomaToHira(char *src, char *dst);

/* ��ȯ (������ �ĺ��� �ۼ�) */
int apiask_19_Henkan(char *buf, char *kouho, char *kouzoku);

/* �ĺ��� ��� �ȿ����� ���� �ĺ� */
int apiask_20_PrevKouho(char *kouho, char *kouzoku);

/* �ĺ��� ��� �ȿ����� ���� �ĺ� */
int apiask_21_NextKouho(char *kouho, char *kouzoku);

/* ���� �ĺ� ����� �ۼ� */
int apiask_22_PrevKouhoBlock(char *kouho, char *kouzoku);

/* ���� �ĺ� ����� �ۼ� */
int apiask_23_NextKouhoBlock(char *kouho, char *kouzoku);

/* ��ü Ȯ�� */
int apiask_24_AllKakutei(char *buf);

/* ���� ���� Ȯ��  */
int apiask_25_FirstKakutei(int no, char *buf, char *kouho, char *kouzoku, short *nkouho);

/* ���� ���� */
int apiask_28_OpenDic(void);

/* ���� Ŭ���� */
int apiask_29_CloseDic(void);

/* ���� �ܾ� ���  */
int apiask_30_TourokuAdd(int ndic, char *yomi, char *tango, int syn);

/* ���� �ܾ� ���� */
int apiask_31_TourokuDelete(int ndic, char *yomi, char *tango, int syn);

/* ���� ���� ���� */
int apiask_32_SetDicName(char *maindic, char *subdic);

/* ���� ������ 1���� ��� �ؼ� �ٽ� ��ȯ */
int apiask_33_LongerKouho(char *kouho, char *kouzoku);

/* ���� ������ 1���� ª�� �ؼ� �ٽ� ��ȯ */
int apiask_34_ShorterKouho(char *kouho, char *kouzoku);

/* ���� �������� �̵� */
int apiask_35_PrevBunsetu(char *kouho, char *kouzoku);

/* ���� �������� �̵� */
int apiask_36_NextBunsetu(char *kouho, char *kouzoku);

/* ���� ������ ������ ��ȯ (īŸī��) */
int apiask_37_FirstHenkanToKata(char *kouho, char *kouzoku);

/* ���� ������ ������ ��ȯ (���󰡳�) */
int apiask_38_FirstHenkanToHira(char *kouho, char *kouzoku);

/* ���� ������ ������ ��ȯ (�ݰ�)  */
int apiask_39_FirstHenkanToHan(char *kouho, char *kouzoku);

/* ���� ������ ������ ��ȯ (����) */
int apiask_40_FirstHenkanToZen(char *kouho, char *kouzoku);

#endif
