/**************************************************************
 *	@file		nxaging.c
 *	Netflix Manager Aging tools for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2016/01/21
 *	@author			humax
 *	@brief			Aging tools application for Netflix Manager
 **************************************************************/



#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include 	<sys/signal.h>
#include 	<sys/socket.h>
#include 	<sys/un.h>

#include	<unistd.h>
#include 	<wait.h>
#include 	<fcntl.h>

#include	<directfb.h>
#include	<sawman.h>

#include	<vkernel.h>
#include	<hlib.h>
#include	<oapi.h>
#include	<hapi.h>
#include	<papi.h>
#include	<apk.h>

//#include	<pcdapi.h>

#ifndef WIN32
#include <curl/curl.h>
#endif

#if defined(CONFIG_ENHANCED_SECURITY)
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/resource.h>
#endif

#include <di_flash.h>

#include <netflixmanager.h>

#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN		"NetflixAging"

#define EXTERNAL_SCENARIO   "/media/drive1/nx_aging.cfg"

#if defined(CONFIG_DEBUG)
#define	NXTool_LOG			printf//("\033[30;44m [%s:%d] %s\033[0m\n", __FUNCTION__, __LINE__, a)
#else
#define	NXTool_LOG			printf
#endif


typedef enum
{    
	NX_AGING_RESUME_SUSPEND,
    NX_AGING_RESUME_SUSPEND_ON_GALLERY,
	NX_AGING_RESUME_SUSPEND_ON_PLAYBACK,
	NX_AGING_ZAPPING_NORMAL,
	NX_AGING_ZAPPING_FOR_NETFLIX,
	NX_AGING_LAUNCH_IN_RBPLUS,
	NX_AGING_STANDBY_ON_OFF,
	NX_AGING_INTERNAL_SCENARIO_MAX, // EXTERNAL_SCENARIO from USB	
}NxAging_Type_t;

static HUINT8   *pszAgingList[NX_AGING_INTERNAL_SCENARIO_MAX+1] =
                    {
                        "0 : AGING_RESUME_SUSPEND : Let's start at profile or gallery display of Netflix",
                        "1 : AGING_RESUME_SUSPEND_ON_GALLERY : Let's start at gallery of Netflix",
                    	"2 : AGING_RESUME_SUSPEND_ON_PLAYBACK: Let's start at gallery of Netflix",
                    	"3 : AGING_ZAPPING_NORMAL : Let's start with favorite group without netflix(ch)",
                    	"4 : AGING_ZAPPING_FOR_NETFLIX : Let's start at ch 2 with tv group with netflix(ch)",
                    	"5 : AGING_LAUNCH_IN_RB+",                    	
                    	"6 : AGING_STANDBY_ON_OFF",
                    	"7 : AGING_WITH_External_Scenario : Insert USB Memory that have nx_aging.cfg file in the root directory",
                    };

typedef struct _tag_NxAging_node_t
{    
	HUINT32             ulKeyCode;
	HUINT32				ulWaitTimeBefore;
    HUINT32             ulWaitTimeAfter;
    HBOOL               bSendInfinity;
    HUINT32             ulSendCnt;                   
} NxAging_node_t;


typedef struct _tag_NxAging_Scenario_t
{    
	HUINT32             ulNodeNum;
    HUINT32             ulTryCnt;
	NxAging_node_t      stScenario[15];
    
} NxAging_Scenario_t;


/**************************************************************
	internal structures and variables
 **************************************************************/

static NxAging_Scenario_t  s_stExternalScenario;

