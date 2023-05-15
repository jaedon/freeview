/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "perftool.h"

LEVEL_NODE *g_pLevelNode = NULL;
LEVEL_NODE *g_pCurrLevel = NULL;
DRM_CHAR *g_pchConfigOutput = "ConfigResult.log";
OEM_FILEHDL fp_output = OEM_INVALID_HANDLE_VALUE, fp_perfdat = OEM_INVALID_HANDLE_VALUE;
AGG_DURATION g_Duration[MAX_MOD][MAX_FUNC]={0,0};

static void PrintUsage( const DRM_WCHAR *pwszAppName )
{
    printf("Syntax: %S [-p:performance trace file] [-o:Output Logs][-c:ConfigFile Name (for specific function latencies) -t:TestCaseID]\n\n", pwszAppName);
    printf("For the -c parameter:\n");
    printf("  The ConfigFile specifies functions in the format <Binary identifier>!<Function identifier>.\n");
    printf("  These identifiers can be found in DrmProfileConstants.h.\n");
    printf("  The corresponding latencies calculated by this tool are appended to ConfigResult.log under the current working directory.\n");
    printf("  The -t parameter is required when -c is specified.\n");
}

/*************************************************************
Function: GetNextDWORD
Synopsis: This function is used to get the 4-byte (DWORD) id from a series of 4 bytes read from a file. 
Arguments: OUT ScopeID : Pointer to a DWORD where the ID is to be stored
           IN pBuffer     : Pointer to the sequence of bytes
           IN i           : Pointer to the offset in pBuffer from where ID is to be extracted
**************************************************************/
static DRM_VOID GetNextDWORD(DRM_DWORD *ScopeID, const DRM_BYTE *pBuffer, DRM_LONG *i)
{
        *ScopeID = (DRM_DWORD)pBuffer[(*i)];
        (*i)++;
        *ScopeID  += (((DRM_DWORD)pBuffer[*i]) << 8);
        (*i)++;
        *ScopeID += (((DRM_DWORD)pBuffer[*i]) << 16);
        (*i)++;
        *ScopeID += (((DRM_DWORD)pBuffer[*i]) << 24);
        (*i)++;
}

DRM_LONG DRM_CALL DRM_Main(DRM_LONG argc, DRM_WCHAR **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrPerfFile   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrOutputFile    = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrConfigFile = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrTestCaseID = EMPTY_DRM_STRING;
    DRM_LONG i = 0, nParseStackLog = 0;

    printf( "Microsoft (R) %s\nCopyright (c) Microsoft Corporation. All rights reserved.\n\n", "Performance Tool" );
    if (argc < 3)
    {
        PrintUsage(argv[0]);
        ChkDR( DRM_E_INVALID_COMMAND_LINE );
    }
    for( i = 1; i < argc; i++ )
    {
        DRM_WCHAR        wchOption =  '\0';
        DRM_CONST_STRING dstrParam = {0};

        if (! DRM_CMD_ParseCmdLine(argv [i], &wchOption, &dstrParam, NULL))
        {
            PrintUsage(argv[0]);
            ChkDR( DRM_E_INVALID_COMMAND_LINE );
        }
        wchOption = DRMCRT_towlower( wchOption );

     
        switch (wchOption)
        {
            case WCHAR_CAST('p'):  
                dstrPerfFile.pwszString = dstrParam.pwszString;
                dstrPerfFile.cchString  = dstrParam.cchString;
                break;

            case WCHAR_CAST('o'):  
                dstrOutputFile.pwszString = dstrParam.pwszString;
                dstrOutputFile.cchString  = dstrParam.cchString;
                break;

            case WCHAR_CAST('c'):
                dstrConfigFile.pwszString = dstrParam.pwszString;
                dstrConfigFile.cchString  = dstrParam.cchString; 
                break;

            case WCHAR_CAST('t'):
                dstrTestCaseID.pwszString = dstrParam.pwszString;
                dstrTestCaseID.cchString = dstrParam.cchString;
                break; 

            case WCHAR_CAST('x'):
                nParseStackLog = 1;
                break;
            default:
            {
                PrintUsage(argv[0]);
                ChkDR( DRM_E_INVALID_COMMAND_LINE );
            }
        }/* end of switch*/
    }/*end for loop*/

    /* Either -c and -t must BOTH be specified or NEITHER */
    if( ( dstrConfigFile.pwszString != NULL ) != ( dstrTestCaseID.pwszString != NULL ) )
    {
        PrintUsage(argv[0]);
        ChkDR( DRM_E_INVALID_COMMAND_LINE );
    }

    if( dstrPerfFile.pwszString == NULL || dstrOutputFile.pwszString == NULL )
    {
        PrintUsage(argv[0]);
        ChkDR( DRM_E_INVALID_COMMAND_LINE );
    }

    fp_perfdat = Oem_File_Open (NULL,
                              dstrPerfFile.pwszString,
                              OEM_GENERIC_READ,
                              OEM_FILE_SHARE_READ,
                              OEM_OPEN_EXISTING,
                              OEM_ATTRIBUTE_NORMAL);
                       
    if (fp_perfdat == OEM_INVALID_HANDLE_VALUE)
    {
        printf("Failed to open performance data file\n");
        ChkDR(DRM_E_FILEREADERROR);
    }
    if(fp_output == OEM_INVALID_HANDLE_VALUE)
    {
        fp_output = Oem_File_Open(NULL,
                                 dstrOutputFile.pwszString,
                                 OEM_GENERIC_WRITE,
                                 OEM_FILE_SHARE_NONE,
                                 OEM_OPEN_EXISTING,
                                 OEM_ATTRIBUTE_NORMAL);
        
        if(fp_output == OEM_INVALID_HANDLE_VALUE)
        {
            fp_output = Oem_File_Open(NULL,
                                     dstrOutputFile.pwszString,
                                     OEM_GENERIC_WRITE,
                                     OEM_FILE_SHARE_NONE,
                                     OEM_CREATE_NEW,
                                     OEM_ATTRIBUTE_NORMAL);

            if(fp_output == OEM_INVALID_HANDLE_VALUE)
                printf("Failed to open output log file\n");
            ChkBOOL(fp_output != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN);
        }
    }

    if(nParseStackLog)
    {
        ChkDR(ParseStackLog());
    }
    else
    {
          
        /*Parse the Binary performance data and extract call graph and function latencies*/
        ChkDR(ParseAndSave());
        /*If we have a config data file, output only the specific function latencies*/
        if(dstrConfigFile.pwszString != NULL)
        {
            if(dstrTestCaseID.pwszString !=NULL)
            {
                ChkDR(ProcessConfigFile(dstrConfigFile, dstrTestCaseID));
            }
            else
            {
                printf("Failed to process the Config File \n");
                ChkDR(DRM_E_FAIL);
            }
        }
    }
ErrorExit: 
    if (fp_perfdat != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close (fp_perfdat);
    }
    if (fp_output != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close (fp_output);
    }

    if( dr!= DRM_SUCCESS && dr != DRM_E_INVALID_COMMAND_LINE )
    {
        printf("Failed to parse performance data: 0x%X\n", dr);
    }
    return dr;
}


