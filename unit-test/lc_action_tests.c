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

#include "lc_action.h"
#include "lc_app.h"
#include "lc_msg.h"
#include "lc_msgdefs.h"
#include "lc_msgids.h"
#include "lc_eventids.h"
#include "lc_version.h"
#include "lc_test_utils.h"
#include "lc_utils.h"
#include "lc_custom.h"
#include "lc_platform_cfg.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

#include <unistd.h>
#include <stdlib.h>

uint8 call_count_CFE_EVS_SendEvent;

void LC_SampleAPs_Test_SingleActionPointNominal(void)
{
    uint16 StartIndex = 0;
    uint16 EndIndex   = 0;

    LC_OperData.ARTPtr[StartIndex].CurrentState = 99;

    /* Execute the function being tested */
    LC_SampleAPs(StartIndex, EndIndex);

    /* Verify results */
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_SampleAPs_Test_SingleActionPointError(void)
{
    uint16 StartIndex = 0;
    uint16 EndIndex   = 0;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Sample AP error, invalid current AP state: AP = %%d, State = %%d");

    LC_OperData.ARTPtr[StartIndex].CurrentState = LC_APSTATE_NOT_USED;

    /* Execute the function being tested */
    LC_SampleAPs(StartIndex, EndIndex);

    /* Verify results */
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSAMPLE_CURR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SampleAPs_Test_MultiActionPointNominal(void)
{
    uint16 StartIndex = 0;
    uint16 EndIndex   = 1;

    /* Execute the function being tested */
    LC_SampleAPs(StartIndex, EndIndex);

    /* Verify results */
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_SampleAPs_Test_SingleActionPointPermOff(void)
{
    uint16 StartIndex = 0;
    uint16 EndIndex   = 0;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Sample AP error, invalid current AP state: AP = %%d, State = %%d");

    LC_OperData.ARTPtr[StartIndex].CurrentState = LC_APSTATE_PERMOFF;

    /* Execute the function being tested */
    LC_SampleAPs(StartIndex, EndIndex);

    /* Verify results */
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APSAMPLE_CURR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SampleSingleAP_Test_StateChangePassToFail(void)
{
    uint16 APNumber = 0;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "AP state change from PASS to FAIL: AP = %%d");

    LC_OperData.ARTPtr[APNumber].CurrentState            = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult            = LC_ACTION_PASS;
    LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent = 0;
    LC_OperData.ADTPtr[APNumber].MaxPassFailEvents       = 100;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);

    /* Verify results */
    UtAssert_True(LC_AppData.APSampleCount == 1, "LC_AppData.APSampleCount == 1");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_FAIL,
                  "LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_FAIL");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 1,
                  "LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 1");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].CumulativeFailCount == 1,
                  "LC_OperData.ARTPtr[APNumber].CumulativeFailCount == 1");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].PassToFailCount == 1,
                  "LC_OperData.ARTPtr[APNumber].PassToFailCount == 1");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 1,
                  "LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 1");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_AP_PASSTOFAIL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    /* Generates 1 event message we don't care about in this test */
}

void LC_SampleSingleAP_Test_StateChangePassToFailMaxChange(void)
{
    uint16 APNumber = 0;

    LC_OperData.ARTPtr[APNumber].CurrentState            = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult            = LC_ACTION_PASS;
    LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent = 0;
    LC_OperData.ARTPtr[APNumber].PassToFailCount         = 11;
    LC_OperData.ADTPtr[APNumber].MaxPassFailEvents       = 10;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.ADTPtr[APNumber].MaxFailsBeforeRTS    = 5;
    LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount = 0;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);

    /* Verify results */
    UtAssert_True(LC_AppData.APSampleCount == 1, "LC_AppData.APSampleCount == 1");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_FAIL,
                  "LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_FAIL");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 1,
                  "LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 1");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].CumulativeFailCount == 1,
                  "LC_OperData.ARTPtr[APNumber].CumulativeFailCount == 1");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].PassToFailCount == 12,
                  "LC_OperData.ARTPtr[APNumber].PassToFailCount == 12");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 0,
                  "LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 0");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);

    /* Generates 1 event message we don't care about in this test */

    UtAssert_True(LC_AppData.RTSExecCount == 0, "LC_AppData.RTSExecCount == 0");
}

void LC_SampleSingleAP_Test_ActiveRequestRTS(void)
{
    uint16 APNumber = 0;

    LC_OperData.ARTPtr[APNumber].CurrentState            = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult            = LC_ACTION_FAIL;
    LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent = 0;
    LC_OperData.ADTPtr[APNumber].MaxPassFailEvents       = 100;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;

    LC_OperData.ADTPtr[APNumber].EventID   = 1;
    LC_OperData.ADTPtr[APNumber].EventType = 2;

    LC_AppData.CurrentLCState = LC_STATE_ACTIVE;

    LC_OperData.ADTPtr[APNumber].MaxFailsBeforeRTS    = 5;
    LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount = LC_OperData.ADTPtr[APNumber].MaxFailsBeforeRTS;
    strncpy(LC_OperData.ADTPtr[APNumber].EventText, "Event Message", sizeof(LC_OperData.ADTPtr[APNumber].EventText));

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);

    /* Verify results */
    UtAssert_True(LC_OperData.ARTPtr[APNumber].CurrentState == LC_APSTATE_PASSIVE,
                  "LC_OperData.ARTPtr[APNumber].CurrentState == LC_APSTATE_PASSIVE");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].CumulativeRTSExecCount == 1,
                  "LC_OperData.ARTPtr[APNumber].CumulativeRTSExecCount == 1");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 1,
                  "LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 1");
    UtAssert_True(LC_AppData.RTSExecCount == 1, "LC_AppData.RTSExecCount == 1");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_OperData.ADTPtr[APNumber].EventID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, LC_OperData.ADTPtr[APNumber].EventType);
}

