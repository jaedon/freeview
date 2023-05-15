#ifndef _P_REC_H_
#define _P_REC_H_

#if	defined(CONFIG_90MIN_TSR_BUFFER)

#define	TSR_TIME_LIMIT			5400 /* 90 min */
/* 그냥 기존 사용하던 대로 120min size 유지 */
#if	defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
/* NEXUS에서 188k align을 맞추기 때문에 LCM(188,192)의 배수로 설정. LCM(188,192) == 192*47 */
#define	TSR_DATA_SOFT_LIMIT			((long long)18048*1024*1024) //20Mbps stream 2시간 분량 기준 ..(18048 = 192*94)
#define	TSR_DATA_HARD_LIMIT			(((long long)18048*1024*1024) + ((long long)1536*987*1024)) // 2시간 + 10분 분량 (1536 = 192*8, 1034 = 47*21)
#else
#define	TSR_DATA_SOFT_LIMIT			((long long)17672*1024*1024) //20Mbps stream 2시간 분량 기준 ..(17672 = 188*94)
#define	TSR_DATA_HARD_LIMIT			(((long long)17672*1024*1024) + ((long long)1504*1024*1024)) // 2시간 + 10분 분량 (1504 = 188*8)
#endif
#define	TSR_INDEX_SOFT_LIMIT			((long long)150*1024*1024) // test 후 조절 필요 ..
#define	TSR_INDEX_HARD_LIMIT			((long long)160*1024*1024)

#elif	defined(CONFIG_150MIN_TSR_BUFFER)

#define	TSR_TIME_LIMIT			9000 /* 150 min */
#if	defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
/* NEXUS에서 188k align을 맞추기 때문에 LCM(188,192)의 배수로 설정. LCM(188,192) == 192*47 */
#define	TSR_DATA_SOFT_LIMIT			((long long)22656*1024*1024) //20Mbps stream 2시간 30분 분량 기준 ..(22656 = 192*(94+24))
#define	TSR_DATA_HARD_LIMIT			(((long long)22656*1024*1024) + ((long long)1536*987*1024)) // 2시간 30분 + 10분 분량 (1536 = 192*8, 1034 = 47*21)
#else
#define	TSR_DATA_SOFT_LIMIT			((long long)22184*1024*1024) //20Mbps stream 2시간 30분 분량 기준 ..(22184 = 188*(94+24))
#define	TSR_DATA_HARD_LIMIT			(((long long)22184*1024*1024) + ((long long)1504*1024*1024)) // 2시간 30분 + 10분 분량 (1504 = 188*8)
#endif
#define	TSR_INDEX_SOFT_LIMIT			((long long)180*1024*1024) // test 후 조절 필요 ..
#define	TSR_INDEX_HARD_LIMIT			((long long)190*1024*1024)

#elif	defined(CONFIG_60MIN_TSR_BUFFER)

#define	TSR_TIME_LIMIT			3600 /* 60 min */
/* 그냥 기존 사용하던 대로 120min size 유지 */
#if	defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
/* NEXUS에서 188k align을 맞추기 때문에 LCM(188,192)의 배수로 설정. LCM(188,192) == 192*47 */
#define	TSR_DATA_SOFT_LIMIT			((long long)18048*1024*1024) //20Mbps stream 2시간 분량 기준 ..(18048 = 192*94)
#define	TSR_DATA_HARD_LIMIT			(((long long)18048*1024*1024) + ((long long)1536*987*1024)) // 2시간 + 10분 분량 (1536 = 192*8, 1034 = 47*21)
#else
#define	TSR_DATA_SOFT_LIMIT			((long long)17672*1024*1024) //20Mbps stream 2시간 분량 기준 ..(17672 = 188*94)
#define	TSR_DATA_HARD_LIMIT			(((long long)17672*1024*1024) + ((long long)1504*1024*1024)) // 2시간 + 10분 분량 (1504 = 188*8)
#endif
#define	TSR_INDEX_SOFT_LIMIT			((long long)150*1024*1024) // test 후 조절 필요 ..
#define	TSR_INDEX_HARD_LIMIT			((long long)160*1024*1024)

#elif defined(CONFIG_90MIN_TSR_BUFFER_FOR_HIGH_BITRATE_30MBPS)

#define	TSR_TIME_LIMIT			5400 /* 90 min */
/* 그냥 기존 사용하던 대로 120min size 유지 */
/* NEXUS에서 188k align을 맞추기 때문에 LCM(188,192)의 배수로 설정. LCM(188,192) == 192*47 */
#define	TSR_DATA_SOFT_LIMIT			((long long)2892288*192*47) //20Mbps stream 2시간 분량 기준 ..(18048 = 192*94)
#define	TSR_DATA_HARD_LIMIT			(((long long)2892288*192*47) + ((long long)361536*192*47)) // 2시간 + 10분 분량 (1536 = 192*8, 1034 = 47*21)
#define	TSR_INDEX_SOFT_LIMIT			((long long)150*1024*1024) // test 후 조절 필요 ..
#define	TSR_INDEX_HARD_LIMIT			((long long)160*1024*1024)

#elif defined(CONFIG_150MIN_TSR_BUFFER_FOR_HIGH_BITRATE_30MBPS)