/*************************************************************
Function: ProcessConfigFile
Synopsis: This function is used process the config file which contains a list of performance counters
          we are intersted in. It gets the values of those specific counters, and writes it out to 
          the ConfigResult.log file.
Arguments: IN dstrConfigFile : structure containing the ConfigFile which has performance counters
           IN dstrTestCaseID : Structure containing the test case id for which the perf.dat is being
                               processed
Returns: DRM_RESULT indicating success/failure.
**************************************************************/

DRM_RESULT ProcessConfigFile(DRM_CONST_STRING dstrConfigFile, DRM_CONST_STRING dstrTestCaseID)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR   rgchFileName[DRM_MAX_PATH]  = {0};
    DRM_CHAR   chLine      [MAX_LINE_SIZE] = {0};
    DRM_CHAR   pModuleName [MAX_LINE_SIZE] = {0}; 
    DRM_CHAR   chOutput    [MAX_LINE_SIZE] = {0};
    DRM_CHAR   chTestCaseID[DRM_MAX_PATH]  = {0};
    DRM_CHAR  *pFunctionName = NULL;
    FILE      *fpConfigFile  = NULL; 
    FILE      *fpOutputFile  = NULL;
    DRM_SIZE_T cchFunctionNameLen = 0;
    DRM_SIZE_T cchLineLen         = 0;
    DRM_DWORD  dwModuleIndex      = 0; 
    DRM_DWORD  dwFunctionIndex    = 0;
    DRM_SCOPE *pPerfCounter  = NULL;
    
    if(dstrConfigFile.cchString > DRM_MAX_PATH || dstrTestCaseID.cchString > DRM_MAX_PATH)
        ChkDR(DRM_E_BUFFERTOOSMALL);
    
    /* Safe to use, assume that input parameter is in ASCII */
    DRM_UTL_DemoteUNICODEtoASCII(dstrConfigFile.pwszString,rgchFileName,dstrConfigFile.cchString);
    PackedCharsToNative( rgchFileName, dstrConfigFile.cchString );

    /* Safe to use, input parameter is in ASCII */
    DRM_UTL_DemoteUNICODEtoASCII(dstrTestCaseID.pwszString,chTestCaseID,dstrTestCaseID.cchString);
    PackedCharsToNative(chTestCaseID, dstrTestCaseID.cchString);

    fpConfigFile = fopen(rgchFileName,"rt");
    if(!fpConfigFile)
        ChkDR(DRM_E_FILEOPEN);

    fpOutputFile = fopen(g_pchConfigOutput,"a+");
    if(!fpOutputFile)
        ChkDR(DRM_E_FILEOPEN);

    ChkDR( snprintf(chOutput,MAX_LINE_SIZE,"Test Case: %s \n", chTestCaseID) );

    if(fputs(chOutput,fpOutputFile) < 0)
        ChkDR(DRM_E_FILEWRITEERROR);

    while(!(feof(fpConfigFile)) && fgets(chLine,MAX_LINE_SIZE,fpConfigFile))
    {
        pFunctionName = (strchr(chLine,'!'));
        if(!pFunctionName)
            ChkDR(DRM_E_INVALIDARG);

        pFunctionName++;
        //StringCchLengthA(pFunctionName, MAX_LINE_SIZE, (size_t *)&cchFunctionNameLen);
        cchFunctionNameLen = strlen(pFunctionName);
        if(pFunctionName[cchFunctionNameLen - 1] == '\n')
        {
            pFunctionName[cchFunctionNameLen - 1] = '\0';
            cchFunctionNameLen--;
        }
        //StringCchLengthA(chLine,MAX_LINE_SIZE, (size_t*)&cchLineLen);
		cchLineLen = strlen(chLine);
        //StringCchCopyNA(pModuleName,MAX_LINE_SIZE,chLine, cchLineLen - cchFunctionNameLen - 1);
        strncpy(pModuleName, chLine, cchLineLen - cchFunctionNameLen - 1);
    
        ChkDR(GetIndex(pModuleName,&dwModuleIndex));
        ChkDR(GetIndex(pFunctionName,&dwFunctionIndex));
        
        ChkDR(GetPerfCounter(&pPerfCounter,dwModuleIndex,dwFunctionIndex));
        
        ChkDR( snprintf(chOutput,MAX_LINE_SIZE,"%s %lu:%lu %lu:%lu\r\n",
                                                    pPerfCounter->m_pszScopeName,
                                                    (pPerfCounter->m_dwABRPMCounter),
                                                    g_Duration[dwModuleIndex][dwFunctionIndex].m_dwRPM,
                                                    (pPerfCounter->m_dwABCPMCounter),
                                                    g_Duration[dwModuleIndex][dwFunctionIndex].m_dwChildRPM ) );

        if(fputs(chOutput,fpOutputFile) < 0)
            ChkDR(DRM_E_FILEWRITEERROR);
    }