void LC_SampleSingleAP_Test_APFailWhileLCStatePassive(void)
{
    uint16 APNumber = 0;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "AP failed while LC App passive: AP = %%d, FailCount = %%d, RTS = %%d");

    LC_OperData.ARTPtr[APNumber].CurrentState      = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult      = LC_ACTION_FAIL;
    LC_OperData.ADTPtr[APNumber].MaxPassFailEvents = 100;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;

    LC_OperData.ADTPtr[APNumber].EventID   = 1;
    LC_OperData.ADTPtr[APNumber].EventType = 2;

    LC_AppData.CurrentLCState = LC_STATE_PASSIVE;

    LC_OperData.ADTPtr[APNumber].MaxFailsBeforeRTS    = 5;
    LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount = LC_OperData.ADTPtr[APNumber].MaxFailsBeforeRTS;
    strncpy(LC_OperData.ADTPtr[APNumber].EventText, "Event Message", sizeof(LC_OperData.ADTPtr[APNumber].EventText));

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);

    /* Verify results */
    UtAssert_True(LC_OperData.ARTPtr[APNumber].CurrentState == LC_APSTATE_PASSIVE,
                  "LC_OperData.ARTPtr[APNumber].CurrentState == LC_APSTATE_PASSIVE");
    UtAssert_True(LC_AppData.PassiveRTSExecCount == 1, "LC_AppData.PassiveRTSExecCount == 1");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_PASSIVE_FAIL_DBG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_DEBUG);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SampleSingleAP_Test_APFailWhilePassive(void)
{
    uint16 APNumber = 0;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "AP failed while passive: AP = %%d, FailCount = %%d, RTS = %%d");

    LC_AppData.CurrentLCState                            = LC_STATE_ACTIVE;
    LC_OperData.ARTPtr[APNumber].CurrentState            = LC_APSTATE_PASSIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult            = LC_ACTION_FAIL;
    LC_OperData.ADTPtr[APNumber].MaxPassFailEvents       = 100;
    LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;

    LC_OperData.ADTPtr[APNumber].EventID   = 1;
    LC_OperData.ADTPtr[APNumber].EventType = 2;

    LC_OperData.ADTPtr[APNumber].MaxFailsBeforeRTS    = 5;
    LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount = LC_OperData.ADTPtr[APNumber].MaxFailsBeforeRTS;
    strncpy(LC_OperData.ADTPtr[APNumber].EventText, "Event Message", sizeof(LC_OperData.ADTPtr[APNumber].EventText));

    LC_OperData.ADTPtr[APNumber].MaxPassiveEvents = 5;
    LC_OperData.ARTPtr[APNumber].PassiveAPCount   = LC_OperData.ADTPtr[APNumber].MaxPassiveEvents - 1;

    LC_AppData.PassiveRTSExecCount = 0;

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);

    /* Verify results */
    UtAssert_True(LC_OperData.ARTPtr[APNumber].PassiveAPCount == 5, "LC_OperData.ARTPtr[APNumber].PassiveAPCount == 5");
    UtAssert_True(LC_AppData.PassiveRTSExecCount == 1, "LC_OperData.ARTPtr[APNumber].PassiveAPCount == 1");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 1,
                  "LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 1");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_AP_PASSIVE_FAIL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SampleSingleAP_Test_APFailWhilePassiveNoEvent(void)
{
    uint16 APNumber = 0;

    LC_AppData.CurrentLCState                            = LC_STATE_ACTIVE;
    LC_OperData.ARTPtr[APNumber].CurrentState            = LC_APSTATE_PASSIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult            = LC_ACTION_FAIL;
    LC_OperData.ADTPtr[APNumber].MaxPassFailEvents       = 100;
    LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;

    LC_OperData.ADTPtr[APNumber].EventID   = 1;
    LC_OperData.ADTPtr[APNumber].EventType = 2;

    LC_OperData.ADTPtr[APNumber].MaxFailsBeforeRTS    = 5;
    LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount = LC_OperData.ADTPtr[APNumber].MaxFailsBeforeRTS;
    strncpy(LC_OperData.ADTPtr[APNumber].EventText, "Event Message", sizeof(LC_OperData.ADTPtr[APNumber].EventText));

    LC_OperData.ADTPtr[APNumber].MaxPassiveEvents = 5;
    LC_OperData.ARTPtr[APNumber].PassiveAPCount   = LC_OperData.ADTPtr[APNumber].MaxPassiveEvents + 1;

    LC_AppData.PassiveRTSExecCount = 0;

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);

    /* Verify results */
    UtAssert_True(LC_OperData.ARTPtr[APNumber].PassiveAPCount == 7, "LC_OperData.ARTPtr[APNumber].PassiveAPCount == 7");
    UtAssert_True(LC_AppData.PassiveRTSExecCount == 1, "LC_OperData.ARTPtr[APNumber].PassiveAPCount == 1");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 0,
                  "LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 0");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_SampleSingleAP_Test_StateChangeFailToPass(void)
{
    uint16 APNumber = 0;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "AP state change from FAIL to PASS: AP = %%d");

    LC_OperData.ARTPtr[APNumber].CurrentState            = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult            = LC_ACTION_FAIL;
    LC_OperData.ADTPtr[APNumber].MaxFailPassEvents       = 1;
    LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_FALSE;

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);

    /* Verify results */
    UtAssert_True(LC_AppData.APSampleCount == 1, "LC_AppData.APSampleCount == 1");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_PASS,
                  "LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_PASS");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].FailToPassCount == 1,
                  "LC_OperData.ARTPtr[APNumber].FailToPassCount == 1");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 1,
                  "LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 1");

    UtAssert_True(LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 0,
                  "LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 0");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_AP_FAILTOPASS_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SampleSingleAP_Test_StateChangeFailToPassNoEvent(void)
{
    uint16 APNumber                                      = 0;
    LC_OperData.ARTPtr[APNumber].CurrentState            = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult            = LC_ACTION_FAIL;
    LC_OperData.ADTPtr[APNumber].MaxFailPassEvents       = 1;
    LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.ARTPtr[APNumber].FailToPassCount   = 6;
    LC_OperData.ADTPtr[APNumber].MaxFailPassEvents = 4;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_FALSE;

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);

    /* Verify results */
    UtAssert_True(LC_AppData.APSampleCount == 1, "LC_AppData.APSampleCount == 1");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_PASS,
                  "LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_PASS");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].FailToPassCount == 7,
                  "LC_OperData.ARTPtr[APNumber].FailToPassCount == 7");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 0,
                  "LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 0");

    UtAssert_True(LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 0,
                  "LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 0");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_SampleSingleAP_Test_ActionStale(void)
{
    uint16 APNumber = 0;

    LC_OperData.ARTPtr[APNumber].CurrentState      = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult      = LC_ACTION_FAIL;
    LC_OperData.ADTPtr[APNumber].MaxFailPassEvents = 1;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_STALE;

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);

    /* Verify results */
    UtAssert_True(LC_AppData.APSampleCount == 1, "LC_AppData.APSampleCount == 1");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_STALE,
                  "LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_STALE");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 0,
                  "LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 0");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_SampleSingleAP_Test_ActionError(void)
{
    uint16 APNumber = 0;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "AP evaluated to error: AP = %%d, Result = %%d");

    LC_OperData.ARTPtr[APNumber].CurrentState      = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult      = LC_ACTION_FAIL;
    LC_OperData.ADTPtr[APNumber].MaxFailPassEvents = 1;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_ERROR;

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);

    /* Verify results */
    UtAssert_True(LC_AppData.APSampleCount == 1, "LC_AppData.APSampleCount == 1");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_ERROR,
                  "LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_ERROR");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_ACTION_ERROR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_SampleSingleAP_Test_ConsecutivePass(void)
{
    uint16 APNumber = 0;

    LC_OperData.ARTPtr[APNumber].CurrentState            = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult            = LC_ACTION_PASS;
    LC_OperData.ADTPtr[APNumber].MaxFailPassEvents       = 1;
    LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_FALSE;

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);

    /* Verify results */
    UtAssert_True(LC_AppData.APSampleCount == 1, "LC_AppData.APSampleCount == 1");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_PASS,
                  "LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_PASS");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].FailToPassCount == 0,
                  "LC_OperData.ARTPtr[APNumber].FailToPassCount == 0");
    UtAssert_True(LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 0,
                  "LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 0");

    UtAssert_True(LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 0,
                  "LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 0");
}

