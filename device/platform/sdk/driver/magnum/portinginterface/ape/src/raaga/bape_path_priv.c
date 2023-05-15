/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_path_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/27 $
 * $brcm_Date: 8/1/12 9:53a $
 *
 * Module Description: APE Path routines
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_path_priv.c $
 * 
 * Hydra_Software_Devel/27   8/1/12 9:53a jgarrett
 * SW7429-217: Fixing warning in release builds
 * 
 * Hydra_Software_Devel/26   7/6/12 3:54p jgarrett
 * SW7425-455: Detecting and handling orphan nodes that will not produce
 * output
 * 
 * Hydra_Software_Devel/25   5/3/12 5:06p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/2   4/6/12 5:22p jgarrett
 * SW7425-2075: Revising input format update when new inputs are added
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:52p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/24   3/16/12 6:10p gskerl
 * SW7425-1178: Modified BAPE_PathNode_P_EnumerateUpstreamPathNodes() so
 * that it calls the callback when unrecursing out of the tree (instead
 * of when recursing into).
 * 
 * Hydra_Software_Devel/23   2/21/12 6:41p jgarrett
 * SW7231-363: Propagating codec and mono format changes properly
 * 
 * Hydra_Software_Devel/22   2/3/12 4:29p jgarrett
 * SW7425-2268: Initial bringup of voice conferencing support
 * 
 * Hydra_Software_Devel/21   1/31/12 6:17p jgarrett
 * SW7425-2268: Adding initial voice conferencing support
 * 
 * Hydra_Software_Devel/20   8/2/11 3:27p jgarrett
 * SWDTV-8150: Adding mixer output connection support
 * 
 * Hydra_Software_Devel/19   6/20/11 7:02p jgarrett
 * SW7425-406: Adding input scaling coefficients to mixer
 * 
 * Hydra_Software_Devel/18   6/15/11 2:12p gskerl
 * SW7425-321: Fixed Coverity dead code issue
 * 
 * Hydra_Software_Devel/17   6/14/11 6:50p gskerl
 * SW7425-321: Added PathNode enumeration functions
 * 
 * Hydra_Software_Devel/16   5/27/11 11:20a jgarrett
 * SW7425-408: Revising format propagation logic
 * 
 * Hydra_Software_Devel/15   5/25/11 5:13p jgarrett
 * SW7425-408: Properly propagating input format when inputs are added to
 * a node
 * 
 * Hydra_Software_Devel/14   5/24/11 3:41p jgarrett
 * SW7425-559: Fixing warning with B_REFSW_DEBUG=n
 * 
 * Hydra_Software_Devel/13   5/18/11 6:50p jgarrett
 * SW7425-408: Adding preliminary DDRE support
 * 
 * Hydra_Software_Devel/12   4/18/11 10:09p jgarrett
 * SW7425-361: Refactoring DSP branch decisions
 * 
 * Hydra_Software_Devel/11   4/17/11 12:29p jgarrett
 * SW7344-46: Adding debug
 * 
 * Hydra_Software_Devel/10   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/9   4/14/11 3:25p jgarrett
 * SW7425-360: Fixing shutdown crash if resources are leaked in the app
 * 
 * Hydra_Software_Devel/8   4/6/11 1:24a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 12:50p jgarrett
 * SW35330-35: FMM Abstraction refactoring to support DTV
 * 
 * Hydra_Software_Devel/7   3/22/11 2:35p jgarrett
 * SW7422-356: Fixing crash on mux stop
 * 
 * Hydra_Software_Devel/6   3/21/11 7:08p jgarrett
 * SW7422-356: Adding MuxOutput
 * 
 * Hydra_Software_Devel/5   3/11/11 12:20p jgarrett
 * SW7422-146: Enabling PPM Correction
 * 
 * Hydra_Software_Devel/4   3/3/11 6:31p jgarrett
 * SW7422-146: Adding SRC and DSOLA and path review feedback
 * 
 * Hydra_Software_Devel/3   2/28/11 5:15p jgarrett
 * SW7422-146: Coverity CID 271
 * 
 * Hydra_Software_Devel/2   2/28/11 4:50p jgarrett
 * SW7422-146: Adding TruVolume
 * 
 * Hydra_Software_Devel/1   2/28/11 1:28p jgarrett
 * SW7422-146: Filter graph reworked to remove mixer dependencies
 * 
 ***************************************************************************/

#include "bape.h"
#include "bape_priv.h"

BDBG_MODULE(bape_path_priv);

BDBG_OBJECT_ID(BAPE_PathNode);
BDBG_OBJECT_ID(BAPE_PathConnector);
BDBG_OBJECT_ID(BAPE_PathConnection);

#if BDBG_DEBUG_BUILD
static const char *g_pPathNodeStateNames[BAPE_PathNodeState_eMax] =
{
    "Idle",
    "AllocatingPath",
    "AllocatedPath",
    "ConfiguredPath",
    "Started"
};
#endif

#define BAPE_PATH_NODE_STATE_NAME(state) (((state)>=BAPE_PathNodeState_eMax)?"invalid":g_pPathNodeStateNames[(state)])

static BERR_Code BAPE_PathNode_P_InputSupported(const BAPE_PathNode *pNode, const BAPE_Connector input, const BAPE_FMT_Descriptor *pFormat)
{
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);

    if ( !BAPE_FMT_P_FormatSupported(&pNode->inputCapabilities, pFormat) )
    {
        BDBG_ERR(("Node %s can not accept input type %s from source %s %s", pNode->pName,
                  BAPE_FMT_P_GetTypeName(pFormat), input->pParent->pName, input->pName));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    return BERR_SUCCESS;
}