ErrorExit:
    if(fpConfigFile)
        fclose(fpConfigFile);
    if(fpOutputFile)
        fclose(fpOutputFile);
    return dr;
}

/*************************************************************
Function: GetPerfCounter
Synopsis: This function is used to locate the node corresponding to the given Function and Module ID
Arguments: OUT pPerfCounter : If the node exists, a pointer to the node structure. 
           IN dwModuleIndex : Module ID of the function to search 
           IN dwFunctionIndex : Function ID of the function to search
Returns: DRM_RESULT indicating success/failure.
***************************************************************/
DRM_RESULT GetPerfCounter(DRM_SCOPE **pPerfCounter, DRM_DWORD dwModuleIndex, DRM_DWORD dwFunctionIndex)
{
    DRM_DWORD   dwScopeID   = PERF_SCOPE_ID(dwModuleIndex,dwFunctionIndex,1);
    DRM_LONG    i           = 0;
    DRM_RESULT  dr          = DRM_SUCCESS;

    ChkArg( pPerfCounter != NULL );

    *pPerfCounter = NULL;

    for( i = 0; g_Scopes[i].m_pszScopeName != NULL; i++ )
    {
        if(g_Scopes[i].m_dwScopeID == dwScopeID)
        {
            *pPerfCounter = &g_Scopes[i];
            break;
        }
    }

    if( *pPerfCounter == NULL )
    {
        TRACE(("Couldn't find a definition for the scope ID (%d: module index %d, function index %d), the perf data file may be newer than this EXE.", dwScopeID, dwModuleIndex, dwFunctionIndex));
        *pPerfCounter = &g_Scopes[++i];
    }
        
ErrorExit:

    return dr;
}