void LC_EvaluateRPN_Test_AndWatchFalseOp1(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_AND;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 0;
    LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_FALSE;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_PASS, "Result == LC_ACTION_PASS");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_AndWatchFalseOp2(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_AND;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_FALSE;
    LC_OperData.WRTPtr[1].WatchResult = 0;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_PASS, "Result == LC_ACTION_PASS");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_OrWatchTrueOp1(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_OR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 0;
    LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_TRUE;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_FAIL, "Result == LC_ACTION_FAIL");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_OrWatchTrueOp2(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_OR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;
    LC_OperData.WRTPtr[1].WatchResult = 0;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_FAIL, "Result == LC_ACTION_FAIL");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_AndWatchErrorOp1(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_AND;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_ERROR;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_AndWatchErrorOp2(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_AND;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 99;
    LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_ERROR;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_OrWatchErrorOp1(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_OR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_ERROR;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_OrWatchErrorOp2(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_OR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 99;
    LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_ERROR;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_XorWatchErrorOp1(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_XOR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_ERROR;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_XorWatchErrorOp2(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_XOR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 99;
    LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_ERROR;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_NotWatchError(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_NOT;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_ERROR;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_AndStaleOp1(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_AND;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_STALE;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_STALE, "Result == LC_ACTION_STALE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_AndStaleOp2(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_AND;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 99;
    LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_STALE;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_STALE, "Result == LC_ACTION_STALE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_OrStaleOp1(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_OR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_STALE;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_STALE, "Result == LC_ACTION_STALE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_OrStaleOp2(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_OR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 99;
    LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_STALE;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_STALE, "Result == LC_ACTION_STALE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_XorStaleOp1(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_XOR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_STALE;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_STALE, "Result == LC_ACTION_STALE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_XorStaleOp2(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_XOR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 99;
    LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_STALE;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_STALE, "Result == LC_ACTION_STALE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_NotStale(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_NOT;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_STALE;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_STALE, "Result == LC_ACTION_STALE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_AndNominal(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_AND;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 77;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_FAIL, "Result == LC_ACTION_FAIL");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_OrNominal(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_OR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 77;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_PASS, "Result == LC_ACTION_PASS");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_XorNominal(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_XOR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 77;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_NotNominal(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_NOT;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 77;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_Equal(void)
{
    uint16 APNumber = 0;

    /* Pass */
    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_FALSE;

    UtAssert_UINT32_EQ(LC_EvaluateRPN(APNumber), LC_ACTION_PASS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* Fail */
    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;

    UtAssert_UINT32_EQ(LC_EvaluateRPN(APNumber), LC_ACTION_FAIL);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* LC_WATCH_ERROR */
    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_ERROR;

    UtAssert_UINT32_EQ(LC_EvaluateRPN(APNumber), LC_ACTION_ERROR);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* LC_WATCH_STALE */
    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_STALE;

    UtAssert_UINT32_EQ(LC_EvaluateRPN(APNumber), LC_ACTION_STALE);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* Fail empty stack pointer check */
    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 77;

    UtAssert_UINT32_EQ(LC_EvaluateRPN(APNumber), LC_ACTION_ERROR);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_INVALID_RPN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void LC_EvaluateRPN_Test_WatchpointNumberNominal(void)
{
    uint8  Result;
    uint16 APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 77;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == 77, "Result == 77");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_EvaluateRPN_Test_DefaultIllegalRPN(void)
{
    uint8  Result;
    uint16 APNumber = 0;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "AP has illegal RPN expression: AP = %%d, LastOperand = %%d, StackPtr = %%d");

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = LC_MAX_WATCHPOINTS;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_INVALID_RPN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_EvaluateRPN_Test_EndOfBufferWhenNotDone(void)
{
    uint8  Result;
    uint16 APNumber = 0;
    uint32 i;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "AP has illegal RPN expression: AP = %%d, LastOperand = %%d, StackPtr = %%d");

    for (i = 0; i < (sizeof(LC_OperData.ADTPtr[0].RPNEquation) / sizeof(LC_OperData.ADTPtr[0].RPNEquation[0])); i++)
    {
        LC_OperData.ADTPtr[APNumber].RPNEquation[i] = 0;
    }
    for (i = 0; i < (sizeof(WRTable) / sizeof(WRTable[0])); i++)
    {
        LC_OperData.WRTPtr[i].WatchResult = 77;
    }

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);

    /* Verify results */
    UtAssert_True(Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_INVALID_RPN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_EvaluateRPN_Test_PushPopFail(void)
{
    /* Fail LC_RPN_AND pop 2 */
    LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[0].RPNEquation[1] = LC_RPN_AND;

    UtAssert_UINT32_EQ(LC_EvaluateRPN(0), LC_ACTION_ERROR);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_INVALID_RPN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Fail LC_RPN_AND pop 1 */
    UT_ResetState(0);
    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_RPN_AND;

    UtAssert_UINT32_EQ(LC_EvaluateRPN(0), LC_ACTION_ERROR);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_INVALID_RPN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Fail LC_RPN_OR pop 2 */
    UT_ResetState(0);
    LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[0].RPNEquation[1] = LC_RPN_OR;

    UtAssert_UINT32_EQ(LC_EvaluateRPN(0), LC_ACTION_ERROR);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_INVALID_RPN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Fail LC_RPN_OR pop 1 */
    UT_ResetState(0);
    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_RPN_OR;

    UtAssert_UINT32_EQ(LC_EvaluateRPN(0), LC_ACTION_ERROR);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_INVALID_RPN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Fail LC_RPN_XOR pop 2 */
    UT_ResetState(0);
    LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[0].RPNEquation[1] = LC_RPN_XOR;

    UtAssert_UINT32_EQ(LC_EvaluateRPN(0), LC_ACTION_ERROR);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_INVALID_RPN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Fail LC_RPN_XOR pop 1 */
    UT_ResetState(0);
    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_RPN_XOR;

    UtAssert_UINT32_EQ(LC_EvaluateRPN(0), LC_ACTION_ERROR);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_INVALID_RPN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Fail LC_RPN_NOT pop */
    UT_ResetState(0);
    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_RPN_NOT;

    UtAssert_UINT32_EQ(LC_EvaluateRPN(0), LC_ACTION_ERROR);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_INVALID_RPN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Fail LC_RPN_EQUAL pop */
    UT_ResetState(0);
    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_RPN_EQUAL;

    UtAssert_UINT32_EQ(LC_EvaluateRPN(0), LC_ACTION_ERROR);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_INVALID_RPN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void LC_ValidateADT_Test_ActionNotUsed(void)
{
    uint8  Result;
    uint32 TableIndex;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "ADT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ADTPtr[TableIndex].DefaultState = LC_APSTATE_NOT_USED;
    }

    /* Execute the function being tested */
    Result = LC_ValidateADT(LC_OperData.ADTPtr);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_ADTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_ValidateADT_Test_InvalidDefaultState(void)
{
    uint8  Result;
    uint32 TableIndex;
    int32  strCmpResult;
    char   ExpectedEventString1[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char   ExpectedEventString2[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    snprintf(ExpectedEventString1, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "ADT verify err: AP = %%d, Err = %%d, State = %%d, RTS = %%d, FailCnt = %%d, EvtType = %%d");
    snprintf(ExpectedEventString2, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "ADT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ADTPtr[TableIndex].DefaultState      = 99;
        LC_OperData.ADTPtr[TableIndex].RTSId             = 88;
        LC_OperData.ADTPtr[TableIndex].MaxFailsBeforeRTS = 77;
        LC_OperData.ADTPtr[TableIndex].EventType         = 66;
    }

    /* Execute the function being tested */
    Result = LC_ValidateADT(LC_OperData.ADTPtr);

    /* Verify results */
    UtAssert_True(Result == LC_ADTVAL_ERR_DEFSTATE, "Result == LC_ADTVAL_ERR_DEFSTATE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_ADTVAL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString1, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, LC_ADTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString2, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);
}

void LC_ValidateADT_Test_BadRtsID(void)
{
    uint8  Result;
    uint32 TableIndex;
    int32  strCmpResult;
    char   ExpectedEventString1[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char   ExpectedEventString2[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    snprintf(ExpectedEventString1, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "ADT verify err: AP = %%d, Err = %%d, State = %%d, RTS = %%d, FailCnt = %%d, EvtType = %%d");
    snprintf(ExpectedEventString2, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "ADT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ADTPtr[TableIndex].DefaultState      = LC_APSTATE_ACTIVE;
        LC_OperData.ADTPtr[TableIndex].RTSId             = LC_MAX_VALID_ADT_RTSID + 1;
        LC_OperData.ADTPtr[TableIndex].MaxFailsBeforeRTS = 77;
        LC_OperData.ADTPtr[TableIndex].EventType         = 66;
    }

    /* Execute the function being tested */
    Result = LC_ValidateADT(LC_OperData.ADTPtr);

    /* Verify results */
    UtAssert_True(Result == LC_ADTVAL_ERR_RTSID, "Result == LC_ADTVAL_ERR_RTSID");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_ADTVAL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString1, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, LC_ADTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString2, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);
}

void LC_ValidateADT_Test_BadFailCount(void)
{
    uint8  Result;
    uint32 TableIndex;
    int32  strCmpResult;
    char   ExpectedEventString1[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char   ExpectedEventString2[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    snprintf(ExpectedEventString1, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "ADT verify err: AP = %%d, Err = %%d, State = %%d, RTS = %%d, FailCnt = %%d, EvtType = %%d");
    snprintf(ExpectedEventString2, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "ADT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ADTPtr[TableIndex].DefaultState      = LC_APSTATE_ACTIVE;
        LC_OperData.ADTPtr[TableIndex].RTSId             = 1;
        LC_OperData.ADTPtr[TableIndex].MaxFailsBeforeRTS = 0;
        LC_OperData.ADTPtr[TableIndex].EventType         = 66;
    }

    /* Execute the function being tested */
    Result = LC_ValidateADT(LC_OperData.ADTPtr);

    /* Verify results */
    UtAssert_True(Result == LC_ADTVAL_ERR_FAILCNT, "Result == LC_ADTVAL_ERR_FAILCNT");
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_ADTVAL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString1, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, LC_ADTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString2, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);
}

void LC_ValidateADT_Test_InvalidEventType(void)
{
    uint8  Result;
    uint32 TableIndex;
    int32  strCmpResult;
    char   ExpectedEventString1[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char   ExpectedEventString2[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    snprintf(ExpectedEventString1, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "ADT verify err: AP = %%d, Err = %%d, State = %%d, RTS = %%d, FailCnt = %%d, EvtType = %%d");
    snprintf(ExpectedEventString2, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "ADT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ADTPtr[TableIndex].DefaultState      = LC_APSTATE_ACTIVE;
        LC_OperData.ADTPtr[TableIndex].RTSId             = 1;
        LC_OperData.ADTPtr[TableIndex].MaxFailsBeforeRTS = 88;
        LC_OperData.ADTPtr[TableIndex].EventType         = 99;
    }

    /* Execute the function being tested */
    Result = LC_ValidateADT(LC_OperData.ADTPtr);

    /* Verify results */
    UtAssert_True(Result == LC_ADTVAL_ERR_EVTTYPE, "Result == LC_ADTVAL_ERR_EVTTYPE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_ADTVAL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString1, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, LC_ADTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString2, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);
}

void LC_ValidateADT_Test_ValidateRpnAdtValError(void)
{
    uint8  Result;
    uint32 TableIndex;
    int32  strCmpResult;
    char   ExpectedEventString1[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char   ExpectedEventString2[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    snprintf(ExpectedEventString1, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "ADT verify RPN err: AP = %%d, Index = %%d, StackDepth = %%d");
    snprintf(ExpectedEventString2, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "ADT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ADTPtr[TableIndex].DefaultState      = LC_APSTATE_ACTIVE;
        LC_OperData.ADTPtr[TableIndex].RTSId             = 1;
        LC_OperData.ADTPtr[TableIndex].MaxFailsBeforeRTS = 88;
        LC_OperData.ADTPtr[TableIndex].EventType         = CFE_EVS_EventType_DEBUG;
        LC_OperData.ADTPtr[TableIndex].RPNEquation[0]    = 999;
    }

    /* Execute the function being tested */
    Result = LC_ValidateADT(LC_OperData.ADTPtr);

    /* Verify results */
    UtAssert_True(Result == LC_ADTVAL_ERR_RPN, "Result == LC_ADTVAL_ERR_RPN");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_ADTVAL_RPNERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString1, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, LC_ADTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString2, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);
}

void LC_ValidateADT_Test_ValidateRpnAdtValNoError(void)
{
    uint8  Result;
    uint32 TableIndex;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "ADT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ADTPtr[TableIndex].DefaultState      = LC_APSTATE_ACTIVE;
        LC_OperData.ADTPtr[TableIndex].RTSId             = 1;
        LC_OperData.ADTPtr[TableIndex].MaxFailsBeforeRTS = 88;
        LC_OperData.ADTPtr[TableIndex].EventType         = CFE_EVS_EventType_DEBUG;

        LC_OperData.ADTPtr[TableIndex].RPNEquation[0] = LC_MAX_WATCHPOINTS - 1;
        LC_OperData.ADTPtr[TableIndex].RPNEquation[1] = LC_RPN_NOT;
        LC_OperData.ADTPtr[TableIndex].RPNEquation[2] = LC_RPN_EQUAL;
    }

    /* Execute the function being tested */
    Result = LC_ValidateADT(LC_OperData.ADTPtr);

    /* Verify results */
    UtAssert_True(Result == LC_ADTVAL_ERR_NONE, "Result == LC_ADTVAL_ERR_NONE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_ADTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_ValidateADT_Test_Nominal(void)
{
    uint8  Result;
    uint32 TableIndex;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "ADT verify results: good = %%d, bad = %%d, unused = %%d");

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ADTPtr[TableIndex].DefaultState      = LC_APSTATE_NOT_USED;
        LC_OperData.ADTPtr[TableIndex].RTSId             = 1;
        LC_OperData.ADTPtr[TableIndex].MaxFailsBeforeRTS = 1;
        LC_OperData.ADTPtr[TableIndex].EventType         = CFE_EVS_EventType_DEBUG;

        LC_OperData.ADTPtr[TableIndex].RPNEquation[0] = LC_MAX_WATCHPOINTS - 1;
        LC_OperData.ADTPtr[TableIndex].RPNEquation[1] = LC_RPN_NOT;
        LC_OperData.ADTPtr[TableIndex].RPNEquation[2] = LC_RPN_EQUAL;
    }

    LC_OperData.ADTPtr[0].DefaultState = LC_APSTATE_ACTIVE;
    LC_OperData.ADTPtr[1].DefaultState = LC_APSTATE_PASSIVE;
    LC_OperData.ADTPtr[2].DefaultState = LC_APSTATE_DISABLED;
    LC_OperData.ADTPtr[3].DefaultState = LC_APSTATE_PERMOFF;

    LC_OperData.ADTPtr[0].EventType = CFE_EVS_EventType_DEBUG;
    LC_OperData.ADTPtr[1].EventType = CFE_EVS_EventType_INFORMATION;
    LC_OperData.ADTPtr[2].EventType = CFE_EVS_EventType_ERROR;
    LC_OperData.ADTPtr[3].EventType = CFE_EVS_EventType_CRITICAL;

    /* Execute the function being tested */
    Result = LC_ValidateADT(LC_OperData.ADTPtr);

    /* Verify results */
    UtAssert_True(Result == LC_ADTVAL_ERR_NONE, "Result == LC_ADTVAL_ERR_NONE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_ADTVAL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_ValidateRPN_Test_RpnAnd(void)
{
    uint8 Result;

    int32 IndexValue, StackDepthValue;

    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_MAX_WATCHPOINTS - 1;
    LC_OperData.ADTPtr[0].RPNEquation[1] = LC_MAX_WATCHPOINTS - 2;
    LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_AND;
    LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;

    /* Execute the function being tested */
    Result = LC_ValidateRPN(LC_OperData.ADTPtr[0].RPNEquation, &IndexValue, &StackDepthValue);

    /* Verify results */
    UtAssert_True(Result == LC_ADTVAL_ERR_NONE, "Result == LC_ADTVAL_ERR_NONE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_ValidateRPN_Test_RpnOr(void)
{
    uint8 Result;

    int32 IndexValue, StackDepthValue;

    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_MAX_WATCHPOINTS - 1;
    LC_OperData.ADTPtr[0].RPNEquation[1] = LC_MAX_WATCHPOINTS - 2;
    LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_OR;
    LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;

    /* Execute the function being tested */
    Result = LC_ValidateRPN(LC_OperData.ADTPtr[0].RPNEquation, &IndexValue, &StackDepthValue);

    /* Verify results */
    UtAssert_True(Result == LC_ADTVAL_ERR_NONE, "Result == LC_ADTVAL_ERR_NONE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_ValidateRPN_Test_RpnXor(void)
{
    uint8 Result;

    int32 IndexValue, StackDepthValue;

    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_MAX_WATCHPOINTS - 1;
    LC_OperData.ADTPtr[0].RPNEquation[1] = LC_MAX_WATCHPOINTS - 2;
    LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_XOR;
    LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;

    /* Execute the function being tested */
    Result = LC_ValidateRPN(LC_OperData.ADTPtr[0].RPNEquation, &IndexValue, &StackDepthValue);

    /* Verify results */
    UtAssert_True(Result == LC_ADTVAL_ERR_NONE, "Result == LC_ADTVAL_ERR_NONE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_ValidateRPN_Test_RpnNot(void)
{
    uint8 Result;

    int32 IndexValue, StackDepthValue;

    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_MAX_WATCHPOINTS - 1;
    LC_OperData.ADTPtr[0].RPNEquation[1] = LC_RPN_NOT;
    LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_EQUAL;

    /* Execute the function being tested */
    Result = LC_ValidateRPN(LC_OperData.ADTPtr[0].RPNEquation, &IndexValue, &StackDepthValue);

    /* Verify results */
    UtAssert_True(Result == LC_ADTVAL_ERR_NONE, "Result == LC_ADTVAL_ERR_NONE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_ValidateRPN_Test_StackDepthZero(void)
{
    uint8 Result;

    int32 IndexValue      = 0;
    int32 StackDepthValue = 0;

    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_RPN_NOT;

    /* Execute the function being tested */
    Result = LC_ValidateRPN(LC_OperData.ADTPtr[0].RPNEquation, &IndexValue, &StackDepthValue);

    /* Verify results */
    UtAssert_True(Result == LC_ADTVAL_ERR_RPN, "Result == LC_ADTVAL_ERR_RPN");
    UtAssert_True(IndexValue == 0, "IndexValue == 0");
    UtAssert_True(StackDepthValue == 0, "StackDepthValue == 0");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_ValidateRPN_Test_StackDepthZero2(void)
{
    uint8 Result;

    int32 IndexValue      = 0;
    int32 StackDepthValue = 0;

    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_RPN_EQUAL;

    /* Execute the function being tested */
    Result = LC_ValidateRPN(LC_OperData.ADTPtr[0].RPNEquation, &IndexValue, &StackDepthValue);

    /* Verify results */
    UtAssert_True(Result == LC_ADTVAL_ERR_RPN, "Result == LC_ADTVAL_ERR_RPN");
    UtAssert_True(IndexValue == 0, "IndexValue == 0");
    UtAssert_True(StackDepthValue == 0, "StackDepthValue == 0");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_ValidateRPN_Test_MaxRPNSize(void)
{
    int   i;
    uint8 Result;

    int32 IndexValue      = 0;
    int32 StackDepthValue = 0;

    for (i = 0; i < LC_MAX_RPN_EQU_SIZE; i++)
    {
        LC_OperData.ADTPtr[0].RPNEquation[i] = LC_MAX_WATCHPOINTS - 1;
    }

    /* Execute the function being tested */
    Result = LC_ValidateRPN(LC_OperData.ADTPtr[0].RPNEquation, &IndexValue, &StackDepthValue);

    /* Verify results */
    UtAssert_True(Result == LC_ADTVAL_ERR_RPN, "Result == LC_ADTVAL_ERR_RPN");
    UtAssert_True(IndexValue == LC_MAX_RPN_EQU_SIZE, "IndexValue == LC_MAX_RPN_EQU_SIZE");
    UtAssert_True(StackDepthValue == LC_MAX_RPN_EQU_SIZE, "StackDepthValue == LC_MAX_RPN_EQU_SIZE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_ValidateRPN_Test_InvalidBufferItem(void)
{
    uint8 Result;

    int32 IndexValue      = 0;
    int32 StackDepthValue = 0;

    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_MAX_WATCHPOINTS;

    /* Execute the function being tested */
    Result = LC_ValidateRPN(LC_OperData.ADTPtr[0].RPNEquation, &IndexValue, &StackDepthValue);

    /* Verify results */
    UtAssert_True(Result == LC_ADTVAL_ERR_RPN, "Result == LC_ADTVAL_ERR_RPN");
    UtAssert_True(IndexValue == 0, "IndexValue == 0");
    UtAssert_True(StackDepthValue == 0, "StackDepthValue == 0");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void UtTest_Setup(void)
{
    UtTest_Add(LC_SampleAPs_Test_SingleActionPointNominal, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleAPs_Test_SingleActionPointNominal");
    UtTest_Add(LC_SampleAPs_Test_SingleActionPointError, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleAPs_Test_SingleActionPointError");

    UtTest_Add(LC_SampleAPs_Test_SingleActionPointPermOff, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleAPs_Test_SingleActionPointPermOff");
    UtTest_Add(LC_SampleAPs_Test_MultiActionPointNominal, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleAPs_Test_MultiActionPointNominal");

    UtTest_Add(LC_SampleSingleAP_Test_StateChangePassToFail, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleSingleAP_Test_StateChangePassToFail");

    UtTest_Add(LC_SampleSingleAP_Test_StateChangePassToFailMaxChange, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleSingleAP_Test_StateChangePassToFailMaxChange");
    UtTest_Add(LC_SampleSingleAP_Test_ActiveRequestRTS, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleSingleAP_Test_ActiveRequestRTS");
    UtTest_Add(LC_SampleSingleAP_Test_APFailWhileLCStatePassive, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleSingleAP_Test_APFailWhileLCStatePassive");
    UtTest_Add(LC_SampleSingleAP_Test_APFailWhilePassive, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleSingleAP_Test_APFailWhilePassive");
    UtTest_Add(LC_SampleSingleAP_Test_APFailWhilePassiveNoEvent, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleSingleAP_Test_APFailWhilePassiveNoEvent");
    UtTest_Add(LC_SampleSingleAP_Test_StateChangeFailToPass, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleSingleAP_Test_StateChangeFailToPass");

    UtTest_Add(LC_SampleSingleAP_Test_StateChangeFailToPassNoEvent, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleSingleAP_Test_StateChangeFailToPassNoEvent");
    UtTest_Add(LC_SampleSingleAP_Test_ActionStale, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleSingleAP_Test_ActionStale");
    UtTest_Add(LC_SampleSingleAP_Test_ActionError, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleSingleAP_Test_ActionError");
    UtTest_Add(LC_SampleSingleAP_Test_ConsecutivePass, LC_Test_Setup, LC_Test_TearDown,
               "LC_SampleSingleAP_Test_ConsecutivePass");

    UtTest_Add(LC_EvaluateRPN_Test_AndWatchFalseOp1, LC_Test_Setup, LC_Test_TearDown,
               "LC_EvaluateRPN_Test_AndWatchFalseOp1");
    UtTest_Add(LC_EvaluateRPN_Test_AndWatchFalseOp2, LC_Test_Setup, LC_Test_TearDown,
               "LC_EvaluateRPN_Test_AndWatchFalseOp2");
    UtTest_Add(LC_EvaluateRPN_Test_OrWatchTrueOp1, LC_Test_Setup, LC_Test_TearDown,
               "LC_EvaluateRPN_Test_OrWatchTrueOp1");
    UtTest_Add(LC_EvaluateRPN_Test_OrWatchTrueOp2, LC_Test_Setup, LC_Test_TearDown,
               "LC_EvaluateRPN_Test_OrWatchTrueOp2");
    UtTest_Add(LC_EvaluateRPN_Test_AndWatchErrorOp1, LC_Test_Setup, LC_Test_TearDown,
               "LC_EvaluateRPN_Test_AndWatchErrorOp1");
    UtTest_Add(LC_EvaluateRPN_Test_AndWatchErrorOp2, LC_Test_Setup, LC_Test_TearDown,
               "LC_EvaluateRPN_Test_AndWatchErrorOp2");
    UtTest_Add(LC_EvaluateRPN_Test_OrWatchErrorOp1, LC_Test_Setup, LC_Test_TearDown,
               "LC_EvaluateRPN_Test_OrWatchErrorOp1");
    UtTest_Add(LC_EvaluateRPN_Test_OrWatchErrorOp2, LC_Test_Setup, LC_Test_TearDown,
               "LC_EvaluateRPN_Test_OrWatchErrorOp2");
    UtTest_Add(LC_EvaluateRPN_Test_XorWatchErrorOp1, LC_Test_Setup, LC_Test_TearDown,
               "LC_EvaluateRPN_Test_XorWatchErrorOp1");
    UtTest_Add(LC_EvaluateRPN_Test_XorWatchErrorOp2, LC_Test_Setup, LC_Test_TearDown,
               "LC_EvaluateRPN_Test_XorWatchErrorOp2");
    UtTest_Add(LC_EvaluateRPN_Test_NotWatchError, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_NotWatchError");
    UtTest_Add(LC_EvaluateRPN_Test_AndStaleOp1, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_AndStaleOp1");
    UtTest_Add(LC_EvaluateRPN_Test_AndStaleOp2, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_AndStaleOp2");
    UtTest_Add(LC_EvaluateRPN_Test_OrStaleOp1, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_OrStaleOp1");
    UtTest_Add(LC_EvaluateRPN_Test_OrStaleOp2, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_OrStaleOp2");
    UtTest_Add(LC_EvaluateRPN_Test_XorStaleOp1, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_XorStaleOp1");
    UtTest_Add(LC_EvaluateRPN_Test_XorStaleOp2, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_XorStaleOp2");
    UtTest_Add(LC_EvaluateRPN_Test_NotStale, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_NotStale");
    UtTest_Add(LC_EvaluateRPN_Test_AndNominal, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_AndNominal");
    UtTest_Add(LC_EvaluateRPN_Test_OrNominal, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_OrNominal");
    UtTest_Add(LC_EvaluateRPN_Test_XorNominal, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_XorNominal");
    UtTest_Add(LC_EvaluateRPN_Test_NotNominal, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_NotNominal");
    UtTest_Add(LC_EvaluateRPN_Test_Equal, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_Equal");
    UtTest_Add(LC_EvaluateRPN_Test_WatchpointNumberNominal, LC_Test_Setup, LC_Test_TearDown,
               "LC_EvaluateRPN_Test_WatchpointNumberNominal");
    UtTest_Add(LC_EvaluateRPN_Test_DefaultIllegalRPN, LC_Test_Setup, LC_Test_TearDown,
               "LC_EvaluateRPN_Test_DefaultIllegalRPN");
    UtTest_Add(LC_EvaluateRPN_Test_EndOfBufferWhenNotDone, LC_Test_Setup, LC_Test_TearDown,
               "LC_EvaluateRPN_Test_EndOfBufferWhenNotDone");
    UtTest_Add(LC_EvaluateRPN_Test_PushPopFail, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_PushPopFail");

    UtTest_Add(LC_ValidateADT_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateADT_Test_Nominal");
    UtTest_Add(LC_ValidateADT_Test_ActionNotUsed, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateADT_Test_ActionNotUsed");
    UtTest_Add(LC_ValidateADT_Test_InvalidDefaultState, LC_Test_Setup, LC_Test_TearDown,
               "LC_ValidateADT_Test_InvalidDefaultState");
    UtTest_Add(LC_ValidateADT_Test_BadRtsID, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateADT_Test_BadRtsID");
    UtTest_Add(LC_ValidateADT_Test_BadFailCount, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateADT_Test_BadFailCount");
    UtTest_Add(LC_ValidateADT_Test_InvalidEventType, LC_Test_Setup, LC_Test_TearDown,
               "LC_ValidateADT_Test_InvalidEventType");
    UtTest_Add(LC_ValidateADT_Test_ValidateRpnAdtValError, LC_Test_Setup, LC_Test_TearDown,
               "LC_ValidateADT_Test_ValidateRpnAdtValError");
    UtTest_Add(LC_ValidateADT_Test_ValidateRpnAdtValNoError, LC_Test_Setup, LC_Test_TearDown,
               "LC_ValidateADT_Test_ValidateRpnAdtValNoError");

    UtTest_Add(LC_ValidateRPN_Test_RpnAnd, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateRPN_Test_RpnAnd");
    UtTest_Add(LC_ValidateRPN_Test_RpnOr, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateRPN_Test_RpnOr");
    UtTest_Add(LC_ValidateRPN_Test_RpnXor, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateRPN_Test_RpnXor");
    UtTest_Add(LC_ValidateRPN_Test_RpnNot, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateRPN_Test_RpnNot");
    UtTest_Add(LC_ValidateRPN_Test_StackDepthZero, LC_Test_Setup, LC_Test_TearDown,
               "LC_ValidateRPN_Test_StackDepthZero");
    UtTest_Add(LC_ValidateRPN_Test_StackDepthZero2, LC_Test_Setup, LC_Test_TearDown,
               "LC_ValidateRPN_Test_StackDepthZero2");
    UtTest_Add(LC_ValidateRPN_Test_MaxRPNSize, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateRPN_Test_MaxRPNSize");

    UtTest_Add(LC_ValidateRPN_Test_InvalidBufferItem, LC_Test_Setup, LC_Test_TearDown,
               "LC_ValidateRPN_Test_InvalidBufferItem");
}