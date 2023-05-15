#include	<otl.h>
#include	<octo_common.h>
#include	<fcntl.h>

#define OTL_LOG_REPORT_BUFFER_SIZE	2048

#define LR_OBAMA_RECORDER_STATE	"/var/.lr_obama_recorder_state.log"

#if 0
STATIC HCHAR s_str_OBAMA_RECORDER_buffer[OTL_LOG_REPORT_BUFFER_SIZE];
#endif

HERROR OTL_LOG_REPORT_OBAMA_RECORDER_MEMO_BEGIN (void)
{
#if 0
	HxSTD_MemSet (&s_str_OBAMA_RECORDER_buffer, 0, sizeof(HCHAR)*OTL_LOG_REPORT_BUFFER_SIZE);
#endif
	return ERR_OK;
}

HERROR OTL_LOG_REPORT_OBAMA_RECORDER_MEMO_END (void)
{
#if 0
	//<--UNSAFE
	{
		int nFd = -1;

		if ( 0 < (nFd = open (LR_OBAMA_RECORDER_STATE, O_WRONLY | O_CREAT, 0644)) )
		{
		  write (nFd, s_str_OBAMA_RECORDER_buffer, strlen(s_str_OBAMA_RECORDER_buffer));
		  close (nFd);
		}
		else return ERR_FAIL;

	}
	//-->UNSAFE
#endif
	return ERR_OK;
}


HERROR OTL_LOG_REPORT_OBAMA_RECORDER_MEMO_INT (HCHAR * pStrKey, HINT32 nValue)
{
#if 0
	if (NULL == pStrKey)
		return ERR_FAIL;

	sprintf(s_str_OBAMA_RECORDER_buffer, "%s\n\r%s : %d", s_str_OBAMA_RECORDER_buffer, pStrKey, nValue);

#endif
	return ERR_OK;
}