/*************************************************************
Function: GetIndex
Synopsis: This function is used to get the numeric value out of a string.
Arguments: IN pStr - Pointer to the input string
Returns: Int value converted from the pStr String if success, else returns -1
****************************************************************/
DRM_RESULT GetIndex(const DRM_CHAR *pStr, DRM_DWORD *pdwRetVal)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SIZE_T cchStr = 0;
    DRM_DWORD  dwTens = 1;
    DRM_DWORD  dwIndex = 0;
    DRM_CHAR   c = '\0';

    ChkArg(pStr != NULL);

    //StringCchLengthA(pStr,MAX_LINE_SIZE, (size_t *)&cchStr);
	cchStr = strlen(pStr);
    ChkArg(cchStr > 0);
    
    *pdwRetVal = 0;
    do
    {
        c = pStr[dwIndex];
        if(c < '0' || c > '9') 
        {
            ChkDR(DRM_E_FAIL);
        } 
        *pdwRetVal = (*pdwRetVal) * dwTens;
        *pdwRetVal = *pdwRetVal + (c - 48);
        dwIndex++;
        dwTens *= 10;
    }while(dwIndex < cchStr);

ErrorExit:
    return dr;
}
/*****************************************
Function: ParseAndSave
Synopsis: This function is used to parse the performance data file and save call graph in
          output files. 
Arguments: none - It uses the g_perfdata file pointer setup earlier in wmain()
Returns: DRM_RESULT indicating a pass or a fail
****************************************************************/

DRM_RESULT ParseAndSave()
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwRead = 0;
    DRM_LONG i = 0;
    DRM_BYTE *pBuffer = NULL;
    DRM_DWORD ScopeID = 0,ModuleID = 0, ParentModuleID = 0,FunctionID = 0, 
    ParentFunctionID = 0,dwTotalRead = 0,dwFileSize = 0, ParentScopeID = 0,
    Duration = 0,ScopeLevel = 0;
    LEVEL_NODE *pCurrLevel = NULL, *pParentLevel = NULL;
    PERF_SCOPE_NODE *pCurrScopeNode = NULL;

    ChkMem( pBuffer = (DRM_BYTE*)Oem_MemAlloc( MAX_PROFILE_BUFFER_SIZE ) );
    
    if(!Oem_File_GetSize(fp_perfdat,&dwFileSize))
        ChkDR(DRM_E_FAIL);
    
    do
    {

        if(!Oem_File_Read(fp_perfdat,pBuffer,MAX_PROFILE_BUFFER_SIZE,&dwRead))
            ChkDR(DRM_E_FILEREADERROR);
        dwTotalRead += dwRead;
        i = 0;
        /*Since MAX_PROFILE_BUFFER_SIZE = 12K bytes, and each while loop requires 
        4 DWORDS = 16bytes, it is guranteed that pBuffer will always contain complete
        data for 750 function traces (4DWORDS for each)*/
        while(i < (DRM_LONG)dwRead)
        {
            /*get scope id, block id, function id*/
            GetNextDWORD(&ScopeID, pBuffer, &i);
            
            /*BlockID = (ScopeID & 0x0000007f);*/
            FunctionID = (ScopeID & 0x0007ff80) >> 7;
            ModuleID = (ScopeID & 0x7ff80000) >> 19;
            ChkArg( FunctionID < MAX_FUNC );
            ChkArg( ModuleID < MAX_MOD );
            
            /*get parent scopeid, functionid */
            GetNextDWORD(&ParentScopeID,pBuffer,&i);
            ParentFunctionID = (ParentScopeID & 0x0007ff80) >> 7;
            ParentModuleID = (ParentScopeID & 0x7ff80000) >> 19;
            /*Get Time spent and Scope Level*/
            GetNextDWORD(&Duration,pBuffer,&i);
            GetNextDWORD(&ScopeLevel,pBuffer,&i);
            
            /*Get the LevelNode for the current Scope Level*/            
            ChkDR(GetScopeLevelNode(&pCurrLevel,ScopeLevel));
            
            /*This node might already have been in the tree, inserted by its child*/
            if(pCurrLevel->m_pNodeList)
            {
                if((pCurrLevel->m_pNodeList)->m_bRunning)
                {
                    if((pCurrLevel->m_pNodeList)->m_dwFunctionID == FunctionID
                    && (pCurrLevel->m_pNodeList)->m_dwModuleID == ModuleID)
                    {
                        (pCurrLevel->m_pNodeList)->m_dwDuration = Duration;
                        (pCurrLevel->m_pNodeList)->m_bRunning = FALSE;
                        (pCurrLevel->m_pNodeList)->m_dwScopeLevel = ScopeLevel;
                        pCurrScopeNode = pCurrLevel->m_pNodeList;
                        pCurrLevel->m_pNodeList = NULL;

                    }
                    else
                    {
                        TRACE(("Function ID and module ID don't match."));
                        ChkDR(DRM_E_FAIL);
                    }
                }
            }
            else
            {
                /*if it is not already inserted, create a new node - DO NOT insert this node in the 
                Level list for this scope level, since this function is not running anymore*/
                ChkDR(CreatePerfScopeNode(&pCurrScopeNode, ModuleID, FunctionID, Duration, ScopeLevel));
            }

            /*Since the function corresponding to this node has returned, save 
            its total duration (RPM)*/
            g_Duration[ModuleID][FunctionID].m_dwRPM += Duration;
                
            /*If this node corresponds to a function which is not at Level 2, inset it in the tree*/
            if(ScopeLevel > 2)
            {
                /*The the Level node for the parent scope level*/
                ChkDR(GetScopeLevelNode(&pParentLevel,ScopeLevel-1));
                /*Insert this node in the tree function node tree*/
                ChkDR(InsertNode(pParentLevel, pCurrScopeNode, ParentModuleID, ParentFunctionID));
            }
            else if(g_pLevelNode)
            {
                /*If this is a level 2 node, output the callgraph collected so far and delete 
                the nodes of the function node tree and scope level list*/
                ChkDR(OutputTree(pCurrScopeNode));
                ChkDR(Cleanup());
            }
            else
            {
                TRACE(("The node is below level 2 but the global node level pointer was NULL."));
                ChkDR(DRM_E_FAIL);
            }
        }/*end while i < dwRead*/
    
    }while(dwTotalRead < dwFileSize);
   
