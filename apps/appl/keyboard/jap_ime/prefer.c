/*
 * (c) 2011-2012 Humax Co., Ltd. 
 * This program is produced by Humax Co., Ltd. ("Humax") and 
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
 * non-assignable, non-transferable and non-exclusive license to use this Software. 
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
 * you agree to the responsibility to take all reasonable efforts to protect the any information 
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, 
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies 
 * of the Software including all documentation. This License will terminate immediately without notice from Humax 
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies 
 * of the Software and all documentation.  

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice. 
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court, 
 * in the Republic of Korea.
*/
/*
	prefer.c
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


#include	"askdef.h"
#include	"pword.h"
#include	"hwork.h"
#include	"dic.h"
#include	"henkan.h"
#include	"prefer.h"


/*
#define POINT	1
*/
#define BUN_END			0xff
#define NOCARE			0xff


int bs_prefer(int pos, int epos, int len)
{
	register BSMEM	*bsm, *maxbsm;
	HCHAR *hp;

	hp = &hchar[pos];

	if (hp == NULL || hp->bs_mem == NULL)
		return(-1);
		
	for (bsm = hp->bs_mem; bsm; bsm = bsm->bs_next) {
		/* �� 1 ���� a BSMEM x �� 2 ���� each BSMEM */
		if (len >= 0 && bsm->bs_len != len)
			continue;
		else if (bs_point(bsm, pos, epos) == -1)
			return(-1);
	}
	
	bsm = hp->bs_mem;
	maxbsm = bsm;
	for (bsm = bsm->bs_next; bsm; bsm = bsm->bs_next) {
		if (bsm->itsprior > maxbsm->itsprior) {
			maxbsm = bsm;
		}
	}
	/* ������ ��1 BSMEM�� ���� �� 2 ������ ���� ����Ʈ */
	bs_ctd(maxbsm, pos, epos);
	return(maxbsm->bs_len);
}

#define ContWeight		1		/* = SetuWeight */
#define Len1Weight		16
#define Len2Weight		17
#define SetuWeight		1
#define SendWeight		50


int	calc_point(int pos, BSMEM *bsm1, BSMEM *bsm2)
{
	int len1, len2;
	int send;
	int pnt;

	if (bsm2 == NULL) {
		len1 = 0;
		len2 = bsm1->bs_len;
		send = 1;
	} else {
		len1 = bsm1->bs_len;
		len2 = bsm2->bs_len;
		send = 0;
	}
	pnt = bsm1->ctdprior * ContWeight
		+ (len1 * Len1Weight + len2 * Len2Weight)
		+ (setu_point(bsm1, bsm2) * SetuWeight)
		+ (send * SendWeight);
#ifdef	POINT
	printpnt(pos, bsm1, bsm2, pnt);
#endif
	return(pnt);
}

int bs_point(BSMEM *bsm, int pos, int epos)
{
	extern int henkan_mode;
	register BSMEM *bsm2;
	int len;
	PRIOR maxpnt, pnt;

	if (ignore_syn(bsm)) {
		bsm->itsprior = 0;
		return(0);
	}
	len = bsm->bs_len;
	if (bsm->bs_syn == BUN_END || pos + len >= epos) {		/* 1���� */
		maxpnt = (u_short)calc_point(pos, bsm, (BSMEM*)NULL);
	} else {												/* 2���� */
		if ((bsm2=hchar[pos + len].bs_mem) == NULL) {
			if (henkan_mode >= FREE_HENKAN)
				return(-1);
		}		
		maxpnt = 0;
		for ( ; bsm2; bsm2 = bsm2->bs_next) {
			pnt = (u_short)calc_point(pos, bsm, bsm2);
			if (pnt > maxpnt)
				maxpnt = pnt;
		}
	}
	bsm->itsprior = maxpnt;
	return(0);
}

void bs_prefix(int pos)
{
	register BSMEM *bsm;
	HCHAR *hp;
	CHMEM *ch;

	hp = &hchar[pos];
	/* �μӾ� ���� (���� 0 �� Ȯ�� ��� ) */
	for (bsm = hp->bs_mem; bsm; bsm = bsm->bs_next) {
		bsm->ctdprior = 0;
		if ((ch=bsm->wd_mem) != NULL) {
			if (ch->ch_syn == 25 && ch->yomilen == 0)
				bsm->ctdprior = 10;
		}
	}
}

