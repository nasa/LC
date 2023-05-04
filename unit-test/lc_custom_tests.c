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

#include "lc_custom.h"
#include "lc_tbldefs.h"
#include "lc_app.h"
#include "lc_msg.h"
#include "lc_msgdefs.h"
#include "lc_msgids.h"
#include "lc_eventids.h"
#include "lc_version.h"
#include "lc_test_utils.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

#include <unistd.h>
#include <stdlib.h>

uint8 call_count_CFE_EVS_SendEvent;

void LC_ExecuteRTS_Test(void)
{
    uint16 RTSId = 1;

    /* Execute the function being tested */
    LC_ExecuteRTS(RTSId);

    /* Verify results */
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);
}

void LC_CustomFunction_Test_WatchIndex0(void)
{
    uint8  Result;
    uint16 WatchIndex = 0;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Unexpected LC_CustomFunction call: WP = %%d");

    /* Execute the function being tested */
    Result = LC_CustomFunction(WatchIndex, 0, 0, 0);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_FALSE, "Result == LC_WATCH_FALSE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_CFCALL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_CustomFunction_Test_WatchIndex1(void)
{
    uint8  Result;
    uint16 WatchIndex = 1;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Unexpected LC_CustomFunction call: WP = %%d");

    /* Execute the function being tested */
    Result = LC_CustomFunction(WatchIndex, 0, 0, 0);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_FALSE, "Result == LC_WATCH_FALSE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_CFCALL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void UtTest_Setup(void)
{
    UtTest_Add(LC_ExecuteRTS_Test, LC_Test_Setup, LC_Test_TearDown, "LC_ExecuteRTS_Test");
    UtTest_Add(LC_CustomFunction_Test_WatchIndex0, LC_Test_Setup, LC_Test_TearDown,
               "LC_CustomFunction_Test_WatchIndex0");
    UtTest_Add(LC_CustomFunction_Test_WatchIndex1, LC_Test_Setup, LC_Test_TearDown,
               "LC_CustomFunction_Test_WatchIndex1");
}