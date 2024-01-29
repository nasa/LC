/************************************************************************
 * NASA Docket No. GSC-18,921-1, and identified as “CFS Limit Checker
 * Application version 2.2.1”
 *
 * Copyright (c) 2021 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *   Functions used for CFS Limit Checker watchpoint processing
 */

/*************************************************************************
** Includes
*************************************************************************/
#include "lc_app.h"
#include "lc_watch.h"
#include "lc_eventids.h"
#include "lc_custom.h"
#include "lc_perfids.h"
#include "lc_platform_cfg.h"

#include <float.h>
#include <math.h>

/*
 * An ISO-compliant math.h header should provide "isnan" and "isfinite" macros
 * as they are dictated by C99.  However some C libraries still in use are not
 * fully compliant.  If these macros are not defined, define a substitute here.
 *
 * Note these are not ideal/complete implementations of these macros, but they
 * are OK based on the way they are used inside this source file.  Use caution if
 * changing the code using these, notably:
 *  - isfinite assumes float type
 *  - both macros evaluate the argument twice - use only with a simple variable
 */
#ifndef isnan
#define isnan(x) ((x) != (x))
#endif
#ifndef isfinite
#define isfinite(x) ((x) > -FLT_MAX && (x) < FLT_MAX)
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* LC_GetHashTableIndex() - convert messageID to hash table index  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uint32 LC_GetHashTableIndex(CFE_SB_MsgId_t MessageID)
{
    /*
    **   The purpose of a hash function is to take the input value
    ** and convert it to an index into the hash table. Assume that
    ** the range of input values is much different (larger) than
    ** the number of entries in the hash table. Then multiple input
    ** values must resolve to the same output table index. This is ok
    ** because each entry in the hash table is a linked list of all
    ** the inputs with the same hash function result.
    **
    **   This particular hash function takes advantage of knowledge
    ** regarding the format of the input values (cFE MessageID). By
    ** ignoring the bits that define version number, packet type and
    ** secondary header (high 5 bits of 16) we are left with the bits
    ** (mask = 0x7FF) that can identify 2048 unique input telemetry
    ** packets. Also, by using a fixed hash table size of 256 entries
    ** and using only the lower 8 bits of the bitmask as the result
    ** of the hash function, no single hash table entry will have more
    ** than 8 elements in its linked list.
    */
    return ((uint32)(CFE_SB_MsgIdToValue(MessageID) & LC_HASH_TABLE_MASK));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* LC_CreateHashTable() - create watchpoint hash table             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void LC_CreateHashTable(void)
{
    LC_WatchPtList_t *WatchPtLink;
    CFE_SB_MsgId_t    LastMessageID;
    CFE_SB_MsgId_t    MessageID;
    int32             MessageLinkIndex;
    int32             WatchPtTblIndex;
    CFE_Status_t      Result;

    /* Un-subscribe to any MessageID's referenced in previous Watchpoint Definition Table */
    for (MessageLinkIndex = 0; MessageLinkIndex < LC_OperData.MessageIDsCount; MessageLinkIndex++)
    {
        MessageID = LC_OperData.MessageLinks[MessageLinkIndex].MessageID;

        if ((Result = CFE_SB_Unsubscribe(MessageID, LC_OperData.CmdPipe)) != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(LC_UNSUB_WP_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Error unsubscribing watchpoint: MID=0x%08lX, RC=0x%08X",
                              (unsigned long)CFE_SB_MsgIdToValue(MessageID), (unsigned int)Result);
        }
    }

    /* Initialize hash table structures */
    memset(LC_OperData.HashTable, 0, sizeof(LC_OperData.HashTable));
    memset(LC_OperData.MessageLinks, 0, sizeof(LC_OperData.MessageLinks));
    memset(LC_OperData.WatchPtLinks, 0, sizeof(LC_OperData.WatchPtLinks));

    LC_OperData.MessageIDsCount = 0;
    LC_OperData.WatchpointCount = 0;

    LastMessageID = CFE_SB_INVALID_MSG_ID;
    WatchPtLink   = (LC_WatchPtList_t *)NULL;

    /* Process each entry in the Watchpoint Definition Table */
    for (WatchPtTblIndex = 0; WatchPtTblIndex < LC_MAX_WATCHPOINTS; WatchPtTblIndex++)
    {
        /* Skip unused watchpoint table entries */
        if (LC_OperData.WDTPtr[WatchPtTblIndex].DataType != LC_DATA_WATCH_NOT_USED)
        {
            MessageID = LC_OperData.WDTPtr[WatchPtTblIndex].MessageID;

            /* Use optimized code path if same MessageID as last watchpoint */
            if (CFE_SB_MsgId_Equal(LastMessageID, MessageID) && (WatchPtLink != (LC_WatchPtList_t *)NULL))
            {
                /* WatchPtLink points to last link in list for this Message ID */
                WatchPtLink->Next = &LC_OperData.WatchPtLinks[LC_OperData.WatchpointCount++];

                /* Add new link to end of list, point to new last link */
                WatchPtLink = WatchPtLink->Next;
            }
            else
            {
                /* May add message list link and subscribe to MessageID */
                WatchPtLink = LC_AddWatchpoint(MessageID);
            }

            /* Set watchpoint table index for this entry in list */
            WatchPtLink->WatchIndex = WatchPtTblIndex;

            LastMessageID = MessageID;
        }
    }

    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* LC_AddWatchpoint() - add one watchpoint entry to hash table     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

LC_WatchPtList_t *LC_AddWatchpoint(CFE_SB_MsgId_t MessageID)
{
    LC_MessageList_t *MessageLink;
    LC_WatchPtList_t *WatchPtLink;
    bool              NeedSubscription;
    int32             HashTableIndex;
    CFE_Status_t      Result;

    /* Hash function converts MessageID into hash table index */
    HashTableIndex = LC_GetHashTableIndex(MessageID);

    /* Subscribe only for first reference to each MessageID */
    NeedSubscription = false;

    /* Each hash table entry is a linked list of MessageID's with same hash result */
    if (LC_OperData.HashTable[HashTableIndex] == (LC_MessageList_t *)NULL)
    {
        /* Get next unused MessageID linked list entry */
        MessageLink = &LC_OperData.MessageLinks[LC_OperData.MessageIDsCount++];

        /* Set first (and only) link in this hash table entry linked list */
        LC_OperData.HashTable[HashTableIndex] = MessageLink;

        /* Set the MessageID for this link */
        MessageLink->MessageID = MessageID;

        /* Subscribe to first link in list */
        NeedSubscription = true;
    }
    else
    {
        /* Get start of linked list (all MID's with same hash result) */
        MessageLink = LC_OperData.HashTable[HashTableIndex];

        /* Find the link for this MessageID */
        while (!CFE_SB_MsgId_Equal(MessageLink->MessageID, MessageID))
        {
            if (MessageLink->Next == (LC_MessageList_t *)NULL)
            {
                /* Reached end of list without finding MessageID */
                MessageLink->Next = &LC_OperData.MessageLinks[LC_OperData.MessageIDsCount++];
                MessageLink       = MessageLink->Next;

                /* Add link with this MessageID (will exit loop) */
                MessageLink->MessageID = MessageID;

                /* Subscribe to new link in list */
                NeedSubscription = true;
            }
            else
            {
                /* Try the next link in the list */
                MessageLink = MessageLink->Next;
            }
        }
    }

    /* Subscribe only once to each MessageID */
    if (NeedSubscription)
    {
        if ((Result = CFE_SB_Subscribe(MessageID, LC_OperData.CmdPipe)) != CFE_SUCCESS)
        {
            /* Signal the error, but continue */
            CFE_EVS_SendEvent(LC_SUB_WP_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Error subscribing watchpoint: MID=0x%08lX, RC=0x%08X",
                              (unsigned long)CFE_SB_MsgIdToValue(MessageID), (unsigned int)Result);
        }
    }

    /* MessageLink points to the link for this MessageID */
    if (MessageLink->WatchPtList == (LC_WatchPtList_t *)NULL)
    {
        /* Get next unused watchpoint linked list entry */
        WatchPtLink = &LC_OperData.WatchPtLinks[LC_OperData.WatchpointCount++];

        /* Set the start (and only) link in the watchpoint link list */
        MessageLink->WatchPtList = WatchPtLink;
    }
    else
    {
        /* Find the end of the watchpoint linked list */
        WatchPtLink = MessageLink->WatchPtList;

        while (WatchPtLink->Next != (LC_WatchPtList_t *)NULL)
        {
            WatchPtLink = WatchPtLink->Next;
        }

        /* Add the new watchpoint link to the end of the list */
        WatchPtLink->Next = &LC_OperData.WatchPtLinks[LC_OperData.WatchpointCount++];
        WatchPtLink       = WatchPtLink->Next;
    }

    /* Return pointer to last link in watchpoint linked list */

    return WatchPtLink;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Handle a message with possible watchpoints                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_CheckMsgForWPs(CFE_SB_MsgId_t MessageID, const CFE_SB_Buffer_t *BufPtr)
{
    CFE_TIME_SysTime_t Timestamp;
    LC_MessageList_t * MessageList;
    LC_WatchPtList_t * WatchPtList;
    bool               WatchPtFound = false;

    Timestamp.Seconds    = 0;
    Timestamp.Subseconds = 0;

    /* Do nothing if disabled at the application level */
    if (LC_AppData.CurrentLCState != LC_STATE_DISABLED)
    {
        /* Use message timestamp - if none, use current time */
        CFE_MSG_GetMsgTime(&BufPtr->Msg, &Timestamp);

        if ((Timestamp.Seconds == 0) && (Timestamp.Subseconds == 0))
        {
            Timestamp = CFE_TIME_GetTime();
        }

        /* Performance Log (start time counter) */
        CFE_ES_PerfLogEntry(LC_WDT_SEARCH_PERF_ID);

        /* Get start of linked list (all MID's with same hash result) */
        MessageList = LC_OperData.HashTable[LC_GetHashTableIndex(MessageID)];

        /* NULL when list is empty or end of list */
        while (MessageList != (LC_MessageList_t *)NULL)
        {
            /* Compare this linked list entry for matching MessageID */
            if (CFE_SB_MsgId_Equal(MessageList->MessageID, MessageID))
            {
                /* Stop the search - we found it */
                break;
            }

            /* Max of 8 links per design */
            MessageList = MessageList->Next;
        }

        /* Should be true - else wouldn't subscribe to MessageID */
        if (MessageList != (LC_MessageList_t *)NULL)
        {
            /* Get linked list of WP's that reference MessageID */
            WatchPtList = MessageList->WatchPtList;

            /* NULL when list is empty or end of list */
            while (WatchPtList != (LC_WatchPtList_t *)NULL)
            {
                WatchPtFound = true;

                /* Verify that WP packet offset is within actual packet */
                if (LC_WPOffsetValid(WatchPtList->WatchIndex, BufPtr) == true)
                {
                    LC_ProcessWP(WatchPtList->WatchIndex, BufPtr, Timestamp);
                }

                /* No limit to how many WP's can reference one MessageID */
                WatchPtList = WatchPtList->Next;
            }
        }

        /* Performance Log (stop time counter) */
        CFE_ES_PerfLogExit(LC_WDT_SEARCH_PERF_ID);

        if (WatchPtFound == true)
        {
            LC_AppData.MonitoredMsgCount++;
        }
        else
        {
            /* MessageID with no defined watchpoints */
            CFE_EVS_SendEvent(LC_MID_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Msg with unreferenced message ID rcvd: ID = 0x%08lX",
                              (unsigned long)CFE_SB_MsgIdToValue(MessageID));
        }
    }

    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Process a single watchpoint                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ProcessWP(uint16 WatchIndex, const CFE_SB_Buffer_t *BufPtr, CFE_TIME_SysTime_t Timestamp)
{
    uint8 *WPDataPtr;
    uint8  PreviousResult;
    uint8  WPEvalResult;
    uint32 SizedWPData;
    uint32 MaskedWPData;
    uint32 StaleCounter;
    bool   SizedDataValid;

    /*
    ** Setup the pointer and get the massaged data
    */
    WPDataPtr = ((uint8 *)BufPtr) + LC_OperData.WDTPtr[WatchIndex].WatchpointOffset;

    SizedDataValid = LC_GetSizedWPData(WatchIndex, WPDataPtr, &SizedWPData);
    if (SizedDataValid == true)
    {
        /*
        ** Get the last evaluation result for this watchpoint
        */
        PreviousResult = LC_OperData.WRTPtr[WatchIndex].WatchResult;

        /*
        ** Apply the defined bitmask for this watchpoint and then
        ** call the mission defined custom function or do our own
        ** relational comparison.
        */
        MaskedWPData = SizedWPData & LC_OperData.WDTPtr[WatchIndex].BitMask;

        if (LC_OperData.WDTPtr[WatchIndex].OperatorID == LC_OPER_CUSTOM)
        {
            WPEvalResult =
                LC_CustomFunction(WatchIndex, MaskedWPData, BufPtr, LC_OperData.WDTPtr[WatchIndex].CustomFuncArgument);
        }
        else
        {
            WPEvalResult = LC_OperatorCompare(WatchIndex, MaskedWPData);
        }

        /*
        ** Update the watch result
        */
        LC_OperData.WRTPtr[WatchIndex].WatchResult = WPEvalResult;

        /*
        ** Update the watchpoint statistics based on the evaluation
        ** result
        */
        LC_OperData.WRTPtr[WatchIndex].EvaluationCount++;

        if (WPEvalResult == LC_WATCH_TRUE)
        {
            LC_OperData.WRTPtr[WatchIndex].CumulativeTrueCount++;
            LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount++;
            StaleCounter                                    = LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale;
            LC_OperData.WRTPtr[WatchIndex].CountdownToStale = StaleCounter;

            if ((PreviousResult == LC_WATCH_FALSE) || (PreviousResult == LC_WATCH_STALE))
            {
                LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.DataType = LC_OperData.WDTPtr[WatchIndex].DataType;

                LC_OperData.WRTPtr[WatchIndex].FalseToTrueCount++;

                LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Value = MaskedWPData;

                LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Timestamp.Seconds = Timestamp.Seconds;

                LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Timestamp.Subseconds = Timestamp.Subseconds;
            }
        }
        else if (WPEvalResult == LC_WATCH_FALSE)
        {
            LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount = 0;
            StaleCounter                                        = LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale;
            LC_OperData.WRTPtr[WatchIndex].CountdownToStale     = StaleCounter;

            if ((PreviousResult == LC_WATCH_TRUE) || (PreviousResult == LC_WATCH_STALE))
            {
                LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.DataType = LC_OperData.WDTPtr[WatchIndex].DataType;

                LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Value = MaskedWPData;

                LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Timestamp.Seconds = Timestamp.Seconds;

                LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Timestamp.Subseconds = Timestamp.Subseconds;
            }
        }
        else
        {
            /*
            ** WPEvalResult is STALE or ERROR
            */
            LC_OperData.WRTPtr[WatchIndex].CountdownToStale = 0;
        }

    } /* end SizedDataValid if */

    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Perform a watchpoint relational comparison                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint8 LC_OperatorCompare(uint16 WatchIndex, uint32 ProcessedWPData)
{
    uint8          EvalResult;
    LC_MultiType_t WatchpointValue;
    LC_MultiType_t ComparisonValue;

    /*
     * The "ProcessedWPData" has been already normalized to be
     * 32 bits wide and in the native CPU byte order.  For actual
     * comparison, it needs to be truncated back down to the same
     * size as the reference value.
     */
    switch (LC_OperData.WDTPtr[WatchIndex].DataType)
    {
        case LC_DATA_WATCH_UBYTE:
        case LC_DATA_WATCH_BYTE:
            WatchpointValue.Unsigned8 = ProcessedWPData & 0xFF;
            break;

        case LC_DATA_WATCH_WORD_BE:
        case LC_DATA_WATCH_WORD_LE:
        case LC_DATA_WATCH_UWORD_BE:
        case LC_DATA_WATCH_UWORD_LE:
            WatchpointValue.Unsigned16 = ProcessedWPData & 0xFFFF;
            break;

        case LC_DATA_WATCH_DWORD_BE:
        case LC_DATA_WATCH_DWORD_LE:
        case LC_DATA_WATCH_UDWORD_BE:
        case LC_DATA_WATCH_UDWORD_LE:
        case LC_DATA_WATCH_FLOAT_BE:
        case LC_DATA_WATCH_FLOAT_LE:
        default:
            WatchpointValue.Unsigned32 = ProcessedWPData;
            break;
    }
    /* SAD: Using memcpy to safely copy the float value from LC_MultiType_t to ComparisonValue, preserving bitwise
     * representation */
    memcpy(&ComparisonValue, &LC_OperData.WDTPtr[WatchIndex].ComparisonValue, sizeof(LC_MultiType_t));

    /*
    ** Handle the comparison appropriately depending on the data type
    ** Any endian difference was handled when the watchpoint
    ** data was extracted from the SB message
    */
    switch (LC_OperData.WDTPtr[WatchIndex].DataType)
    {
        /*
        ** Signed integer types will get sign extended
        */
        case LC_DATA_WATCH_BYTE:
            EvalResult = LC_SignedCompare(WatchIndex, WatchpointValue.Signed8, ComparisonValue.Signed8);
            break;

        case LC_DATA_WATCH_WORD_BE:
        case LC_DATA_WATCH_WORD_LE:
            EvalResult = LC_SignedCompare(WatchIndex, WatchpointValue.Signed16, ComparisonValue.Signed16);
            break;

        case LC_DATA_WATCH_DWORD_BE:
        case LC_DATA_WATCH_DWORD_LE:
            EvalResult = LC_SignedCompare(WatchIndex, WatchpointValue.Signed32, ComparisonValue.Signed32);
            break;

        /*
        ** Unsigned integer types will get zero extended
        */
        case LC_DATA_WATCH_UBYTE:
            EvalResult = LC_UnsignedCompare(WatchIndex, WatchpointValue.Unsigned8, ComparisonValue.Unsigned8);
            break;

        case LC_DATA_WATCH_UWORD_BE:
        case LC_DATA_WATCH_UWORD_LE:
            EvalResult = LC_UnsignedCompare(WatchIndex, WatchpointValue.Unsigned16, ComparisonValue.Unsigned16);
            break;

        case LC_DATA_WATCH_UDWORD_BE:
        case LC_DATA_WATCH_UDWORD_LE:
            EvalResult = LC_UnsignedCompare(WatchIndex, WatchpointValue.Unsigned32, ComparisonValue.Unsigned32);
            break;

        /*
        ** Floating point values are handled separately
        */
        case LC_DATA_WATCH_FLOAT_BE:
        case LC_DATA_WATCH_FLOAT_LE:
            EvalResult = LC_FloatCompare(WatchIndex, &WatchpointValue, &ComparisonValue);
            break;

        default:
            /*
            ** This should have been caught before now, but we'll
            ** handle it just in case we ever get here.
            */
            CFE_EVS_SendEvent(LC_WP_DATATYPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "WP has undefined data type: WP = %d, DataType = %d", WatchIndex,
                              LC_OperData.WDTPtr[WatchIndex].DataType);

            EvalResult = LC_WATCH_ERROR;
            break;
    }

    return EvalResult;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Perform a watchpoint signed integer comparison                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint8 LC_SignedCompare(uint16 WatchIndex, int32 WPValue, int32 CompareValue)
{
    uint8 EvalResult;
    uint8 OperatorID;

    OperatorID = LC_OperData.WDTPtr[WatchIndex].OperatorID;

    switch (OperatorID)
    {
        case LC_OPER_LE:
            EvalResult = (WPValue <= CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_LT:
            EvalResult = (WPValue < CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_EQ:
            EvalResult = (WPValue == CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_NE:
            EvalResult = (WPValue != CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_GT:
            EvalResult = (WPValue > CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_GE:
            EvalResult = (WPValue >= CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        default:
            /*
            ** This should have been caught before now, but we'll
            ** handle it just in case we ever get here.
            */
            CFE_EVS_SendEvent(LC_WP_OPERID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "WP has invalid operator ID: WP = %d, OperID = %d", WatchIndex, OperatorID);

            EvalResult = LC_WATCH_ERROR;
            break;
    }

    return EvalResult;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Perform a watchpoint unsigned integer comparison                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint8 LC_UnsignedCompare(uint16 WatchIndex, uint32 WPValue, uint32 CompareValue)
{
    uint8 EvalResult;
    uint8 OperatorID;

    OperatorID = LC_OperData.WDTPtr[WatchIndex].OperatorID;

    switch (OperatorID)
    {
        case LC_OPER_LE:
            EvalResult = (WPValue <= CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_LT:
            EvalResult = (WPValue < CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_EQ:
            EvalResult = (WPValue == CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_NE:
            EvalResult = (WPValue != CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_GT:
            EvalResult = (WPValue > CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        case LC_OPER_GE:
            EvalResult = (WPValue >= CompareValue) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
            break;

        default:
            /*
            ** This should have been caught before now, but we'll
            ** handle it just in case we ever get here.
            */
            CFE_EVS_SendEvent(LC_WP_OPERID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "WP has invalid operator ID: WP = %d, OperID = %d", WatchIndex, OperatorID);

            EvalResult = LC_WATCH_ERROR;
            break;
    }

    return EvalResult;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Perform a floating point number comparison                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint8 LC_FloatCompare(uint16 WatchIndex, LC_MultiType_t *WPMultiType, LC_MultiType_t *CompareMultiType)
{
    uint8 EvalResult;
    uint8 OperatorID;
    float WPFloat;
    float CompareFloat;
    float Diff;

    OperatorID = LC_OperData.WDTPtr[WatchIndex].OperatorID;

    /* SAD: Using memcpy to safely copy the float value from LC_MultiType_t to WPFloat, preserving bitwise
     * representation */
    memcpy(&WPFloat, WPMultiType, sizeof(float));
    /* SAD: Using memcpy to safely copy the float value from LC_MultiType_t to CompareFloat, preserving bitwise
     * representation */
    memcpy(&CompareFloat, CompareMultiType, sizeof(float));

    /*
    ** Before we do any comparison, check the watchpoint value for
    ** a floating point NAN (not-a-number). NAN comparisons don't
    ** work and can generate floating point exceptions. By contrast
    ** comparisons with infinite numbers will behave as they should
    ** so we don't try to catch those (we would rather they generate
    ** watchpoint violations).
    **
    ** The comparison (threshold) value comes from the Watchpoint
    ** Definition Table (WDT) and any weird values should get nailed
    ** during table validation.
    */
    if (!isnan(WPFloat))
    {
        switch (OperatorID)
        {
            case LC_OPER_LE:
                EvalResult = (WPFloat <= CompareFloat) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
                break;

            case LC_OPER_LT:
                EvalResult = (WPFloat < CompareFloat) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
                break;

            case LC_OPER_EQ:
                Diff       = (WPFloat > CompareFloat) ? (WPFloat - CompareFloat) : (CompareFloat - WPFloat);
                EvalResult = (Diff <= (float)LC_FLOAT_TOLERANCE) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
                break;

            case LC_OPER_NE:
                Diff       = (WPFloat > CompareFloat) ? (WPFloat - CompareFloat) : (CompareFloat - WPFloat);
                EvalResult = (Diff > (float)LC_FLOAT_TOLERANCE) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
                break;

            case LC_OPER_GT:
                EvalResult = (WPFloat > CompareFloat) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
                break;

            case LC_OPER_GE:
                EvalResult = (WPFloat >= CompareFloat) ? LC_WATCH_TRUE : LC_WATCH_FALSE;
                break;

            default:
                /*
                ** This should have been caught before now, but we'll
                ** handle it just in case we ever get here.
                */
                CFE_EVS_SendEvent(LC_WP_OPERID_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "WP has invalid operator ID: WP = %d, OperID = %d", WatchIndex, OperatorID);

                EvalResult = LC_WATCH_ERROR;
                break;

        } /*  end of switch  */

    } /* end LC_WPIsNAN if */
    else
    {
        CFE_EVS_SendEvent(LC_WP_NAN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "WP data value is a float NAN: WP = %d, Value = 0x%08X", WatchIndex,
                          *((unsigned int *)WPMultiType));

        EvalResult = LC_WATCH_ERROR;
    }

    return EvalResult;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Checks if a defined watchpoint offset will send us past the     */
/* end of the received message                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool LC_WPOffsetValid(uint16 WatchIndex, const CFE_SB_Buffer_t *BufPtr)
{
    size_t         MsgLength = 0;
    uint32         Offset;
    uint32         NumOfDataBytes = 0;
    bool           OffsetValid    = true;
    CFE_SB_MsgId_t MessageID      = CFE_SB_INVALID_MSG_ID;

    /*
    ** Check the message length against the watchpoint
    ** offset and data type to make sure we won't
    ** try to read past it.
    */
    switch (LC_OperData.WDTPtr[WatchIndex].DataType)
    {
        case LC_DATA_WATCH_BYTE:
        case LC_DATA_WATCH_UBYTE:
            NumOfDataBytes = sizeof(uint8);
            break;

        case LC_DATA_WATCH_WORD_BE:
        case LC_DATA_WATCH_WORD_LE:
        case LC_DATA_WATCH_UWORD_BE:
        case LC_DATA_WATCH_UWORD_LE:
            NumOfDataBytes = sizeof(uint16);
            break;

        case LC_DATA_WATCH_DWORD_BE:
        case LC_DATA_WATCH_DWORD_LE:
        case LC_DATA_WATCH_UDWORD_BE:
        case LC_DATA_WATCH_UDWORD_LE:
            NumOfDataBytes = sizeof(uint32);
            break;

        case LC_DATA_WATCH_FLOAT_BE:
        case LC_DATA_WATCH_FLOAT_LE:
            NumOfDataBytes = sizeof(float);
            break;

        default:
            /*
            ** This should have been caught before now, but we'll
            ** handle it just in case we ever get here.
            */
            CFE_EVS_SendEvent(LC_WP_DATATYPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "WP has undefined data type: WP = %d, DataType = %d", WatchIndex,
                              LC_OperData.WDTPtr[WatchIndex].DataType);

            LC_OperData.WRTPtr[WatchIndex].WatchResult      = LC_WATCH_ERROR;
            LC_OperData.WRTPtr[WatchIndex].CountdownToStale = 0;

            return false;
            break;

    } /* end switch */

    CFE_MSG_GetSize(&BufPtr->Msg, &MsgLength);

    Offset = LC_OperData.WDTPtr[WatchIndex].WatchpointOffset;

    if ((Offset + NumOfDataBytes) > MsgLength)
    {
        OffsetValid = false;

        CFE_MSG_GetMsgId(&BufPtr->Msg, &MessageID);

        CFE_EVS_SendEvent(LC_WP_OFFSET_ERR_EID, CFE_EVS_EventType_ERROR,
                          "WP offset error: MID = 0x%08lX, WP = %d, Offset = %d, DataSize = %d, MsgLen = %d",
                          (unsigned long)CFE_SB_MsgIdToValue(MessageID), WatchIndex, (int)Offset, (int)NumOfDataBytes,
                          (int)MsgLength);

        LC_OperData.WRTPtr[WatchIndex].WatchResult      = LC_WATCH_ERROR;
        LC_OperData.WRTPtr[WatchIndex].CountdownToStale = 0;
    }

    return OffsetValid;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Copy a single watchpoint datum and simultaneously byteswap it   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_CopyBytesWithSwap(LC_MultiType_t *DestBuffer, const uint8 *SrcPtr, LC_MultiType_t *SwapMap, uint32 NumBytes)
{
    uint8 *      DestPtr;
    const uint8 *SwapMapPtr;

    DestPtr    = (uint8 *)DestBuffer;
    SwapMapPtr = (const uint8 *)SwapMap;
    while (NumBytes > 0)
    {
        --NumBytes;
        DestPtr[NumBytes] = SrcPtr[SwapMapPtr[NumBytes] & 0x3];
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Get sized watchpoint data                                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool LC_GetSizedWPData(uint16 WatchIndex, const uint8 *WPDataPtr, uint32 *SizedDataPtr)
{
    bool           Success = true;
    LC_MultiType_t ConvBuffer;
    LC_MultiType_t TempBuffer;

    ConvBuffer.Unsigned32 = 0;
    TempBuffer.Unsigned32 = 0;

    /*
    ** Get the watchpoint data value (which may be on a misaligned
    ** address boundary) and put it into an unsigned 32 properly
    ** handling endian and sign extension issues
    */
    switch (LC_OperData.WDTPtr[WatchIndex].DataType)
    {
        case LC_DATA_WATCH_BYTE:
            TempBuffer.Signed8 = *WPDataPtr;
            *SizedDataPtr      = TempBuffer.Signed8; /* Extend signed 8 bit value to 32 bits */
            break;

        case LC_DATA_WATCH_UBYTE:
            *SizedDataPtr = *WPDataPtr; /* Extend unsigned 8 bit value to 32 bits */
            break;

        case LC_DATA_WATCH_WORD_BE:
            ConvBuffer.Unsigned16 = LC_16BIT_BE_VAL;
            LC_CopyBytesWithSwap(&TempBuffer, WPDataPtr, &ConvBuffer, sizeof(int16));
            *SizedDataPtr = TempBuffer.Signed16; /* Extend signed 16 bit value to 32 bits */
            break;

        case LC_DATA_WATCH_WORD_LE:
            ConvBuffer.Unsigned16 = LC_16BIT_LE_VAL;
            LC_CopyBytesWithSwap(&TempBuffer, WPDataPtr, &ConvBuffer, sizeof(int16));
            *SizedDataPtr = TempBuffer.Signed16; /* Extend signed 16 bit value to 32 bits */
            break;

        case LC_DATA_WATCH_UWORD_BE:
            ConvBuffer.Unsigned16 = LC_16BIT_BE_VAL;
            LC_CopyBytesWithSwap(&TempBuffer, WPDataPtr, &ConvBuffer, sizeof(uint16));
            *SizedDataPtr = TempBuffer.Unsigned16; /* Extend unsigned 16 bit value to 32 bits */
            break;

        case LC_DATA_WATCH_UWORD_LE:
            ConvBuffer.Unsigned16 = LC_16BIT_LE_VAL;
            LC_CopyBytesWithSwap(&TempBuffer, WPDataPtr, &ConvBuffer, sizeof(uint16));
            *SizedDataPtr = TempBuffer.Unsigned16; /* Extend unsigned 16 bit value to 32 bits */
            break;

        case LC_DATA_WATCH_DWORD_BE:
        case LC_DATA_WATCH_UDWORD_BE:
        case LC_DATA_WATCH_FLOAT_BE:
            ConvBuffer.Unsigned32 = LC_32BIT_BE_VAL;
            LC_CopyBytesWithSwap(&TempBuffer, WPDataPtr, &ConvBuffer, sizeof(uint32));
            *SizedDataPtr = TempBuffer.Unsigned32; /* Straight copy - no extension (signed or unsigned) */
            break;

        case LC_DATA_WATCH_DWORD_LE:
        case LC_DATA_WATCH_UDWORD_LE:
        case LC_DATA_WATCH_FLOAT_LE:
            ConvBuffer.Unsigned32 = LC_32BIT_LE_VAL;
            LC_CopyBytesWithSwap(&TempBuffer, WPDataPtr, &ConvBuffer, sizeof(uint32));
            *SizedDataPtr = TempBuffer.Unsigned32; /* Straight copy - no extension (signed or unsigned) */
            break;

        default:
            /*
            ** This should have been caught before now, but we'll
            ** handle it just in case we ever get here.
            */
            CFE_EVS_SendEvent(LC_WP_DATATYPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "WP has undefined data type: WP = %d, DataType = %d", WatchIndex,
                              LC_OperData.WDTPtr[WatchIndex].DataType);

            LC_OperData.WRTPtr[WatchIndex].WatchResult      = LC_WATCH_ERROR;
            LC_OperData.WRTPtr[WatchIndex].CountdownToStale = 0;

            Success       = false;
            *SizedDataPtr = 0;
            break;

    } /* end switch */

    /*
    ** Return success flag
    */
    return Success;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Validate the watchpoint definition table (WDT)                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 LC_ValidateWDT(void *TableData)
{
    LC_WDTEntry_t *TableArray = (LC_WDTEntry_t *)TableData;

    int32 EntryResult = LC_WDTVAL_ERR_NONE;
    int32 TableResult = CFE_SUCCESS;
    int32 TableIndex;

    uint8          DataType;
    uint8          OperatorID;
    CFE_SB_MsgId_t MessageID;
    uint32         PrintableBits;
    float          FloatValue;

    int32 GoodCount   = 0;
    int32 BadCount    = 0;
    int32 UnusedCount = 0;

    /*
    ** Verify each entry in the pending watchpoint definition table
    */
    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        DataType   = TableArray[TableIndex].DataType;
        OperatorID = TableArray[TableIndex].OperatorID;
        MessageID  = TableArray[TableIndex].MessageID;

        if (DataType == LC_DATA_WATCH_NOT_USED)
        {
            /*
            ** Unused table entry
            */
            UnusedCount++;
        }
        else if ((DataType != LC_DATA_WATCH_BYTE) && (DataType != LC_DATA_WATCH_UBYTE) &&
                 (DataType != LC_DATA_WATCH_WORD_BE) && (DataType != LC_DATA_WATCH_WORD_LE) &&
                 (DataType != LC_DATA_WATCH_UWORD_BE) && (DataType != LC_DATA_WATCH_UWORD_LE) &&
                 (DataType != LC_DATA_WATCH_DWORD_BE) && (DataType != LC_DATA_WATCH_DWORD_LE) &&
                 (DataType != LC_DATA_WATCH_UDWORD_BE) && (DataType != LC_DATA_WATCH_UDWORD_LE) &&
                 (DataType != LC_DATA_WATCH_FLOAT_BE) && (DataType != LC_DATA_WATCH_FLOAT_LE))
        {
            /*
            ** Invalid data type
            */
            BadCount++;
            EntryResult = LC_WDTVAL_ERR_DATATYPE;
        }
        else if ((OperatorID != LC_OPER_LT) && (OperatorID != LC_OPER_LE) && (OperatorID != LC_OPER_NE) &&
                 (OperatorID != LC_OPER_EQ) && (OperatorID != LC_OPER_GE) && (OperatorID != LC_OPER_GT) &&
                 (OperatorID != LC_OPER_CUSTOM))
        {
            /*
            ** Invalid operator
            */
            BadCount++;
            EntryResult = LC_WDTVAL_ERR_OPER;
        }
        else if (!CFE_SB_IsValidMsgId(MessageID))
        {
            /*
            ** Bad message ID
            */
            BadCount++;
            EntryResult = LC_WDTVAL_ERR_MID;
        }
        else if ((DataType == LC_DATA_WATCH_FLOAT_BE) || (DataType == LC_DATA_WATCH_FLOAT_LE))
        {
            /* SAD: Using memcpy to safely copy the float value from LC_MultiType_t to FloatValue, preserving bitwise
             * representation */
            memcpy(&FloatValue, &TableArray[TableIndex].ComparisonValue, sizeof(FloatValue));

            /*
            ** Check the floating point comparison value for
            ** NAN (not-a-number) or infinite values
            */
            if (isnan(FloatValue))
            {
                BadCount++;
                EntryResult = LC_WDTVAL_ERR_FPNAN;
            }
            else if (!isfinite(FloatValue))
            {
                BadCount++;
                EntryResult = LC_WDTVAL_ERR_FPINF;
            }
            else
            {
                /*
                ** We passed all checks for this floating point entry
                */
                GoodCount++;
            }
        }
        else
        {
            /*
            ** We passed all checks for this non-floating point entry
            */
            GoodCount++;
        }

        /*
        ** Generate detailed event for "first" error
        */
        if ((EntryResult != LC_WDTVAL_ERR_NONE) && (TableResult == CFE_SUCCESS))
        {
            if ((EntryResult == LC_WDTVAL_ERR_FPNAN) || (EntryResult == LC_WDTVAL_ERR_FPINF))
            {
                /* SAD: Using memcpy to safely copy the uint32 value from LC_MultiType_t to PrintableBits, preserving
                 * bitwise representation */
                memcpy(&PrintableBits, &TableArray[TableIndex].ComparisonValue, sizeof(PrintableBits));
                CFE_EVS_SendEvent(LC_WDTVAL_FPERR_EID, CFE_EVS_EventType_ERROR,
                                  "WDT verify float err: WP = %d, Err = %d, ComparisonValue = 0x%08X", (int)TableIndex,
                                  (int)EntryResult, (unsigned int)PrintableBits);
            }
            else
            {
                CFE_EVS_SendEvent(LC_WDTVAL_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "WDT verify err: WP = %d, Err = %d, DType = %d, Oper = %d, MID = 0x%08lX",
                                  (int)TableIndex, (int)EntryResult, DataType, OperatorID,
                                  (unsigned long)CFE_SB_MsgIdToValue(MessageID));
            }

            TableResult = EntryResult;
        }

    } /* end TableIndex for */

    /*
    ** Generate informational event with error totals
    */
    CFE_EVS_SendEvent(LC_WDTVAL_INF_EID, CFE_EVS_EventType_INFORMATION,
                      "WDT verify results: good = %d, bad = %d, unused = %d", (int)GoodCount, (int)BadCount,
                      (int)UnusedCount);

    return TableResult;
}