void bs_ctd(BSMEM *bsm, int pos, int epos)
{
	int len;
	int pnt;
	BSMEM *bsm2;

	len = bsm->bs_len;
	if (bsm->bs_syn == BUN_END || pos + len >= epos)	/* 1���� */
		return;

	if ((bsm2=hchar[pos + len].bs_mem) == NULL) 
		return;

	for ( ; bsm2; bsm2 = bsm2->bs_next) {
		bsm2->ctdprior = 0;
		if (!ignore_syn(bsm2) && ((pnt=setu_point(bsm, bsm2)) > 0))
			bsm2->ctdprior = (u_short)pnt;
	}
}

/*------------------------------------------------------*/
/*		2���� �򰡷� �����ϴ� ǰ��						*/
/*				40 -> ��������						*/
/*				41 -> ��ȣ 							*/
/*		���� ǰ�絵 �����ϴ� �ĺ� 						*/
/*				26 -> ����							*/
/*				27 -> �θ�(����)						*/
/*				28 -> �θ�(�̸�)						*/
/*				29 -> �����	 						*/
/*				30 -> ������ü��						*/
/*------------------------------------------------------*/
int ignore_syn(BSMEM *bsm)
{
	CHMEM *ch;

	if ((ch=bsm->wd_mem) != NULL) {
		if (ch->ch_syn == 40 || ch->ch_syn == 41
			/* || (ch->ch_syn >= 26 && ch->ch_syn <= 30) */
			)
			return(1);
	}
	return(0);
}

/*------------------------------------------------------*/
/*		���� ����Ʈ�� ���̴� ���						*/
/*		0) ��1����(ǰ�� ��) �� �ܵ� �켱�� 				*/
/*		1) ��1������ Ȱ�� + ��2������ ǰ���� ���ռ� 		*/
/*		2) ���ξ� + ��2������ ǰ���� ���ռ� 				*/
/*		3) ��1������ ǰ�� + ���̾��� ���ռ�				*/
/*		4) ��2������ ǰ�� + ��2������ ǰ���� ���ռ�		*/
/*		5) ��1������ ���� + ��2������ ǰ���� ���ռ�		*/
/*------------------------------------------------------*/
int setu_point(BSMEM *bsm1, BSMEM *bsm2)
{
	SYNTAX syn, syn2, chsyn;
	KATUYO ktu, setu;
	CHMEM *ch;
	int pnt=0;

	ktu = setu = 0;
	if ((syn=bsm1->bs_syn) >= 45 && syn <= 56)
		setu = bsm1->bs_ktu;
	else
		ktu = bsm1->bs_ktu;
	if (bsm2 == NULL)
		syn2 = 0;
	else if ((ch=bsm2->wd_mem) == NULL)
		syn2 = 0;
	else {
		if ((syn2=ch->ch_syn) == 40)	/* �ܹ��� */
			return(0);
		if (syn2 == 41 || (syn2 == 25 && ch->yomilen > 0)
		   || (syn2 == 31 && ch->entid == -1))
			/* ��2������ ��ȣ, ���ڿ�, Ȯ���� */
			pnt += 20;
	}
	/*------------------------------------------------------*/
	/*		0) ��1����(ǰ�� ��)�� �ܵ� �켱�� 				*/
	/*------------------------------------------------------*/
	/* �μӾ� ���� (���� 0�� Ȯ�� ��� + �μӾ�)	 */
	if ((ch=bsm1->wd_mem) != NULL) {
		chsyn = ch->ch_syn;
		if (chsyn == 25 && ch->yomilen == 0)
			pnt += 10;
/* 2000/02/23 �θ�� ������ �켱������ �ø��� add s */
		else if (chsyn == 27 || chsyn == 28)
			pnt+=3;	/* �θ� */
		else if (chsyn ==26 )
			pnt+=2; /* ���� */
/* 2000/02/23 �θ�� ������ �켱������ �ø��� add e */
		else if (chsyn <= 25 || chsyn >=31)
			pnt++;	/* ������� �̿� */
	}
	/*------------------------------------------------------*/
	/*		1) ��1������ Ȱ�� + ��2 ������ ǰ���� ���ռ�		*/
	/*------------------------------------------------------*/
	if (ktu != 0 || syn == 13) {
		return(ktu_match(syn, ktu, syn2, pnt));
	}
	/*------------------------------------------------------*/
	/*		2) ���ξ� + ��2������ ǰ���� ���ռ�				*/
	/*------------------------------------------------------*/
	if (syn == 45) {
		return(setu_match(setu, syn2, NOCARE, pnt, 0));
	}
	/*------------------------------------------------------*/
	/*		3) ��1������ ǰ�� + ���̾��� ���ռ� 				*/
	/*------------------------------------------------------*/
	if (syn2 >= 50 && syn2 <= 56) {
		if ((ch=bsm2->wd_mem) != NULL)
			setu = ch->ch_ktu;
		else
			setu = 0;
		return(setu_match(setu, syn, ktu, pnt, 2));
	}
	/*------------------------------------------------------*/
	/*		4) ��1������ ǰ�� + ��2������ ǰ���� ���ռ�		*/
	/*------------------------------------------------------*/
	if (syn >= 20 && syn < 0xc0) {
		return(syn_match(syn, syn2, pnt));
	}
	/*------------------------------------------------------*/
	/*		5) ��1������ ���� + ��2������ ǰ���� ���ռ�		*/
	/*------------------------------------------------------*/
	if (syn >= 0xc0) {
		return(josi_match(bsm1, syn, syn2, pnt));
	}
	return(pnt);
}


