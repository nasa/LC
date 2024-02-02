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
 *   This file contains unit test cases for the functions contained in the file lc_dispatch.c
 */

/*
 * Includes
 */

#include "lc_cmds.h"
#include "lc_app.h"
#include "lc_dispatch.h"
#include "lc_msg.h"
#include "lc_msgdefs.h"
#include "lc_msgids.h"
#include "lc_eventids.h"
#include "lc_utils.h"
#include "lc_watch.h"

#include "lc_test_utils.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

#include <unistd.h>
#include <stdlib.h>

/*
 * Helper functions
 */
static void LC_Dispatch_Test_SetupMsg(CFE_SB_MsgId_t MsgId, CFE_MSG_FcnCode_t FcnCode, size_t MsgSize)
{
    /* Note some paths get the MsgId/FcnCode multiple times, so register accordingly, just in case */
    CFE_SB_MsgId_t    RegMsgId[2]   = {MsgId, MsgId};
    CFE_MSG_FcnCode_t RegFcnCode[2] = {FcnCode, FcnCode};
    size_t            RegMsgSize[2] = {MsgSize, MsgSize};

    UT_ResetState(UT_KEY(CFE_MSG_GetMsgId));
    UT_ResetState(UT_KEY(CFE_MSG_GetFcnCode));
    UT_ResetState(UT_KEY(CFE_MSG_GetSize));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), RegMsgId, sizeof(RegMsgId), true);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), RegFcnCode, sizeof(RegFcnCode), true);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), RegMsgSize, sizeof(RegMsgSize), true);
}