#define	TSR_TIME_LIMIT			9000 /* 150 min */
/* NEXUS에서 188k align을 맞추기 때문에 LCM(188,192)의 배수로 설정. LCM(188,192) == 192*47 */
#define	TSR_DATA_SOFT_LIMIT			((long long)3615360*192*47) //20Mbps stream 2시간 30분 분량 기준 ..(22656 = 192*(94+24))
#define	TSR_DATA_HARD_LIMIT			(((long long)3615360*192*47) + ((long long)361536*192*47)) // 2시간 30분 + 10분 분량 (1536 = 192*8, 1034 = 47*21)
#define	TSR_INDEX_SOFT_LIMIT			((long long)180*1024*1024) // test 후 조절 필요 ..
#define	TSR_INDEX_HARD_LIMIT			((long long)190*1024*1024)

#elif defined(CONFIG_60MIN_TSR_BUFFER_FOR_HIGH_BITRATE_30MBPS)

#define	TSR_TIME_LIMIT			3600 /* 60 min */
/* 그냥 기존 사용하던 대로 120min size 유지 */
/* NEXUS에서 188k align을 맞추기 때문에 LCM(188,192)의 배수로 설정. LCM(188,192) == 192*47 */
#define	TSR_DATA_SOFT_LIMIT			((long long)2892288*192*47) //20Mbps stream 2시간 분량 기준 ..(18048 = 192*94)
#define	TSR_DATA_HARD_LIMIT			(((long long)2892288*192*47) + ((long long)361536*192*47)) // 2시간 + 10분 분량 (1536 = 192*8, 1034 = 47*21)
#define	TSR_INDEX_SOFT_LIMIT			((long long)150*1024*1024) // test 후 조절 필요 ..
#define	TSR_INDEX_HARD_LIMIT			((long long)160*1024*1024)

#elif defined(CONFIG_120MIN_TSR_BUFFER_FOR_HIGH_BITRATE_30MBPS)

#define	TSR_TIME_LIMIT			7200 /* 120 min */
/* NEXUS에서 188k align을 맞추기 때문에 LCM(188,192)의 배수로 설정. LCM(188,192) == 192*47 */
#define	TSR_DATA_SOFT_LIMIT			((long long)2892288*192*47) //20Mbps stream 2시간 분량 기준 ..(18048 = 192*94)
#define	TSR_DATA_HARD_LIMIT			(((long long)2892288*192*47) + ((long long)361536*192*47)) // 2시간 + 10분 분량 (1536 = 192*8, 1034 = 47*21)
#define	TSR_INDEX_SOFT_LIMIT			((long long)150*1024*1024) // test 후 조절 필요 ..
#define	TSR_INDEX_HARD_LIMIT			((long long)160*1024*1024)

#else /*CONFIG_120MIN_TSR_BUFFER : default */

#define	TSR_TIME_LIMIT			7200 /* 120 min */
#if	defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
#if defined(CONFIG_HDOUT_2160P)
#define  TSR_DATA_SOFT_LIMIT ((long long)168*192*1024*1024) //35Mbps stream 2시간 분량 기준 ..(32256 = 192*168)
#define  TSR_DATA_HARD_LIMIT (((long long)TSR_DATA_SOFT_LIMIT) + ((long long)14*192*1024*1024)) // 2시간 + 10분 분량 (2688 = 192*14, 1034 = 47*21)
#else
/* NEXUS에서 188k align을 맞추기 때문에 LCM(188,192)의 배수로 설정. LCM(188,192) == 192*47 */
#define	TSR_DATA_SOFT_LIMIT			((long long)18048*1024*1024) //20Mbps stream 2시간 분량 기준 ..(18048 = 192*94)
#define	TSR_DATA_HARD_LIMIT			(((long long)18048*1024*1024) + ((long long)1536*987*1024)) // 2시간 + 10분 분량 (1536 = 192*8, 1034 = 47*21)
#endif
#else
#if defined(CONFIG_HDOUT_2160P)
#define  TSR_DATA_SOFT_LIMIT ((long long)168*188*1024*1024) //35Mbps stream 2시간 분량 기준 ..(32256 = 192*168)
#define  TSR_DATA_HARD_LIMIT (((long long)TSR_DATA_SOFT_LIMIT) + ((long long)14*188*1024*1024)) // 2시간 + 10분 분량 (2688 = 192*14, 1034 = 47*21)
#else
#define	TSR_DATA_SOFT_LIMIT			((long long)17672*1024*1024) //20Mbps stream 2시간 분량 기준 ..(17672 = 188*94)
#define	TSR_DATA_HARD_LIMIT			(((long long)17672*1024*1024) + ((long long)1504*1024*1024)) // 2시간 + 10분 분량 (1504 = 188*8)
#endif
#endif
#if defined(CONFIG_HDOUT_2160P)
#define  TSR_INDEX_SOFT_LIMIT              ((long long)270*1024*1024) // test 후 조절 필요 ..
#define  TSR_INDEX_HARD_LIMIT             ((long long)288*1024*1024)
#else
#define	TSR_INDEX_SOFT_LIMIT			((long long)150*1024*1024) // test 후 조절 필요 ..
#define	TSR_INDEX_HARD_LIMIT			((long long)160*1024*1024)

#endif
#endif

#define ITB_BUFFER_CHUNK_SIZE	(6*4*16)
#if defined(CONFIG_DOUBLE_SIZE_RECORD_BUFFER)
#define ITB_BUFFER_CHUNK_COUNT	192  /*actually over than double*/
#else
#define ITB_BUFFER_CHUNK_COUNT	48
#endif

#endif /*_P_REC_H_*/