ErrorExit:
    SAFE_OEM_FREE( pBuffer );
    return dr;
}

/*****************************************
Function: OutputTree
Synopsis: This function is used to traverse the call graph while giving out the results.
Arguments: IN pStartNode - root node of the call graph tree. 
Returns: DRM_RESULT indicating a pass or a fail
****************************************************************/
DRM_RESULT OutputTree(PERF_SCOPE_NODE *pStartNode)
{
    DRM_RESULT dr = DRM_SUCCESS;
    static DRM_LONG n_indent = 5;
    
    PERF_SCOPE_NODE *pTraversalNode = NULL; 
    
    if(pStartNode == NULL)
        ChkDR(DRM_E_FAIL);

    pTraversalNode = pStartNode;

    n_indent += 5;
    ChkDR(PrintNode(pTraversalNode,n_indent));
    if((pTraversalNode->m_pChild)!= NULL)
        ChkDR(OutputTree(pTraversalNode->m_pChild));
    
    n_indent -= 5;
    if((pTraversalNode->m_pPeer) != NULL)
        ChkDR(OutputTree(pTraversalNode->m_pPeer));

    SAFE_OEM_FREE(pTraversalNode);
    
ErrorExit:
    return dr;
}



/*Print out the logs to specified file*/
/*****************************************
Function: PrintNode
Synopsis: This function is used to print the functin name/duration to the output log. It uses fp_output
          file pointer set in wmain function to log the results.
Arguments: pNode - Node corresponding to the function being printed
           n_indent - indicating the depth of this function in the call graph
Returns: DRM_RESULT indicating a pass or a fail
****************************************************************/
DRM_RESULT PrintNode(PERF_SCOPE_NODE *pNode, DRM_LONG n_indent)
{
    DRM_CHAR   cWriteBuffer[MAX_LINE_SIZE] = {0};
    DRM_CHAR   cSpaceHolder[MAX_LINE_SIZE] = {0};
    DRM_DWORD  dwWritten = 0;
    DRM_SIZE_T dwLen     = 0;
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_LONG   i         = 0;
    DRM_SCOPE *pPerfCounter = NULL;

    if(fp_output == OEM_INVALID_HANDLE_VALUE)
    {
        ChkDR(DRM_E_FILEREADERROR);
    }

    if (!Oem_File_SetFilePointer(fp_output, 0, OEM_FILE_END, NULL))
    {
        ChkDR(DRM_E_FILESEEKERROR);
    }
    for(i = 0;i < n_indent && i < MAX_LINE_SIZE - 1 ;i++)
    {
        cSpaceHolder[i] = '.';
    }
    cSpaceHolder[i] = '\0';

    ChkDR(GetPerfCounter(&pPerfCounter,pNode->m_dwModuleID,pNode->m_dwFunctionID));

    /* Note that the duration is being converted from microseconds (the base unit from the log files)
    ** to milliseconds by using a conversion factor of 1000
    */
    snprintf(cWriteBuffer,MAX_LINE_SIZE,"%s %s - Duration: %.3g ms - Level: %lu \r\n",cSpaceHolder, pPerfCounter->m_pszScopeName, (float)pNode->m_dwDuration / 1000.0, pNode->m_dwScopeLevel);
    
    //StringCchLengthA(cWriteBuffer,MAX_LINE_SIZE, (size_t *)&dwLen);
	dwLen = strlen(cWriteBuffer);
    ChkDR(Oem_File_Write(fp_output, cWriteBuffer,dwLen, &dwWritten));

ErrorExit:
    return dr;
}