int ktu_match(SYNTAX syn, KATUYO ktu, SYNTAX syn2, int pnt)
{
	/* 1�� ������ ������ (�) + ��� */
	if (syn == 13 && ktu == 0) {
		if (syn2 >= 1 && syn2 <= 23)
			return(pnt+3);
		goto ktu_nomatch;
	}
	/* ������ + ��� */
	if ((ktu >= 4 && ktu <= 6) && (syn2 >= 1 && syn2 <= 23)) {
		/* ������� ������ "��" / ���뵿���� ������ "��" */
		if (syn == 0x8d || (syn == 0x8e && ktu == 6))
			return(pnt+10);
		/* ������ ������ */
		else if (syn <= 0x8c)
			return(pnt+3);
		goto ktu_nomatch;
	}
	/* ��ü�� + ü�� */
	if (ktu == 8 && (syn2 >= 20 && syn2 <= 24)) {
		/* ������ü��  */
		/* ����, �����, ���뵿��, ���� "�ҿ���" "Ÿ" "�����" */
		/* "�����" "����" "Ÿ��" "����" */
		if ((syn >= 0x80 && syn <= 0x8e)
			|| syn == 146 || syn == 148 || syn == 160
			|| syn == 162 || syn == 174 || syn == 175 || syn == 177
			|| syn == 1 || (syn >= 10 && syn <= 12))
			return(pnt+15);
		/* ���� ��ü�� */
		else
			return(pnt+3);

	}

ktu_nomatch:
	return(pnt);
}


int syn_match(SYNTAX syn, SYNTAX syn2, int pnt)
{
	/* ü�� + ü�� */
	if (syn >= 20 && syn <= 24) {	
		if (syn2 >= 20 && syn2 <= 24)
			return(pnt+3);
		return(pnt);
	}
	/* ����/��ǥ�� + ����/��ǥ�� */
	if (syn == 31 && syn2 == 31) {
		return(pnt+15);
	}
	/* ���ӻ�, ��ü��, ���뱸 */
	if (syn == 33 || syn == 38 || syn == 39) {
		return(pnt+20);
	}
	/* ������, �λ� */
	if (syn == 32 || syn == 34) {
		return(pnt+15);
	}
	/* �θ�(��) + �θ� (�̸�) */
	if (syn == 27 && syn2 == 28) {
		return(pnt+20);
	}
	/* ���� + ����  */
	if (syn == 26 && syn2 == 26) {
		return(pnt+5);
	}
	return(pnt);
}

int josi_match(BSMEM *bsm1, SYNTAX syn, SYNTAX syn2, int pnt)
{
	CHMEM *ch;
	SYNTAX chsyn;
	int istaigen=0;

	if ((ch=bsm1->wd_mem) != NULL) {
		chsyn = ch->ch_syn;
		if ((chsyn >= 20 && chsyn <= 25) || chsyn == 50) {
			istaigen++;
			if (ch->yomilen + 1 == bsm1->bs_len)
				istaigen++;
		}
	}
	/* ���� "��" */
	if (syn == 0xc7) {
		/*  + ����  */
		return((syn2 >= 1 && syn2 <= 13) ? pnt+30 : pnt+25);
	}
	/* ��1 ���� == ü�� + ���� '��' */
	if (syn == 0xc8) {
		if (syn2 == 0)
			return(pnt);
		return((istaigen == 2) ? pnt+20 : pnt+5);
	}
	/* ��1����=ü��+�������� '��' '��' */
	if (syn == 0xc1 || syn == 0xe1) {
		return((istaigen == 2) ? pnt+5 : pnt);
	}
	/* ���� '��' '��' 'Ÿ��' '�ٸ�' '��' '��' + ���� */
	if (syn == 0xc4 || syn == 0xc5 || (syn >= 0xce && syn <= 0xd1)) {
		if ((syn2 >= 1 && syn2 <= 13) || (syn2 >= 22 && syn2 <= 23))
			return(pnt+5);
		else
			return(pnt);
	}
	/* ���� '��'  */
	if (syn == 0xc2) {
		if (syn2 == 0)
			return(pnt);
		/* ü�� + ���� ���� '��' */
		if (istaigen == 2)
			pnt += 3;
		return(pnt);
	}
	/* ��򡸪䡹�� ü�� */
	/* ��򡸪ȣ��������������ˣ��������������� */
	if ((syn >= 0xfc && syn <= 0xfd) && (syn2 >= 1 && syn2 <= 23)) {
		return(pnt+18);
	}
	/* ��򡸪Σ���������������������������?�� */
	if ((syn == 0xfa || syn == 0xfe) && (syn2 >= 20 && syn2 <= 24)) {
		return(pnt+18);
	}
	return(pnt);
}


