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

#include "lc_cmds.h"
#include "lc_app.h"
#include "lc_utils.h"
#include "lc_msg.h"
#include "lc_msgdefs.h"
#include "lc_msgids.h"
#include "lc_eventids.h"
#include "lc_version.h"
#include "lc_test_utils.h"
#include "lc_watch.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

#include <unistd.h>
#include <stdlib.h>

uint8 call_count_CFE_EVS_SendEvent;

void LC_SampleAPReq_Test_StateDisabled(void)
{
    LC_AppData.CurrentLCState = LC_STATE_DISABLED;

    LC_SampleAPReq(&UT_CmdBuf.Buf);
}

void LC_SampleAPReq_Test_AllowSampleAllWatchStale(void)
{
    CFE_SB_MsgId_t         TestMsgId;
    LC_SampleAP_Payload_t *PayloadPtr = &UT_CmdBuf.SampleAPCmd.Payload;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    LC_AppData.CurrentLCState = 99;

    PayloadPtr->StartIndex                 = LC_ALL_ACTIONPOINTS;
    PayloadPtr->EndIndex                   = LC_ALL_ACTIONPOINTS;
    PayloadPtr->UpdateAge                  = 1;
    LC_OperData.WRTPtr[0].CountdownToStale = 1;

    /* Execute the function being tested */
    LC_SampleAPReq(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_OperData.WRTPtr[0].WatchResult == LC_WATCH_STALE,
                  "LC_OperData.WRTPtr[0].WatchResult == LC_WATCH_STALE");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SampleAPReq_Test_AllowSampleAllWatchNotStale(void)
{
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    LC_AppData.CurrentLCState              = 99;
    LC_SampleAP_Payload_t *PayloadPtr      = &UT_CmdBuf.SampleAPCmd.Payload;
    PayloadPtr->StartIndex                 = LC_ALL_ACTIONPOINTS;
    PayloadPtr->EndIndex                   = LC_ALL_ACTIONPOINTS;
    PayloadPtr->UpdateAge                  = 1;
    LC_OperData.WRTPtr[0].CountdownToStale = 2;

    /* Execute the function being tested */
    LC_SampleAPReq(&UT_CmdBuf.Buf);

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SampleAPReq_Test_StartLessOrEqualToEndAndEndWithinArrayWatchStale(void)
{
    CFE_SB_MsgId_t         TestMsgId;
    LC_SampleAP_Payload_t *PayloadPtr = &UT_CmdBuf.SampleAPCmd.Payload;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    LC_AppData.CurrentLCState              = 99;
    PayloadPtr->StartIndex                 = LC_MAX_ACTIONPOINTS - 1;
    PayloadPtr->EndIndex                   = LC_MAX_ACTIONPOINTS - 1;
    PayloadPtr->UpdateAge                  = 1;
    LC_OperData.WRTPtr[0].CountdownToStale = 1;

    /* Execute the function being tested */
    LC_SampleAPReq(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_OperData.WRTPtr[0].WatchResult == LC_WATCH_STALE,
                  "LC_OperData.WRTPtr[0].WatchResult == LC_WATCH_STALE");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SampleAPReq_Test_ArrayIndexOutOfRange(void)
{
    CFE_SB_MsgId_t         TestMsgId;
    LC_SampleAP_Payload_t *PayloadPtr = &UT_CmdBuf.SampleAPCmd.Payload;
    int32                  strCmpResult;
    char                   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    LC_AppData.CurrentLCState = 99;
    PayloadPtr->StartIndex    = 2;
    PayloadPtr->EndIndex      = 1;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Sample AP error: invalid AP number, start = %%d, end = %%d");

    /* Execute the function being tested */
    LC_SampleAPReq(&UT_CmdBuf.Buf);

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSAMPLE_APNUM_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SampleAPReq_Test_BadSampleAllArgs(void)
{
    CFE_SB_MsgId_t         TestMsgId;
    LC_SampleAP_Payload_t *PayloadPtr = &UT_CmdBuf.SampleAPCmd.Payload;
    int32                  strCmpResult;
    char                   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Sample AP error: invalid AP number, start = %%d, end = %%d");

    LC_AppData.CurrentLCState              = 99;
    PayloadPtr->StartIndex                 = LC_ALL_ACTIONPOINTS;
    PayloadPtr->EndIndex                   = 1;
    PayloadPtr->UpdateAge                  = 1;
    LC_OperData.WRTPtr[0].CountdownToStale = 1;

    /* Execute the function being tested */
    LC_SampleAPReq(&UT_CmdBuf.Buf);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSAMPLE_APNUM_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SampleAPReq_Test_ArrayEndIndexTooHigh(void)
{
    CFE_SB_MsgId_t         TestMsgId;
    LC_SampleAP_Payload_t *PayloadPtr = &UT_CmdBuf.SampleAPCmd.Payload;
    int32                  strCmpResult;
    char                   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Sample AP error: invalid AP number, start = %%d, end = %%d");

    LC_AppData.CurrentLCState              = 99;
    PayloadPtr->StartIndex                 = 1;
    PayloadPtr->EndIndex                   = LC_MAX_ACTIONPOINTS;
    PayloadPtr->UpdateAge                  = 1;
    LC_OperData.WRTPtr[0].CountdownToStale = 1;

    /* Execute the function being tested */
    LC_SampleAPReq(&UT_CmdBuf.Buf);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSAMPLE_APNUM_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SampleAPReq_Test_SampleAllUpdateAgeZero(void)
{
    CFE_SB_MsgId_t         TestMsgId;
    LC_SampleAP_Payload_t *PayloadPtr = &UT_CmdBuf.SampleAPCmd.Payload;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    LC_AppData.CurrentLCState              = 99;
    PayloadPtr->StartIndex                 = LC_ALL_ACTIONPOINTS;
    PayloadPtr->EndIndex                   = LC_ALL_ACTIONPOINTS;
    PayloadPtr->UpdateAge                  = 0;
    LC_OperData.WRTPtr[0].CountdownToStale = 1;

    /* Execute the function being tested */
    LC_SampleAPReq(&UT_CmdBuf.Buf);

    /* Verify results */

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SendHkCmd_Test_WatchStale(void)
{
    CFE_Status_t              Result;
    uint16                    TableIndex;
    uint16                    HKIndex;
    uint8                     ExpectedByteData;
    CFE_SB_MsgId_t            TestMsgId;
    const LC_HkTlm_Payload_t *PayloadPtr = &LC_OperData.HkPacket.Payload;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WRTPtr[TableIndex].WatchResult = LC_WATCH_STALE;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_SendHkCmd(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKWR_STALE << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_STALE << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_STALE << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKWR_STALE);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(PayloadPtr->WPResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->WPResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_WATCHPOINTS / 2) / 4;
    UtAssert_True(PayloadPtr->WPResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->WPResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_WATCHPOINTS - 1) / 4;
    UtAssert_True(PayloadPtr->WPResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->WPResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SendHkCmd_Test_WatchFalse(void)
{
    CFE_Status_t              Result;
    uint16                    TableIndex;
    uint16                    HKIndex;
    uint8                     ExpectedByteData;
    CFE_SB_MsgId_t            TestMsgId;
    const LC_HkTlm_Payload_t *PayloadPtr = &LC_OperData.HkPacket.Payload;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WRTPtr[TableIndex].WatchResult = LC_WATCH_FALSE;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_SendHkCmd(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKWR_FALSE << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_FALSE << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_FALSE << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKWR_FALSE);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(PayloadPtr->WPResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->WPResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_WATCHPOINTS / 2) / 4;
    UtAssert_True(PayloadPtr->WPResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->WPResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_WATCHPOINTS - 1) / 4;
    UtAssert_True(PayloadPtr->WPResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->WPResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SendHkCmd_Test_WatchTrue(void)
{
    CFE_Status_t              Result;
    uint16                    TableIndex;
    uint16                    HKIndex;
    uint8                     ExpectedByteData;
    CFE_SB_MsgId_t            TestMsgId;
    const LC_HkTlm_Payload_t *PayloadPtr = &LC_OperData.HkPacket.Payload;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WRTPtr[TableIndex].WatchResult = LC_WATCH_TRUE;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_SendHkCmd(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKWR_TRUE << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_TRUE << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_TRUE << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKWR_TRUE);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(PayloadPtr->WPResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->WPResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_WATCHPOINTS / 2) / 4;
    UtAssert_True(PayloadPtr->WPResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->WPResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_WATCHPOINTS - 1) / 4;
    UtAssert_True(PayloadPtr->WPResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->WPResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SendHkCmd_Test_WatchError(void)
{
    CFE_Status_t              Result;
    uint16                    TableIndex;
    uint16                    HKIndex;
    uint8                     ExpectedByteData;
    const LC_HkTlm_Payload_t *PayloadPtr = &LC_OperData.HkPacket.Payload;

    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WRTPtr[TableIndex].WatchResult = LC_WATCH_ERROR;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_SendHkCmd(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKWR_ERROR << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_ERROR << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_ERROR << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKWR_ERROR);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(PayloadPtr->WPResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->WPResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_WATCHPOINTS / 2) / 4;
    UtAssert_True(PayloadPtr->WPResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->WPResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_WATCHPOINTS - 1) / 4;
    UtAssert_True(PayloadPtr->WPResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->WPResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SendHkCmd_Test_DefaultWatchResult(void)
{
    CFE_Status_t              Result;
    uint16                    TableIndex;
    uint16                    HKIndex;
    uint8                     ExpectedByteData;
    CFE_SB_MsgId_t            TestMsgId;
    const LC_HkTlm_Payload_t *PayloadPtr = &LC_OperData.HkPacket.Payload;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WRTPtr[TableIndex].WatchResult = 99;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_SendHkCmd(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKWR_ERROR << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_ERROR << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_ERROR << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKWR_ERROR);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(PayloadPtr->WPResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->WPResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_WATCHPOINTS / 2) / 4;
    UtAssert_True(PayloadPtr->WPResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->WPResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_WATCHPOINTS - 1) / 4;
    UtAssert_True(PayloadPtr->WPResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->WPResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SendHkCmd_Test_ActionNotUsedStale(void)
{
    CFE_Status_t              Result;
    uint16                    TableIndex;
    uint16                    HKIndex;
    uint8                     ExpectedByteData;
    CFE_SB_MsgId_t            TestMsgId;
    const LC_HkTlm_Payload_t *PayloadPtr = &LC_OperData.HkPacket.Payload;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = LC_APSTATE_NOT_USED;
        LC_OperData.ARTPtr[TableIndex].ActionResult = LC_ACTION_STALE;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_SendHkCmd(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_NOT_USED << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STALE << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_NOT_USED << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_STALE);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS / 2) / 2;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SendHkCmd_Test_APStateActiveActionPass(void)
{
    CFE_Status_t              Result;
    uint16                    TableIndex;
    uint16                    HKIndex;
    uint8                     ExpectedByteData;
    CFE_SB_MsgId_t            TestMsgId;
    const LC_HkTlm_Payload_t *PayloadPtr = &LC_OperData.HkPacket.Payload;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = LC_APSTATE_ACTIVE;
        LC_OperData.ARTPtr[TableIndex].ActionResult = LC_ACTION_PASS;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_SendHkCmd(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_ACTIVE << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_PASS << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_ACTIVE << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_PASS);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS / 2) / 2;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    UtAssert_True(PayloadPtr->ActiveAPs == LC_MAX_ACTIONPOINTS, "PayloadPtr->ActiveAPs = LC_MAX_ACTIONPOINTS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SendHkCmd_Test_APStatePassiveActionFail(void)
{
    CFE_Status_t              Result;
    uint16                    TableIndex;
    uint16                    HKIndex;
    uint8                     ExpectedByteData;
    CFE_SB_MsgId_t            TestMsgId;
    const LC_HkTlm_Payload_t *PayloadPtr = &LC_OperData.HkPacket.Payload;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = LC_APSTATE_PASSIVE;
        LC_OperData.ARTPtr[TableIndex].ActionResult = LC_ACTION_FAIL;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_SendHkCmd(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_PASSIVE << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_FAIL << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_PASSIVE << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_FAIL);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS / 2) / 2;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SendHkCmd_Test_APStateDisabledActionError(void)
{
    CFE_Status_t              Result;
    uint16                    TableIndex;
    uint16                    HKIndex;
    uint8                     ExpectedByteData;
    CFE_SB_MsgId_t            TestMsgId;
    const LC_HkTlm_Payload_t *PayloadPtr = &LC_OperData.HkPacket.Payload;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = LC_APSTATE_DISABLED;
        LC_OperData.ARTPtr[TableIndex].ActionResult = LC_ACTION_ERROR;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_SendHkCmd(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_DISABLED << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_ERROR << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_DISABLED << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_ERROR);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS / 2) / 2;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SendHkCmd_Test_APStatePermOffActionError(void)
{
    CFE_Status_t              Result;
    uint16                    TableIndex;
    uint16                    HKIndex;
    uint8                     ExpectedByteData;
    CFE_SB_MsgId_t            TestMsgId;
    const LC_HkTlm_Payload_t *PayloadPtr = &LC_OperData.HkPacket.Payload;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = LC_APSTATE_PERMOFF;
        LC_OperData.ARTPtr[TableIndex].ActionResult = LC_ACTION_ERROR;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_SendHkCmd(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_NOT_USED << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_ERROR << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_NOT_USED << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_ERROR);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS / 2) / 2;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SendHkCmd_Test_DefaultCurrentStateAndActionResult(void)
{
    CFE_Status_t              Result;
    uint16                    TableIndex;
    uint16                    HKIndex;
    uint8                     ExpectedByteData;
    CFE_SB_MsgId_t            TestMsgId;
    const LC_HkTlm_Payload_t *PayloadPtr = &LC_OperData.HkPacket.Payload;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = 99;
        LC_OperData.ARTPtr[TableIndex].ActionResult = 99;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_SendHkCmd(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_NOT_USED << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_ERROR << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_NOT_USED << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_ERROR);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS / 2) / 2;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True(PayloadPtr->APResults[HKIndex] == ExpectedByteData,
                  "PayloadPtr->APResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SendHkCmd_Test_ManageTablesError(void)
{
    CFE_Status_t   Result;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_PerformMaintenance), -1);

    /* Set to satisfy condition "(Result = LC_ManageTables()) != CFE_SUCCESS" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), -1);

    /* Execute the function being tested */
    Result = LC_SendHkCmd(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_NoopCmd_Test(void)
{
    LC_AppData.CmdCount = 1;

    LC_NoopCmd(&UT_CmdBuf.Buf);

    UtAssert_UINT8_EQ(LC_AppData.CmdCount, 2);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void LC_ResetCountersCmd_Test(void)
{
    LC_ResetCountersCmd(&UT_CmdBuf.Buf);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void LC_ResetCounters_Test(void)
{
    LC_AppData.CmdCount    = 1;
    LC_AppData.CmdErrCount = 2;

    LC_AppData.APSampleCount       = 3;
    LC_AppData.MonitoredMsgCount   = 4;
    LC_AppData.RTSExecCount        = 5;
    LC_AppData.PassiveRTSExecCount = 6;

    /* Execute the function being tested */
    LC_ResetCounters();

    /* Verify results */
    UtAssert_True(LC_AppData.CmdCount == 0, "LC_AppData.CmdCount == 0");
    UtAssert_True(LC_AppData.CmdErrCount == 0, "LC_AppData.CmdErrCount == 0");
    UtAssert_True(LC_AppData.APSampleCount == 0, "LC_AppData.APSampleCount == 0");
    UtAssert_True(LC_AppData.MonitoredMsgCount == 0, "LC_AppData.MonitoredMsgCount == 0");
    UtAssert_True(LC_AppData.RTSExecCount == 0, "LC_AppData.RTSExecCount == 0");
    UtAssert_True(LC_AppData.PassiveRTSExecCount == 0, "LC_AppData.PassiveRTSExecCount == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SetLCStateCmd_Test_Active(void)
{
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetLCState_Payload_t *PayloadPtr = &UT_CmdBuf.SetLCStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set LC state command: new state = %%d");

    PayloadPtr->NewLCState = LC_STATE_ACTIVE;

    /* Execute the function being tested */
    LC_SetLCStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CurrentLCState == LC_STATE_ACTIVE, "LC_AppData.CurrentLCState == LC_STATE_ACTIVE");
    UtAssert_True(LC_AppData.CmdCount == 1, "LC_AppData.CmdCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_LCSTATE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetLCStateCmd_Test_Passive(void)
{
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetLCState_Payload_t *PayloadPtr = &UT_CmdBuf.SetLCStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set LC state command: new state = %%d");

    PayloadPtr->NewLCState = LC_STATE_PASSIVE;

    /* Execute the function being tested */
    LC_SetLCStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CurrentLCState == LC_STATE_PASSIVE, "LC_AppData.CurrentLCState == LC_STATE_PASSIVE");
    UtAssert_True(LC_AppData.CmdCount == 1, "LC_AppData.CmdCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_LCSTATE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetLCStateCmd_Test_Disabled(void)
{
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetLCState_Payload_t *PayloadPtr = &UT_CmdBuf.SetLCStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set LC state command: new state = %%d");

    PayloadPtr->NewLCState = LC_STATE_DISABLED;

    /* Execute the function being tested */
    LC_SetLCStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CurrentLCState == LC_STATE_DISABLED, "LC_AppData.CurrentLCState == LC_STATE_DISABLED");
    UtAssert_True(LC_AppData.CmdCount == 1, "LC_AppData.CmdCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_LCSTATE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetLCStateCmd_Test_Default(void)
{
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetLCState_Payload_t *PayloadPtr = &UT_CmdBuf.SetLCStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set LC state error: invalid state = %%d");

    PayloadPtr->NewLCState = 99;

    /* Execute the function being tested */
    LC_SetLCStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_LCSTATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_Default(void)
{
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetAPState_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state error: AP = %%d, Invalid new state = %%d");

    PayloadPtr->APNumber   = 1;
    PayloadPtr->NewAPState = 99;

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_NEW_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_SetAllActionPointsActive(void)
{
    uint16                   TableIndex;
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetAPState_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state command: AP = %%d, New state = %%d");

    PayloadPtr->NewAPState = LC_APSTATE_ACTIVE;
    PayloadPtr->APNumber   = LC_ALL_ACTIONPOINTS;

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = 99;
    }

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */

    /* Checks for first index: */
    TableIndex = 0;
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE,
                  "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE");

    /* Checks for middle index: */
    TableIndex = LC_MAX_ACTIONPOINTS / 2;
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE,
                  "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE");

    /* Checks for last index: */
    TableIndex = LC_MAX_ACTIONPOINTS - 1;
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE,
                  "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_SetAllActionPointsActiveOneNotUsed(void)
{
    uint16                   TableIndex;
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetAPState_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state command: AP = %%d, New state = %%d");

    PayloadPtr->NewAPState = LC_APSTATE_ACTIVE;
    PayloadPtr->APNumber   = LC_ALL_ACTIONPOINTS;

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = 99;
    }
    LC_OperData.ARTPtr[0].CurrentState = LC_APSTATE_NOT_USED;

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */

    /* Checks for first index: */
    TableIndex = 0;
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_NOT_USED,
                  "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_NOT_USED");

    /* Checks for middle index: */
    TableIndex = LC_MAX_ACTIONPOINTS / 2;
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE,
                  "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE");

    /* Checks for last index: */
    TableIndex = LC_MAX_ACTIONPOINTS - 1;
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE,
                  "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_SetAllActionPointsActiveOnePermOff(void)
{
    uint16                   TableIndex;
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetAPState_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state command: AP = %%d, New state = %%d");

    PayloadPtr->NewAPState = LC_APSTATE_ACTIVE;
    PayloadPtr->APNumber   = LC_ALL_ACTIONPOINTS;

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = 99;
    }
    LC_OperData.ARTPtr[0].CurrentState = LC_APSTATE_PERMOFF;

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */

    /* Checks for first index: */
    TableIndex = 0;
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_PERMOFF,
                  "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_PERMOFF");

    /* Checks for middle index: */
    TableIndex = LC_MAX_ACTIONPOINTS / 2;
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE,
                  "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE");

    /* Checks for last index: */
    TableIndex = LC_MAX_ACTIONPOINTS - 1;
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE,
                  "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_SetAllActionPointsPassive(void)
{
    uint16                   TableIndex;
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetAPState_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state command: AP = %%d, New state = %%d");

    PayloadPtr->NewAPState = LC_APSTATE_PASSIVE;
    PayloadPtr->APNumber   = LC_ALL_ACTIONPOINTS;

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = 99;
    }

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */

    /* Checks for first index: */
    TableIndex = 0;
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_PASSIVE,
                  "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_PASSIVE");

    /* Checks for middle index: */
    TableIndex = LC_MAX_ACTIONPOINTS / 2;
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_PASSIVE,
                  "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_PASSIVE");

    /* Checks for last index: */
    TableIndex = LC_MAX_ACTIONPOINTS - 1;
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_PASSIVE,
                  "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_PASSIVE");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_SetAllActionPointsDisabled(void)
{
    uint16                   TableIndex;
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetAPState_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state command: AP = %%d, New state = %%d");

    PayloadPtr->NewAPState = LC_APSTATE_DISABLED;
    PayloadPtr->APNumber   = LC_ALL_ACTIONPOINTS;

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = 99;
    }

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */

    /* Checks for first index: */
    TableIndex = 0;
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_DISABLED,
                  "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_DISABLED");

    /* Checks for middle index: */
    TableIndex = LC_MAX_ACTIONPOINTS / 2;
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_DISABLED,
                  "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_DISABLED");

    /* Checks for last index: */
    TableIndex = LC_MAX_ACTIONPOINTS - 1;
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_DISABLED,
                  "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_DISABLED");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_UpdateSingleActionPointActive(void)
{
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetAPState_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state command: AP = %%d, New state = %%d");

    PayloadPtr->NewAPState = LC_APSTATE_ACTIVE;
    PayloadPtr->APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState = 99;

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState == LC_APSTATE_ACTIVE,
                  "LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState == LC_APSTATE_ACTIVE");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_UpdateSingleActionPointNotUsed(void)
{
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetAPState_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state error: AP = %%d, Invalid current AP state = %%d");

    PayloadPtr->NewAPState = LC_APSTATE_ACTIVE;
    PayloadPtr->APNumber   = 0;

    LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState = LC_APSTATE_NOT_USED;

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState == LC_APSTATE_NOT_USED,
                  "LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState == LC_APSTATE_NOT_USED");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_CURR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_UpdateSingleActionPointPassive(void)
{
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetAPState_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state command: AP = %%d, New state = %%d");

    PayloadPtr->NewAPState = LC_APSTATE_PASSIVE;
    PayloadPtr->APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState = 99;

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState == LC_APSTATE_PASSIVE,
                  "LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState == LC_APSTATE_PASSIVE");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_UpdateSingleActionPointDisabled(void)
{
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetAPState_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state command: AP = %%d, New state = %%d");

    PayloadPtr->NewAPState = LC_APSTATE_DISABLED;
    PayloadPtr->APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState = 99;

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState == LC_APSTATE_DISABLED,
                  "LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState == LC_APSTATE_DISABLED");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_InvalidCurrentAPStateActive(void)
{
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetAPState_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state error: AP = %%d, Invalid current AP state = %%d");

    PayloadPtr->NewAPState = LC_APSTATE_ACTIVE;
    PayloadPtr->APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState = LC_APSTATE_PERMOFF;

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_CURR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_InvalidCurrentAPStatePassive(void)
{
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetAPState_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state error: AP = %%d, Invalid current AP state = %%d");

    PayloadPtr->NewAPState = LC_APSTATE_PASSIVE;
    PayloadPtr->APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState = LC_APSTATE_PERMOFF;

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_CURR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_InvalidCurrentAPStateDisabled(void)
{
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetAPState_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state error: AP = %%d, Invalid current AP state = %%d");

    PayloadPtr->NewAPState = LC_APSTATE_DISABLED;
    PayloadPtr->APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState = LC_APSTATE_PERMOFF;

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_CURR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_InvalidAPNumberActive(void)
{
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetAPState_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set AP state error: Invalid AP number = %%d");

    PayloadPtr->NewAPState = LC_APSTATE_ACTIVE;
    PayloadPtr->APNumber   = LC_MAX_ACTIONPOINTS;

    LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState = LC_APSTATE_PERMOFF;

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_APNUM_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_InvalidAPNumberPassive(void)
{
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetAPState_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set AP state error: Invalid AP number = %%d");

    PayloadPtr->NewAPState = LC_APSTATE_PASSIVE;
    PayloadPtr->APNumber   = LC_MAX_ACTIONPOINTS;

    LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState = LC_APSTATE_PERMOFF;

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_APNUM_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_InvalidAPNumberDisabled(void)
{
    CFE_SB_MsgId_t           TestMsgId;
    LC_SetAPState_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPStateCmd.Payload;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set AP state error: Invalid AP number = %%d");

    PayloadPtr->NewAPState = LC_APSTATE_DISABLED;
    PayloadPtr->APNumber   = LC_MAX_ACTIONPOINTS;

    LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState = LC_APSTATE_PERMOFF;

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_APNUM_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPPermOffCmd_Test_InvalidAPNumberMaxActionpoints(void)
{
    CFE_SB_MsgId_t             TestMsgId;
    LC_SetAPPermOff_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPPermOffCmd.Payload;
    int32                      strCmpResult;
    char                       ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set AP perm off error: Invalid AP number = %%d");

    PayloadPtr->APNumber = LC_MAX_ACTIONPOINTS;

    /* Execute the function being tested */
    LC_SetAPPermOffCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APOFF_APNUM_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPPermOffCmd_Test_InvalidAPNumberAllActionpoints(void)
{
    CFE_SB_MsgId_t             TestMsgId;
    LC_SetAPPermOff_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPPermOffCmd.Payload;
    int32                      strCmpResult;
    char                       ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set AP perm off error: Invalid AP number = %%d");

    PayloadPtr->APNumber = LC_ALL_ACTIONPOINTS;

    /* Execute the function being tested */
    LC_SetAPPermOffCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APOFF_APNUM_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPPermOffCmd_Test_APNotDisabled(void)
{
    CFE_SB_MsgId_t             TestMsgId;
    LC_SetAPPermOff_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPPermOffCmd.Payload;
    int32                      strCmpResult;
    char                       ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP perm off error, AP NOT Disabled: AP = %%d, Current state = %%d");

    PayloadPtr->APNumber = 1;

    LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState = 99;

    /* Execute the function being tested */
    LC_SetAPPermOffCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APOFF_CURR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPPermOffCmd_Test_Nominal(void)
{
    CFE_SB_MsgId_t             TestMsgId;
    LC_SetAPPermOff_Payload_t *PayloadPtr = &UT_CmdBuf.SetAPPermOffCmd.Payload;
    int32                      strCmpResult;
    char                       ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set AP permanently off command: AP = %%d");

    PayloadPtr->APNumber = 1;

    LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState = LC_APSTATE_DISABLED;

    /* Execute the function being tested */
    LC_SetAPPermOffCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState == 4,
                  "LC_OperData.ARTPtr[PayloadPtr->APNumber].CurrentState == 4");
    UtAssert_True(LC_AppData.CmdCount == 1, "LC_AppData.CmdCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APOFF_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_ResetAPStatsCmd_Test_AllActionPoints(void)
{
    CFE_SB_MsgId_t             TestMsgId;
    LC_ResetAPStats_Payload_t *PayloadPtr = &UT_CmdBuf.ResetAPStatsCmd.Payload;
    int32                      strCmpResult;
    char                       ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Reset AP stats command: AP = %%d");

    PayloadPtr->APNumber = LC_ALL_ACTIONPOINTS;

    /* Execute the function being tested */
    LC_ResetAPStatsCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CmdCount == 1, "LC_AppData.CmdCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATS_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_ResetAPStatsCmd_Test_SingleActionPoint(void)
{
    CFE_SB_MsgId_t             TestMsgId;
    LC_ResetAPStats_Payload_t *PayloadPtr = &UT_CmdBuf.ResetAPStatsCmd.Payload;
    int32                      strCmpResult;
    char                       ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Reset AP stats command: AP = %%d");

    PayloadPtr->APNumber = LC_MAX_ACTIONPOINTS - 1;

    /* Execute the function being tested */
    LC_ResetAPStatsCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CmdCount == 1, "LC_AppData.CmdCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATS_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_ResetAPStatsCmd_Test_InvalidAPNumber(void)
{
    CFE_SB_MsgId_t             TestMsgId;
    LC_ResetAPStats_Payload_t *PayloadPtr = &UT_CmdBuf.ResetAPStatsCmd.Payload;
    int32                      strCmpResult;
    char                       ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Reset AP stats error: invalid AP number = %%d");

    PayloadPtr->APNumber = LC_MAX_ACTIONPOINTS;

    /* Execute the function being tested */
    LC_ResetAPStatsCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATS_APNUM_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_ResetResultsAP_Test(void)
{
    uint32 TableIndex;
    uint32 StartIndex    = 0;
    uint32 EndIndex      = 5;
    bool   ResetStatsCmd = false;

    for (TableIndex = StartIndex; TableIndex <= EndIndex; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].ActionResult = 99;
        LC_OperData.ARTPtr[TableIndex].CurrentState = 99;

        LC_OperData.ARTPtr[TableIndex].PassiveAPCount  = 99;
        LC_OperData.ARTPtr[TableIndex].FailToPassCount = 99;
        LC_OperData.ARTPtr[TableIndex].PassToFailCount = 99;

        LC_OperData.ARTPtr[TableIndex].ConsecutiveFailCount    = 99;
        LC_OperData.ARTPtr[TableIndex].CumulativeFailCount     = 99;
        LC_OperData.ARTPtr[TableIndex].CumulativeRTSExecCount  = 99;
        LC_OperData.ARTPtr[TableIndex].CumulativeEventMsgsSent = 99;

        LC_OperData.ADTPtr[TableIndex].DefaultState = 1;
    }

    /* Execute the function being tested */
    LC_ResetResultsAP(StartIndex, EndIndex, ResetStatsCmd);

    /* Verify results */

    /* Checks for first index: */
    TableIndex = StartIndex;

    UtAssert_True(LC_OperData.ARTPtr[TableIndex].ActionResult == LC_ACTION_STALE,
                  "LC_OperData.ARTPtr[TableIndex].ActionResult == LC_ACTION_STALE");

    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == 1, "LC_OperData.ARTPtr[TableIndex].CurrentState == 1");

    UtAssert_True(LC_OperData.ARTPtr[TableIndex].PassiveAPCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].PassiveAPCount == 0");
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].FailToPassCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].FailToPassCount == 0");
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].PassToFailCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].PassToFailCount == 0");

    UtAssert_True(LC_OperData.ARTPtr[TableIndex].ConsecutiveFailCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].ConsecutiveFailCount == 0");
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CumulativeFailCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].CumulativeFailCount == 0");
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CumulativeRTSExecCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].CumulativeRTSExecCount == 0");
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CumulativeEventMsgsSent == 0,
                  "LC_OperData.ARTPtr[TableIndex].CumulativeEventMsgsSent == 0");

    /* Checks for middle index: */
    TableIndex = (StartIndex + EndIndex) / 2;

    UtAssert_True(LC_OperData.ARTPtr[TableIndex].ActionResult == LC_ACTION_STALE,
                  "LC_OperData.ARTPtr[TableIndex].ActionResult == LC_ACTION_STALE");

    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == 1, "LC_OperData.ARTPtr[TableIndex].CurrentState == 1");

    UtAssert_True(LC_OperData.ARTPtr[TableIndex].PassiveAPCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].PassiveAPCount == 0");
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].FailToPassCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].FailToPassCount == 0");
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].PassToFailCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].PassToFailCount == 0");

    UtAssert_True(LC_OperData.ARTPtr[TableIndex].ConsecutiveFailCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].ConsecutiveFailCount == 0");
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CumulativeFailCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].CumulativeFailCount == 0");
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CumulativeRTSExecCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].CumulativeRTSExecCount == 0");

    /* Checks for last index: */
    TableIndex = EndIndex;

    UtAssert_True(LC_OperData.ARTPtr[TableIndex].ActionResult == LC_ACTION_STALE,
                  "LC_OperData.ARTPtr[TableIndex].ActionResult == LC_ACTION_STALE");

    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CurrentState == 1, "LC_OperData.ARTPtr[TableIndex].CurrentState == 1");

    UtAssert_True(LC_OperData.ARTPtr[TableIndex].PassiveAPCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].PassiveAPCount == 0");
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].FailToPassCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].FailToPassCount == 0");
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].PassToFailCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].PassToFailCount == 0");

    UtAssert_True(LC_OperData.ARTPtr[TableIndex].ConsecutiveFailCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].ConsecutiveFailCount == 0");
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CumulativeFailCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].CumulativeFailCount == 0");
    UtAssert_True(LC_OperData.ARTPtr[TableIndex].CumulativeRTSExecCount == 0,
                  "LC_OperData.ARTPtr[TableIndex].CumulativeRTSExecCount == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_ResetWPStatsCmd_Test_AllWatchPoints(void)
{
    CFE_SB_MsgId_t             TestMsgId;
    LC_ResetWPStats_Payload_t *PayloadPtr = &UT_CmdBuf.ResetWPStatsCmd.Payload;
    int32                      strCmpResult;
    char                       ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Reset WP stats command: WP = %%d");

    PayloadPtr->WPNumber = LC_ALL_WATCHPOINTS;

    /* Execute the function being tested */
    LC_ResetWPStatsCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CmdCount == 1, "LC_AppData.CmdCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WPSTATS_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_ResetWPStatsCmd_Test_SingleWatchPoint(void)
{
    CFE_SB_MsgId_t             TestMsgId;
    LC_ResetWPStats_Payload_t *PayloadPtr = &UT_CmdBuf.ResetWPStatsCmd.Payload;
    int32                      strCmpResult;
    char                       ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Reset WP stats command: WP = %%d");

    PayloadPtr->WPNumber = LC_MAX_WATCHPOINTS - 1;

    /* Execute the function being tested */
    LC_ResetWPStatsCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CmdCount == 1, "LC_AppData.CmdCount == 1");
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WPSTATS_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_ResetWPStatsCmd_Test_InvalidWPNumber(void)
{
    CFE_SB_MsgId_t             TestMsgId;
    LC_ResetWPStats_Payload_t *PayloadPtr = &UT_CmdBuf.ResetWPStatsCmd.Payload;
    int32                      strCmpResult;
    char                       ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Reset WP stats error: invalid WP number = %%d");

    PayloadPtr->WPNumber = LC_MAX_WATCHPOINTS;

    /* Execute the function being tested */
    LC_ResetWPStatsCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WPSTATS_WPNUM_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_ResetResultsWP_Test(void)
{
    uint32 TableIndex;
    uint32 StartIndex    = 0;
    uint32 EndIndex      = 5;
    bool   ResetStatsCmd = false;

    for (TableIndex = StartIndex; TableIndex <= EndIndex; TableIndex++)
    {
        LC_OperData.WRTPtr[TableIndex].WatchResult      = 99;
        LC_OperData.WRTPtr[TableIndex].CountdownToStale = 99;

        LC_OperData.WRTPtr[TableIndex].EvaluationCount      = 99;
        LC_OperData.WRTPtr[TableIndex].FalseToTrueCount     = 99;
        LC_OperData.WRTPtr[TableIndex].ConsecutiveTrueCount = 99;
        LC_OperData.WRTPtr[TableIndex].CumulativeTrueCount  = 99;

        LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Value                = 99;
        LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Seconds    = 99;
        LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Subseconds = 99;

        LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Value                = 99;
        LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Seconds    = 99;
        LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Subseconds = 99;
    }

    /* Execute the function being tested */
    LC_ResetResultsWP(StartIndex, EndIndex, ResetStatsCmd);

    /* Verify results */

    /* Checks for first index: */
    TableIndex = StartIndex;

    UtAssert_True(LC_OperData.WRTPtr[TableIndex].WatchResult == LC_WATCH_STALE,
                  "LC_OperData.WRTPtr[TableIndex].WatchResult == LC_WATCH_STALE");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].CountdownToStale == 0,
                  "LC_OperData.WRTPtr[TableIndex].CountdownToStale == 0");

    UtAssert_True(LC_OperData.WRTPtr[TableIndex].EvaluationCount == 0,
                  "LC_OperData.WRTPtr[TableIndex].EvaluationCount == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].FalseToTrueCount == 0,
                  "LC_OperData.WRTPtr[TableIndex].FalseToTrueCount == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].ConsecutiveTrueCount == 0,
                  "LC_OperData.WRTPtr[TableIndex].ConsecutiveTrueCount == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].CumulativeTrueCount == 0,
                  "LC_OperData.WRTPtr[TableIndex].CumulativeTrueCount == 0");

    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Value == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Value == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Seconds == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Seconds == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Subseconds == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Subseconds == 0");

    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Value == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Value == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Seconds == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Seconds == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Subseconds == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Subseconds == 0");

    /* Checks for middle index: */
    TableIndex = (StartIndex + EndIndex) / 2;

    UtAssert_True(LC_OperData.WRTPtr[TableIndex].WatchResult == LC_WATCH_STALE,
                  "LC_OperData.WRTPtr[TableIndex].WatchResult == LC_WATCH_STALE");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].CountdownToStale == 0,
                  "LC_OperData.WRTPtr[TableIndex].CountdownToStale == 0");

    UtAssert_True(LC_OperData.WRTPtr[TableIndex].EvaluationCount == 0,
                  "LC_OperData.WRTPtr[TableIndex].EvaluationCount == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].FalseToTrueCount == 0,
                  "LC_OperData.WRTPtr[TableIndex].FalseToTrueCount == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].ConsecutiveTrueCount == 0,
                  "LC_OperData.WRTPtr[TableIndex].ConsecutiveTrueCount == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].CumulativeTrueCount == 0,
                  "LC_OperData.WRTPtr[TableIndex].CumulativeTrueCount == 0");

    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Value == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Value == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Seconds == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Seconds == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Subseconds == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Subseconds == 0");

    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Value == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Value == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Seconds == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Seconds == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Subseconds == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Subseconds == 0");

    /* Checks for last index: */
    TableIndex = EndIndex;

    UtAssert_True(LC_OperData.WRTPtr[TableIndex].WatchResult == LC_WATCH_STALE,
                  "LC_OperData.WRTPtr[TableIndex].WatchResult == LC_WATCH_STALE");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].CountdownToStale == 0,
                  "LC_OperData.WRTPtr[TableIndex].CountdownToStale == 0");

    UtAssert_True(LC_OperData.WRTPtr[TableIndex].EvaluationCount == 0,
                  "LC_OperData.WRTPtr[TableIndex].EvaluationCount == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].FalseToTrueCount == 0,
                  "LC_OperData.WRTPtr[TableIndex].FalseToTrueCount == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].ConsecutiveTrueCount == 0,
                  "LC_OperData.WRTPtr[TableIndex].ConsecutiveTrueCount == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].CumulativeTrueCount == 0,
                  "LC_OperData.WRTPtr[TableIndex].CumulativeTrueCount == 0");

    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Value == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Value == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Seconds == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Seconds == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Subseconds == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Subseconds == 0");

    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Value == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Value == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Seconds == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Seconds == 0");
    UtAssert_True(LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Subseconds == 0,
                  "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Subseconds == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void UtTest_Setup(void)
{
    UtTest_Add(LC_SampleAPReq_Test_AllowSampleAllWatchStale, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleAPReq_Test_AllowSampleAllWatchStale");
    UtTest_Add(LC_SampleAPReq_Test_AllowSampleAllWatchNotStale, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleAPReq_Test_AllowSampleAllWatchNotStale");

    UtTest_Add(LC_SampleAPReq_Test_StartLessOrEqualToEndAndEndWithinArrayWatchStale, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleAPReq_Test_StartLessOrEqualToEndAndEndWithinArrayWatchStale");
    UtTest_Add(LC_SampleAPReq_Test_ArrayIndexOutOfRange, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleAPReq_Test_ArrayIndexOutOfRange");
    UtTest_Add(LC_SampleAPReq_Test_StateDisabled, LC_Test_Setup, LC_Test_TearDown, "LC_SampleAPReq_Test_StateDisabled");
    UtTest_Add(LC_SampleAPReq_Test_ArrayEndIndexTooHigh, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleAPReq_Test_ArrayEndIndexTooHigh");
    UtTest_Add(LC_SampleAPReq_Test_SampleAllUpdateAgeZero, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleAPReq_Test_SampleAllUpdateAgeZero");
    UtTest_Add(LC_SampleAPReq_Test_BadSampleAllArgs, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleAPReq_Test_BadSampleAllArgs");

    UtTest_Add(LC_SendHkCmd_Test_WatchStale, LC_Test_Setup, LC_Test_TearDown, "LC_SendHkCmd_Test_WatchStale");
    UtTest_Add(LC_SendHkCmd_Test_WatchFalse, LC_Test_Setup, LC_Test_TearDown, "LC_SendHkCmd_Test_WatchFalse");
    UtTest_Add(LC_SendHkCmd_Test_WatchTrue, LC_Test_Setup, LC_Test_TearDown, "LC_SendHkCmd_Test_WatchTrue");
    UtTest_Add(LC_SendHkCmd_Test_WatchError, LC_Test_Setup, LC_Test_TearDown, "LC_SendHkCmd_Test_WatchError");
    UtTest_Add(LC_SendHkCmd_Test_DefaultWatchResult, LC_Test_Setup, LC_Test_TearDown,
               "LC_SendHkCmd_Test_DefaultWatchResult");
    UtTest_Add(LC_SendHkCmd_Test_ActionNotUsedStale, LC_Test_Setup, LC_Test_TearDown,
               "LC_SendHkCmd_Test_ActionNotUsedStale");
    UtTest_Add(LC_SendHkCmd_Test_APStateActiveActionPass, LC_Test_Setup, LC_Test_TearDown,
               "LC_SendHkCmd_Test_APStateActiveActionPass");
    UtTest_Add(LC_SendHkCmd_Test_APStatePassiveActionFail, LC_Test_Setup, LC_Test_TearDown,
               "LC_SendHkCmd_Test_APStatePassiveActionFail");
    UtTest_Add(LC_SendHkCmd_Test_APStateDisabledActionError, LC_Test_Setup, LC_Test_TearDown,
               "LC_SendHkCmd_Test_APStateDisabledActionError");
    UtTest_Add(LC_SendHkCmd_Test_APStatePermOffActionError, LC_Test_Setup, LC_Test_TearDown,
               "LC_SendHkCmd_Test_APStatePermOffActionError");
    UtTest_Add(LC_SendHkCmd_Test_DefaultCurrentStateAndActionResult, LC_Test_Setup, LC_Test_TearDown,
               "LC_SendHkCmd_Test_DefaultCurrentStateAndActionResult");
    UtTest_Add(LC_SendHkCmd_Test_ManageTablesError, LC_Test_Setup, LC_Test_TearDown,
               "LC_SendHkCmd_Test_ManageTablesError");

    UtTest_Add(LC_NoopCmd_Test, LC_Test_Setup, LC_Test_TearDown, "LC_NoopCmd_Test");
    UtTest_Add(LC_ResetCountersCmd_Test, LC_Test_Setup, LC_Test_TearDown, "LC_ResetCountersCmd_Test");

    UtTest_Add(LC_ResetCounters_Test, LC_Test_Setup, LC_Test_TearDown, "LC_ResetCounters_Test");

    UtTest_Add(LC_SetLCStateCmd_Test_Active, LC_Test_Setup, LC_Test_TearDown, "LC_SetLCStateCmd_Test_Active");
    UtTest_Add(LC_SetLCStateCmd_Test_Passive, LC_Test_Setup, LC_Test_TearDown, "LC_SetLCStateCmd_Test_Passive");
    UtTest_Add(LC_SetLCStateCmd_Test_Disabled, LC_Test_Setup, LC_Test_TearDown, "LC_SetLCStateCmd_Test_Disabled");
    UtTest_Add(LC_SetLCStateCmd_Test_Default, LC_Test_Setup, LC_Test_TearDown, "LC_SetLCStateCmd_Test_Default");

    UtTest_Add(LC_SetAPStateCmd_Test_Default, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPStateCmd_Test_Default");
    UtTest_Add(LC_SetAPStateCmd_Test_SetAllActionPointsActive, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPStateCmd_Test_SetAllActionPointsActive");
    UtTest_Add(LC_SetAPStateCmd_Test_SetAllActionPointsPassive, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPStateCmd_Test_SetAllActionPointsPassive");
    UtTest_Add(LC_SetAPStateCmd_Test_SetAllActionPointsDisabled, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPStateCmd_Test_SetAllActionPointsDisabled");
    UtTest_Add(LC_SetAPStateCmd_Test_UpdateSingleActionPointActive, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPStateCmd_Test_UpdateSingleActionPointActive");
    UtTest_Add(LC_SetAPStateCmd_Test_UpdateSingleActionPointNotUsed, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPStateCmd_Test_UpdateSingleActionPointNotUsed");

    UtTest_Add(LC_SetAPStateCmd_Test_UpdateSingleActionPointPassive, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPStateCmd_Test_UpdateSingleActionPointPassive");
    UtTest_Add(LC_SetAPStateCmd_Test_UpdateSingleActionPointDisabled, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPStateCmd_Test_UpdateSingleActionPointDisabled");
    UtTest_Add(LC_SetAPStateCmd_Test_InvalidCurrentAPStateActive, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPStateCmd_Test_InvalidCurrentAPStateActive");
    UtTest_Add(LC_SetAPStateCmd_Test_InvalidCurrentAPStatePassive, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPStateCmd_Test_InvalidCurrentAPStatePassive");
    UtTest_Add(LC_SetAPStateCmd_Test_InvalidCurrentAPStateDisabled, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPStateCmd_Test_InvalidCurrentAPStateDisabled");
    UtTest_Add(LC_SetAPStateCmd_Test_InvalidAPNumberActive, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPStateCmd_Test_InvalidAPNumberActive");
    UtTest_Add(LC_SetAPStateCmd_Test_InvalidAPNumberPassive, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPStateCmd_Test_InvalidAPNumberPassive");
    UtTest_Add(LC_SetAPStateCmd_Test_InvalidAPNumberDisabled, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPStateCmd_Test_InvalidAPNumberDisabled");

    UtTest_Add(LC_SetAPPermOffCmd_Test_InvalidAPNumberMaxActionpoints, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPPermOffCmd_Test_InvalidAPNumberMaxActionpoints");
    UtTest_Add(LC_SetAPPermOffCmd_Test_InvalidAPNumberAllActionpoints, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPPermOffCmd_Test_InvalidAPNumberAllActionpoints");
    UtTest_Add(LC_SetAPPermOffCmd_Test_APNotDisabled, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPPermOffCmd_Test_APNotDisabled");
    UtTest_Add(LC_SetAPPermOffCmd_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPPermOffCmd_Test_Nominal");
    UtTest_Add(LC_SetAPStateCmd_Test_SetAllActionPointsActiveOneNotUsed, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPStateCmd_Test_SetAllActionPointsActiveOneNotUsed");
    UtTest_Add(LC_SetAPStateCmd_Test_SetAllActionPointsActiveOnePermOff, LC_Test_Setup, LC_Test_TearDown,
               "LC_SetAPStateCmd_Test_SetAllActionPointsActiveOnePermOff");

    UtTest_Add(LC_ResetAPStatsCmd_Test_AllActionPoints, LC_Test_Setup, LC_Test_TearDown,
               "LC_ResetAPStatsCmd_Test_AllActionPoints");
    UtTest_Add(LC_ResetAPStatsCmd_Test_SingleActionPoint, LC_Test_Setup, LC_Test_TearDown,
               "LC_ResetAPStatsCmd_Test_SingleActionPoint");
    UtTest_Add(LC_ResetAPStatsCmd_Test_InvalidAPNumber, LC_Test_Setup, LC_Test_TearDown,
               "LC_ResetAPStatsCmd_Test_InvalidAPNumber");
    UtTest_Add(LC_ResetResultsAP_Test, LC_Test_Setup, LC_Test_TearDown, "LC_ResetResultsAP_Test");

    UtTest_Add(LC_ResetWPStatsCmd_Test_AllWatchPoints, LC_Test_Setup, LC_Test_TearDown,
               "LC_ResetWPStatsCmd_Test_AllWatchPoints");
    UtTest_Add(LC_ResetWPStatsCmd_Test_SingleWatchPoint, LC_Test_Setup, LC_Test_TearDown,
               "LC_ResetWPStatsCmd_Test_SingleWatchPoint");
    UtTest_Add(LC_ResetWPStatsCmd_Test_InvalidWPNumber, LC_Test_Setup, LC_Test_TearDown,
               "LC_ResetWPStatsCmd_Test_InvalidWPNumber");

    UtTest_Add(LC_ResetResultsWP_Test, LC_Test_Setup, LC_Test_TearDown, "LC_ResetResultsWP_Test");
}