static BERR_Code BAPE_PathNode_P_AcquirePathResources_impl(
    BAPE_PathNode *pNode
    )
{
    unsigned pathNum;
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);

    BDBG_MSG(("Acquire Path Resources for node %s", pNode->pName));

    if ( pNode->orphan )
    {
        BDBG_MSG(("Node %s is an orphan", pNode->pName));
        return BERR_SUCCESS;
    }
    if ( pNode->state == BAPE_PathNodeState_eStarted )
    {
        /* Stop recursing if we hit a running node.
           This may occur with mixers, when one input has
           started, others are allowed to start as well. */
        BDBG_MSG(("Node %s already started", pNode->pName));
        return BERR_SUCCESS;
    }

    pNode->state = BAPE_PathNodeState_eAllocatingPath;

    for ( pathNum = 0; pathNum < pNode->numConnectors; pathNum++ )
    {
        BAPE_PathConnection *pConnection;

        for ( pConnection = BLST_SQ_FIRST(&pNode->connectors[pathNum].connectionList);
              NULL != pConnection;
              pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
        {
            BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
            BDBG_OBJECT_ASSERT(pConnection->pSink, BAPE_PathNode);

            /* Skip if the sink is an orphan */
            if ( pConnection->pSink->orphan )
            {
                continue;
            }

            /* First, allocate connection resources */
            if ( NULL != pConnection->pSink->allocatePathFromInput )
            {
                BDBG_MSG(("Building connection from %s to %s using connector %s", 
                          pNode->pName, pConnection->pSink->pName, 
                          pNode->connectors[pathNum].pName));
                errCode = pConnection->pSink->allocatePathFromInput(pConnection->pSink, pConnection);
                if ( errCode )
                {
                    errCode = BERR_TRACE(errCode);
                    goto err_acquire;
                }
            }
            else
            {
                BDBG_MSG(("Downstream node %s does not support allocatePathFromInput", pConnection->pSink->pName));
            }
            if ( NULL != pNode->allocatePathToOutput )
            {
                BDBG_MSG(("Building connection to %s from %s using connector %s", 
                          pConnection->pSink->pName, pNode->pName, 
                          pNode->connectors[pathNum].pName));
                errCode = pNode->allocatePathToOutput(pNode, pConnection);
                if ( errCode )
                {
                    errCode = BERR_TRACE(errCode);
                    goto err_acquire;
                }
            }
            /* Now recurse into children */
            errCode = BAPE_PathNode_P_AcquirePathResources_impl(pConnection->pSink);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_acquire;
            }
        }
    }

    pNode->state = BAPE_PathNodeState_eAllocatedPath;
    return BERR_SUCCESS;

err_acquire:
    /* If we've failed, recursively cleanup when we hit a root node */
    if ( BLST_S_EMPTY(&pNode->upstreamList) )
    {
        BAPE_PathNode_P_ReleasePathResources(pNode);
    }
    else
    {
        /* Set state to allocated and wait for cleanup when we exit back to the top of the stack */
        pNode->state = BAPE_PathNodeState_eAllocatedPath;
    }
    return errCode;
}