void LC_AppPipe_Test_SampleAPRequest(void)
{
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID);

    LC_Dispatch_Test_SetupMsg(TestMsgId, 0, sizeof(LC_SampleAPCmd_t));

    /* Execute the function being tested */
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    UtAssert_STUB_COUNT(LC_SampleAPReq, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* Bad Length */
    LC_Dispatch_Test_SetupMsg(TestMsgId, 0, 1);
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    /* Verify handler NOT called again */
    UtAssert_STUB_COUNT(LC_SampleAPReq, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSAMPLE_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void LC_AppPipe_Test_SendHkCmd(void)
{
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
    LC_Dispatch_Test_SetupMsg(TestMsgId, 0, sizeof(LC_SendHkCmd_t));

    /* Set to prevent unintended error messages in subfunctions */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    UtAssert_STUB_COUNT(LC_SendHkCmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* Bad Length */
    LC_Dispatch_Test_SetupMsg(TestMsgId, 0, 1);
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    /* Verify handler NOT called again */
    UtAssert_STUB_COUNT(LC_SendHkCmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_HKREQ_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void LC_AppPipe_Test_Noop(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    FcnCode   = LC_NOOP_CC;
    LC_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, sizeof(LC_NoopCmd_t));

    /* Execute the function being tested */
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    UtAssert_STUB_COUNT(LC_NoopCmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* Bad Length */
    LC_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, 1);
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    /* Verify handler NOT called again */
    UtAssert_STUB_COUNT(LC_NoopCmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_CMD_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void LC_AppPipe_Test_Reset(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    FcnCode   = LC_RESET_CC;
    LC_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, sizeof(LC_ResetCountersCmd_t));

    /* Execute the function being tested */
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    UtAssert_STUB_COUNT(LC_ResetCountersCmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* Bad Length */
    LC_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, 1);
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    /* Verify handler NOT called again */
    UtAssert_STUB_COUNT(LC_ResetCountersCmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_CMD_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void LC_AppPipe_Test_SetLCState(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    FcnCode   = LC_SET_LC_STATE_CC;
    LC_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, sizeof(LC_SetLCStateCmd_t));

    /* Execute the function being tested */
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    UtAssert_STUB_COUNT(LC_SetLCStateCmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* Bad Length */
    LC_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, 1);
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    /* Verify handler NOT called again */
    UtAssert_STUB_COUNT(LC_SetLCStateCmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_CMD_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void LC_AppPipe_Test_SetAPState(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    FcnCode   = LC_SET_AP_STATE_CC;
    LC_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, sizeof(LC_SetAPStateCmd_t));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    UtAssert_STUB_COUNT(LC_SetAPStateCmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* Bad Length */
    LC_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, 1);
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    /* Verify handler NOT called again */
    UtAssert_STUB_COUNT(LC_SetAPStateCmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_CMD_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void LC_AppPipe_Test_SetAPPermOff(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    FcnCode   = LC_SET_AP_PERM_OFF_CC;
    LC_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, sizeof(LC_SetAPPermOffCmd_t));

    /* Execute the function being tested */
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    UtAssert_STUB_COUNT(LC_SetAPPermOffCmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* Bad Length */
    LC_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, 1);
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    /* Verify handler NOT called again */
    UtAssert_STUB_COUNT(LC_SetAPPermOffCmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_CMD_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void LC_AppPipe_Test_ResetAPStats(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    FcnCode   = LC_RESET_AP_STATS_CC;
    LC_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, sizeof(LC_ResetAPStatsCmd_t));

    /* Execute the function being tested */
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    UtAssert_STUB_COUNT(LC_ResetAPStatsCmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* Bad Length */
    LC_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, 1);
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    /* Verify handler NOT called again */
    UtAssert_STUB_COUNT(LC_ResetAPStatsCmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_CMD_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void LC_AppPipe_Test_ResetWPStats(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    FcnCode   = LC_RESET_WP_STATS_CC;
    LC_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, sizeof(LC_ResetWPStatsCmd_t));

    /* Execute the function being tested */
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    UtAssert_STUB_COUNT(LC_ResetWPStatsCmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* Bad Length */
    LC_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, 1);
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    /* Verify handler NOT called again */
    UtAssert_STUB_COUNT(LC_ResetWPStatsCmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_CMD_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void LC_AppPipe_Test_InvalidCommandCode(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    TestMsgId = CFE_SB_ValueToMsgId(LC_CMD_MID);
    FcnCode   = 99;
    LC_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, 0);

    /* Execute the function being tested */
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_CC_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void LC_AppPipe_Test_MonitorPacket(void)
{
    CFE_SB_MsgId_t TestMsgId;

    TestMsgId = LC_UT_MID_1;
    LC_Dispatch_Test_SetupMsg(TestMsgId, 0, 0);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    /* Execute the function being tested */
    UtAssert_INT32_EQ(LC_AppPipe(&UT_CmdBuf.Buf), CFE_SUCCESS);
    UtAssert_STUB_COUNT(LC_CheckMsgForWPs, 1);
}

void UtTest_Setup(void)
{
    UtTest_Add(LC_AppPipe_Test_SampleAPRequest, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_SampleAPRequest");
    UtTest_Add(LC_AppPipe_Test_SendHkCmd, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_SendHkCmd");
    UtTest_Add(LC_AppPipe_Test_Noop, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_Noop");
    UtTest_Add(LC_AppPipe_Test_Reset, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_Reset");
    UtTest_Add(LC_AppPipe_Test_SetLCState, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_SetLCState");
    UtTest_Add(LC_AppPipe_Test_SetAPState, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_SetAPState");
    UtTest_Add(LC_AppPipe_Test_SetAPPermOff, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_SetAPPermOff");
    UtTest_Add(LC_AppPipe_Test_ResetAPStats, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_ResetAPStats");
    UtTest_Add(LC_AppPipe_Test_ResetWPStats, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_ResetWPStats");
    UtTest_Add(LC_AppPipe_Test_InvalidCommandCode, LC_Test_Setup, LC_Test_TearDown,
               "LC_AppPipe_Test_InvalidCommandCode");
    UtTest_Add(LC_AppPipe_Test_MonitorPacket, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_MonitorPacket");
}
