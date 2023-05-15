/** **********************************************************************************************************
	@file 		mw_mp_manager.c

	@date		2012/01/03
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
	@breif

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */


#include "_svc_mp_manager.h"
#include "svc_mplayer.h"
#include "pal_media.h"

#include "vkernel.h"

/*---------------------------------------------------------------------------------
 * defines
 *---------------------------------------------------------------------------------*/
#define MP_MGR_SEMAPHORE_NAME		"MediaPlayerMgrSem"



/*---------------------------------------------------------------------------------
 * typedef
 *---------------------------------------------------------------------------------*/
typedef struct _mp_manager_t {
	SvcMp_MediaPlayer_t	*head;
	HULONG				lock;
} mp_manager_t;



/*---------------------------------------------------------------------------------
 * static variables
 *---------------------------------------------------------------------------------*/
static mp_manager_t		s_mp_manager;



/*---------------------------------------------------------------------------------
 * static functions
 *---------------------------------------------------------------------------------*/
static inline PAL_MEDIA_t *__pal(void *player)
{
	return ((SvcMp_MediaPlayer_t *)player)->phyMediaHandle;
}

static inline SvcMp_MediaPlayer_t *__mp(void *player)
{
	return (SvcMp_MediaPlayer_t *)player;
}



/*---------------------------------------------------------------------------------
 * Interfaces
 *---------------------------------------------------------------------------------*/
void media_player_manager_init(void)
{
	s_mp_manager.head = NULL;

	if ( VK_SEM_Create(&(s_mp_manager.lock), MP_MGR_SEMAPHORE_NAME,
			VK_SUSPENDTYPE_PRIORITY ) != VK_OK )
	{
		return;
	}
}

void media_player_manager_uninit(void)
{
	VK_SEM_Destroy(s_mp_manager.lock);

	media_player_manager_del_all();
	s_mp_manager.head = NULL;
}

void media_player_manager_lock(void)
{
	VK_SEM_Get(s_mp_manager.lock);
}

void media_player_manager_unlock(void)
{
	VK_SEM_Release(s_mp_manager.lock);
}

HERROR media_player_manager_add(void *player)
{
	SvcMp_MediaPlayer_t *mp = __mp(player);

	if ( mp == NULL ) return ERR_FAIL;

	media_player_manager_lock();

	if ( s_mp_manager.head == NULL )
	{
		s_mp_manager.head = mp;
	}
	else
	{
		SvcMp_MediaPlayer_t *temp = s_mp_manager.head;
		s_mp_manager.head = mp;
		mp->next = temp;
	}

	media_player_manager_unlock();

	return ERR_OK;
}

void media_player_manager_del(void *player)
{
	SvcMp_MediaPlayer_t *pos = NULL;
	SvcMp_MediaPlayer_t *prev = NULL;
	SvcMp_MediaPlayer_t *mp = __mp(player);

	media_player_manager_lock();

	pos = prev = s_mp_manager.head;

	while ( pos != NULL )
	{
		if ( pos == mp )
		{
			if ( pos == s_mp_manager.head )
			{
				s_mp_manager.head = pos->next;
			}
			else
			{
				prev->next = pos->next;
			}
			media_player_manager_unlock();
			return;
		}
		prev = pos;
		pos = pos->next;
	}

	media_player_manager_unlock();
}

void media_player_manager_del_all(void)
{
	SvcMp_MediaPlayer_t *pos = NULL;
	SvcMp_MediaPlayer_t *next = NULL;

	media_player_manager_lock();

	pos = next = s_mp_manager.head;

	while ( pos != NULL )
	{
		next = pos->next;
		HLIB_STD_MemFree(pos);
		pos = next;
	}

	media_player_manager_unlock();
}

void *media_player_manager_find(void *identifier)
{
	SvcMp_MediaPlayer_t *pos = NULL;

	media_player_manager_lock();

	pos = s_mp_manager.head;
	while ( pos != NULL )
	{
		if ( __pal(pos)->pIdentifier == identifier )
		{
			media_player_manager_unlock();
			return pos;
		}
		pos = pos->next;
	}

	media_player_manager_unlock();

	return NULL;
}

void *media_player_manager_find_by_action(HUINT32 hActionId)
{
	SvcMp_MediaPlayer_t *pos = NULL;

	media_player_manager_lock();

	pos = s_mp_manager.head;
	while ( pos != NULL )
	{
		if ( pos->hActionId == hActionId )
		{
			media_player_manager_unlock();
			return pos;
		}
		pos = pos->next;
	}

	media_player_manager_unlock();

	return NULL;
}

HINT32 media_player_manager_get_count(void)
{
	HINT32 nCount = 0;
	SvcMp_MediaPlayer_t *pos = NULL;

	media_player_manager_lock();

	pos = s_mp_manager.head;
	while ( pos != NULL )
	{
		nCount++;
		pos = pos->next;
	}

	media_player_manager_unlock();

	return nCount;
}

void *media_player_manager_get_first(void)
{
	return s_mp_manager.head;
}

void *media_player_manager_get_next(void *player)
{
	SvcMp_MediaPlayer_t *pos = NULL;
	SvcMp_MediaPlayer_t *mp = __mp(player);

	pos = media_player_manager_find(__pal(mp)->pIdentifier);

	return (pos) ? pos->next : pos;
}
