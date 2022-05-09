/*************************************************************************
** File:
**   $Id: lc_app_test.c 1.5 2017/01/22 17:24:33EST sstrege Exp  $
**
**  Copyright (c) 2007-2020 United States Government as represented by the
**  Administrator of the National Aeronautics and Space Administration.
**  All Other Rights Reserved.
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be
**   used, distributed and modified only pursuant to the terms of that
**   agreement.
**
** Purpose:
**   This file contains unit test cases for the functions contained in the file lc_app.c
**
** References:
**   Flight Software Branch C Coding Standard Version 1.2
**   CFS Development Standards Document
**
** Notes:
**
*************************************************************************/

/*
 * Includes
 */

#include "lc_custom.h"
#include "lc_app.h"
#include "lc_msg.h"
#include "lc_msgdefs.h"
#include "lc_msgids.h"
#include "lc_events.h"
#include "lc_version.h"
#include "lc_test_utils.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

#include <sys/fcntl.h>
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

} /* end LC_ExecuteRTS_Test */

void LC_CustomFunction_Test_WatchIndex0(void)
{
    uint8  Result;
    uint16 WatchIndex = 0;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Unexpected LC_CustomFunction call: WP = %%d");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Execute the function being tested */
    Result = LC_CustomFunction(WatchIndex, 0, 0, 0);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_FALSE, "Result == LC_WATCH_FALSE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, LC_CFCALL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

} /* end LC_CustomFunction_Test_WatchIndex0 */

void LC_CustomFunction_Test_WatchIndex1(void)
{
    uint8  Result;
    uint16 WatchIndex = 1;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Unexpected LC_CustomFunction call: WP = %%d");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Execute the function being tested */
    Result = LC_CustomFunction(WatchIndex, 0, 0, 0);

    /* Verify results */
    UtAssert_True(Result == LC_WATCH_FALSE, "Result == LC_WATCH_FALSE");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, LC_CFCALL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

} /* end LC_CustomFunction_Test_WatchIndex1 */

void UtTest_Setup(void)
{
    UtTest_Add(LC_ExecuteRTS_Test, LC_Test_Setup, LC_Test_TearDown, "LC_ExecuteRTS_Test");
    UtTest_Add(LC_CustomFunction_Test_WatchIndex0, LC_Test_Setup, LC_Test_TearDown,
               "LC_CustomFunction_Test_WatchIndex0");
    UtTest_Add(LC_CustomFunction_Test_WatchIndex1, LC_Test_Setup, LC_Test_TearDown,
               "LC_CustomFunction_Test_WatchIndex1");
}
/************************/
/*  End of File Comment */
/************************/