static NxAging_Scenario_t  s_stAgingScenario[NX_AGING_INTERNAL_SCENARIO_MAX] =
        {
            //NX_AGING_RESUME_SUSPEND
            {
                2,
                0,
                { 
                    {DIKS_EXIT,         0, 4000, TRUE, 0},
                    {DIKS_CUSTOM68,     0, 4000, TRUE, 0}, 
                }
            },
            //NX_AGING_RESUME_SUSPEND_ON_GALLERY
            {
                7,
                0,
                {
                    {DIKS_CURSOR_DOWN,  1000, 1000, TRUE,   0},
                    {DIKS_CURSOR_DOWN,  0, 1000, TRUE,   0},
                    {DIKS_CURSOR_RIGHT, 0, 1000, TRUE,   0},
                    {DIKS_CURSOR_RIGHT, 0, 1000, TRUE,   0},
                    {DIKS_CURSOR_DOWN,  0, 1000, TRUE,   0},
                    {DIKS_EXIT,         0, 4000, TRUE,   0},
                    {DIKS_CUSTOM68,     0, 1000, TRUE,   0}, 
                }
            },    
            //NX_AGING_RESUME_SUSPEND_ON_PLAYBACK
            {
                6,
                0,
                {            
                    {DIKS_CURSOR_DOWN,  1000, 1000, FALSE,  1},
                    {DIKS_CURSOR_DOWN,  0, 1000, FALSE,  1},
                    {DIKS_ENTER,        0, 3000, FALSE,  1}, // MDP
                    {DIKS_ENTER,        0, 40000, TRUE,   0}, // start playback
                    {DIKS_EXIT,         0, 1000, TRUE,   0},   
                    {DIKS_CUSTOM68,     3000, 5000, TRUE,   0}, 
                }
            },
            //NX_AGING_ZAPPING_FOR_NETFLIX => except without nextflix(use favorite group)
            {
                1,
                0,
                {
                    {DIKS_CHANNEL_UP,     5000, 0, TRUE,   0},       
                }
            },
            //NX_AGING_ZAPPING_FOR_NETFLIX => Have to start at channel 2 
            {
                8,
                0,
                {
                    {DIKS_CHANNEL_DOWN,     0, 5000, TRUE,   0}, // 2->1
                    {DIKS_CHANNEL_DOWN,     0, 5000, TRUE,   0}, // 1->Netflix
                    {DIKS_CHANNEL_DOWN,     0, 5000, TRUE,   0}, // Netflix->99
                    {DIKS_CHANNEL_DOWN,     0, 5000, TRUE,   0}, // 99->95
                    {DIKS_CHANNEL_UP,       0, 5000, TRUE,   0}, // 95->99
                    {DIKS_CHANNEL_UP,       0, 5000, TRUE,   0}, // 99->Netflix
                    {DIKS_CHANNEL_UP,       0, 5000, TRUE,   0}, // Netflix->1
                    {DIKS_CHANNEL_UP,       0, 5000, TRUE,   0}, // 1->2   
                }
            },            
            //NX_AGING_LAUNCH_IN_RBPLUS => Have to start at channel that support RB+ 
            {
                3,
                0,
                {
                    {DIKS_RED,              17000, 35000, TRUE,   0}, // RB+
                    {DIKS_CUSTOM68,         0, 20000, TRUE,   0}, // Netflix
                    {DIKS_EXIT        ,     0, 3000, TRUE,   0}, // Netflix->99                    
                }
            },
            //NX_AGING_STANDBY_ON_OFF  
            {
                2,
                0,
                {
                    {DIKS_POWER,         180000, 1000, TRUE,   0}, 
                    {DIKS_POWER,         45000, 1000, TRUE,   0}, 
                }
            }     
        };