/*Insert this function node in the node tree*/
/*****************************************
Function: InsertNode
Synopsis: This function is used to insert a node corresponding to a specific function in the call graph
Arguments: IN pParentLevel - level of the parent of this node in callgraph
           IN pCurrNode - current node being inserted to the callgraph
           IN dwParentModuleID - Module ID of the caller of this function
           IN dwParentFunctionID - FunctionID of the caller of this function
           
Returns: DRM_RESULT indicating a pass or a fail
****************************************************************/
DRM_RESULT InsertNode(LEVEL_NODE *pParentLevel, PERF_SCOPE_NODE *pCurrNode, DRM_DWORD dwParentModuleID, DRM_DWORD dwParentFunctionID )
{
    
    DRM_RESULT dr = DRM_SUCCESS;
    PERF_SCOPE_NODE *pCurrLevelChild;

    ChkArg(pParentLevel != NULL);

    do
    {
        /*if the parent of this node exists, then add the current node as its child*/     
        if(pParentLevel->m_pNodeList)
        {
            if((pParentLevel->m_pNodeList)->m_bRunning &&  (pParentLevel->m_pNodeList)->m_dwModuleID == dwParentModuleID && (pParentLevel->m_pNodeList)->m_dwFunctionID == dwParentFunctionID)
            {
                /*Add the RPM of current function from child RPM of its parent*/
                g_Duration[(pParentLevel->m_pNodeList)->m_dwModuleID][(pParentLevel->m_pNodeList)->m_dwFunctionID].m_dwChildRPM += pCurrNode->m_dwDuration;
                   
                pCurrLevelChild = (pParentLevel->m_pNodeList)->m_pChild;
                /*if this parent does not have any other child*/
                 if(!pCurrLevelChild)
                 {
                    (pParentLevel->m_pNodeList)->m_pChild = pCurrNode;
                    pCurrNode->m_pParent = pParentLevel->m_pNodeList;
                    break;
                }
                /*Loop till you find the last child in the list*/
                while((pCurrLevelChild->m_pPeer) != NULL)
                    pCurrLevelChild = pCurrLevelChild->m_pPeer;
                   
                /*Add the current node to the end of the child list*/
                pCurrLevelChild->m_pPeer = pCurrNode;
                pCurrNode->m_pParent = pParentLevel->m_pNodeList;
                pCurrNode->m_bRunning = FALSE;
                break;
            }
        }

        /*Create a new parent node for this level*/
        ChkDR(CreatePerfScopeNode(&(pParentLevel->m_pNodeList),dwParentModuleID, dwParentFunctionID, 0, 0 ));
        
        /*Add the RPM of current function to the child RPM of its parent*/
        g_Duration[(pParentLevel->m_pNodeList)->m_dwModuleID][(pParentLevel->m_pNodeList)->m_dwFunctionID].m_dwChildRPM += pCurrNode->m_dwDuration;
        (pParentLevel->m_pNodeList)->m_pChild = pCurrNode;
        pCurrNode->m_pParent = pParentLevel->m_pNodeList;
        pCurrNode->m_bRunning = FALSE;
    }while(FALSE);

ErrorExit:
    return dr;
}

/*Create a node for the scope level list*/
/*****************************************
Function: CreatePerfScopeNode
Synopsis: This function is used to create a scope node for the given function
Arguments: IN OUT ppCurrNode - ppCurrNode should point to enough memory for all the members of the 
                               PERF_SCOPE_NODE structure to be filled in by this function.
           IN dwModuleID - Module ID of the function.
           IN dwFunctionID - Function ID of the function
           IN dwDuration - Time spent by this function
           IN dwScopeLevel - Depth of this function in the call graph 
Returns: DRM_RESULT indicating a pass or a fail
****************************************************************/
DRM_RESULT CreatePerfScopeNode(PERF_SCOPE_NODE **ppCurrNode, DRM_DWORD dwModuleID, DRM_DWORD dwFunctionID, DRM_DWORD dwDuration, DRM_DWORD dwScopeLevel)
{
    DRM_RESULT dr = DRM_SUCCESS;
    *ppCurrNode = (PERF_SCOPE_NODE *)Oem_MemAlloc(SIZEOF(PERF_SCOPE_NODE));
    
    if(*ppCurrNode == NULL)
        ChkDR(DRM_E_OUTOFMEMORY);

    (*ppCurrNode)->m_dwModuleID = dwModuleID;
    (*ppCurrNode)->m_dwFunctionID = dwFunctionID;
    (*ppCurrNode)->m_dwDuration = dwDuration;
    (*ppCurrNode)->m_pChild = NULL;
    (*ppCurrNode)->m_pParent = NULL;
    (*ppCurrNode)->m_pPeer = NULL;
    (*ppCurrNode)->m_bRunning = TRUE;
    (*ppCurrNode)->m_dwScopeLevel = dwScopeLevel;

ErrorExit:
    return dr;
}

