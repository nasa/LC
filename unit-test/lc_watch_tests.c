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
 *   This file contains unit test cases for the functions contained in the file lc_app.c
 */

/*
 * Includes
 */

#include "lc_watch.h"
#include "lc_app.h"
#include "lc_msg.h"
#include "lc_msgdefs.h"
#include "lc_msgids.h"
#include "lc_eventids.h"
#include "lc_version.h"
#include "lc_test_utils.h"
#include "lc_utils.h"
#include "lc_custom.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

#include <unistd.h>
#include <stdlib.h>

uint8 call_count_CFE_EVS_SendEvent;

void LC_CreateHashTable_Test(void)
{
    uint32 i;

    /* One valid and one invalid unsubscribe */
    LC_OperData.MessageIDsCount = 2;
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Unsubscribe), 1, -1);

    /* Default entries to unused */
    for (i = 0; i < LC_MAX_WATCHPOINTS; i++)
    {
        LC_OperData.WDTPtr[i].DataType = LC_DATA_WATCH_NOT_USED;
    }

    /* Match MsgID but null watch point list */
    LC_OperData.WDTPtr[0].DataType  = LC_DATA_WATCH_BYTE;
    LC_OperData.WDTPtr[0].MessageID = CFE_SB_INVALID_MSG_ID;

    /* MsgID mismatch */
    LC_OperData.WDTPtr[1].DataType  = LC_DATA_WATCH_BYTE;
    LC_OperData.WDTPtr[1].MessageID = LC_UT_MID_1;

    /* Match MsgID and non-NULL WatchPLink */
    LC_OperData.WDTPtr[2].DataType  = LC_DATA_WATCH_BYTE;
    LC_OperData.WDTPtr[2].MessageID = LC_UT_MID_1;

    /* Execute the function being tested */
    LC_CreateHashTable();

    UtAssert_UINT32_EQ(LC_OperData.WatchpointCount, 3);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_UNSUB_WP_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void LC_AddWatchpoint_Test_HashTableAndWatchPtListNullPointersNominal(void)
{
    CFE_SB_MsgId_t MessageID = LC_UT_MID_1;
    uint32         HashIndex;

    HashIndex = LC_GetHashTableIndex(MessageID);

    /* Execute the function being tested */
    UtAssert_ADDRESS_EQ(LC_AddWatchpoint(MessageID), &LC_OperData.WatchPtLinks[0]);

    /* Verify results */
    UtAssert_ADDRESS_EQ(LC_OperData.HashTable[HashIndex], &LC_OperData.MessageLinks[0]);
    UtAssert_BOOL_TRUE(CFE_SB_MsgId_Equal(LC_OperData.MessageLinks[0].MessageID, MessageID));
    UtAssert_ADDRESS_EQ(LC_OperData.MessageLinks[0].WatchPtList, &LC_OperData.WatchPtLinks[0]);
    UtAssert_UINT16_EQ(LC_OperData.MessageIDsCount, 1);
    UtAssert_UINT16_EQ(LC_OperData.WatchpointCount, 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void LC_AddWatchpoint_Test_HashTableAndWatchPtListNotNullPointerTwoMsgLinksMIDFound(void)
{
    CFE_SB_MsgId_t MessageID = LC_UT_MID_1;

    /* Mark number of used elements set below */
    LC_OperData.WatchpointCount = 2;
    LC_OperData.MessageIDsCount = 2;

    /* Point hash to MessageLink 1 */
    LC_OperData.HashTable[LC_GetHashTableIndex(MessageID)] = &LC_OperData.MessageLinks[0];

    /* No match on first message link */
    LC_OperData.MessageLinks[0].MessageID = LC_UT_MID_2;
    LC_OperData.MessageLinks[0].Next      = &LC_OperData.MessageLinks[1];

    /* Match on second with a non-null watchpoint list */
    LC_OperData.MessageLinks[1].MessageID   = MessageID;
    LC_OperData.MessageLinks[1].WatchPtList = &LC_OperData.WatchPtLinks[0];
    LC_OperData.WatchPtLinks[0].Next        = &LC_OperData.WatchPtLinks[1];

    /* Execute the function being tested */
    UtAssert_ADDRESS_EQ(LC_AddWatchpoint(MessageID), &LC_OperData.WatchPtLinks[2]);

    /* Verify additional used watchpoint */
    UtAssert_UINT16_EQ(LC_OperData.WatchpointCount, 3);
    UtAssert_ADDRESS_EQ(LC_OperData.WatchPtLinks[1].Next, &LC_OperData.WatchPtLinks[2]);

    /* Verify no new message id used */
    UtAssert_UINT16_EQ(LC_OperData.MessageIDsCount, 2);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void LC_AddWatchpoint_Test_NullPointersErrorSubscribingWatchpoint(void)
{
    CFE_SB_MsgId_t MessageID = LC_UT_MID_1;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    uint32         HashIndex;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Error subscribing watchpoint: MID=0x%%08lX, RC=0x%%08X");

    HashIndex = LC_GetHashTableIndex(MessageID);

    /* Set to generate error message LC_SUB_WP_ERR_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_Subscribe), -1);

    /* Execute the function being tested */
    UtAssert_ADDRESS_EQ(LC_AddWatchpoint(MessageID), &LC_OperData.WatchPtLinks[0]);

    /* Verify results */
    UtAssert_ADDRESS_EQ(LC_OperData.HashTable[HashIndex], &LC_OperData.MessageLinks[0]);
    UtAssert_BOOL_TRUE(CFE_SB_MsgId_Equal(LC_OperData.MessageLinks[0].MessageID, MessageID));
    UtAssert_ADDRESS_EQ(LC_OperData.MessageLinks[0].WatchPtList, &LC_OperData.WatchPtLinks[0]);
    UtAssert_UINT16_EQ(LC_OperData.MessageIDsCount, 1);
    UtAssert_UINT16_EQ(LC_OperData.WatchpointCount, 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_SUB_WP_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_AddWatchpoint_Test_NonNullMessageList(void)
{
    CFE_SB_MsgId_t MessageID = LC_UT_MID_1;
    uint16         HashIndex;

    HashIndex = LC_GetHashTableIndex(MessageID);

    /* Point to first element */
    LC_OperData.HashTable[HashIndex] = &LC_OperData.MessageLinks[0];

    /* List without a match */
    LC_OperData.MessageIDsCount           = 1;
    LC_OperData.MessageLinks[0].MessageID = LC_UT_MID_2;

    /* Execute the function being tested */
    UtAssert_ADDRESS_EQ(LC_AddWatchpoint(MessageID), &LC_OperData.WatchPtLinks[0]);

    /* Verify results */
    UtAssert_ADDRESS_EQ(LC_OperData.HashTable[HashIndex], &LC_OperData.MessageLinks[0]);
    UtAssert_ADDRESS_EQ(LC_OperData.MessageLinks[0].Next, &LC_OperData.MessageLinks[1]);
    UtAssert_BOOL_TRUE(CFE_SB_MsgId_Equal(LC_OperData.MessageLinks[1].MessageID, MessageID));
    UtAssert_ADDRESS_EQ(LC_OperData.MessageLinks[1].WatchPtList, &LC_OperData.WatchPtLinks[0]);
    UtAssert_UINT16_EQ(LC_OperData.MessageIDsCount, 2);
    UtAssert_UINT16_EQ(LC_OperData.WatchpointCount, 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void LC_CheckMsgForWPs_Test_Nominal(void)
{
    uint16             WatchIndex = 0;
    CFE_SB_MsgId_t     TestMsgId  = LC_UT_MID_1;
    size_t             MsgSize    = sizeof(UT_CmdBuf);
    CFE_TIME_SysTime_t Timestamp  = {.Seconds = 0, .Subseconds = 0};

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgTime), &Timestamp, sizeof(Timestamp), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    LC_OperData.HashTable[LC_GetHashTableIndex(TestMsgId)] = &LC_OperData.MessageLinks[0];

    /* Not disabled */
    LC_AppData.CurrentLCState = LC_STATE_ACTIVE;

    /* Miss on first, match on next */
    LC_OperData.MessageLinks[0].MessageID   = LC_UT_MID_2;
    LC_OperData.MessageLinks[0].Next        = &LC_OperData.MessageLinks[1];
    LC_OperData.MessageLinks[1].MessageID   = TestMsgId;
    LC_OperData.MessageLinks[1].WatchPtList = &LC_OperData.WatchPtLinks[0];

    LC_OperData.WatchPtLinks[0].WatchIndex          = WatchIndex;
    LC_OperData.WDTPtr[WatchIndex].DataType         = LC_DATA_WATCH_BYTE;
    LC_OperData.WDTPtr[WatchIndex].WatchpointOffset = 0;
    LC_OperData.WDTPtr[WatchIndex].OperatorID       = LC_OPER_CUSTOM;

    /* Execute the function being tested */
    LC_CheckMsgForWPs(TestMsgId, &UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_UINT32_EQ(LC_AppData.MonitoredMsgCount, 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CFE_TIME_GetTime, 1);
}

void LC_CheckMsgForWPs_Test_NominalMsgTime1(void)
{
    CFE_SB_MsgId_t     TestMsgId = LC_UT_MID_1;
    size_t             MsgSize   = sizeof(UT_CmdBuf);
    CFE_TIME_SysTime_t Timestamp = {.Seconds = 1, .Subseconds = 0};

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgTime), &Timestamp, sizeof(Timestamp), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    LC_OperData.HashTable[LC_GetHashTableIndex(TestMsgId)] = &LC_OperData.MessageLinks[0];

    /* Not disabled */
    LC_AppData.CurrentLCState = LC_STATE_ACTIVE;

    /* Match on first, Null message list */
    LC_OperData.MessageLinks[0].MessageID = TestMsgId;

    /* Execute the function being tested */
    LC_CheckMsgForWPs(TestMsgId, &UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_UINT32_EQ(LC_AppData.MonitoredMsgCount, 0);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_MID_INF_EID);
    UtAssert_STUB_COUNT(CFE_TIME_GetTime, 0);
}

void LC_CheckMsgForWPs_Test_NominalMsgTime2(void)
{
    uint16             WatchIndex = 0;
    CFE_SB_MsgId_t     TestMsgId  = LC_UT_MID_1;
    size_t             MsgSize    = sizeof(UT_CmdBuf);
    CFE_TIME_SysTime_t Timestamp  = {.Seconds = 0, .Subseconds = 1};

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgTime), &Timestamp, sizeof(Timestamp), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    LC_OperData.HashTable[LC_GetHashTableIndex(TestMsgId)] = &LC_OperData.MessageLinks[0];

    /* Not disabled */
    LC_AppData.CurrentLCState = LC_STATE_ACTIVE;

    /* Miss on first */
    LC_OperData.MessageLinks[0].MessageID   = TestMsgId;
    LC_OperData.MessageLinks[0].WatchPtList = &LC_OperData.WatchPtLinks[0];
    LC_OperData.WatchPtLinks[0].WatchIndex  = WatchIndex;

    /* Bad offset */
    LC_OperData.WDTPtr[WatchIndex].DataType         = LC_DATA_WATCH_BYTE;
    LC_OperData.WDTPtr[WatchIndex].WatchpointOffset = MsgSize + 1;
    LC_OperData.WDTPtr[WatchIndex].OperatorID       = LC_OPER_CUSTOM;

    /* Execute the function being tested */
    LC_CheckMsgForWPs(TestMsgId, &UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_UINT32_EQ(LC_AppData.MonitoredMsgCount, 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WP_OFFSET_ERR_EID);
    UtAssert_STUB_COUNT(CFE_TIME_GetTime, 0);
}

void LC_CheckMsgForWPs_Test_NominalDisabled(void)
{
    CFE_SB_MsgId_t TestMsgId = LC_UT_MID_1;

    LC_AppData.CurrentLCState = LC_STATE_DISABLED;

    /* Execute the function being tested */
    LC_CheckMsgForWPs(TestMsgId, &UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_UINT32_EQ(LC_AppData.MonitoredMsgCount, 0);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CFE_TIME_GetTime, 0);
}

void LC_CheckMsgForWPs_Test_UnreferencedMessageID(void)
{
    CFE_SB_MsgId_t     TestMsgId = LC_UT_MID_1;
    CFE_TIME_SysTime_t Timestamp = {.Seconds = 0, .Subseconds = 0};
    int32              strCmpResult;
    char               ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Msg with unreferenced message ID rcvd: ID = 0x%%08lX");

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgTime), &Timestamp, sizeof(Timestamp), false);

    /* Execute the function being tested */
    LC_CheckMsgForWPs(TestMsgId, &UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_MID_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_ProcessWP_Test_CustomFunctionWatchFalse(void)
{
    uint16             WatchIndex = 0;
    CFE_TIME_SysTime_t Timestamp;
    CFE_SB_MsgId_t     TestMsgId = LC_UT_MID_1;

    memset(&Timestamp, 0, sizeof(Timestamp));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    LC_OperData.WDTPtr[WatchIndex].DataType           = LC_DATA_WATCH_BYTE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID         = LC_OPER_CUSTOM;
    LC_OperData.WDTPtr[WatchIndex].CustomFuncArgument = 0;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale = 1;

    /* Execute the function being tested */
    LC_ProcessWP(WatchIndex, &UT_CmdBuf.Buf, Timestamp);

    /* Verify results */
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 0,
                  "LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 0");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1,
                  "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1");

    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_FALSE,
                  "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_FALSE");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1,
                  "LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1");

    /* Note: this event message is generated in a subfunction, but is tested anyway to verify that the correct code path
     * has been reached */
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_ProcessWP_Test_OperatorCompareError(void)
{
    uint16             WatchIndex = 0;
    CFE_TIME_SysTime_t Timestamp;
    CFE_SB_MsgId_t     TestMsgId = LC_UT_MID_1;

    memset(&Timestamp, 0, sizeof(Timestamp));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    LC_OperData.WDTPtr[WatchIndex].DataType        = LC_DATA_WATCH_BYTE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID      = 99;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount = 0;

    /* Execute the function being tested */
    LC_ProcessWP(WatchIndex, &UT_CmdBuf.Buf, Timestamp);

    /* Verify results */
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_ERROR,
                  "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_ERROR");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1,
                  "LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0,
                  "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    /* Generates 1 event message we don't care about in this test */
}

void LC_ProcessWP_Test_OperatorCompareWatchTruePreviousStale(void)
{
    uint16             WatchIndex = 0;
    CFE_TIME_SysTime_t Timestamp;

    Timestamp.Seconds        = 3;
    Timestamp.Subseconds     = 5;
    CFE_SB_MsgId_t TestMsgId = LC_UT_MID_1;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    LC_OperData.WDTPtr[WatchIndex].DataType                = LC_DATA_WATCH_BYTE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID              = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult             = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale      = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask                 = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Signed8 = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount         = 0;

    /* Execute the function being tested */
    LC_ProcessWP(WatchIndex, &UT_CmdBuf.Buf, Timestamp);

    /* Verify results */
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_TRUE,
                  "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_TRUE");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1,
                  "LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1");

    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].CumulativeTrueCount == 1,
                  "LC_OperData.WRTPtr[WatchIndex].CumulativeTrueCount == 1");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 1,
                  "LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 1");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1,
                  "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1");

    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].FalseToTrueCount == 1,
                  "LC_OperData.WRTPtr[WatchIndex].FalseToTrueCount == 1");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Value == 0,
                  "LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Value == 0");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Timestamp.Seconds == 3,
                  "LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Timestamp.Seconds == 3");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Timestamp.Subseconds == 5,
                  "LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Timestamp.Subseconds == 5");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_ProcessWP_Test_OperatorCompareWatchTruePreviousFalse(void)
{
    uint16             WatchIndex = 0;
    CFE_TIME_SysTime_t Timestamp;

    Timestamp.Seconds        = 3;
    Timestamp.Subseconds     = 5;
    CFE_SB_MsgId_t TestMsgId = LC_UT_MID_1;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    LC_OperData.WDTPtr[WatchIndex].DataType                = LC_DATA_WATCH_BYTE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID              = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult             = LC_WATCH_FALSE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale      = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask                 = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Signed8 = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount         = 0;

    /* Execute the function being tested */
    LC_ProcessWP(WatchIndex, &UT_CmdBuf.Buf, Timestamp);

    /* Verify results */
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_TRUE,
                  "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_TRUE");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1,
                  "LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1");

    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].CumulativeTrueCount == 1,
                  "LC_OperData.WRTPtr[WatchIndex].CumulativeTrueCount == 1");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 1,
                  "LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 1");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1,
                  "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1");

    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].FalseToTrueCount == 1,
                  "LC_OperData.WRTPtr[WatchIndex].FalseToTrueCount == 1");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Value == 0,
                  "LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Value == 0");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Timestamp.Seconds == 3,
                  "LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Timestamp.Seconds == 3");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Timestamp.Subseconds == 5,
                  "LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Timestamp.Subseconds == 5");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_ProcessWP_Test_OperatorCompareWatchFalsePreviousStale(void)
{
    uint16             WatchIndex = 0;
    CFE_TIME_SysTime_t Timestamp;
    CFE_SB_MsgId_t     TestMsgId = LC_UT_MID_1;

    Timestamp.Seconds    = 3;
    Timestamp.Subseconds = 5;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    LC_OperData.WDTPtr[WatchIndex].DataType                = LC_DATA_WATCH_BYTE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID              = LC_OPER_GE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult             = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale      = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask                 = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Signed8 = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount         = 0;

    /* Execute the function being tested */
    LC_ProcessWP(WatchIndex, &UT_CmdBuf.Buf, Timestamp);

    /* Verify results */
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_FALSE,
                  "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_FALSE");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1,
                  "LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1");

    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 0,
                  "LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 0");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1,
                  "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1");

    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Value == 0,
                  "LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Value == 0");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Timestamp.Seconds == 3,
                  "LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Timestamp.Seconds == 3");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Timestamp.Subseconds == 5,
                  "LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Timestamp.Subseconds == 5");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_ProcessWP_Test_OperatorCompareWatchFalsePreviousTrue(void)
{
    uint16             WatchIndex = 0;
    CFE_TIME_SysTime_t Timestamp;
    CFE_SB_MsgId_t     TestMsgId = LC_UT_MID_1;

    Timestamp.Seconds    = 3;
    Timestamp.Subseconds = 5;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    LC_OperData.WDTPtr[WatchIndex].DataType                = LC_DATA_WATCH_BYTE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID              = LC_OPER_GE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult             = LC_WATCH_TRUE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale      = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask                 = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Signed8 = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount         = 0;

    /* Execute the function being tested */
    LC_ProcessWP(WatchIndex, &UT_CmdBuf.Buf, Timestamp);

    /* Verify results */
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_FALSE,
                  "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_FALSE");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1,
                  "LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1");

    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 0,
                  "LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 0");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1,
                  "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1");

    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Value == 0,
                  "LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Value == 0");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Timestamp.Seconds == 3,
                  "LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Timestamp.Seconds == 3");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Timestamp.Subseconds == 5,
                  "LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Timestamp.Subseconds == 5");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_ProcessWP_Test_BadSize(void)
{
    uint16             WatchIndex = 0;
    CFE_TIME_SysTime_t Timestamp;
    CFE_SB_MsgId_t     TestMsgId = LC_UT_MID_1;

    memset(&Timestamp, 0, sizeof(Timestamp));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    LC_OperData.WDTPtr[WatchIndex].DataType           = 99;
    LC_OperData.WDTPtr[WatchIndex].OperatorID         = LC_OPER_CUSTOM;
    LC_OperData.WDTPtr[WatchIndex].CustomFuncArgument = 0;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale = 1;

    /* Execute the function being tested */
    LC_ProcessWP(WatchIndex, &UT_CmdBuf.Buf, Timestamp);

    /* Verify results */

    /* this generates 1 event message in a subfunction */
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
}