int setu_match(KATUYO kind, KATUYO syn, SYNTAX ktu, int pnt, int add)
{
	int new = 0;

	switch (kind) {
	case 0:						/* ü�� */
		if ((syn >= 20 && syn <= 25) || syn == 50)
			new = 15;
		break;
	case 1:						/* ���� */
		if (syn == 26)
/* 2000/02/23 "�θ� + ��" �� �켱 chg s */
			/* new = 20; */
			new = 15;
/* 2000/02/23 "�θ� + ��" �� �켱 chg e */
		break;
	case 2:						/* �θ� */
		if (syn == 27 || syn == 28)
/* 2000/02/23 "�θ� + ��" �� �켱 chg s */
			/* new = 15; */
			new = 20;
/* 2000/02/23 "�θ� + ��" �� �켱 chg e */
		break;
	case 3:						/* ����/��ǥ�� */
		if (syn == 31 || syn == 56)
			new = 20;
		break;
	case 4:						/* ���� ���� ��� */
		if (syn == 22)
			new = 20;
		break;
	case 5:						/* ������ ������ */
		/* ���ξ��� 嶧�� ���� */
		if (ktu == NOCARE) {
			if (syn >= 1 && syn <= 13)
				new = 15;
		} else {
			if ((syn >= 0x80 && syn <= 0x8b && ktu == 4) ||
				(syn == 13 && ktu == 0)) /* �Ϲ� */
				new = 15;
			}
			break;
	case 6:						/* ����/��ǥ�� + ü�� */
		if (syn >= 20 && syn <= 25)
			new = 15;
		else if (syn == 31 || syn == 56)
			new = 20;
		break;
	case 7:						/* ����� */
		if (syn == 29)
			new = 15;
		break;
	case 8:						/* ���ξ� '��' */
		if ((syn >= 20 && syn <= 25) || (syn >= 1 && syn <= 13))
			new = 20;
		break;
	}
	if (new == 0)
		return(pnt);
	else
		return(pnt + new + add);
}

/*------------------------------*/
/*	   �ĺ��� �켱���� ���Ѵ�. 	*/
/*------------------------------*/

PRIOR kh_priority(BSMEM *bsm, RTIME rtime)
{
	PRIOR pnt;

	if (rtime != 0)		/* �н��Ǿ� ����. */
		pnt = (u_short)(1000 + rtime);
	else
		pnt = bsm->itsprior;
	return(pnt);
}

#ifdef POINT
/*----------------------*/
/* ������ ������ ǥ��	*/
/* for ����			*/
/*----------------------*/
void printpnt(int pos, BSMEM *bsm1, BSMEM *bsm2, int pnt)
{
	char *hinsi_of(), *katu_of();
	int syn;

	printf("%d: ", pos);
	/* ��1���� */
	printf("�O����(");
	if (bsm1 == NULL)
		printf("������, len1=0");
	else {
		if (bsm1->wd_mem == NULL)
			syn = 0;
		else
			syn = bsm1->wd_mem->ch_syn;
		if (syn == 0) {
			printf("���P��,len1=%d", bsm1->bs_len);
		} else if (syn == bsm1->bs_syn) {
			printf("%s%s,len1=%d",
					hinsi_of(bsm1->bs_syn), 
					katu_of(bsm1->bs_syn, bsm1->bs_ktu),
					bsm1->bs_len);
		} else {
			printf("%s+%s%s, len1=%d",
					hinsi_of(syn),
					hinsi_of(bsm1->bs_syn), 
					katu_of(bsm1->bs_syn, bsm1->bs_ktu),
					bsm1->bs_len);
		}
	}
	printf(")+");

	/* ��1���� */
	printf("�㕶��(");
	if (bsm2 == NULL)
		printf("������,len2=0");
	else {
		if (bsm2->wd_mem == NULL)
			syn = 0;
		else
			syn = bsm2->wd_mem->ch_syn;
		if (syn == 0) {
			printf("���P��,len2=%d", bsm2->bs_len);
		} else if (syn == bsm2->bs_syn) {
			printf("%s%s,len2=%d",
					hinsi_of(bsm2->bs_syn), 
					katu_of(bsm2->bs_syn, bsm2->bs_ktu),
					bsm2->bs_len);
		} else {
			printf("%s+%s%s, len2=%d",
					hinsi_of(syn),
					hinsi_of(bsm2->bs_syn), 
					katu_of(bsm2->bs_syn, bsm2->bs_ktu),
					bsm2->bs_len);
		}
	}
	printf(")=");

	printf("%d\n", pnt);
}