/*****************************************
Function: GetScopeLevelNode
Synopsis: This function is used to find a node at the given level within the call graph
Arguments: OUT pCurrLevel - pointer to the node at the current level
           IN dwScopeLevel - level for which the node is to be found
Returns: DRM_RESULT indicating a pass or a fail
****************************************************************/
DRM_RESULT GetScopeLevelNode(LEVEL_NODE **pCurrLevel, DRM_DWORD dwScopeLevel)
{
    
    DRM_RESULT dr = DRM_SUCCESS;
    LEVEL_NODE *pTempLevelNode, *pNewLevelNode = NULL;
    

    do
    {
        *pCurrLevel = NULL;    
         /*if g_pCurrLevel is pointing to the level we are looking for, then return this node*/
        if(g_pCurrLevel)
        {
            if(g_pCurrLevel->nLevel == (DRM_LONG)dwScopeLevel)
            {
                *pCurrLevel = g_pCurrLevel;
                break;
            }
        }
        /*Search for the level node we are intersted in */
        if(g_pLevelNode)
        {
            pTempLevelNode = g_pLevelNode;
            while(pTempLevelNode != NULL)
            {
                if(pTempLevelNode->nLevel == (DRM_LONG)dwScopeLevel)
                {
                    g_pCurrLevel = pTempLevelNode;
                   *pCurrLevel = pTempLevelNode;
                    break;
                }
                else if(pTempLevelNode->nLevel > (DRM_LONG)dwScopeLevel)
                {
                    /*Node for current level has not yet been inserted in the level node list, so create
                    a new one*/
                    pNewLevelNode = (LEVEL_NODE *)Oem_MemAlloc(SIZEOF(LEVEL_NODE));
                    if(!pNewLevelNode)
                    {
                        ChkDR(DRM_E_OUTOFMEMORY);
                    }
                    
                    pNewLevelNode->nLevel = (DRM_LONG)dwScopeLevel;
                    pNewLevelNode->m_pNodeList = NULL;
                    pNewLevelNode->m_pNextLevelNode = pTempLevelNode;
                    pNewLevelNode->m_pPreviousLevelNode = pTempLevelNode->m_pPreviousLevelNode;
                    
                    (pTempLevelNode->m_pPreviousLevelNode) = pNewLevelNode;
                    if(pNewLevelNode->m_pPreviousLevelNode)
                    {
                        (pNewLevelNode->m_pPreviousLevelNode)->m_pNextLevelNode = pNewLevelNode;
                    }
                
                    g_pCurrLevel = pNewLevelNode;
                    /*g_pLevelNode points to first node in the list of level nodes - if we are adding something 
                    before the current first node, then change g_pLevelNode*/
                    if(g_pLevelNode == pTempLevelNode)
                    {
                        g_pLevelNode = pNewLevelNode;
                    }
                    *pCurrLevel = pNewLevelNode;
                    break;
                }
                else if(!pTempLevelNode->m_pNextLevelNode)
                {
                    /*pTempLevelNode is the last node in the tree, so we need to insert a new node
                    for the current level */
                    pNewLevelNode = (LEVEL_NODE *)Oem_MemAlloc(SIZEOF(LEVEL_NODE));
                    if(!pNewLevelNode)
                    {
                        ChkDR(DRM_E_OUTOFMEMORY);
                    }
                    
                    pNewLevelNode->nLevel = (DRM_LONG)dwScopeLevel;
                    pNewLevelNode->m_pNodeList = NULL;
                    pNewLevelNode->m_pNextLevelNode = NULL;
                    pNewLevelNode->m_pPreviousLevelNode = pTempLevelNode;
                    pTempLevelNode->m_pNextLevelNode = pNewLevelNode;

                    g_pCurrLevel = pNewLevelNode;
                    *pCurrLevel = pNewLevelNode;
                    break;
                }

                pTempLevelNode = pTempLevelNode->m_pNextLevelNode;
            }/*end while loop*/
            if(*pCurrLevel != NULL)
            {
                break;
            }
        } /*end if(g_pLevelNode)*/
        
        /*The Level list has not yet been created, this is the first node in this list*/
        pNewLevelNode = (LEVEL_NODE *)Oem_MemAlloc(SIZEOF(LEVEL_NODE));
        if(!pNewLevelNode)
        {
            ChkDR(DRM_E_OUTOFMEMORY);
        }
        
        pNewLevelNode->nLevel = (DRM_LONG)dwScopeLevel;
        pNewLevelNode->m_pNextLevelNode = NULL;
        pNewLevelNode->m_pPreviousLevelNode = NULL;
        pNewLevelNode->m_pNodeList = NULL;

        g_pCurrLevel = pNewLevelNode;
        *pCurrLevel = pNewLevelNode;
        g_pLevelNode = pNewLevelNode;
     }while(FALSE);

ErrorExit:
    return dr;
}

