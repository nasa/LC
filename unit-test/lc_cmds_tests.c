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
#include "lc_events.h"
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

void LC_AppPipe_Test_SampleAPRequest(void)
{
    int32          Result;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);
    /* Execute the function being tested */
    Result = LC_AppPipe(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
}

void LC_AppPipe_Test_HousekeepingRequest(void)
{
    int32          Result;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), false);

    /* Set to prevent unintended error messages in subfunctions */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_AppPipe(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);

    uint8 call_count_LC_PerformMaintenance = UT_GetStubCount(UT_KEY(LC_PerformMaintenance));
    UtAssert_INT32_EQ(call_count_LC_PerformMaintenance, 1);
    /* Generates 1 event message we don't care about in this test */
}

void LC_AppPipe_Test_Noop(void)
{
    int32             Result;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "No-op command: Version %%d.%%d.%%d.%%d");

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    FcnCode   = LC_NOOP_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    /* Execute the function being tested */
    Result = LC_AppPipe(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    /* Note: this event message is generated in a subfunction, but is tested anyway to verify that the correct code path
     * has been reached */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_NOOP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_AppPipe_Test_Reset(void)
{
    int32             Result;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Reset counters command");

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    FcnCode   = LC_RESET_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    /* Execute the function being tested */
    Result = LC_AppPipe(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    /* Note: this event message is generated in a subfunction, but is tested anyway to verify that the correct code path
     * has been reached */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_RESET_DBG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_DEBUG);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_AppPipe_Test_SetLCState(void)
{
    int32             Result;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    FcnCode   = LC_SET_LC_STATE_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    /* Execute the function being tested */
    Result = LC_AppPipe(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);

    /* Generates 1 event message we don't care about in this test */
}

void LC_AppPipe_Test_SetAPState(void)
{
    int32             Result;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    FcnCode   = LC_SET_AP_STATE_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    /* Execute the function being tested */
    Result = LC_AppPipe(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);

    /* Generates 1 event message we don't care about in this test */
}

void LC_AppPipe_Test_SetAPPermoff(void)
{
    int32             Result;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    FcnCode   = LC_SET_AP_PERM_OFF_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    /* Execute the function being tested */
    Result = LC_AppPipe(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);

    /* Generates 1 event message we don't care about in this test */
}

void LC_AppPipe_Test_ResetAPStats(void)
{
    int32             Result;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    FcnCode   = LC_RESET_AP_STATS_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    /* Execute the function being tested */
    Result = LC_AppPipe(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    /* Generates 1 event message we don't care about in this test */
}

void LC_AppPipe_Test_ResetWPStats(void)
{
    int32             Result;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    FcnCode   = LC_RESET_WP_STATS_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    /* Execute the function being tested */
    Result = LC_AppPipe(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    /* Generates 1 event message we don't care about in this test */
}

void LC_AppPipe_Test_InvalidCommandCode(void)
{
    int32             Result;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    FcnCode   = 99;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Invalid command code: ID = 0x%%08lX, CC = %%d");

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    /* Execute the function being tested */
    Result = LC_AppPipe(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_CC_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_AppPipe_Test_MonitorPacket(void)
{
    int32          Result;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = LC_UT_MID_1;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    /* Execute the function being tested */
    Result = LC_AppPipe(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);

    uint8 call_count_LC_CheckMsgForWPs = UT_GetStubCount(UT_KEY(LC_CheckMsgForWPs));
    UtAssert_INT32_EQ(call_count_LC_CheckMsgForWPs, 1);
}

void LC_SampleAPReq_Test_BadLength(void)
{
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), false);

    LC_SampleAPReq(&UT_CmdBuf.Buf);
}

void LC_SampleAPReq_Test_StateDisabled(void)
{
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    LC_AppData.CurrentLCState = LC_STATE_DISABLED;

    LC_SampleAPReq(&UT_CmdBuf.Buf);
}

void LC_SampleAPReq_Test_AllowSampleAllWatchStale(void)
{
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    LC_AppData.CurrentLCState              = 99;
    UT_CmdBuf.SampleAPCmd.StartIndex       = LC_ALL_ACTIONPOINTS;
    UT_CmdBuf.SampleAPCmd.EndIndex         = LC_ALL_ACTIONPOINTS;
    UT_CmdBuf.SampleAPCmd.UpdateAge        = 1;
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

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    LC_AppData.CurrentLCState              = 99;
    UT_CmdBuf.SampleAPCmd.StartIndex       = LC_ALL_ACTIONPOINTS;
    UT_CmdBuf.SampleAPCmd.EndIndex         = LC_ALL_ACTIONPOINTS;
    UT_CmdBuf.SampleAPCmd.UpdateAge        = 1;
    LC_OperData.WRTPtr[0].CountdownToStale = 2;

    /* Execute the function being tested */
    LC_SampleAPReq(&UT_CmdBuf.Buf);

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SampleAPReq_Test_StartLessOrEqualToEndAndEndWithinArrayWatchStale(void)
{
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    LC_AppData.CurrentLCState              = 99;
    UT_CmdBuf.SampleAPCmd.StartIndex       = LC_MAX_ACTIONPOINTS - 1;
    UT_CmdBuf.SampleAPCmd.EndIndex         = LC_MAX_ACTIONPOINTS - 1;
    UT_CmdBuf.SampleAPCmd.UpdateAge        = 1;
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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    LC_AppData.CurrentLCState        = 99;
    UT_CmdBuf.SampleAPCmd.StartIndex = 2;
    UT_CmdBuf.SampleAPCmd.EndIndex   = 1;

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Sample AP error: invalid AP number, start = %%d, end = %%d");

    LC_AppData.CurrentLCState              = 99;
    UT_CmdBuf.SampleAPCmd.StartIndex       = LC_ALL_ACTIONPOINTS;
    UT_CmdBuf.SampleAPCmd.EndIndex         = 1;
    UT_CmdBuf.SampleAPCmd.UpdateAge        = 1;
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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Sample AP error: invalid AP number, start = %%d, end = %%d");

    LC_AppData.CurrentLCState              = 99;
    UT_CmdBuf.SampleAPCmd.StartIndex       = 1;
    UT_CmdBuf.SampleAPCmd.EndIndex         = LC_MAX_ACTIONPOINTS;
    UT_CmdBuf.SampleAPCmd.UpdateAge        = 1;
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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    LC_AppData.CurrentLCState              = 99;
    UT_CmdBuf.SampleAPCmd.StartIndex       = LC_ALL_ACTIONPOINTS;
    UT_CmdBuf.SampleAPCmd.EndIndex         = LC_ALL_ACTIONPOINTS;
    UT_CmdBuf.SampleAPCmd.UpdateAge        = 0;
    LC_OperData.WRTPtr[0].CountdownToStale = 1;

    /* Execute the function being tested */
    LC_SampleAPReq(&UT_CmdBuf.Buf);

    /* Verify results */

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_HousekeepingReq_Test_WatchStale(void)
{
    int32          Result;
    uint16         TableIndex;
    uint16         HKIndex;
    uint8          ExpectedByteData;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WRTPtr[TableIndex].WatchResult = LC_WATCH_STALE;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKWR_STALE << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_STALE << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_STALE << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKWR_STALE);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_WATCHPOINTS / 2) / 4;
    UtAssert_True(LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_WATCHPOINTS - 1) / 4;
    UtAssert_True(LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_HousekeepingReq_Test_WatchFalse(void)
{
    int32          Result;
    uint16         TableIndex;
    uint16         HKIndex;
    uint8          ExpectedByteData;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WRTPtr[TableIndex].WatchResult = LC_WATCH_FALSE;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKWR_FALSE << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_FALSE << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_FALSE << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKWR_FALSE);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_WATCHPOINTS / 2) / 4;
    UtAssert_True(LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_WATCHPOINTS - 1) / 4;
    UtAssert_True(LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_HousekeepingReq_Test_WatchTrue(void)
{
    int32          Result;
    uint16         TableIndex;
    uint16         HKIndex;
    uint8          ExpectedByteData;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WRTPtr[TableIndex].WatchResult = LC_WATCH_TRUE;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKWR_TRUE << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_TRUE << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_TRUE << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKWR_TRUE);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_WATCHPOINTS / 2) / 4;
    UtAssert_True(LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_WATCHPOINTS - 1) / 4;
    UtAssert_True(LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_HousekeepingReq_Test_WatchError(void)
{
    int32  Result;
    uint16 TableIndex;
    uint16 HKIndex;
    uint8  ExpectedByteData;

    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WRTPtr[TableIndex].WatchResult = LC_WATCH_ERROR;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKWR_ERROR << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_ERROR << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_ERROR << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKWR_ERROR);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_WATCHPOINTS / 2) / 4;
    UtAssert_True(LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_WATCHPOINTS - 1) / 4;
    UtAssert_True(LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_HousekeepingReq_Test_DefaultWatchResult(void)
{
    int32          Result;
    uint16         TableIndex;
    uint16         HKIndex;
    uint8          ExpectedByteData;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WRTPtr[TableIndex].WatchResult = 99;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKWR_ERROR << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_ERROR << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_ERROR << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKWR_ERROR);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_WATCHPOINTS / 2) / 4;
    UtAssert_True(LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_WATCHPOINTS - 1) / 4;
    UtAssert_True(LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_HousekeepingReq_Test_ActionNotUsedStale(void)
{
    int32          Result;
    uint16         TableIndex;
    uint16         HKIndex;
    uint8          ExpectedByteData;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = LC_APSTATE_NOT_USED;
        LC_OperData.ARTPtr[TableIndex].ActionResult = LC_ACTION_STALE;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_NOT_USED << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STALE << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_NOT_USED << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_STALE);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS / 2) / 2;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_HousekeepingReq_Test_APStateActiveActionPass(void)
{
    int32          Result;
    uint16         TableIndex;
    uint16         HKIndex;
    uint8          ExpectedByteData;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = LC_APSTATE_ACTIVE;
        LC_OperData.ARTPtr[TableIndex].ActionResult = LC_ACTION_PASS;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_ACTIVE << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_PASS << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_ACTIVE << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_PASS);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS / 2) / 2;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    UtAssert_True(LC_OperData.HkPacket.ActiveAPs == LC_MAX_ACTIONPOINTS,
                  "LC_OperData.HkPacket.ActiveAPs = LC_MAX_ACTIONPOINTS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_HousekeepingReq_Test_APStatePassiveActionFail(void)
{
    int32          Result;
    uint16         TableIndex;
    uint16         HKIndex;
    uint8          ExpectedByteData;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = LC_APSTATE_PASSIVE;
        LC_OperData.ARTPtr[TableIndex].ActionResult = LC_ACTION_FAIL;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_PASSIVE << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_FAIL << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_PASSIVE << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_FAIL);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS / 2) / 2;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_HousekeepingReq_Test_APStateDisabledActionError(void)
{
    int32          Result;
    uint16         TableIndex;
    uint16         HKIndex;
    uint8          ExpectedByteData;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = LC_APSTATE_DISABLED;
        LC_OperData.ARTPtr[TableIndex].ActionResult = LC_ACTION_ERROR;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_DISABLED << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_ERROR << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_DISABLED << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_ERROR);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS / 2) / 2;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_HousekeepingReq_Test_APStatePermOffActionError(void)
{
    int32          Result;
    uint16         TableIndex;
    uint16         HKIndex;
    uint8          ExpectedByteData;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = LC_APSTATE_PERMOFF;
        LC_OperData.ARTPtr[TableIndex].ActionResult = LC_ACTION_ERROR;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_NOT_USED << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_ERROR << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_NOT_USED << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_ERROR);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS / 2) / 2;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_HousekeepingReq_Test_DefaultCurrentStateAndActionResult(void)
{
    int32          Result;
    uint16         TableIndex;
    uint16         HKIndex;
    uint8          ExpectedByteData;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = 99;
        LC_OperData.ARTPtr[TableIndex].ActionResult = 99;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_NOT_USED << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_ERROR << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_NOT_USED << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_ERROR);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS / 2) / 2;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True(LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData,
                  "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_HousekeepingReq_Test_ManageTablesError(void)
{
    int32          Result;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_SetDefaultReturnValue(UT_KEY(LC_PerformMaintenance), -1);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    /* Set to satisfy condition "(Result = LC_ManageTables()) != CFE_SUCCESS" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), -1);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq(&UT_CmdBuf.SendHkCmd.CommandHeader);

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_NoopCmd_Test_BadLength(void)
{
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), false);

    LC_NoopCmd(&UT_CmdBuf.Buf);

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_ResetCmd_Test_BadLength(void)
{
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), false);

    LC_ResetCmd(&UT_CmdBuf.Buf);

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
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

void LC_SetLCStateCmd_Test_BadLength(void)
{
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), false);

    LC_SetLCStateCmd(&UT_CmdBuf.Buf);

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_SetLCStateCmd_Test_Active(void)
{
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set LC state command: new state = %%d");

    UT_CmdBuf.SetLCStateCmd.NewLCState = LC_STATE_ACTIVE;

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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set LC state command: new state = %%d");

    UT_CmdBuf.SetLCStateCmd.NewLCState = LC_STATE_PASSIVE;

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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set LC state command: new state = %%d");

    UT_CmdBuf.SetLCStateCmd.NewLCState = LC_STATE_DISABLED;

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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set LC state error: invalid state = %%d");

    UT_CmdBuf.SetLCStateCmd.NewLCState = 99;

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

void LC_SetAPStateCmd_Test_BadLength(void)
{
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), false);

    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_SetAPStateCmd_Test_Default(void)
{
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state error: AP = %%d, Invalid new state = %%d");

    UT_CmdBuf.SetAPStateCmd.APNumber   = 1;
    UT_CmdBuf.SetAPStateCmd.NewAPState = 99;

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
    uint16         TableIndex;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state command: AP = %%d, New state = %%d");

    UT_CmdBuf.SetAPStateCmd.NewAPState = LC_APSTATE_ACTIVE;
    UT_CmdBuf.SetAPStateCmd.APNumber   = LC_ALL_ACTIONPOINTS;

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
    uint16         TableIndex;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state command: AP = %%d, New state = %%d");

    UT_CmdBuf.SetAPStateCmd.NewAPState = LC_APSTATE_ACTIVE;
    UT_CmdBuf.SetAPStateCmd.APNumber   = LC_ALL_ACTIONPOINTS;

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
    uint16         TableIndex;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state command: AP = %%d, New state = %%d");

    UT_CmdBuf.SetAPStateCmd.NewAPState = LC_APSTATE_ACTIVE;
    UT_CmdBuf.SetAPStateCmd.APNumber   = LC_ALL_ACTIONPOINTS;

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
    uint16         TableIndex;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state command: AP = %%d, New state = %%d");

    UT_CmdBuf.SetAPStateCmd.NewAPState = LC_APSTATE_PASSIVE;
    UT_CmdBuf.SetAPStateCmd.APNumber   = LC_ALL_ACTIONPOINTS;

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
    uint16         TableIndex;
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state command: AP = %%d, New state = %%d");

    UT_CmdBuf.SetAPStateCmd.NewAPState = LC_APSTATE_DISABLED;
    UT_CmdBuf.SetAPStateCmd.APNumber   = LC_ALL_ACTIONPOINTS;

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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state command: AP = %%d, New state = %%d");

    UT_CmdBuf.SetAPStateCmd.NewAPState = LC_APSTATE_ACTIVE;
    UT_CmdBuf.SetAPStateCmd.APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState = 99;

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState == LC_APSTATE_ACTIVE,
                  "LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState == LC_APSTATE_ACTIVE");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_UpdateSingleActionPointNotUsed(void)
{
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state error: AP = %%d, Invalid current AP state = %%d");

    UT_CmdBuf.SetAPStateCmd.NewAPState = LC_APSTATE_ACTIVE;
    UT_CmdBuf.SetAPStateCmd.APNumber   = 0;

    LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState = LC_APSTATE_NOT_USED;

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState == LC_APSTATE_NOT_USED,
                  "LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState == LC_APSTATE_NOT_USED");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_CURR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_UpdateSingleActionPointPassive(void)
{
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state command: AP = %%d, New state = %%d");

    UT_CmdBuf.SetAPStateCmd.NewAPState = LC_APSTATE_PASSIVE;
    UT_CmdBuf.SetAPStateCmd.APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState = 99;

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState == LC_APSTATE_PASSIVE,
                  "LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState == LC_APSTATE_PASSIVE");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_UpdateSingleActionPointDisabled(void)
{
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state command: AP = %%d, New state = %%d");

    UT_CmdBuf.SetAPStateCmd.NewAPState = LC_APSTATE_DISABLED;
    UT_CmdBuf.SetAPStateCmd.APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState = 99;

    /* Execute the function being tested */
    LC_SetAPStateCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState == LC_APSTATE_DISABLED,
                  "LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState == LC_APSTATE_DISABLED");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSTATE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SetAPStateCmd_Test_InvalidCurrentAPStateActive(void)
{
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state error: AP = %%d, Invalid current AP state = %%d");

    UT_CmdBuf.SetAPStateCmd.NewAPState = LC_APSTATE_ACTIVE;
    UT_CmdBuf.SetAPStateCmd.APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState = LC_APSTATE_PERMOFF;

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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state error: AP = %%d, Invalid current AP state = %%d");

    UT_CmdBuf.SetAPStateCmd.NewAPState = LC_APSTATE_PASSIVE;
    UT_CmdBuf.SetAPStateCmd.APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState = LC_APSTATE_PERMOFF;

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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP state error: AP = %%d, Invalid current AP state = %%d");

    UT_CmdBuf.SetAPStateCmd.NewAPState = LC_APSTATE_DISABLED;
    UT_CmdBuf.SetAPStateCmd.APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState = LC_APSTATE_PERMOFF;

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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set AP state error: Invalid AP number = %%d");

    UT_CmdBuf.SetAPStateCmd.NewAPState = LC_APSTATE_ACTIVE;
    UT_CmdBuf.SetAPStateCmd.APNumber   = LC_MAX_ACTIONPOINTS;

    LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState = LC_APSTATE_PERMOFF;

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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set AP state error: Invalid AP number = %%d");

    UT_CmdBuf.SetAPStateCmd.NewAPState = LC_APSTATE_PASSIVE;
    UT_CmdBuf.SetAPStateCmd.APNumber   = LC_MAX_ACTIONPOINTS;

    LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState = LC_APSTATE_PERMOFF;

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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set AP state error: Invalid AP number = %%d");

    UT_CmdBuf.SetAPStateCmd.NewAPState = LC_APSTATE_DISABLED;
    UT_CmdBuf.SetAPStateCmd.APNumber   = LC_MAX_ACTIONPOINTS;

    LC_OperData.ARTPtr[UT_CmdBuf.SetAPStateCmd.APNumber].CurrentState = LC_APSTATE_PERMOFF;

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

void LC_SetAPPermOffCmd_Test_BadLength(void)
{
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), false);

    LC_SetAPPermOffCmd(&UT_CmdBuf.Buf);

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_SetAPPermOffCmd_Test_InvalidAPNumberMaxActionpoints(void)
{
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set AP perm off error: Invalid AP number = %%d");

    UT_CmdBuf.SetAPPermOffCmd.APNumber = LC_MAX_ACTIONPOINTS;

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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set AP perm off error: Invalid AP number = %%d");

    UT_CmdBuf.SetAPPermOffCmd.APNumber = LC_ALL_ACTIONPOINTS;

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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set AP perm off error, AP NOT Disabled: AP = %%d, Current state = %%d");

    UT_CmdBuf.SetAPPermOffCmd.APNumber = 1;

    LC_OperData.ARTPtr[UT_CmdBuf.SetAPPermOffCmd.APNumber].CurrentState = 99;

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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Set AP permanently off command: AP = %%d");

    UT_CmdBuf.SetAPPermOffCmd.APNumber = 1;

    LC_OperData.ARTPtr[UT_CmdBuf.SetAPPermOffCmd.APNumber].CurrentState = LC_APSTATE_DISABLED;

    /* Execute the function being tested */
    LC_SetAPPermOffCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(LC_OperData.ARTPtr[UT_CmdBuf.SetAPPermOffCmd.APNumber].CurrentState == 4,
                  "LC_OperData.ARTPtr[UT_CmdBuf.SetAPPermOffCmd.APNumber].CurrentState == 4");
    UtAssert_True(LC_AppData.CmdCount == 1, "LC_AppData.CmdCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APOFF_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_ResetAPStatsCmd_Test_BadLength(void)
{
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), false);

    LC_ResetAPStatsCmd(&UT_CmdBuf.Buf);

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_ResetAPStatsCmd_Test_AllActionPoints(void)
{
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Reset AP stats command: AP = %%d");

    UT_CmdBuf.ResetAPStatsCmd.APNumber = LC_ALL_ACTIONPOINTS;

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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Reset AP stats command: AP = %%d");

    UT_CmdBuf.ResetAPStatsCmd.APNumber = LC_MAX_ACTIONPOINTS - 1;

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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Reset AP stats error: invalid AP number = %%d");

    UT_CmdBuf.ResetAPStatsCmd.APNumber = LC_MAX_ACTIONPOINTS;

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

void LC_ResetWPStatsCmd_Test_BadLength(void)
{
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), false);

    LC_ResetWPStatsCmd(&UT_CmdBuf.Buf);

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_ResetWPStatsCmd_Test_AllWatchPoints(void)
{
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Reset WP stats command: WP = %%d");

    UT_CmdBuf.ResetWPStatsCmd.WPNumber = LC_ALL_WATCHPOINTS;

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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Reset WP stats command: WP = %%d");

    UT_CmdBuf.ResetWPStatsCmd.WPNumber = LC_MAX_WATCHPOINTS - 1;

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
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDefaultReturnValue(UT_KEY(LC_VerifyMsgLength), true);

    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Reset WP stats error: invalid WP number = %%d");

    UT_CmdBuf.ResetWPStatsCmd.WPNumber = LC_MAX_WATCHPOINTS;

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
    UtTest_Add(LC_AppPipe_Test_SampleAPRequest, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_SampleAPRequest");
    UtTest_Add(LC_AppPipe_Test_HousekeepingRequest, LC_Test_Setup, LC_Test_TearDown,
               "LC_AppPipe_Test_HousekeepingRequest");
    UtTest_Add(LC_AppPipe_Test_Noop, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_Noop");
    UtTest_Add(LC_AppPipe_Test_Reset, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_Reset");
    UtTest_Add(LC_AppPipe_Test_SetLCState, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_SetLCState");
    UtTest_Add(LC_AppPipe_Test_SetAPState, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_SetAPState");
    UtTest_Add(LC_AppPipe_Test_SetAPPermoff, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_SetAPPermoff");
    UtTest_Add(LC_AppPipe_Test_ResetAPStats, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_ResetAPStats");
    UtTest_Add(LC_AppPipe_Test_ResetWPStats, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_ResetWPStats");
    UtTest_Add(LC_AppPipe_Test_InvalidCommandCode, LC_Test_Setup, LC_Test_TearDown,
               "LC_AppPipe_Test_InvalidCommandCode");
    UtTest_Add(LC_AppPipe_Test_MonitorPacket, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_MonitorPacket");
    UtTest_Add(LC_SampleAPReq_Test_BadLength, LC_Test_Setup, LC_Test_TearDown, "LC_SampleAPReq_Test_BadLength");
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

    UtTest_Add(LC_HousekeepingReq_Test_WatchStale, LC_Test_Setup, LC_Test_TearDown,
               "LC_HousekeepingReq_Test_WatchStale");
    UtTest_Add(LC_HousekeepingReq_Test_WatchFalse, LC_Test_Setup, LC_Test_TearDown,
               "LC_HousekeepingReq_Test_WatchFalse");
    UtTest_Add(LC_HousekeepingReq_Test_WatchTrue, LC_Test_Setup, LC_Test_TearDown, "LC_HousekeepingReq_Test_WatchTrue");
    UtTest_Add(LC_HousekeepingReq_Test_WatchError, LC_Test_Setup, LC_Test_TearDown,
               "LC_HousekeepingReq_Test_WatchError");
    UtTest_Add(LC_HousekeepingReq_Test_DefaultWatchResult, LC_Test_Setup, LC_Test_TearDown,
               "LC_HousekeepingReq_Test_DefaultWatchResult");
    UtTest_Add(LC_HousekeepingReq_Test_ActionNotUsedStale, LC_Test_Setup, LC_Test_TearDown,
               "LC_HousekeepingReq_Test_ActionNotUsedStale");
    UtTest_Add(LC_HousekeepingReq_Test_APStateActiveActionPass, LC_Test_Setup, LC_Test_TearDown,
               "LC_HousekeepingReq_Test_APStateActiveActionPass");
    UtTest_Add(LC_HousekeepingReq_Test_APStatePassiveActionFail, LC_Test_Setup, LC_Test_TearDown,
               "LC_HousekeepingReq_Test_APStatePassiveActionFail");
    UtTest_Add(LC_HousekeepingReq_Test_APStateDisabledActionError, LC_Test_Setup, LC_Test_TearDown,
               "LC_HousekeepingReq_Test_APStateDisabledActionError");
    UtTest_Add(LC_HousekeepingReq_Test_APStatePermOffActionError, LC_Test_Setup, LC_Test_TearDown,
               "LC_HousekeepingReq_Test_APStatePermOffActionError");
    UtTest_Add(LC_HousekeepingReq_Test_DefaultCurrentStateAndActionResult, LC_Test_Setup, LC_Test_TearDown,
               "LC_HousekeepingReq_Test_DefaultCurrentStateAndActionResult");
    UtTest_Add(LC_HousekeepingReq_Test_ManageTablesError, LC_Test_Setup, LC_Test_TearDown,
               "LC_HousekeepingReq_Test_ManageTablesError");

    UtTest_Add(LC_NoopCmd_Test_BadLength, LC_Test_Setup, LC_Test_TearDown, "LC_NoopCmd_Test_BadLength");
    UtTest_Add(LC_ResetCmd_Test_BadLength, LC_Test_Setup, LC_Test_TearDown, "LC_ResetCmd_Test_BadLength");

    UtTest_Add(LC_ResetCounters_Test, LC_Test_Setup, LC_Test_TearDown, "LC_ResetCounters_Test");

    UtTest_Add(LC_SetLCStateCmd_Test_Active, LC_Test_Setup, LC_Test_TearDown, "LC_SetLCStateCmd_Test_Active");
    UtTest_Add(LC_SetLCStateCmd_Test_Passive, LC_Test_Setup, LC_Test_TearDown, "LC_SetLCStateCmd_Test_Passive");
    UtTest_Add(LC_SetLCStateCmd_Test_Disabled, LC_Test_Setup, LC_Test_TearDown, "LC_SetLCStateCmd_Test_Disabled");
    UtTest_Add(LC_SetLCStateCmd_Test_Default, LC_Test_Setup, LC_Test_TearDown, "LC_SetLCStateCmd_Test_Default");
    UtTest_Add(LC_SetLCStateCmd_Test_BadLength, LC_Test_Setup, LC_Test_TearDown, "LC_SetLCStateCmd_Test_BadLength");

    UtTest_Add(LC_SetAPStateCmd_Test_BadLength, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPStateCmd_Test_BadLength");
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
    UtTest_Add(LC_SetAPPermOffCmd_Test_BadLength, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPPermOffCmd_Test_BadLength");
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
    UtTest_Add(LC_ResetAPStatsCmd_Test_BadLength, LC_Test_Setup, LC_Test_TearDown, "LC_ResetAPStatsCmd_Test_BadLength");
    UtTest_Add(LC_ResetResultsAP_Test, LC_Test_Setup, LC_Test_TearDown, "LC_ResetResultsAP_Test");

    UtTest_Add(LC_ResetWPStatsCmd_Test_AllWatchPoints, LC_Test_Setup, LC_Test_TearDown,
               "LC_ResetWPStatsCmd_Test_AllWatchPoints");
    UtTest_Add(LC_ResetWPStatsCmd_Test_SingleWatchPoint, LC_Test_Setup, LC_Test_TearDown,
               "LC_ResetWPStatsCmd_Test_SingleWatchPoint");
    UtTest_Add(LC_ResetWPStatsCmd_Test_InvalidWPNumber, LC_Test_Setup, LC_Test_TearDown,
               "LC_ResetWPStatsCmd_Test_InvalidWPNumber");

    UtTest_Add(LC_ResetWPStatsCmd_Test_BadLength, LC_Test_Setup, LC_Test_TearDown, "LC_ResetWPStatsCmd_Test_BadLength");
    UtTest_Add(LC_ResetResultsWP_Test, LC_Test_Setup, LC_Test_TearDown, "LC_ResetResultsWP_Test");
}