static void BAPE_PathNode_P_FindOrphans(BAPE_PathNode *pNode)
{
    unsigned pathNum;
    bool allChildrenOrphans = true;

    if ( pNode->state == BAPE_PathNodeState_eStarted )
    {
        return;
    }

    if ( pNode->type == BAPE_PathNodeType_eMuxOutput || pNode->type == BAPE_PathNodeType_eEchoCanceller )
    {
        /* DRAM output */
        pNode->orphan = false;
        BDBG_MSG(("Node %s is not an orphan", pNode->pName));
        return;
    }
    else if ( pNode->type == BAPE_PathNodeType_eMixer && pNode->subtype != BAPE_MixerType_eDsp )
    {
        BAPE_MixerHandle hMixer;
        /* HW Mixer - check if outputs are attached. */
        hMixer = pNode->pHandle;
        if ( hMixer->numOutputs > 0 )
        {
            BDBG_MSG(("Node %s is not an orphan", pNode->pName));
            pNode->orphan = false;
            return;
        }
    }

    /* Recurse into children first.  This must scan all children and not
       stop simply when a non-orphan is found. */
    for ( pathNum = 0; pathNum < pNode->numConnectors; pathNum++ )
    {
        BAPE_PathConnection *pConnection;

        for ( pConnection = BLST_SQ_FIRST(&pNode->connectors[pathNum].connectionList);
              NULL != pConnection;
              pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
        {
            BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
            BDBG_OBJECT_ASSERT(pConnection->pSink, BAPE_PathNode);
            BAPE_PathNode_P_FindOrphans(pConnection->pSink);
            allChildrenOrphans = allChildrenOrphans && pConnection->pSink->orphan;
        }
    }

    BDBG_MSG(("Node %s is %san orphan.", pNode->pName, allChildrenOrphans?"":"not "));
    pNode->orphan = allChildrenOrphans;
}

/***************************************************************************
Summary:
Build paths for all downstream connections from a node
***************************************************************************/
BERR_Code BAPE_PathNode_P_AcquirePathResources(
    BAPE_PathNode *pNode
    )
{
    BAPE_PathNode_P_FindOrphans(pNode);

    return BAPE_PathNode_P_AcquirePathResources_impl(pNode);
}

/***************************************************************************
Summary:
Teardown paths for all downstream connections from a node as much as possible
***************************************************************************/
void BAPE_PathNode_P_ReleasePathResources(
    BAPE_PathNode *pNode
    )
{
    unsigned pathNum;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);

    BDBG_MSG(("Release Path Resources for node %s", pNode->pName));

    switch ( pNode->state )
    {
    case BAPE_PathNodeState_eAllocatingPath:
    case BAPE_PathNodeState_eStarted:
        BDBG_MSG(("Can not release Path Resources for node %s", pNode->pName));
        return;
    default:
        break;
    }

    for ( pathNum = 0; pathNum < pNode->numConnectors; pathNum++ )
    {
        BAPE_PathConnection *pConnection;

        for ( pConnection = BLST_SQ_FIRST(&pNode->connectors[pathNum].connectionList);
              NULL != pConnection;
              pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
        {
            BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
            BDBG_OBJECT_ASSERT(pConnection->pSink, BAPE_PathNode);
            if ( pConnection->pSink->freePathFromInput )
            {
                BDBG_MSG(("Teardown connection from %s to %s using connector %s", 
                          pNode->pName, pConnection->pSink->pName, 
                          pNode->connectors[pathNum].pName));
                pConnection->pSink->freePathFromInput(pConnection->pSink, pConnection);
            }
            BAPE_PathNode_P_ReleasePathResources(pConnection->pSink);
        }
    }

    pNode->state = BAPE_PathNodeState_eIdle;
}

/***************************************************************************
Summary:
Configure allocated resources. 
 
Description: 
FMM Resources will be configured from producer -> consumer.  DSP task is 
allocated prior to this call, and stage settings will be applied.  
***************************************************************************/
BERR_Code BAPE_PathNode_P_ConfigurePathResources(
    BAPE_PathNode *pNode
    )
{
    BERR_Code errCode;
    unsigned pathNum;
    BAPE_PathConnection *pConnection;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);

    BDBG_MSG(("Configure Path Resources for node %s", pNode->pName));

    if ( pNode->orphan )
    {
        BDBG_MSG(("Node %s is an orphan", pNode->pName));
        return BERR_SUCCESS;
    }

    switch ( pNode->state )
    {
    case BAPE_PathNodeState_eAllocatedPath:
    case BAPE_PathNodeState_eConfiguredPath:
        break;
    case BAPE_PathNodeState_eStarted:
        return BERR_SUCCESS;
    default:
        BDBG_ERR(("Invalid path node state transition (%s -> %s)", BAPE_PATH_NODE_STATE_NAME(pNode->state), BAPE_PATH_NODE_STATE_NAME(BAPE_PathNodeState_eConfiguredPath))); 
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    for ( pathNum = 0; pathNum < pNode->numConnectors; pathNum++ )
    {
        for ( pConnection = BLST_SQ_FIRST(&pNode->connectors[pathNum].connectionList);
              NULL != pConnection;
              pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
        {
            BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
            BDBG_OBJECT_ASSERT(pConnection->pSink, BAPE_PathNode);

            /* Skip if the sink is an orphan */
            if ( pConnection->pSink->orphan )
            {
                continue;
            }

            /* First, configure my output to this connection */
            if ( NULL != pNode->configPathToOutput )
            {
                BDBG_MSG(("Configuring connection from %s to %s using connector %s", 
                          pNode->pName, pConnection->pSink->pName, 
                          pNode->connectors[pathNum].pName));
                errCode = pNode->configPathToOutput(pNode, pConnection);
                if ( errCode )
                {
                    BDBG_ERR(("Error configuring output from %s to %s on connector %s", pNode->pName, pConnection->pSink->pName, pNode->connectors[pathNum].pName));
                    return BERR_TRACE(errCode);
                }
            }
            /* Next, configure the downstream node's input from this connection */
            if ( NULL != pConnection->pSink->configPathFromInput )
            {
                BDBG_MSG(("Configuring connection to %s from %s using connector %s", 
                          pConnection->pSink->pName, pNode->pName,
                          pNode->connectors[pathNum].pName));
                errCode = pConnection->pSink->configPathFromInput(pConnection->pSink, pConnection);
                if ( errCode )
                {
                    BDBG_ERR(("Error configuring %s input from %s on connector %s", pConnection->pSink->pName, pNode->pName, pNode->connectors[pathNum].pName));
                    return BERR_TRACE(errCode);
                }
            }
            /* Now, recurse into the downstream node */
            errCode = BAPE_PathNode_P_ConfigurePathResources(pConnection->pSink);
            if ( errCode )
            {
                /* Error trace skipped, it will print in one of the cases above */
                return errCode;
            }
        }
    }

    /* Update state */
    pNode->state = BAPE_PathNodeState_eConfiguredPath;

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Start paths for all downstream connections from a node
***************************************************************************/
BERR_Code BAPE_PathNode_P_StartPaths(
    BAPE_PathNode *pNode
    )
{
    BERR_Code errCode;
    unsigned pathNum;
    BAPE_PathConnection *pConnection;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);

    BDBG_MSG(("Start Path for node %s", pNode->pName));

    if ( pNode->orphan )
    {
        BDBG_MSG(("Node %s is an orphan", pNode->pName));
        return BERR_SUCCESS;
    }

    switch ( pNode->state )
    {
    case BAPE_PathNodeState_eStarted:
        pNode->numStarts++;
        return BERR_SUCCESS;
    case BAPE_PathNodeState_eConfiguredPath:
        break;
    default:
        BDBG_ERR(("Must allocate and configure path prior to starting object"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    for ( pathNum = 0; pathNum < pNode->numConnectors; pathNum++ )
    {
        for ( pConnection = BLST_SQ_FIRST(&pNode->connectors[pathNum].connectionList);
              NULL != pConnection;
              pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
        {
            BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
            BDBG_OBJECT_ASSERT(pConnection->pSink, BAPE_PathNode);

            /* Skip if the sink is an orphan */
            if ( pConnection->pSink->orphan )
            {
                continue;
            }

            /* First, recurse into children.  This routine starts from consumer..producer */
            errCode = BAPE_PathNode_P_StartPaths(pConnection->pSink);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_start;
            }

            /* Now start per-connection consumers */
            if ( NULL != pConnection->pSink->startPathFromInput )
            {
                BDBG_MSG(("Starting connection to %s from %s using connector %s", 
                          pConnection->pSink->pName, pNode->pName,
                          pNode->connectors[pathNum].pName));
                errCode = pConnection->pSink->startPathFromInput(pConnection->pSink, pConnection);
                if ( errCode )
                {
                    errCode = BERR_TRACE(errCode);
                    goto err_start;
                }
            }
            if ( NULL != pNode->startPathToOutput )
            {
                BDBG_MSG(("Starting connection from %s to %s using connector %s", 
                          pNode->pName, pConnection->pSink->pName,
                          pNode->connectors[pathNum].pName));
                errCode = pNode->startPathToOutput(pNode, pConnection);
                if ( errCode )
                {
                    errCode = BERR_TRACE(errCode);
                    goto err_start;
                }
            }
        }
    }

    pNode->numStarts = 1;
    pNode->state = BAPE_PathNodeState_eStarted;
    return BERR_SUCCESS;

err_start:
    /* If we've failed, recursively cleanup when we hit a root node */
    if ( BLST_S_EMPTY(&pNode->upstreamList) )
    {
        BAPE_PathNode_P_StopPaths(pNode);
    }
    return errCode;    
}

/***************************************************************************
Summary:
Stop paths for downstream connections from this node
***************************************************************************/
void BAPE_PathNode_P_StopPaths(
    BAPE_PathNode *pNode
    )
{
    unsigned pathNum;
    BAPE_PathConnection *pConnection;
    BAPE_PathNodeState exitState;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);

    BDBG_MSG(("Stop Path for node %s", pNode->pName));

    if ( pNode->orphan )
    {
        BDBG_MSG(("Node %s is an orphan", pNode->pName));
        return;
    }

    switch ( pNode->state )
    {
    case BAPE_PathNodeState_eIdle:
        exitState = BAPE_PathNodeState_eIdle;
        break;
    case BAPE_PathNodeState_eStarted:
        if ( pNode->numStarts > 1 )
        {
            pNode->numStarts--;
            return;
        }
        /* Otherwise, fall through */
    default:
        exitState = BAPE_PathNodeState_eAllocatedPath;
        break;
    }

    for ( pathNum = 0; pathNum < pNode->numConnectors; pathNum++ )
    {
        for ( pConnection = BLST_SQ_FIRST(&pNode->connectors[pathNum].connectionList);
              NULL != pConnection;
              pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
        {
            BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
            BDBG_OBJECT_ASSERT(pConnection->pSink, BAPE_PathNode);

            /* Skip if the sink is an orphan */
            if ( pConnection->pSink->orphan )
            {
                continue;
            }

            /* Stop from producer..consumer */
            if ( NULL != pNode->stopPathToOutput )
            {
                BDBG_MSG(("Stopping connection from %s to %s using connector %s", 
                          pNode->pName, pConnection->pSink->pName, 
                          pNode->connectors[pathNum].pName));
                pNode->stopPathToOutput(pNode, pConnection);
            }            
            if ( NULL != pConnection->pSink->stopPathFromInput )
            {
                BDBG_MSG(("Stopping connection to %s from %s using connector %s", 
                          pConnection->pSink->pName, pNode->pName,
                          pNode->connectors[pathNum].pName));
                pConnection->pSink->stopPathFromInput(pConnection->pSink, pConnection);
            }
            BAPE_PathNode_P_StopPaths(pConnection->pSink);
        }
    }

    pNode->numStarts = 0;
    pNode->state = exitState;
}

static BAPE_PathConnection *BAPE_PathNode_P_FindConnection(
    BAPE_PathNode *pNode,
    BAPE_Connector input
    )
{
    /* Search the upstream list of the sink node to see if we find the source connector */
    BAPE_PathConnection *pConnection;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);

    for ( pConnection = BLST_S_FIRST(&pNode->upstreamList);
          NULL != pConnection;
          pConnection = BLST_S_NEXT(pConnection, upstreamNode) )
    {
        if ( pConnection->pSource == input )
        {
            break;
        }
    }

#if BDBG_DEBUG_BUILD
    /* Sanity check, make sure the link is bi-directional */
    if ( NULL != pConnection )
    {
        BAPE_PathConnection *pDownstreamConnection;
        for ( pDownstreamConnection = BLST_SQ_FIRST(&input->connectionList);
              NULL != pDownstreamConnection; 
              pDownstreamConnection = BLST_SQ_NEXT(pDownstreamConnection, downstreamNode) )
        {
            if ( pDownstreamConnection == pConnection )
            {
                break;
            }
        }

        BDBG_ASSERT(NULL != pDownstreamConnection);
    }
#endif

    return pConnection;
}

/***************************************************************************
Summary:
Make a connection
***************************************************************************/
BERR_Code BAPE_PathNode_P_AddInput(
    BAPE_PathNode *pNode,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BAPE_PathConnection *pConnection;
    unsigned i;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);
    
    BDBG_MSG(("Adding input %s %s to node %s", input->pParent->pName, input->pName, pNode->pName));

    /* Make sure we don't have the same connection already */
    pConnection = BAPE_PathNode_P_FindConnection(pNode, input);
    if ( pConnection )
    {
        BDBG_ERR(("Input %s is already connected to %s", input->pParent->pName, pNode->pName));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Check if this is a valid connection */
    errCode = BAPE_PathNode_P_InputSupported(pNode, input, &input->format);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Build the connection node */
    pConnection = BKNI_Malloc(sizeof(BAPE_PathConnection));
    if ( NULL == pConnection )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    /* This sets all the handles to invalid entries */
    BKNI_Memset(pConnection, 0x0, sizeof(BAPE_PathConnection));
    BAPE_FciIdGroup_Init(&pConnection->inputFciGroup);
    BAPE_FMT_P_InitDescriptor(&pConnection->format);
    pConnection->pSource = input;
    pConnection->pSink = pNode;
    BLST_S_INSERT_HEAD(&pNode->upstreamList, pConnection, upstreamNode);
    /* Insert FMM nodes to the head of the list and DSP nodes to the tail */
    /* This allows branch logic to work better */
    if ( pNode->type == BAPE_PathNodeType_eEqualizer ||
         (pNode->type == BAPE_PathNodeType_eMixer && pNode->subtype != BAPE_MixerType_eDsp) )
    {
        /* FMM Node */
        BLST_SQ_INSERT_HEAD(&input->connectionList, pConnection, downstreamNode);
    }
    else
    {
        /* DSP Node */
        BLST_SQ_INSERT_TAIL(&input->connectionList, pConnection, downstreamNode);
    }
    BDBG_OBJECT_SET(pConnection, BAPE_PathConnection);

    /* Notify source that a connection has been made */
    if ( input->pParent->outputConnectionAdded )
    {
        errCode = input->pParent->outputConnectionAdded(input->pParent, pConnection);
        if ( errCode )
        {
            (void)BAPE_PathNode_P_RemoveInput(pNode, input);
            return BERR_TRACE(errCode);            
        }
    }

    /* Propagate format */
    if ( pNode->inputFormatChange )
    {
        errCode = pNode->inputFormatChange(pNode, pConnection, &input->format);
        if ( errCode )
        {
            (void)BAPE_PathNode_P_RemoveInput(pNode, input);
            return BERR_TRACE(errCode);            
        }
    }
    else
    {
        /* If a format change handler is not installed, the output format will propagate */
        pConnection->format = input->format;
        for ( i = 0; i < pConnection->pSink->numConnectors; i++ )
        {
            errCode = BAPE_Connector_P_SetFormat(&pConnection->pSink->connectors[i], &input->format);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }
    }
       
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Destroy a connection
***************************************************************************/
BERR_Code BAPE_PathNode_P_RemoveInput(
    BAPE_PathNode *pNode,
    BAPE_Connector input
    )
{
    BAPE_PathConnection *pConnection;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);

    BDBG_MSG(("Removing input %s %s from node %s", input->pParent->pName, input->pName, pNode->pName));

    pConnection = BAPE_PathNode_P_FindConnection(pNode, input);
    if ( NULL == pConnection )
    {
        BDBG_ERR(("Input %s not connected to %s", input->pParent->pName, pNode->pName));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    /* Notify source that a connection is being removed */
    if ( input->pParent->outputConnectionRemoved )
    {
        input->pParent->outputConnectionRemoved(input->pParent, pConnection);
    }

    /* Make sure resources are freed */
    if ( pNode->freePathFromInput )
    {
        pNode->freePathFromInput(pNode, pConnection);
    }

    /* Make really sure we don't leak resources */
    BDBG_ASSERT(NULL == pConnection->sfifoGroup);
    BDBG_ASSERT(NULL == pConnection->srcGroup);
    BDBG_ASSERT(NULL == pConnection->mixerGroup);
    BDBG_ASSERT(NULL == pConnection->dfifoGroup);
    BDBG_ASSERT(NULL == pConnection->loopbackGroup);

    /* Remove connections and free node */
    BLST_S_REMOVE(&pNode->upstreamList, pConnection, BAPE_PathConnection, upstreamNode);
    BLST_SQ_REMOVE(&input->connectionList, pConnection, BAPE_PathConnection, downstreamNode);
    BDBG_OBJECT_DESTROY(pConnection, BAPE_PathConnection);
    BKNI_Free(pConnection);

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Remove all inputs
***************************************************************************/
BERR_Code BAPE_PathNode_P_RemoveAllInputs(
    BAPE_PathNode *pNode
    )
{
    BERR_Code errCode;
    BAPE_PathConnection *pConnection;

    /* Keep removing the first item from the list until it's empty */
    for ( pConnection = BLST_S_FIRST(&pNode->upstreamList);
          pConnection != NULL;
          pConnection = BLST_S_FIRST(&pNode->upstreamList) )
    {
        errCode = BAPE_PathNode_P_RemoveInput(pNode, pConnection->pSource);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
    return BERR_SUCCESS;
}

static void BAPE_PathNode_P_AddOutputStatus(
    BAPE_PathNode *pNode,
    unsigned rootConnector,
    BAPE_PathNodeOutputStatus *pStatus      /* [out] */
    )
{
    unsigned connectorNum;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_ASSERT(NULL != pStatus);

    for ( connectorNum = 0; connectorNum < pNode->numConnectors; connectorNum++ )
    {
        BAPE_PathConnection *pConnection;
        unsigned numConnections=0;

        for ( pConnection = BLST_SQ_FIRST(&pNode->connectors[connectorNum].connectionList);
              NULL != pConnection;
              pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
        {
            pStatus->connectorStatus[rootConnector].totalConnections++;
            numConnections++;
            if ( pConnection->pSink->type == BAPE_PathNodeType_eMixer ||
                 pConnection->pSink->type == BAPE_PathNodeType_eEqualizer )
            {
                pStatus->connectorStatus[rootConnector].numOutputs[pConnection->pSource->format.type]++;
            }
            if ( !pConnection->pSink->orphan )
            {
                pStatus->connectorStatus[rootConnector].numValidOutputs++;
            }
            BAPE_PathNode_P_AddOutputStatus(pConnection->pSink, rootConnector, pStatus);            
        }
        if ( numConnections > 0 )
        {
            pStatus->connectorStatus[rootConnector].totalBranches += numConnections-1;
            pStatus->activeConnectors++;
        }
    }
}

void BAPE_PathNode_P_GetInputCapabilities(
    BAPE_PathNode *pNode, 
    BAPE_FMT_Capabilities *pCaps    /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_ASSERT(NULL != pCaps);
    *pCaps = pNode->inputCapabilities;
}

BERR_Code BAPE_PathNode_P_SetInputCapabilities(
    BAPE_PathNode *pNode, 
    const BAPE_FMT_Capabilities *pCaps
    )
{
    BAPE_PathConnection *pPathConnection;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_ASSERT(NULL != pCaps);
    for ( pPathConnection = BLST_S_FIRST(&pNode->upstreamList);
          pPathConnection != NULL;
          pPathConnection = BLST_S_NEXT(pPathConnection, upstreamNode) )
    {
        if ( !BAPE_FMT_P_FormatSupported(pCaps, &pPathConnection->pSource->format) )
        {
            BDBG_ERR(("Node %s is already attached to an input that does not match new capabilities."));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
    }
    pNode->inputCapabilities = *pCaps;
    return BERR_SUCCESS;
}

void BAPE_PathNode_P_GetOutputStatus(
    BAPE_PathNode *pNode,
    BAPE_PathNodeOutputStatus *pStatus      /* [out] */
    )
{
    unsigned connectorNum;
    unsigned activeConnectors=0;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_ASSERT(NULL != pStatus);

    BAPE_PathNode_P_FindOrphans(pNode);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    pStatus->numConnectors = pNode->numConnectors;

    for ( connectorNum = 0; connectorNum < pNode->numConnectors; connectorNum++ )
    {
        BAPE_PathConnection *pConnection;
        unsigned numConnections=0;
        unsigned numFmmOutputs=0;

        for ( pConnection = BLST_SQ_FIRST(&pNode->connectors[connectorNum].connectionList);
              NULL != pConnection;
              pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
        {
            pStatus->connectorStatus[connectorNum].directConnections++;
            pStatus->connectorStatus[connectorNum].totalConnections++;
            numConnections++;
            if ( pConnection->pSink->type == BAPE_PathNodeType_eMixer ||
                 pConnection->pSink->type == BAPE_PathNodeType_eEqualizer )
            {
                numFmmOutputs++;
            }
            if ( !pConnection->pSink->orphan )
            {
                pStatus->connectorStatus[connectorNum].numValidOutputs++;
            }
            BAPE_PathNode_P_AddOutputStatus(pConnection->pSink, connectorNum, pStatus);            
        }
        if ( numConnections > 0 )
        {
            pStatus->connectorStatus[connectorNum].numOutputs[pNode->connectors[connectorNum].format.type] += numFmmOutputs;
            pStatus->connectorStatus[connectorNum].totalBranches += numConnections-1;
            pStatus->activeConnectors++;
            pStatus->directConnections += numConnections;
            activeConnectors++;
        }
    }
    if ( activeConnectors > 0 )
    {
        pStatus->totalBranches += activeConnectors-1;
        for ( connectorNum = 0; connectorNum < pNode->numConnectors; connectorNum++ )
        {
            unsigned i;
            for ( i = 0; i < BAPE_DataType_eMax; i++ )
            {
                pStatus->numOutputs[i] += pStatus->connectorStatus[connectorNum].numOutputs[i];
            }
            pStatus->totalConnections += pStatus->connectorStatus[connectorNum].totalConnections;
            pStatus->numValidOutputs += pStatus->connectorStatus[connectorNum].numValidOutputs;
        }
    }
}   

/***************************************************************************
Summary:
Set an connector's sample rate on the fly (used with decoders)
***************************************************************************/
void BAPE_Connector_P_SetSampleRate_isr(
    BAPE_Connector connector,
    unsigned newSampleRate
    )
{
    BAPE_PathConnection *pConnection;
    
    BDBG_OBJECT_ASSERT(connector, BAPE_PathConnector);
    BKNI_ASSERT_ISR_CONTEXT();

    connector->format.sampleRate = newSampleRate;

    for ( pConnection = BLST_SQ_FIRST(&connector->connectionList);
          NULL != pConnection;
          pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
    {
        BAPE_PathNode *pNode = pConnection->pSink;

        BDBG_ASSERT(NULL != pNode);

        if ( pConnection->pSink->inputSampleRateChange_isr )
        {
            pNode->inputSampleRateChange_isr(pConnection->pSink, pConnection, newSampleRate);
        }
        else
        {
            unsigned i;
            /* By default just propagate the sample rate change */
            for ( i = 0; i < pNode->numConnectors; i++ )
            {
                BAPE_Connector_P_SetSampleRate_isr(&pNode->connectors[i], newSampleRate);
            }
        }
    }
}

/***************************************************************************
Summary:
Set a connector's format
***************************************************************************/
void BAPE_Connector_P_GetFormat(
    BAPE_Connector connector,
    BAPE_FMT_Descriptor *pFormat /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(connector, BAPE_PathConnector);
    BDBG_ASSERT(NULL != pFormat);
    *pFormat = connector->format;
}

/***************************************************************************
Summary:
Set a connector's format
***************************************************************************/
BERR_Code BAPE_Connector_P_SetFormat(
    BAPE_Connector connector,
    const BAPE_FMT_Descriptor *pNewFormat
    )
{
    BAPE_PathConnection *pConnection;
    BAPE_FMT_Descriptor oldFormat;
    BERR_Code errCode;
    unsigned i;

    BDBG_OBJECT_ASSERT(connector, BAPE_PathConnector);

    /* If nothing changed, return. */
    if ( BAPE_FMT_P_IsEqual(pNewFormat, &connector->format) )
    {
        return BERR_SUCCESS;
    }
    /* If only sample rate changed, set the sample rate */
    oldFormat = connector->format;
    oldFormat.sampleRate = pNewFormat->sampleRate;
    if ( BAPE_FMT_P_IsEqual(&oldFormat, pNewFormat) )
    {
        BKNI_EnterCriticalSection();
            BAPE_Connector_P_SetSampleRate_isr(connector, pNewFormat->sampleRate);
        BKNI_LeaveCriticalSection();
        return BERR_SUCCESS;
    }

    /* save the old format in case we need to error out later*/
    oldFormat = connector->format;        

    for ( pConnection = BLST_SQ_FIRST(&connector->connectionList);
          NULL != pConnection;
          pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
    {
        BERR_Code errCode;

        #if BDBG_DEBUG_BUILD
        BAPE_PathNode *pNode = pConnection->pSink;

        BDBG_ASSERT(NULL != pNode);

        BDBG_MSG(("Format changed between %s and %s connector %s",
                  pConnection->pSource->pParent->pName, pNode->pName, connector->pName));
        #endif

        errCode = BAPE_PathNode_P_InputSupported(pConnection->pSink, pConnection->pSource, pNewFormat);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    /* All consumers can accept this format.  Change it and notify them. */
    connector->format = *pNewFormat;
    for ( pConnection = BLST_SQ_FIRST(&connector->connectionList);
          NULL != pConnection;
          pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
    {
        /* Does this node care? */
        if ( pConnection->pSink->inputFormatChange )
        {
            errCode = pConnection->pSink->inputFormatChange(pConnection->pSink, pConnection, pNewFormat);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }
        else
        {
            pConnection->format = pConnection->pSource->format;
            /* If a format change handler is not installed, the output format will propagate */
            for ( i = 0; i < pConnection->pSink->numConnectors; i++ )
            {
                errCode = BAPE_Connector_P_SetFormat(&pConnection->pSink->connectors[i], &pConnection->format);
                if ( errCode )
                {
                    return BERR_TRACE(errCode);
                }
            }
        }        
    }

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Get number of downstream connections for a connector
***************************************************************************/
unsigned BAPE_Connector_P_GetNumConnections(
    BAPE_Connector connector
    )
{
    BAPE_PathConnection *pConnection;
    unsigned numConnections=0;

    BDBG_OBJECT_ASSERT(connector, BAPE_PathConnector);

    for ( pConnection = BLST_SQ_FIRST(&connector->connectionList);
          NULL != pConnection;
          pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
    {
        numConnections++;
    }

    return numConnections;
}

/***************************************************************************
Summary:
Set the connector mute state
***************************************************************************/
void BAPE_Connector_P_SetMute(
    BAPE_Connector connector,
    bool muted
    )
{
    BAPE_PathConnection *pConnection;

    BDBG_OBJECT_ASSERT(connector, BAPE_PathConnector);

    for ( pConnection = BLST_SQ_FIRST(&connector->connectionList);
          NULL != pConnection;
          pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
    {
        BAPE_PathNode *pNode = pConnection->pSink;

        BDBG_ASSERT(NULL != pNode);

        BDBG_MSG(("Set mute between %s and %s connector %s to %u",
                  pConnection->pSource->pParent->pName, pNode->pName, connector->pName, muted));

        if ( pNode->inputMute )
        {
            /* Stop once we've found a node that can handle this operation */
            pNode->inputMute(pNode, pConnection, muted);
            return;
        }
        else
        {
            unsigned i;

            /* Recurse until handled */
            for ( i = 0; i < pNode->numConnectors; i++ )
            {
                BAPE_Connector_P_SetMute(&pNode->connectors[i], muted);
            }
        }
    }
}

/***************************************************************************
Summary:
Remove all downstream connections from this connector
***************************************************************************/
void BAPE_Connector_P_RemoveAllConnections(
    BAPE_Connector connector
    )
{
    BAPE_PathConnection *pConnection;

    BDBG_OBJECT_ASSERT(connector, BAPE_PathConnector);
    
    for ( pConnection = BLST_SQ_FIRST(&connector->connectionList);
          NULL != pConnection;
          pConnection = BLST_SQ_FIRST(&connector->connectionList) )
    {
        BAPE_PathNode *pNode;
        BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
        pNode = pConnection->pSink;
        BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
        if ( NULL != pNode->removeInput )
        {
            pNode->removeInput(pNode, connector);
        }
        else
        {
            BERR_Code errCode;
            errCode = BAPE_PathNode_P_RemoveInput(pNode, connector);
            if ( errCode )
            {
                (void)BERR_TRACE(errCode);
                BDBG_ASSERT(errCode == BERR_SUCCESS);   /* No way to recover if this fails. */
                return;
            }
        }
        /* At this point, pConnector will be invalid */
    }
}

static void BAPE_PathNode_P_FindConsumers(
    BAPE_PathNode *pNode,
    BAPE_PathNodeType type,
    unsigned subtype,
    bool checkSubtype,
    unsigned maxConsumers,
    unsigned *pNumFound,        /* [out] */
    BAPE_PathNode **pConsumers   /* [out] Must be an array of at least maxConsumers length */
    )
{
    unsigned connectorNum;
    BAPE_PathConnection *pConnection;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);

    for ( connectorNum = 0; connectorNum < pNode->numConnectors; connectorNum++ )
    {
        for ( pConnection = BLST_SQ_FIRST(&pNode->connectors[connectorNum].connectionList);
              NULL != pConnection;
              pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
        {
            BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
            BDBG_OBJECT_ASSERT(pConnection->pSink, BAPE_PathNode);

            if ( pConnection->pSink->type == type )
            {
                if ( false == checkSubtype || pConnection->pSink->subtype == subtype )
                {
                    if ( *pNumFound < maxConsumers )
                    {
                        pConsumers[*pNumFound] = pConnection->pSink;
                        *pNumFound = (*pNumFound)+1;
                    }
                    else
                    {
                        return;
                    }
                }
            }
            /* recurse */
            BAPE_PathNode_P_FindConsumers(pConnection->pSink, type, subtype, checkSubtype, maxConsumers, pNumFound, pConsumers);
        }
    }
}

/***************************************************************************
Summary:
Search for consumers by a type
***************************************************************************/
void BAPE_PathNode_P_FindConsumersByType(
    BAPE_PathNode *pNode,
    BAPE_PathNodeType type,
    unsigned maxConsumers,
    unsigned *pNumFound,        /* [out] */
    BAPE_PathNode **pConsumers   /* [out] Must be an array of at least maxConsumers length */
    )
{
    BDBG_ASSERT(NULL != pNumFound);
    *pNumFound = 0;
    BAPE_PathNode_P_FindConsumers(pNode, type, 0, false, maxConsumers, pNumFound, pConsumers);
}

/***************************************************************************
Summary:
Search for consumers by a type and subtype
***************************************************************************/
void BAPE_PathNode_P_FindConsumersBySubtype(
    BAPE_PathNode *pNode,
    BAPE_PathNodeType type,
    unsigned subtype,
    unsigned maxConsumers,
    unsigned *pNumFound,        /* [out] */
    BAPE_PathNode **pConsumers   /* [out] Must be an array of at least maxConsumers length */
    )
{
    BDBG_ASSERT(NULL != pNumFound);
    *pNumFound = 0;
    BAPE_PathNode_P_FindConsumers(pNode, type, subtype, true, maxConsumers, pNumFound, pConsumers);
}

/***************************************************************************
Summary:
Determine if a node is a consumer from this node
***************************************************************************/
bool BAPE_PathNode_P_NodeIsConsumer(
    BAPE_PathNode *pSourceNode,
    BAPE_PathNode *pConsumerNode
    )
{
    unsigned connectorNum;
    BAPE_PathConnection *pConnection;

    BDBG_OBJECT_ASSERT(pSourceNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConsumerNode, BAPE_PathNode);

    for ( connectorNum = 0; connectorNum < pSourceNode->numConnectors; connectorNum++ )
    {
        for ( pConnection = BLST_SQ_FIRST(&pSourceNode->connectors[connectorNum].connectionList);
              NULL != pConnection;
              pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
        {
            if ( pConnection->pSink == pConsumerNode )
            {
                return true;
            }
            if ( BAPE_PathNode_P_NodeIsConsumer(pConnection->pSink, pConsumerNode) )
            {
                return true;
            }
        }
    }
    return false;
}

BAPE_PathConnection *BAPE_Connector_P_GetConnectionToSink(
    BAPE_Connector connector,
    BAPE_PathNode *pSink
    )
{
    BAPE_PathConnection *pConnection;

    BDBG_OBJECT_ASSERT(connector, BAPE_PathConnector);
    BDBG_OBJECT_ASSERT(pSink, BAPE_PathNode);

    for ( pConnection = BLST_SQ_FIRST(&connector->connectionList);
          NULL != pConnection;
          pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
    {
        if ( pConnection->pSink == pSink )
        {
            return pConnection;
        }
    }

    return NULL;    /* No connection found if we reach here */
}

/***************************************************************************
Summary:
Returns a null-terminated string representation of a BAPE_PathNodeType enum.
***************************************************************************/
const char *BAPE_PathNode_P_PathNodeTypeToText( BAPE_PathNodeType pathNodeType )
{
    return ( pathNodeType == BAPE_PathNodeType_eDecoder         ?  "Decoder"        : 
             pathNodeType == BAPE_PathNodeType_ePlayback        ?  "Playback"       : 
             pathNodeType == BAPE_PathNodeType_eInputCapture    ?  "InputCapture"   : 
             pathNodeType == BAPE_PathNodeType_ePostProcessor   ?  "PostProcessor"  : 
             pathNodeType == BAPE_PathNodeType_eEncoder         ?  "Encoder"        : 
             pathNodeType == BAPE_PathNodeType_eMixer           ?  "Mixer"          : 
             pathNodeType == BAPE_PathNodeType_eEqualizer       ?  "Equalizer"      : 
             pathNodeType == BAPE_PathNodeType_eMuxOutput       ?  "MuxOutput"      : 
             pathNodeType == BAPE_PathNodeType_eMax             ?  "<eMax>"         : 
                                                                   "<Undefined>"    );
}


/***************************************************************************
Summary:
Do a depth-first, pre-order traversal of the downstream PathNodes.  The 
specified callback will be called for each PathNode visited (except for the 
starting PathNode)
***************************************************************************/
BERR_Code BAPE_PathNode_P_EnumerateUpstreamPathNodes( BAPE_PathNode  *pPathNode, int level, BAPE_PathNode_P_EnumerateCallback callback)
{
    BERR_Code errCode=BERR_SUCCESS;
    BAPE_PathConnection *pPathConnection;
    BAPE_PathConnector *pPathConnector;
    int index;

    /* BDBG_MSG(("%*s%s : %d : pPathNode:%p  level:%d", level*4, "", __FUNCTION__, __LINE__ , 
                                                pPathNode, level ));   */
    for ( pPathConnection = BLST_S_FIRST(&pPathNode->upstreamList), index=0;
          pPathConnection != NULL;
          pPathConnection = BLST_S_NEXT(pPathConnection, upstreamNode), index++ )
    {
        /* BDBG_MSG(("%*s%s : %d : pPathNode:%p  Found pPathConnection:%p", level*4, "", __FUNCTION__, __LINE__ , 
                                                pPathNode, pPathConnection ));  */
        pPathConnector =  pPathConnection->pSource ;
        if ( NULL == pPathConnector )
        {
            return errCode;
        }
    
        if ( pPathConnector->pParent )
        {
            level++;
                BAPE_PathNode_P_EnumerateUpstreamPathNodes( pPathConnector->pParent, level, callback);
                (*callback)(pPathConnector->pParent, level, index);
            level--;
        }
    }
    return( errCode );
}


/***************************************************************************
Summary:
Do a depth-first, pre-order traversal of the upstream PathNodes.  The 
specified callback will be called for each PathNode visited (except for the 
starting PathNode)
***************************************************************************/
BERR_Code BAPE_PathNode_P_EnumerateDownstreamPathNodes( BAPE_PathNode  *pPathNode, int level, BAPE_PathNode_P_EnumerateCallback callback)
{
    BERR_Code errCode=BERR_SUCCESS;
    unsigned i;
    BSTD_UNUSED(level);

    for ( i=0 ; i<BAPE_ConnectorFormat_eMax ; i++ )
    {
        BAPE_PathConnector *pPathConnector = &pPathNode->connectors[i];
        BAPE_PathConnection  *pPathConnection;
        int index;

        /* BDBG_MSG(("%*s%s : %d : pPathNode:%p Found pPathConnector:%p  level:%d", level*4, "", __FUNCTION__, __LINE__ , 
                                                    pPathNode, pPathConnector, level ));  */

        for ( pPathConnection = BLST_SQ_FIRST(&pPathConnector->connectionList), index=0;
              pPathConnection != NULL;
              pPathConnection = BLST_SQ_NEXT(pPathConnection, downstreamNode), index++ )
        {
            /* BDBG_MSG(("%*s%s : %d : pPathNode:%p pPathConnector:%p Found pPathConnection:%p  ", level*4, "", __FUNCTION__, __LINE__ , 
                                                    pPathNode, pPathConnector, pPathConnection ));  */
            if ( pPathConnection->pSink )
            {
                level++;
                (*callback)(pPathConnection->pSink, level, index);
                BAPE_PathNode_P_EnumerateDownstreamPathNodes( pPathConnection->pSink, level, callback);
                level--;
            }
        }
    }
    return( errCode);
}