/*****************************************
Function: Cleanup
Synopsis: This function is used to clean up the call graph after printing out the result
Arguments: none 
Returns: DRM_RESULT indicating a pass or a fail
****************************************************************/
DRM_RESULT Cleanup(DRM_VOID)
{
    DRM_RESULT dr = DRM_SUCCESS;
    LEVEL_NODE *pTempLevelNode; 

    /*Only nodes of the scope level list have to be deleted now, othere nodes were already deleted
    in OutputTree Function*/
    ChkArg(g_pLevelNode != NULL);
    pTempLevelNode = g_pLevelNode->m_pNextLevelNode;
    do
    {
        SAFE_OEM_FREE(g_pLevelNode);
        g_pLevelNode=pTempLevelNode;
        if(pTempLevelNode != NULL)
            pTempLevelNode = pTempLevelNode->m_pNextLevelNode;
    }while(g_pLevelNode);

    g_pCurrLevel = NULL;

    
ErrorExit:
    return dr;
}


 DRM_RESULT ParseStackLog()
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dwRead      = 0;
    DRM_DWORD  dwFileSize  = 0;
    DRM_DWORD  dwTotalRead = 0;
    DRM_SIZE_T cchLen      = 0;
    DRM_DWORD  dwWritten   = 0;
    DRM_LONG   i           = 0;
    DRM_BYTE  *pBuffer     = NULL;
    DRM_DWORD ScopeID      = 0;
    DRM_DWORD dwModuleID   = 0;
    DRM_DWORD dwStackUsage = 0;
    DRM_DWORD dwFunctionID = 0;
    DRM_CHAR  chOutput[MAX_LINE_SIZE] = {0};
    DRM_SCOPE *pPerfCounter = NULL;


    ChkMem( pBuffer = (DRM_BYTE*)Oem_MemAlloc(256) );
    
    if(!Oem_File_GetSize(fp_perfdat,&dwFileSize))
        ChkDR(DRM_E_FAIL);
    
    do
    {

        if(!Oem_File_Read(fp_perfdat,pBuffer,256,&dwRead))
            ChkDR(DRM_E_FILEREADERROR);
        dwTotalRead += dwRead;
        i = 0;
        while(i < (DRM_LONG)dwRead)
        {
            /*get scope id, block id, function id*/
            GetNextDWORD(&ScopeID, pBuffer, &i);
            
            /*BlockID = (ScopeID & 0x0000007f);*/
            dwFunctionID = (ScopeID & 0x0007ff80) >> 7;
            dwModuleID = (ScopeID & 0x7ff80000) >> 19;
            ChkArg( dwFunctionID < MAX_FUNC );
            ChkArg( dwModuleID < MAX_MOD );
            
            /*get the stack usage for this scope*/
            GetNextDWORD(&dwStackUsage,pBuffer,&i);

            ChkDR(GetPerfCounter(&pPerfCounter,dwModuleID,dwFunctionID));

            ChkDR(snprintf(chOutput,MAX_LINE_SIZE,"%s %lu \r\n", pPerfCounter->m_pszScopeName, dwStackUsage));
            //ChkDR( StringCchLengthA(chOutput,MAX_LINE_SIZE, (size_t *)&cchLen));
			ChkDR(cchLen = strlen(chOutput));
            ChkBOOL(Oem_File_Write(fp_output, chOutput,cchLen, &dwWritten),DRM_E_FILEWRITEERROR);
            
            
           }
        }while(dwTotalRead < dwFileSize);

ErrorExit:
    return dr;
}
           
 
    
/*****************************************
Function: PackedCharsToNative
Synopsis: This function is used to convert a string containing packed chars to native representation
Arguments: IN OUT f_pPackedString - String containing packed chars. Native representation will be 
                                    stored in the same location
Returns: void
****************************************************************/
static void PackedCharsToNative( DRM_CHAR *f_pPackedString, DRM_DWORD cch )
{
    DRM_DWORD ich;

    if( f_pPackedString != NULL && cch != 0 )
    {
        for( ich = cch; ich > 0; ich-- )
        {
            f_pPackedString[ich] = GET_CHAR( f_pPackedString, ich );
        }
        f_pPackedString[0]   = GET_CHAR( f_pPackedString, 0 ) ;
        f_pPackedString[cch] = '\0';
    }
}