char *hinsi_tab[] = {
	"",				/* 0 */
	"���J��",		/* 1 */
	"���K��",		/* 2 */
	"���T��",		/* 3 */
	"���^��",		/* 4 */
	"���i��",		/* 5 */
	"���o��",		/* 6 */
	"���}��",		/* 7 */
	"������",		/* 8 */
	"������",		/* 9 */
	"���T��",		/* 10 */
	"���U��",		/* 11 */
	"���J��",		/* 12 */
	"����i",		/* 13 */
	"�`�e�O",		/* 14 */
	"�`�e�P",		/* 15 */
	"�`�e�Q",		/* 16 */
	"�`�e�R",		/* 17 */
	"�`�e�S",		/* 18 */
	"�`�e��",		/* 19 */
	"�`�e��",		/* 20 */
	"�^���g",		/* 21 */
	"�T�ϖ�",		/* 22 */
	"�U�ϖ�",		/* 23 */
	"��  ��",		/* 24 */
	"�g����",		/* 25 */
	"�n  ��",		/* 26 */
	"�l����",		/* 27 */
	"�l����",		/* 28 */
	"�c�̖�",		/* 29 */
	"������",		/* 30 */
	"���\��",		/* 31 */
	"������",		/* 32 */
	"�ڑ���",		/* 33 */
	"�����O",		/* 34 */
	"�����P",		/* 35 */
	"�����Q",		/* 36 */
	"�����R",		/* 37 */
	"�A�̎�",		/* 38 */
	"���p��",		/* 39 */
	"�P����",		/* 40 */
	"�p�L��",		/* 41 */
	"",				/* 42 */
	"",				/* 43 */
	"",				/* 44 */
	"�ړ���",		/* 45 */
	"",				/* 46 */
	"",				/* 47 */
	"",				/* 48 */
	"",				/* 49 */
	"�ږ���",		/* 50 */
	"�ڃT��",		/* 51 */
	"�ڌ`��",		/* 52 */
	"�ڌ`�e",		/* 53 */
	"�ڃJ��",		/* 54 */
	"�ڃ���",		/* 55 */
	"�ڐ���"		/* 56 */
};

char *katu_tab[] = {
	"",				/* 0 */
	":���P",		/* 1 */
	":���Q",		/* 2 */
	":���R",		/* 3 */
	":�p�P",		/* 4 */
	":�p�Q",		/* 5 */
	":�p�R",		/* 6 */
	":�I�~",		/* 7 */
	":�A��",		/* 8 */
	":����",		/* 9 */
	":���P",		/* 10 */
	":���Q"			/* 11 */
};

char *setu_tab[] = {
	":�̌�",		/* 0 */
	":�n��",		/* 1 */
	":�l��",		/* 2 */
	":����",		/* 3 */
	":�T��",		/* 4 */
	":����",		/* 5 */
	":����"			/* 6 */
};

static	char jodobuf[] = "����??";
static	char josibuf[] = "����??";

char *hinsi_of(int syn)
{
	int n;

	if (syn == 0xff) {
		return("��Ǔ_");
	} else if (syn < 64) {
		return(hinsi_tab[syn]);
	} else if (syn < 0xc0) {
		n = syn - 0x80;
		jodobuf[4] = '0' + n / 10;
		jodobuf[5] = '0' + n % 10;
		return(jodobuf);
	} else {
		n = syn - 0xc0;
		josibuf[4] = '0' + n / 10;
		josibuf[5] = '0' + n % 10;
		return(josibuf);
	}
}


char *katu_of(int syn, int ktu)
{
	if (syn >= 45 && syn <= 56)		/* ���ξ� / ���̾� */
		return(setu_tab[ktu]);
	else
		return(katu_tab[ktu]);
}
#endif