void LC_ProcessWP_Test_OperatorCompareWatchTruePreviousTrue(void)
{
    uint16             WatchIndex = 0;
    CFE_TIME_SysTime_t Timestamp;
    CFE_SB_MsgId_t     TestMsgId = LC_UT_MID_1;

    Timestamp.Seconds    = 3;
    Timestamp.Subseconds = 5;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    LC_OperData.WDTPtr[WatchIndex].DataType                = LC_DATA_WATCH_BYTE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID              = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult             = LC_WATCH_TRUE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale      = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask                 = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Signed8 = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount         = 0;

    /* Execute the function being tested */
    LC_ProcessWP(WatchIndex, &UT_CmdBuf.Buf, Timestamp);

    /* Verify results */
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_TRUE,
                  "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_TRUE");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1,
                  "LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1");

    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].CumulativeTrueCount == 1,
                  "LC_OperData.WRTPtr[WatchIndex].CumulativeTrueCount == 1");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 1,
                  "LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 1");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1,
                  "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1");

    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].FalseToTrueCount == 0,
                  "LC_OperData.WRTPtr[WatchIndex].FalseToTrueCount == 0");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Value == 0,
                  "LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Value == 0");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_ProcessWP_Test_OperatorCompareWatchFalsePreviousFalse(void)
{
    uint16             WatchIndex = 0;
    CFE_TIME_SysTime_t Timestamp;
    CFE_SB_MsgId_t     TestMsgId = LC_UT_MID_1;

    Timestamp.Seconds    = 3;
    Timestamp.Subseconds = 5;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    LC_OperData.WDTPtr[WatchIndex].DataType                = LC_DATA_WATCH_BYTE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID              = LC_OPER_GE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult             = LC_WATCH_FALSE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale      = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask                 = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Signed8 = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount         = 0;

    /* Execute the function being tested */
    LC_ProcessWP(WatchIndex, &UT_CmdBuf.Buf, Timestamp);

    /* Verify results */
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_FALSE,
                  "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_FALSE");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1,
                  "LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1");

    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 0,
                  "LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 0");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1,
                  "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1");

    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Value == 0,
                  "LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Value == 0");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_OperatorCompare_Test_DataByte(void)
{
    uint8  Result;
    uint16 WatchIndex      = 0;
    uint32 ProcessedWPData = 0;

    LC_OperData.WDTPtr[WatchIndex].DataType                = LC_DATA_WATCH_BYTE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID              = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult             = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale      = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask                 = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Signed8 = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount         = 0;

    /* Execute the function being tested */
    Result = LC_OperatorCompare(WatchIndex, ProcessedWPData);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_OperatorCompare_Test_DataWordLE(void)
{
    uint8  Result;
    uint16 WatchIndex      = 0;
    uint32 ProcessedWPData = 0;

    LC_OperData.WDTPtr[WatchIndex].DataType                 = LC_DATA_WATCH_WORD_LE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID               = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult              = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale       = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask                  = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Signed16 = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount          = 0;

    /* Execute the function being tested */
    Result = LC_OperatorCompare(WatchIndex, ProcessedWPData);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_OperatorCompare_Test_DataDWordLE(void)
{
    uint8  Result;
    uint16 WatchIndex      = 0;
    uint32 ProcessedWPData = 0;

    LC_OperData.WDTPtr[WatchIndex].DataType                 = LC_DATA_WATCH_DWORD_LE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID               = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult              = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale       = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask                  = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Signed32 = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount          = 0;

    /* Execute the function being tested */
    Result = LC_OperatorCompare(WatchIndex, ProcessedWPData);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_OperatorCompare_Test_DataUByte(void)
{
    uint8  Result;
    uint16 WatchIndex      = 0;
    uint32 ProcessedWPData = 0;

    LC_OperData.WDTPtr[WatchIndex].DataType                  = LC_DATA_WATCH_UBYTE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID                = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult               = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale        = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask                   = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Unsigned8 = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount           = 0;

    /* Execute the function being tested */
    Result = LC_OperatorCompare(WatchIndex, ProcessedWPData);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_OperatorCompare_Test_DataUWordLE(void)
{
    uint8  Result;
    uint16 WatchIndex      = 0;
    uint32 ProcessedWPData = 0;

    LC_OperData.WDTPtr[WatchIndex].DataType                   = LC_DATA_WATCH_UWORD_LE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID                 = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult                = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale         = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask                    = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Unsigned16 = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount            = 0;

    /* Execute the function being tested */
    Result = LC_OperatorCompare(WatchIndex, ProcessedWPData);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_OperatorCompare_Test_DataUDWordLE(void)
{
    uint8  Result;
    uint16 WatchIndex      = 0;
    uint32 ProcessedWPData = 0;

    LC_OperData.WDTPtr[WatchIndex].DataType                   = LC_DATA_WATCH_UDWORD_LE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID                 = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult                = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale         = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask                    = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Unsigned32 = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount            = 0;

    /* Execute the function being tested */
    Result = LC_OperatorCompare(WatchIndex, ProcessedWPData);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_OperatorCompare_Test_DataFloatLE(void)
{
    uint8  Result;
    uint16 WatchIndex      = 0;
    uint32 ProcessedWPData = 0;

    LC_OperData.WDTPtr[WatchIndex].DataType                = LC_DATA_WATCH_FLOAT_LE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID              = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult             = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale      = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask                 = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Float32 = 1.0;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount         = 0;

    /* Execute the function being tested */
    Result = LC_OperatorCompare(WatchIndex, ProcessedWPData);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_OperatorCompare_Test_DataTypeError(void)
{
    uint8  Result;
    uint16 WatchIndex      = 0;
    uint32 ProcessedWPData = 0;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    LC_OperData.WDTPtr[WatchIndex].DataType                = 99;
    LC_OperData.WDTPtr[WatchIndex].OperatorID              = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult             = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale      = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask                 = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Float32 = 1.0;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount         = 0;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WP has undefined data type: WP = %%d, DataType = %%d");

    /* Execute the function being tested */
    Result = LC_OperatorCompare(WatchIndex, ProcessedWPData);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_ERROR, "Result == LC_WATCH_ERROR");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WP_DATATYPE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SignedCompare_Test_LE(void)
{
    uint8  Result;
    uint16 WatchIndex   = 0;
    int32  WPValue      = 0;
    int32  CompareValue = 1;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_LE;

    /* Execute the function being tested */
    Result = LC_SignedCompare(WatchIndex, WPValue, CompareValue);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_SignedCompare_Test_LT(void)
{
    uint8  Result;
    uint16 WatchIndex   = 0;
    int32  WPValue      = 0;
    int32  CompareValue = 1;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_LT;

    /* Execute the function being tested */
    Result = LC_SignedCompare(WatchIndex, WPValue, CompareValue);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_SignedCompare_Test_EQ(void)
{
    uint8  Result;
    uint16 WatchIndex   = 0;
    int32  WPValue      = 1;
    int32  CompareValue = 1;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_EQ;

    /* Execute the function being tested */
    Result = LC_SignedCompare(WatchIndex, WPValue, CompareValue);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_SignedCompare_Test_NE(void)
{
    uint8  Result;
    uint16 WatchIndex   = 0;
    int32  WPValue      = 0;
    int32  CompareValue = 1;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_NE;

    /* Execute the function being tested */
    Result = LC_SignedCompare(WatchIndex, WPValue, CompareValue);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_SignedCompare_Test_GT(void)
{
    uint8  Result;
    uint16 WatchIndex   = 0;
    int32  WPValue      = 1;
    int32  CompareValue = 0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_GT;

    /* Execute the function being tested */
    Result = LC_SignedCompare(WatchIndex, WPValue, CompareValue);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_SignedCompare_Test_GE(void)
{
    uint8  Result;
    uint16 WatchIndex   = 0;
    int32  WPValue      = 1;
    int32  CompareValue = 0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_GE;

    /* Execute the function being tested */
    Result = LC_SignedCompare(WatchIndex, WPValue, CompareValue);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_SignedCompare_Test_InvalidOperatorID(void)
{
    uint8  Result;
    uint16 WatchIndex   = 0;
    int32  WPValue      = 1;
    int32  CompareValue = 0;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    LC_OperData.WDTPtr[WatchIndex].OperatorID = 99;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WP has invalid operator ID: WP = %%d, OperID = %%d");

    /* Execute the function being tested */
    Result = LC_SignedCompare(WatchIndex, WPValue, CompareValue);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_ERROR, "Result == LC_WATCH_ERROR");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WP_OPERID_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_UnsignedCompare_Test_LE(void)
{
    uint8  Result;
    uint16 WatchIndex   = 0;
    int32  WPValue      = 0;
    int32  CompareValue = 1;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_LE;

    /* Execute the function being tested */
    Result = LC_UnsignedCompare(WatchIndex, WPValue, CompareValue);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_UnsignedCompare_Test_LT(void)
{
    uint8  Result;
    uint16 WatchIndex   = 0;
    int32  WPValue      = 0;
    int32  CompareValue = 1;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_LT;

    /* Execute the function being tested */
    Result = LC_UnsignedCompare(WatchIndex, WPValue, CompareValue);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_UnsignedCompare_Test_EQ(void)
{
    uint8  Result;
    uint16 WatchIndex   = 0;
    int32  WPValue      = 1;
    int32  CompareValue = 1;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_EQ;

    /* Execute the function being tested */
    Result = LC_UnsignedCompare(WatchIndex, WPValue, CompareValue);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_UnsignedCompare_Test_NE(void)
{
    uint8  Result;
    uint16 WatchIndex   = 0;
    int32  WPValue      = 0;
    int32  CompareValue = 1;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_NE;

    /* Execute the function being tested */
    Result = LC_UnsignedCompare(WatchIndex, WPValue, CompareValue);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_UnsignedCompare_Test_GT(void)
{
    uint8  Result;
    uint16 WatchIndex   = 0;
    int32  WPValue      = 1;
    int32  CompareValue = 0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_GT;

    /* Execute the function being tested */
    Result = LC_UnsignedCompare(WatchIndex, WPValue, CompareValue);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_UnsignedCompare_Test_GE(void)
{
    uint8  Result;
    uint16 WatchIndex   = 0;
    int32  WPValue      = 1;
    int32  CompareValue = 0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_GE;

    /* Execute the function being tested */
    Result = LC_UnsignedCompare(WatchIndex, WPValue, CompareValue);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_UnsignedCompare_Test_InvalidOperatorID(void)
{
    uint8  Result;
    uint16 WatchIndex   = 0;
    int32  WPValue      = 1;
    int32  CompareValue = 0;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WP has invalid operator ID: WP = %%d, OperID = %%d");

    LC_OperData.WDTPtr[WatchIndex].OperatorID = 99;

    /* Execute the function being tested */
    Result = LC_UnsignedCompare(WatchIndex, WPValue, CompareValue);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_ERROR, "Result == LC_WATCH_ERROR");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WP_OPERID_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_FloatCompare_Test_LE(void)
{
    uint8          Result;
    uint16         WatchIndex = 0;
    LC_MultiType_t WPMultiType;
    LC_MultiType_t CompareMultiType;

    WPMultiType.Float32      = 0.0;
    CompareMultiType.Float32 = 1.0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_LE;

    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, &WPMultiType, &CompareMultiType);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_FloatCompare_Test_LT(void)
{
    uint8          Result;
    uint16         WatchIndex = 0;
    LC_MultiType_t WPMultiType;
    LC_MultiType_t CompareMultiType;

    WPMultiType.Float32      = 0.0;
    CompareMultiType.Float32 = 1.0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_LT;

    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, &WPMultiType, &CompareMultiType);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_FloatCompare_Test_EQ(void)
{
    uint8          Result;
    uint16         WatchIndex = 0;
    LC_MultiType_t WPMultiType;
    LC_MultiType_t CompareMultiType;

    WPMultiType.Float32      = 1.0;
    CompareMultiType.Float32 = 1.0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_EQ;

    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, &WPMultiType, &CompareMultiType);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_FloatCompare_Test_EQFail(void)
{
    uint8          Result;
    uint16         WatchIndex = 0;
    LC_MultiType_t WPMultiType;
    LC_MultiType_t CompareMultiType;

    WPMultiType.Float32      = 2.0;
    CompareMultiType.Float32 = 1.0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_EQ;

    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, &WPMultiType, &CompareMultiType);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_FALSE, "Result == LC_WATCH_FALSE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_FloatCompare_Test_NE(void)
{
    uint8          Result;
    uint16         WatchIndex = 0;
    LC_MultiType_t WPMultiType;
    LC_MultiType_t CompareMultiType;

    WPMultiType.Float32      = 1.0;
    CompareMultiType.Float32 = 0.0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_NE;

    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, &WPMultiType, &CompareMultiType);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_FloatCompare_Test_NEFail(void)
{
    uint8          Result;
    uint16         WatchIndex = 0;
    LC_MultiType_t WPMultiType;
    LC_MultiType_t CompareMultiType;

    WPMultiType.Float32      = 1.0;
    CompareMultiType.Float32 = 1.0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_NE;

    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, &WPMultiType, &CompareMultiType);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_FALSE, "Result == LC_WATCH_FALSE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_FloatCompare_Test_GT(void)
{
    uint8          Result;
    uint16         WatchIndex = 0;
    LC_MultiType_t WPMultiType;
    LC_MultiType_t CompareMultiType;

    WPMultiType.Float32      = 1.0;
    CompareMultiType.Float32 = 0.0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_GT;

    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, &WPMultiType, &CompareMultiType);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_FloatCompare_Test_GE(void)
{
    uint8          Result;
    uint16         WatchIndex = 0;
    LC_MultiType_t WPMultiType;
    LC_MultiType_t CompareMultiType;

    WPMultiType.Float32      = 1.0;
    CompareMultiType.Float32 = 0.0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_GE;

    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, &WPMultiType, &CompareMultiType);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_FloatCompare_Test_InvalidOperatorID(void)
{
    uint8          Result;
    uint16         WatchIndex = 0;
    LC_MultiType_t WPMultiType;
    LC_MultiType_t CompareMultiType;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    WPMultiType.Float32      = 1.0;
    CompareMultiType.Float32 = 0.0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = 99;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WP has invalid operator ID: WP = %%d, OperID = %%d");

    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, &WPMultiType, &CompareMultiType);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_ERROR, "Result == LC_WATCH_ERROR");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WP_OPERID_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_FloatCompare_Test_NaN(void)
{
    uint8          Result;
    uint16         WatchIndex = 0;
    LC_MultiType_t WPMultiType;
    LC_MultiType_t CompareMultiType;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    WPMultiType.Float32      = 1.0;
    WPMultiType.Unsigned32   = 0x7F8FFFFF;
    CompareMultiType.Float32 = 0.0;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WP data value is a float NAN: WP = %%d, Value = 0x%%08X");

    LC_OperData.WDTPtr[WatchIndex].OperatorID = 99;

    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, &WPMultiType, &CompareMultiType);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_ERROR, "Result == LC_WATCH_ERROR");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WP_NAN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_WPOffsetValid_Test_DataUByte(void)
{
    bool           Result;
    uint16         WatchIndex = 0;
    CFE_SB_MsgId_t TestMsgId  = LC_UT_MID_1;
    size_t         MsgSize    = 16;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    LC_OperData.WDTPtr[WatchIndex].DataType         = LC_DATA_WATCH_UBYTE;
    LC_OperData.WDTPtr[WatchIndex].WatchpointOffset = 0;

    /* Execute the function being tested */
    Result = LC_WPOffsetValid(WatchIndex, &UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_WPOffsetValid_Test_UWordLE(void)
{
    bool           Result;
    uint16         WatchIndex = 0;
    CFE_SB_MsgId_t TestMsgId  = LC_UT_MID_1;
    size_t         MsgSize    = 16;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    LC_OperData.WDTPtr[WatchIndex].DataType         = LC_DATA_WATCH_UWORD_LE;
    LC_OperData.WDTPtr[WatchIndex].WatchpointOffset = 0;

    /* Execute the function being tested */
    Result = LC_WPOffsetValid(WatchIndex, &UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_WPOffsetValid_Test_UDWordLE(void)
{
    bool           Result;
    uint16         WatchIndex = 0;
    CFE_SB_MsgId_t TestMsgId  = LC_UT_MID_1;
    size_t         MsgSize    = 16;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    LC_OperData.WDTPtr[WatchIndex].DataType         = LC_DATA_WATCH_UDWORD_LE;
    LC_OperData.WDTPtr[WatchIndex].WatchpointOffset = 0;

    /* Execute the function being tested */
    Result = LC_WPOffsetValid(WatchIndex, &UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_WPOffsetValid_Test_FloatLE(void)
{
    bool           Result;
    uint16         WatchIndex = 0;
    CFE_SB_MsgId_t TestMsgId  = LC_UT_MID_1;
    size_t         MsgSize    = 16;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    LC_OperData.WDTPtr[WatchIndex].DataType         = LC_DATA_WATCH_FLOAT_LE;
    LC_OperData.WDTPtr[WatchIndex].WatchpointOffset = 0;

    /* Execute the function being tested */
    Result = LC_WPOffsetValid(WatchIndex, &UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_WPOffsetValid_Test_DataTypeError(void)
{
    bool           Result;
    uint16         WatchIndex = 0;
    CFE_SB_MsgId_t TestMsgId  = LC_UT_MID_1;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WP has undefined data type: WP = %%d, DataType = %%d");

    LC_OperData.WDTPtr[WatchIndex].DataType         = 99;
    LC_OperData.WDTPtr[WatchIndex].WatchpointOffset = 0;

    /* Execute the function being tested */
    Result = LC_WPOffsetValid(WatchIndex, &UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_ERROR,
                  "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_ERROR");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0,
                  "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WP_DATATYPE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_WPOffsetValid_Test_OffsetError(void)
{
    bool           Result;
    uint16         WatchIndex = 0;
    CFE_SB_MsgId_t TestMsgId  = LC_UT_MID_1;
    size_t         MsgSize    = 0;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WP offset error: MID = 0x%%08lX, WP = %%d, Offset = %%d, DataSize = %%d, MsgLen = %%d");

    LC_OperData.WDTPtr[WatchIndex].DataType         = LC_DATA_WATCH_UBYTE;
    LC_OperData.WDTPtr[WatchIndex].WatchpointOffset = sizeof(LC_NoopCmd_t) - 1;

    /* Execute the function being tested */
    Result = LC_WPOffsetValid(WatchIndex, &UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_ERROR,
                  "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_ERROR");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0,
                  "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WP_OFFSET_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_GetSizedWPData_Test_DataByte(void)
{
    bool   Result;
    uint16 WatchIndex = 0;
    uint8  WPData[4];
    uint32 SizedData = 0;

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_WATCH_BYTE;

    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");
    UtAssert_True(SizedData == 1, "SizedData == 1");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_GetSizedWPData_Test_DataUByte(void)
{
    bool   Result;
    uint16 WatchIndex = 0;
    uint8  WPData[4];
    uint32 SizedData = 0;

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_WATCH_UBYTE;

    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");
    UtAssert_True(SizedData == 1, "SizedData == 1");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_GetSizedWPData_Test_DataWordBELittleEndian(void)
{
    bool   Result;
    uint16 WatchIndex = 0;
    uint8  WPData[4];
    uint32 SizedData = 0;

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_WATCH_WORD_BE;

    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");
    UtAssert_True(SizedData == 0x0102, "SizedData == 0x0102");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_GetSizedWPData_Test_DataWordLELittleEndian(void)
{
    bool   Result;
    uint16 WatchIndex = 0;
    uint8  WPData[4];
    uint32 SizedData = 0;

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_WATCH_WORD_LE;

    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");
    UtAssert_True(SizedData == 0x0201, "SizedData == 0x0201");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_GetSizedWPData_Test_DataUWordBELittleEndian(void)
{
    bool   Result;
    uint16 WatchIndex = 0;
    uint8  WPData[4];
    uint32 SizedData = 0;

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_WATCH_UWORD_BE;

    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");
    UtAssert_True(SizedData == 0x0102, "SizedData == 0x0102");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_GetSizedWPData_Test_DataUWordLELittleEndian(void)
{
    bool   Result;
    uint16 WatchIndex = 0;
    uint8  WPData[4];
    uint32 SizedData = 0;

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_WATCH_UWORD_LE;

    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");
    UtAssert_True(SizedData == 0x0201, "SizedData == 0x0201");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_GetSizedWPData_Test_DataFloatBELittleEndian(void)
{
    bool   Result;
    uint16 WatchIndex = 0;
    uint8  WPData[4];
    uint32 SizedData = 0;

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_WATCH_FLOAT_BE;

    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");
    UtAssert_True(SizedData == 0x01020304, "SizedData == 0x01020304");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_GetSizedWPData_Test_DataFloatLELittleEndian(void)
{
    bool   Result;
    uint16 WatchIndex = 0;
    uint8  WPData[4];
    uint32 SizedData = 0;

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_WATCH_FLOAT_LE;

    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");
    UtAssert_True(SizedData == 0x04030201, "SizedData == 0x04030201");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 0");
}

void LC_GetSizedWPData_Test_DataTypeError(void)
{
    bool   Result;
    uint16 WatchIndex = 0;
    uint8  WPData[4];
    uint32 SizedData;

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WP has undefined data type: WP = %%d, DataType = %%d");

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = 99;

    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_ERROR,
                  "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_ERROR");
    UtAssert_True(LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0,
                  "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WP_DATATYPE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_ValidateWDT_Test_UnusedTableEntry(void)
{
    int32 Result;
    int32 TableIndex;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WDTPtr[TableIndex].DataType = LC_DATA_WATCH_NOT_USED;
    }

    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WDTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_ValidateWDT_Test_AllDataTypes(void)
{
    int32 Result;
    int32 TableIndex;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char  ExpectedEventString2[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify err: WP = %%d, Err = %%d, DType = %%d, Oper = %%d, MID = 0x%%08lX");

    snprintf(ExpectedEventString2, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WDTPtr[TableIndex].DataType = LC_DATA_WATCH_NOT_USED;
    }

    /* Add an entry for each data type */
    LC_OperData.WDTPtr[0].DataType  = LC_DATA_WATCH_BYTE;
    LC_OperData.WDTPtr[1].DataType  = LC_DATA_WATCH_UBYTE;
    LC_OperData.WDTPtr[2].DataType  = LC_DATA_WATCH_WORD_BE;
    LC_OperData.WDTPtr[3].DataType  = LC_DATA_WATCH_WORD_LE;
    LC_OperData.WDTPtr[4].DataType  = LC_DATA_WATCH_UWORD_BE;
    LC_OperData.WDTPtr[5].DataType  = LC_DATA_WATCH_UWORD_LE;
    LC_OperData.WDTPtr[6].DataType  = LC_DATA_WATCH_DWORD_BE;
    LC_OperData.WDTPtr[7].DataType  = LC_DATA_WATCH_DWORD_LE;
    LC_OperData.WDTPtr[8].DataType  = LC_DATA_WATCH_UDWORD_BE;
    LC_OperData.WDTPtr[9].DataType  = LC_DATA_WATCH_UDWORD_LE;
    LC_OperData.WDTPtr[10].DataType = LC_DATA_WATCH_FLOAT_BE;
    LC_OperData.WDTPtr[11].DataType = LC_DATA_WATCH_FLOAT_LE;

    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);

    /* Verify results */
    UtAssert_INT32_EQ(Result, LC_WDTVAL_ERR_OPER);

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WDTVAL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string 1 matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, LC_WDTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString2, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string 2 matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);
}

void LC_ValidateWDT_Test_AllOperatorIDs(void)
{
    int32 Result;
    int32 TableIndex;

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char  ExpectedEventString2[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify err: WP = %%d, Err = %%d, DType = %%d, Oper = %%d, MID = 0x%%08lX");

    snprintf(ExpectedEventString2, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WDTPtr[TableIndex].DataType  = LC_DATA_WATCH_BYTE;
        LC_OperData.WDTPtr[TableIndex].MessageID = LC_UT_MID_1;
    }

    /* Add an entry for each data type */
    LC_OperData.WDTPtr[0].OperatorID = LC_OPER_LT;
    LC_OperData.WDTPtr[1].OperatorID = LC_OPER_LE;
    LC_OperData.WDTPtr[2].OperatorID = LC_OPER_NE;
    LC_OperData.WDTPtr[3].OperatorID = LC_OPER_EQ;
    LC_OperData.WDTPtr[4].OperatorID = LC_OPER_GE;
    LC_OperData.WDTPtr[5].OperatorID = LC_OPER_GT;
    LC_OperData.WDTPtr[6].OperatorID = LC_OPER_CUSTOM;

    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);

    /* Verify results */
    UtAssert_INT32_EQ(Result, LC_WDTVAL_ERR_OPER);
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WDTVAL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string 1 matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, LC_WDTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString2, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string 2 matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);
}

void LC_ValidateWDT_Test_InvalidDataType(void)
{
    int32 Result;
    int32 TableIndex;
    int32 strCmpResult;
    char  ExpectedEventString1[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char  ExpectedEventString2[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString1, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify err: WP = %%d, Err = %%d, DType = %%d, Oper = %%d, MID = 0x%%08lX");
    snprintf(ExpectedEventString2, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WDTPtr[TableIndex].DataType                   = 99;
        LC_OperData.WDTPtr[TableIndex].OperatorID                 = 1;
        LC_OperData.WDTPtr[TableIndex].MessageID                  = LC_UT_MID_2;
        LC_OperData.WDTPtr[TableIndex].ComparisonValue.Unsigned32 = 3;
    }

    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);

    /* Verify results */
    UtAssert_True(Result == LC_WDTVAL_ERR_DATATYPE, "Result == LC_WDTVAL_ERR_DATATYPE");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 2, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 2");
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WDTVAL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString1, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, LC_WDTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString2, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);
}

void LC_ValidateWDT_Test_InvalidOperator(void)
{
    int32 Result;
    int32 TableIndex;
    int32 strCmpResult;
    char  ExpectedEventString1[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char  ExpectedEventString2[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    snprintf(ExpectedEventString1, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify err: WP = %%d, Err = %%d, DType = %%d, Oper = %%d, MID = 0x%%08lX");
    snprintf(ExpectedEventString2, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WDTPtr[TableIndex].DataType                   = LC_DATA_WATCH_BYTE;
        LC_OperData.WDTPtr[TableIndex].OperatorID                 = 99;
        LC_OperData.WDTPtr[TableIndex].MessageID                  = LC_UT_MID_2;
        LC_OperData.WDTPtr[TableIndex].ComparisonValue.Unsigned32 = 3;
    }

    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);

    /* Verify results */
    UtAssert_True(Result == LC_WDTVAL_ERR_OPER, "Result == LC_WDTVAL_ERR_OPER");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 2, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 2");
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WDTVAL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString1, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, LC_WDTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString2, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);
}

void LC_ValidateWDT_Test_BadMessageID(void)
{
    int32 Result;
    int32 TableIndex;
    int32 strCmpResult;
    char  ExpectedEventString1[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char  ExpectedEventString2[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    snprintf(ExpectedEventString1, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify err: WP = %%d, Err = %%d, DType = %%d, Oper = %%d, MID = 0x%%08lX");
    snprintf(ExpectedEventString2, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WDTPtr[TableIndex].DataType                   = LC_DATA_WATCH_BYTE;
        LC_OperData.WDTPtr[TableIndex].OperatorID                 = LC_OPER_LT;
        LC_OperData.WDTPtr[TableIndex].MessageID                  = CFE_SB_INVALID_MSG_ID;
        LC_OperData.WDTPtr[TableIndex].ComparisonValue.Unsigned32 = 3;
    }

    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);

    /* Verify results */
    UtAssert_True(Result == LC_WDTVAL_ERR_MID, "Result == LC_WDTVAL_ERR_MID");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 2, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 2");
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WDTVAL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString1, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, LC_WDTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString2, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);
}

void LC_ValidateWDT_Test_NaN(void)
{
    int32 Result;
    int32 TableIndex;
    int32 strCmpResult;
    char  ExpectedEventString1[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char  ExpectedEventString2[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    snprintf(ExpectedEventString1, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify float err: WP = %%d, Err = %%d, ComparisonValue = 0x%%08X");
    snprintf(ExpectedEventString2, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WDTPtr[TableIndex].DataType                   = LC_DATA_WATCH_FLOAT_LE;
        LC_OperData.WDTPtr[TableIndex].OperatorID                 = LC_OPER_LT;
        LC_OperData.WDTPtr[TableIndex].MessageID                  = LC_UT_MID_1;
        LC_OperData.WDTPtr[TableIndex].ComparisonValue.Unsigned32 = 0x7F8FFFFF;
    }

    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);

    /* Verify results */
    UtAssert_True(Result == LC_WDTVAL_ERR_FPNAN, "Result == LC_WDTVAL_ERR_FPNAN");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 2, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 2");
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WDTVAL_FPERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString1, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, LC_WDTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString2, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);
}

void LC_ValidateWDT_Test_Inf(void)
{
    int32 Result;
    int32 TableIndex;
    int32 strCmpResult;
    char  ExpectedEventString1[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char  ExpectedEventString2[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    snprintf(ExpectedEventString1, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify float err: WP = %%d, Err = %%d, ComparisonValue = 0x%%08X");
    snprintf(ExpectedEventString2, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WDTPtr[TableIndex].DataType                   = LC_DATA_WATCH_FLOAT_LE;
        LC_OperData.WDTPtr[TableIndex].OperatorID                 = LC_OPER_LT;
        LC_OperData.WDTPtr[TableIndex].MessageID                  = LC_UT_MID_1;
        LC_OperData.WDTPtr[TableIndex].ComparisonValue.Unsigned32 = 0x7F800000;
    }

    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);

    /* Verify results */
    UtAssert_True(Result == LC_WDTVAL_ERR_FPINF, "Result == LC_WDTVAL_ERR_FPINF");

    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 2, "UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 2");
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WDTVAL_FPERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString1, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, LC_WDTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString2, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);
}

void LC_ValidateWDT_Test_FloatingPointPassed(void)
{
    int32 Result;
    int32 TableIndex;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WDTPtr[TableIndex].DataType                   = LC_DATA_WATCH_FLOAT_LE;
        LC_OperData.WDTPtr[TableIndex].OperatorID                 = LC_OPER_LT;
        LC_OperData.WDTPtr[TableIndex].MessageID                  = LC_UT_MID_1;
        LC_OperData.WDTPtr[TableIndex].ComparisonValue.Unsigned32 = 1;
    }

    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);

    /* Verify results */
    UtAssert_True(Result == LC_WDTVAL_ERR_NONE, "Result == LC_WDTVAL_ERR_NONE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WDTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_ValidateWDT_Test_NonFloatingPointPassed(void)
{
    int32 Result;
    int32 TableIndex;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WDTPtr[TableIndex].DataType                   = LC_DATA_WATCH_BYTE;
        LC_OperData.WDTPtr[TableIndex].OperatorID                 = LC_OPER_LT;
        LC_OperData.WDTPtr[TableIndex].MessageID                  = LC_UT_MID_1;
        LC_OperData.WDTPtr[TableIndex].ComparisonValue.Unsigned32 = 1;
    }

    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);

    /* Verify results */
    UtAssert_True(Result == LC_WDTVAL_ERR_NONE, "Result == LC_WDTVAL_ERR_NONE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WDTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_ValidateWDT_Test_FloatBE(void)
{
    int32 Result;
    int32 TableIndex;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "WDT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WDTPtr[TableIndex].DataType                   = LC_DATA_WATCH_FLOAT_BE;
        LC_OperData.WDTPtr[TableIndex].OperatorID                 = LC_OPER_LT;
        LC_OperData.WDTPtr[TableIndex].MessageID                  = LC_UT_MID_1;
        LC_OperData.WDTPtr[TableIndex].ComparisonValue.Unsigned32 = 1;
    }

    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);

    /* Verify results */
    UtAssert_True(Result == LC_WDTVAL_ERR_NONE, "Result == LC_WDTVAL_ERR_NONE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WDTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void UtTest_Setup(void)
{
    UtTest_Add(LC_CreateHashTable_Test, LC_Test_Setup, LC_Test_TearDown, "LC_CreateHashTable_Test");

    UtTest_Add(LC_AddWatchpoint_Test_HashTableAndWatchPtListNullPointersNominal, LC_Test_Setup, LC_Test_TearDown,
               "LC_AddWatchpoint_Test_HashTableAndWatchPtListNullPointersNominal");
    UtTest_Add(LC_AddWatchpoint_Test_HashTableAndWatchPtListNotNullPointerTwoMsgLinksMIDFound, LC_Test_Setup,
               LC_Test_TearDown, "LC_AddWatchpoint_Test_HashTableAndWatchPtListNotNullPointerTwoMsgLinksMIDFound");
    UtTest_Add(LC_AddWatchpoint_Test_NullPointersErrorSubscribingWatchpoint, LC_Test_Setup, LC_Test_TearDown,
               "LC_AddWatchpoint_Test_NullPointersErrorSubscribingWatchpoint");
    UtTest_Add(LC_AddWatchpoint_Test_NonNullMessageList, LC_Test_Setup, LC_Test_TearDown,
               "LC_AddWatchpoint_Test_NonNullMessageList");

    UtTest_Add(LC_CheckMsgForWPs_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_CheckMsgForWPs_Test_Nominal");
    UtTest_Add(LC_CheckMsgForWPs_Test_NominalMsgTime1, LC_Test_Setup, LC_Test_TearDown,
               "LC_CheckMsgForWPs_Test_NominalMsgTime1");
    UtTest_Add(LC_CheckMsgForWPs_Test_NominalMsgTime2, LC_Test_Setup, LC_Test_TearDown,
               "LC_CheckMsgForWPs_Test_NominalMsgTime2");
    UtTest_Add(LC_CheckMsgForWPs_Test_NominalDisabled, LC_Test_Setup, LC_Test_TearDown,
               "LC_CheckMsgForWPs_Test_NominalDisabled");
    UtTest_Add(LC_CheckMsgForWPs_Test_UnreferencedMessageID, LC_Test_Setup, LC_Test_TearDown,
               "LC_CheckMsgForWPs_Test_UnreferencedMessageID");

    UtTest_Add(LC_ProcessWP_Test_CustomFunctionWatchFalse, LC_Test_Setup, LC_Test_TearDown,
               "LC_ProcessWP_Test_CustomFunctionWatchFalse");
    UtTest_Add(LC_ProcessWP_Test_OperatorCompareError, LC_Test_Setup, LC_Test_TearDown,
               "LC_ProcessWP_Test_OperatorCompareError");
    UtTest_Add(LC_ProcessWP_Test_OperatorCompareWatchTruePreviousStale, LC_Test_Setup, LC_Test_TearDown,
               "LC_ProcessWP_Test_OperatorCompareWatchTruePreviousStale");
    UtTest_Add(LC_ProcessWP_Test_OperatorCompareWatchTruePreviousFalse, LC_Test_Setup, LC_Test_TearDown,
               "LC_ProcessWP_Test_OperatorCompareWatchTruePreviousFalse");
    UtTest_Add(LC_ProcessWP_Test_OperatorCompareWatchFalsePreviousStale, LC_Test_Setup, LC_Test_TearDown,
               "LC_ProcessWP_Test_OperatorCompareWatchFalsePreviousStale");
    UtTest_Add(LC_ProcessWP_Test_OperatorCompareWatchFalsePreviousTrue, LC_Test_Setup, LC_Test_TearDown,
               "LC_ProcessWP_Test_OperatorCompareWatchFalsePreviousTrue");

    UtTest_Add(LC_ProcessWP_Test_BadSize, LC_Test_Setup, LC_Test_TearDown, "LC_ProcessWP_Test_BadSize");
    UtTest_Add(LC_ProcessWP_Test_OperatorCompareWatchTruePreviousTrue, LC_Test_Setup, LC_Test_TearDown,
               "LC_ProcessWP_Test_OperatorCompareWatchTruePreviousTrue");
    UtTest_Add(LC_ProcessWP_Test_OperatorCompareWatchFalsePreviousFalse, LC_Test_Setup, LC_Test_TearDown,
               "LC_ProcessWP_Test_OperatorCompareWatchFalsePreviousFalse");

    /* Note: Only testing little-ending (LE) cases.  Not testing big-endian (BE) cases. */
    UtTest_Add(LC_OperatorCompare_Test_DataByte, LC_Test_Setup, LC_Test_TearDown, "LC_OperatorCompare_Test_DataByte");
    UtTest_Add(LC_OperatorCompare_Test_DataWordLE, LC_Test_Setup, LC_Test_TearDown,
               "LC_OperatorCompare_Test_DataWordLE");
    UtTest_Add(LC_OperatorCompare_Test_DataDWordLE, LC_Test_Setup, LC_Test_TearDown,
               "LC_OperatorCompare_Test_DataDWordLE");
    UtTest_Add(LC_OperatorCompare_Test_DataUByte, LC_Test_Setup, LC_Test_TearDown, "LC_OperatorCompare_Test_DataUByte");
    UtTest_Add(LC_OperatorCompare_Test_DataUWordLE, LC_Test_Setup, LC_Test_TearDown,
               "LC_OperatorCompare_Test_DataUWordLE");
    UtTest_Add(LC_OperatorCompare_Test_DataUDWordLE, LC_Test_Setup, LC_Test_TearDown,
               "LC_OperatorCompare_Test_DataUDWordLE");
    UtTest_Add(LC_OperatorCompare_Test_DataFloatLE, LC_Test_Setup, LC_Test_TearDown,
               "LC_OperatorCompare_Test_DataFloatLE");
    UtTest_Add(LC_OperatorCompare_Test_DataTypeError, LC_Test_Setup, LC_Test_TearDown,
               "LC_OperatorCompare_Test_DataTypeError");

    UtTest_Add(LC_SignedCompare_Test_LE, LC_Test_Setup, LC_Test_TearDown, "LC_SignedCompare_Test_LE");
    UtTest_Add(LC_SignedCompare_Test_LT, LC_Test_Setup, LC_Test_TearDown, "LC_SignedCompare_Test_LT");
    UtTest_Add(LC_SignedCompare_Test_EQ, LC_Test_Setup, LC_Test_TearDown, "LC_SignedCompare_Test_EQ");
    UtTest_Add(LC_SignedCompare_Test_NE, LC_Test_Setup, LC_Test_TearDown, "LC_SignedCompare_Test_NE");
    UtTest_Add(LC_SignedCompare_Test_GT, LC_Test_Setup, LC_Test_TearDown, "LC_SignedCompare_Test_GT");
    UtTest_Add(LC_SignedCompare_Test_GE, LC_Test_Setup, LC_Test_TearDown, "LC_SignedCompare_Test_GE");
    UtTest_Add(LC_SignedCompare_Test_InvalidOperatorID, LC_Test_Setup, LC_Test_TearDown,
               "LC_SignedCompare_Test_InvalidOperatorID");

    UtTest_Add(LC_UnsignedCompare_Test_LE, LC_Test_Setup, LC_Test_TearDown, "LC_UnsignedCompare_Test_LE");
    UtTest_Add(LC_UnsignedCompare_Test_LT, LC_Test_Setup, LC_Test_TearDown, "LC_UnsignedCompare_Test_LT");
    UtTest_Add(LC_UnsignedCompare_Test_EQ, LC_Test_Setup, LC_Test_TearDown, "LC_UnsignedCompare_Test_EQ");
    UtTest_Add(LC_UnsignedCompare_Test_NE, LC_Test_Setup, LC_Test_TearDown, "LC_UnsignedCompare_Test_NE");
    UtTest_Add(LC_UnsignedCompare_Test_GT, LC_Test_Setup, LC_Test_TearDown, "LC_UnsignedCompare_Test_GT");
    UtTest_Add(LC_UnsignedCompare_Test_GE, LC_Test_Setup, LC_Test_TearDown, "LC_UnsignedCompare_Test_GE");
    UtTest_Add(LC_UnsignedCompare_Test_InvalidOperatorID, LC_Test_Setup, LC_Test_TearDown,
               "LC_UnsignedCompare_Test_InvalidOperatorID");

    UtTest_Add(LC_FloatCompare_Test_LE, LC_Test_Setup, LC_Test_TearDown, "LC_FloatCompare_Test_LE");
    UtTest_Add(LC_FloatCompare_Test_LT, LC_Test_Setup, LC_Test_TearDown, "LC_FloatCompare_Test_LT");
    UtTest_Add(LC_FloatCompare_Test_EQ, LC_Test_Setup, LC_Test_TearDown, "LC_FloatCompare_Test_EQ");
    UtTest_Add(LC_FloatCompare_Test_EQFail, LC_Test_Setup, LC_Test_TearDown, "LC_FloatCompare_Test_EQFail");
    UtTest_Add(LC_FloatCompare_Test_NE, LC_Test_Setup, LC_Test_TearDown, "LC_FloatCompare_Test_NE");
    UtTest_Add(LC_FloatCompare_Test_NEFail, LC_Test_Setup, LC_Test_TearDown, "LC_FloatCompare_Test_NEFail");
    UtTest_Add(LC_FloatCompare_Test_GT, LC_Test_Setup, LC_Test_TearDown, "LC_FloatCompare_Test_GT");
    UtTest_Add(LC_FloatCompare_Test_GE, LC_Test_Setup, LC_Test_TearDown, "LC_FloatCompare_Test_GE");
    UtTest_Add(LC_FloatCompare_Test_InvalidOperatorID, LC_Test_Setup, LC_Test_TearDown,
               "LC_FloatCompare_Test_InvalidOperatorID");
    UtTest_Add(LC_FloatCompare_Test_NaN, LC_Test_Setup, LC_Test_TearDown, "LC_FloatCompare_Test_NaN");

    UtTest_Add(LC_WPOffsetValid_Test_DataUByte, LC_Test_Setup, LC_Test_TearDown, "LC_WPOffsetValid_Test_DataUByte");
    UtTest_Add(LC_WPOffsetValid_Test_UWordLE, LC_Test_Setup, LC_Test_TearDown, "LC_WPOffsetValid_Test_UWordLE");
    UtTest_Add(LC_WPOffsetValid_Test_UDWordLE, LC_Test_Setup, LC_Test_TearDown, "LC_WPOffsetValid_Test_UDWordLE");
    UtTest_Add(LC_WPOffsetValid_Test_FloatLE, LC_Test_Setup, LC_Test_TearDown, "LC_WPOffsetValid_Test_FloatLE");
    UtTest_Add(LC_WPOffsetValid_Test_DataTypeError, LC_Test_Setup, LC_Test_TearDown,
               "LC_WPOffsetValid_Test_DataTypeError");
    UtTest_Add(LC_WPOffsetValid_Test_OffsetError, LC_Test_Setup, LC_Test_TearDown, "LC_WPOffsetValid_Test_OffsetError");

    UtTest_Add(LC_GetSizedWPData_Test_DataByte, LC_Test_Setup, LC_Test_TearDown, "LC_GetSizedWPData_Test_DataByte");
    UtTest_Add(LC_GetSizedWPData_Test_DataUByte, LC_Test_Setup, LC_Test_TearDown, "LC_GetSizedWPData_Test_DataUByte");
    UtTest_Add(LC_GetSizedWPData_Test_DataWordBELittleEndian, LC_Test_Setup, LC_Test_TearDown,
               "LC_GetSizedWPData_Test_DataWordBELittleEndian");
    UtTest_Add(LC_GetSizedWPData_Test_DataWordLELittleEndian, LC_Test_Setup, LC_Test_TearDown,
               "LC_GetSizedWPData_Test_DataWordLELittleEndian");
    UtTest_Add(LC_GetSizedWPData_Test_DataUWordBELittleEndian, LC_Test_Setup, LC_Test_TearDown,
               "LC_GetSizedWPData_Test_DataUWordBELittleEndian");
    UtTest_Add(LC_GetSizedWPData_Test_DataUWordLELittleEndian, LC_Test_Setup, LC_Test_TearDown,
               "LC_GetSizedWPData_Test_DataUWordLELittleEndian");
    UtTest_Add(LC_GetSizedWPData_Test_DataFloatBELittleEndian, LC_Test_Setup, LC_Test_TearDown,
               "LC_GetSizedWPData_Test_DataFloatBELittleEndian");
    UtTest_Add(LC_GetSizedWPData_Test_DataFloatLELittleEndian, LC_Test_Setup, LC_Test_TearDown,
               "LC_GetSizedWPData_Test_DataFloatLELittleEndian");
    UtTest_Add(LC_GetSizedWPData_Test_DataTypeError, LC_Test_Setup, LC_Test_TearDown,
               "LC_GetSizedWPData_Test_DataTypeError");

    UtTest_Add(LC_ValidateWDT_Test_UnusedTableEntry, LC_Test_Setup, LC_Test_TearDown,
               "LC_ValidateWDT_Test_UnusedTableEntry");
    UtTest_Add(LC_ValidateWDT_Test_InvalidDataType, LC_Test_Setup, LC_Test_TearDown,
               "LC_ValidateWDT_Test_InvalidDataType");
    UtTest_Add(LC_ValidateWDT_Test_InvalidOperator, LC_Test_Setup, LC_Test_TearDown,
               "LC_ValidateWDT_Test_InvalidOperator");
    UtTest_Add(LC_ValidateWDT_Test_BadMessageID, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateWDT_Test_BadMessageID");
    UtTest_Add(LC_ValidateWDT_Test_NaN, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateWDT_Test_NaN");
    UtTest_Add(LC_ValidateWDT_Test_Inf, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateWDT_Test_Inf");
    UtTest_Add(LC_ValidateWDT_Test_FloatingPointPassed, LC_Test_Setup, LC_Test_TearDown,
               "LC_ValidateWDT_Test_FloatingPointPassed");
    UtTest_Add(LC_ValidateWDT_Test_NonFloatingPointPassed, LC_Test_Setup, LC_Test_TearDown,
               "LC_ValidateWDT_Test_NonFloatingPointPassed");

    UtTest_Add(LC_ValidateWDT_Test_AllDataTypes, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateWDT_Test_AllDataTypes");
    UtTest_Add(LC_ValidateWDT_Test_AllOperatorIDs, LC_Test_Setup, LC_Test_TearDown,
               "LC_ValidateWDT_Test_AllOperatorIDs");

    UtTest_Add(LC_ValidateWDT_Test_FloatBE, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateWDT_Test_FloatBE");
}