int		main(int argc, char **argv)
{
    HUINT32             i, ulScenario;
    NxAging_Scenario_t  *pAgingScenario = NULL;

    if(argc < 2)
    {
        NXTool_LOG("\n\n @@@[NxAging] No parameter...!! => Exit\n");
        NXTool_LOG("\tParameter : \n");        

        for(i = 0; i <= NX_AGING_INTERNAL_SCENARIO_MAX; i++)
        {
            NXTool_LOG("\t %s\n", pszAgingList[i]);
        }
        NXTool_LOG("  \t  Example for nx_aging.cfg : \n");
        NXTool_LOG("  \t     2                      # node number\n");
        NXTool_LOG("  \t     F062, 1000, 5000, 0, 3 # keycode(DIKS_EXIT), beforeWait(ms), afterWait(ms), infinityFlag, sendCount\n");
        NXTool_LOG("  \t     F544, 0, 5000, 1, 0    # keycode(DIKS_CUSTOM68), beforeWait(ms), afterWait(ms), infinityFlag, sendCount\n");
        NXTool_LOG("  \t  *Please, refer to  directfb_keyboard.h for keycode.\n");     
        NXTool_LOG("  \t  *If infinityFlag is 1(true), Don't care sendCount.\n");       
        NXTool_LOG("  \t  *# is indicater for line-comment.\n\n\n");
        
        return 0;
    }

    ulScenario = atoi(argv[1]);

    if(ulScenario > NX_AGING_INTERNAL_SCENARIO_MAX)
    {
        NXTool_LOG("\n\n @@@[NxAging] Invalid Scenario...!! => Exit\n");    
        return 0;
    }
    else if(ulScenario == NX_AGING_INTERNAL_SCENARIO_MAX)
    {
        struct      stat sts;
        HxFILE_t	fExtScenario = NULL;
        HBOOL       bLoadingOK = FALSE;
        HUINT8      i, j, k, ucChar;
        HUINT8       szBuf[256];
        HBOOL       bComment;
        
        HxSTD_memset(&s_stExternalScenario, 0, sizeof(NxAging_Scenario_t));
        NXTool_LOG("\n\n [NxAging] You select external scenario => Try to loading scenario(nx_aging.cfg) from USB..\n");
   
        if(stat(EXTERNAL_SCENARIO, &sts) == -1 && errno == ENOENT)
        {
            NXTool_LOG("\n\n [NxAging] No %s file..!!\n", EXTERNAL_SCENARIO);
            goto EXIT_LOAD;
        }

        fExtScenario = HLIB_FILE_Open(EXTERNAL_SCENARIO, "r");
        if(fExtScenario == NULL)
        {
            NXTool_LOG("\n\n [NxAging] Can't open %s file..!!\n", EXTERNAL_SCENARIO);
            goto EXIT_LOAD;
        }

        i = j = k = 0;
        bComment = FALSE;
        while((ucChar = (HUINT8)HLIB_FILE_GetChar(fExtScenario)) != EOF)
        {
            if(i == 0 && ucChar == '#') bComment = TRUE;
            if(bComment == FALSE)
            {
                if(ucChar == '\n' || ucChar == ',' || ucChar == '#')
                {       
                    szBuf[i] = 0;
                    //NXTool_LOG("\t[NxAging] szBuf : %s\n", szBuf);

                    if(j == 0)
                    {
                        s_stExternalScenario.ulNodeNum = atoi(szBuf);   
                        NXTool_LOG("\t[NxAging] s_stExternalScenario.ulNodeNum : %d\n", s_stExternalScenario.ulNodeNum);
                        j++;                    
                    }
                    else
                    {
                        switch(k)
                        {
                            case 0: // key code
                                s_stExternalScenario.stScenario[j-1].ulKeyCode = atoi(szBuf);
                                NXTool_LOG("\t[NxAging] s_stExternalScenario.stScenario[%d].ulKeyCode : %d\n", j-1, s_stExternalScenario.stScenario[j-1].ulKeyCode);
                                k++;
                                break;
                            case 1: // before wait time (ms)
                                s_stExternalScenario.stScenario[j-1].ulWaitTimeBefore = atoi(szBuf);
                                NXTool_LOG("\t[NxAging] s_stExternalScenario.stScenario[%d].ulWaitTimeBefore : %d\n", j-1, s_stExternalScenario.stScenario[j-1].ulWaitTimeBefore);
                                k++;
                                break;
                            case 2: // after wait time (ms)
                                s_stExternalScenario.stScenario[j-1].ulWaitTimeAfter = atoi(szBuf);
                                NXTool_LOG("\t[NxAging] s_stExternalScenario.stScenario[%d].ulWaitTimeAfter : %d\n", j-1, s_stExternalScenario.stScenario[j-1].ulWaitTimeAfter);
                                k++;
                                break;
                            case 3: // infinity flag
                                s_stExternalScenario.stScenario[j-1].bSendInfinity = (HBOOL)(szBuf[0] - '0');
                                NXTool_LOG("\t[NxAging] s_stExternalScenario.stScenario[%d].bSendInfinity : %d\n", j-1, s_stExternalScenario.stScenario[j-1].bSendInfinity);
                                k++;
                                break;
                            case 4: // key send count
                                s_stExternalScenario.stScenario[j-1].ulSendCnt = atoi(szBuf);
                                NXTool_LOG("\t[NxAging] s_stExternalScenario.stScenario[%d].ulSendCnt : %d\n", j-1, s_stExternalScenario.stScenario[j-1].ulSendCnt);
                                k = 0;
                                j++;
                                break;                            
                            default:
                                break;                        
                        }
                    }

                    i = 0;
                }
                else if(ucChar != ' ')
                {
                    szBuf[i] = ucChar;
                    i++;
                }
            }
            else if(ucChar == '\n')
            {
                bComment = FALSE;
            }

            if(ucChar == '#') 
            {
                bComment = TRUE;
            }

            if(j > s_stExternalScenario.ulNodeNum) break;
        }

        bLoadingOK = TRUE;

EXIT_LOAD:        
        if(bLoadingOK == FALSE)
        {
            NXTool_LOG("\n\n @@@[NxAging] External aging scenario loading is failed..!! => Exit\n");    
            return 0;
        }
        else
        {       
            NXTool_LOG("\n\n @@@[NxAging] External aging scenario loading is OK.\n");  
            pAgingScenario = &s_stExternalScenario;
        }        
    }
    else
    {
        NXTool_LOG("\n\n [NxAging] Aging Scenario[%s]\n", pszAgingList[ulScenario]);
        pAgingScenario = &(s_stAgingScenario[ulScenario]);
    }
           

    VK_Init();
	HLIB_DBG_Init();

    while(1)
    {
        for(i = 0; i < pAgingScenario->ulNodeNum; i++)
        {
            if(pAgingScenario->stScenario[i].bSendInfinity == FALSE)                 
            {
                if(pAgingScenario->stScenario[i].ulSendCnt == 0)
                {
                    continue;
                }
                else
                {
                    pAgingScenario->stScenario[i].ulSendCnt--;
                }
            }
                        
            HLIB_STD_TaskSleep(pAgingScenario->stScenario[i].ulWaitTimeBefore);            
            HAPI_SendRemoteRCUKey(pAgingScenario->stScenario[i].ulKeyCode);
            HLIB_STD_TaskSleep(70);
            HAPI_SendRemoteRCUKeyRelease(pAgingScenario->stScenario[i].ulKeyCode);
            HLIB_STD_TaskSleep(pAgingScenario->stScenario[i].ulWaitTimeAfter);
        }
        pAgingScenario->ulTryCnt++;
        NXTool_LOG("\n #\n #\n #\n #\n @@@[NxAging] TryCount[%d]\n\n", pAgingScenario->ulTryCnt);   
    }

	return 0;
}


