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

#include "lc_app.h"
#include "lc_msg.h"
#include "lc_tbldefs.h"
#include "lc_msgdefs.h"
#include "lc_msgids.h"
#include "lc_eventids.h"
#include "lc_version.h"
#include "lc_utils.h"
#include "lc_test_utils.h"
#include "lc_dispatch.h"
#include "lc_platform_cfg.h"

#include "cfe.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

#include <unistd.h>
#include <stdlib.h>

uint8 call_count_CFE_EVS_SendEvent;

/*
 * Function Definitions
 */

uint16 LC_APP_TEST_CFE_TBL_RegisterHookCount;
int32  LC_APP_TEST_CFE_TBL_RegisterHook1(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                         const UT_StubContext_t *Context)
{
    LC_APP_TEST_CFE_TBL_RegisterHookCount++;

    if (LC_APP_TEST_CFE_TBL_RegisterHookCount == 1)
        return 99;
    else
        return CFE_TBL_INFO_RECOVERED_TBL;
}

int32 LC_APP_TEST_CFE_TBL_RegisterHook2(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                        const UT_StubContext_t *Context)
{
    LC_APP_TEST_CFE_TBL_RegisterHookCount++;

    if (LC_APP_TEST_CFE_TBL_RegisterHookCount == 2)
        return 99;
    else
        return CFE_TBL_INFO_RECOVERED_TBL;
}

int32 LC_APP_TEST_CFE_TBL_RegisterHook3(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                        const UT_StubContext_t *Context)
{
    LC_APP_TEST_CFE_TBL_RegisterHookCount++;

    if (LC_APP_TEST_CFE_TBL_RegisterHookCount == 1)
        return CFE_TBL_INFO_RECOVERED_TBL;
    else
        return CFE_SUCCESS;
}

int32 LC_APP_TEST_CFE_TBL_RegisterHook4(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                        const UT_StubContext_t *Context)
{
    LC_APP_TEST_CFE_TBL_RegisterHookCount++;

    if (LC_APP_TEST_CFE_TBL_RegisterHookCount == 1)
        return CFE_TBL_INFO_RECOVERED_TBL;
    if (LC_APP_TEST_CFE_TBL_RegisterHookCount == 2)
        return CFE_SUCCESS;
    else
        return -1;
}

/* Hook used to override CDS settings */
static void UT_Handler_CFE_ES_RunLoop(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    uint16 CallCount;
    bool   Status = false;

    CallCount = UT_GetStubCount(FuncKey);

    /* Exercise each option for coverage */
    if (CallCount == 1)
    {
        LC_OperData.HaveActiveCDS = false;
        LC_AppData.CDSSavedOnExit = 0;
    }
    else if (CallCount == 2)
    {
        LC_OperData.HaveActiveCDS = true;
        LC_AppData.CDSSavedOnExit = 0;
    }
    else
    {
        LC_OperData.HaveActiveCDS = true;
        LC_AppData.CDSSavedOnExit = LC_CDS_SAVED;
    }

    /* Always return false */
    UT_Stub_SetReturnValue(FuncKey, Status);
}

void LC_AppMain_Test_Nominal(void)
{
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, false);

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(LC_AppPipe), CFE_SUCCESS);

    LC_AppMain();

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    /* generates 2 messages we don't care about in this test */
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 2);

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_ES_ExitApp)), 1);

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(LC_AppPipe)), 1);
}

void LC_AppMain_Test_NominalCDSSave(void)
{
    /* Handler forces CDS conditions based on call count */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_RunLoop), &UT_Handler_CFE_ES_RunLoop, NULL);

    /* Cycle through all conditions forced by handler */
    LC_AppMain();
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(LC_UpdateTaskCDS)), 0);

    /* Reset SendEvent to avoid filling buffer */
    UT_ResetState(UT_KEY(CFE_EVS_SendEvent));
    LC_AppMain();
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(LC_UpdateTaskCDS)), 0);

    UT_ResetState(UT_KEY(CFE_EVS_SendEvent));
    LC_AppMain();
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(LC_UpdateTaskCDS)), 1);
}

void LC_AppMain_Test_NominalCDSNoSave(void)
{
    LC_OperData.HaveActiveCDS = true;
    LC_AppData.CDSSavedOnExit = LC_CDS_NOT_SAVED;

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, false);

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(LC_AppPipe), CFE_SUCCESS);

    LC_AppMain();

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    /* generates 2 messages we don't care about in this test */
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 2);

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_ES_ExitApp)), 1);

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(LC_AppPipe)), 1);

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(LC_UpdateTaskCDS)), 0);
}

void LC_AppMain_Test_AppInitFail(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Task terminating, err = 0x%%08X");

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), -1);

    LC_AppMain();
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    /* generates one message we don't care about in this test */
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 2);

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_ES_ExitApp)), 1);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, LC_TASK_EXIT_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_CRITICAL);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);
}

void LC_AppMain_Test_SbError(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Task terminating, err = 0x%%08X");

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, false);

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(LC_PerformMaintenance), CFE_SUCCESS);

    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, -1);

    LC_AppMain();

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    /* generates 2 messages we don't care about in this test */
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 3);

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_ES_ExitApp)), 1);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventID, LC_TASK_EXIT_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventType, CFE_EVS_EventType_CRITICAL);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[2].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[2].Spec);
}

void LC_AppMain_Test_SbTimeout(void)
{
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, false);

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(LC_PerformMaintenance), CFE_SUCCESS);

    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SB_TIME_OUT);

    LC_AppMain();

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_ES_ExitApp)), 1);
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(LC_PerformMaintenance)), 1);
}

void LC_AppInit_Test_Nominal(void)
{
    CFE_Status_t Result;
    char         Message[125];

    memset(&LC_OperData, 1, sizeof(LC_OperData));
    memset(&LC_AppData, 1, sizeof(LC_AppData));

    /* Set to prevent errors when initializing tables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_AppInit();

    /* Verify results */
    sprintf(Message, "LC Initialized. Version %d.%d.%d.%d", LC_MAJOR_VERSION, LC_MINOR_VERSION, LC_REVISION,
            LC_MISSION_REV);

    UtAssert_INT32_EQ(Result, CFE_SUCCESS);

    UtAssert_BOOL_FALSE(CFE_RESOURCEID_TEST_DEFINED(LC_OperData.CmdPipe));
    /* Note: not testing LC_OperData.WDTPtr == 0, because it is modified by a subfunction */
    /* Note: not testing LC_OperData.ADTPtr == 0, because it is modified by a subfunction */
    /* Note: not testing LC_OperData.WRTPtr == 0, because it is modified by a subfunction */
    /* Note: not testing LC_OperData.ARTPtr == 0, because it is modified by a subfunction */
    /* Note: not testing LC_OperData.WDTHandle == 0, because it is modified by a subfunction */
    /* Note: not testing LC_OperData.ADTHandle == 0, because it is modified by a subfunction */
    UtAssert_INT32_EQ(LC_OperData.WRTHandle, 0);
    /* Note: not testing LC_OperData.ARTHandle == 0, because it is modified by a subfunction */
    UtAssert_BOOL_FALSE(CFE_RESOURCEID_TEST_DEFINED(LC_OperData.WRTDataCDSHandle));
    UtAssert_BOOL_FALSE(CFE_RESOURCEID_TEST_DEFINED(LC_OperData.ARTDataCDSHandle));
    UtAssert_BOOL_FALSE(CFE_RESOURCEID_TEST_DEFINED(LC_OperData.AppDataCDSHandle));
    /* Note: not testing LC_OperData.WatchpointCount == 0, because it is modified by a subfunction */
    /* Note: not testing LC_OperData.MessageIDsCount == 0, because it is modified by a subfunction */
    /* Note: not testing LC_OperData.HkPacket == 0, because it is modified by a subfunction */
    /* Note: not testing LC_OperData.TableResults == 0, because it is modified by a subfunction */
    /* Note: not testing LC_OperData.HashTable == 0, because it is modified by a subfunction */
    /* Note: not testing LC_OperData.MessageLinks == 0, because it is modified by a subfunction */
    /* Note: not testing LC_OperData.WatchPtLinks == 0, because it is modified by a subfunction */
    UtAssert_INT32_EQ(LC_OperData.HaveActiveCDS, 0);

    UtAssert_INT32_EQ(LC_AppData.CmdCount, 0);
    UtAssert_INT32_EQ(LC_AppData.CmdErrCount, 0);
    UtAssert_INT32_EQ(LC_AppData.APSampleCount, 0);
    UtAssert_INT32_EQ(LC_AppData.MonitoredMsgCount, 0);
    UtAssert_INT32_EQ(LC_AppData.RTSExecCount, 0);
    UtAssert_INT32_EQ(LC_AppData.PassiveRTSExecCount, 0);
    UtAssert_INT32_EQ(LC_AppData.CDSSavedOnExit, 0);
    /* Note: not testing LC_AppData.CurrentLCState == 0, because it is modified by a subfunction */

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

#ifndef LC_SAVE_TO_CDS /* When this is not defined EVENT ID=23 is always issued (LC use of Critical Data Store \
                          disabled) */
    /* Generates 3 event message we don't care about in this test */
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 2);
#else /* Alt confg unit tests */
    /* Generates 2 event message we don't care about in this test */
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
#endif
}

void LC_AppInit_Test_EVSInitError(void)
{
    CFE_Status_t Result;

    /* Set to prevent errors when initializing tables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to cause LC_EvsInit to return -1, in order to satisfy subsequent condition "Status != CFE_SUCCESS" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_EVS_Register), -1);

    /* Execute the function being tested */
    Result = LC_AppInit();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_AppInit_Test_SBInitError(void)
{
    CFE_Status_t Result;

    /* Set to prevent errors when initializing tables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to cause LC_SbInit to return -1, in order to satisfy subsequent condition "Status != CFE_SUCCESS" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_CreatePipe), -1);

    /* Execute the function being tested */
    Result = LC_AppInit();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    /* Generates one event message we don't care about in this test */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
}

void LC_AppInit_Test_TableInitError(void)
{
    CFE_Status_t Result;

    /* Set to cause LC_TableInit to return -1, in order to satisfy subsequent condition "Status != CFE_SUCCESS" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), -1);

    /* Execute the function being tested */
    Result = LC_AppInit();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    /* Generates one event message we don't care about in this test */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
}

void LC_EvsInit_Test_Nominal(void)
{
    CFE_Status_t Result;

    /* Execute the function being tested */
    Result = LC_EvsInit();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_EvsInit_Test_EVSRegisterError(void)
{
    CFE_Status_t Result;

    /* Set to generate system log message "LC App: Error Registering For Event Services" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_EVS_Register), -1);

    /* Execute the function being tested */
    Result = LC_EvsInit();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SbInit_Test_Nominal(void)
{
    CFE_Status_t Result;

    /* Execute the function being tested */
    Result = LC_SbInit();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_SbInit_Test_CreatePipeError(void)
{
    CFE_Status_t Result;

    /* Set to generate error message LC_CR_PIPE_ERR_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_CreatePipe), -1);

    /* Execute the function being tested */
    Result = LC_SbInit();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
}

void LC_SbInit_Test_SubscribeHKReqError(void)
{
    CFE_Status_t Result;

    /* Set to generate error message LC_SUB_HK_REQ_ERR_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_Subscribe), -1);

    /* Execute the function being tested */
    Result = LC_SbInit();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
}

void LC_SbInit_Test_SubscribeGndCmdError(void)
{
    CFE_Status_t Result;

    /* Set to generate error message LC_SUB_GND_CMD_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 2, -1);

    /* Execute the function being tested */
    Result = LC_SbInit();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
}

void LC_SbInit_Test_SubscribeSampleCmdError(void)
{
    CFE_Status_t Result;

    /* Set to generate error message LC_SUB_SAMPLE_CMD_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 3, -1);

    /* Execute the function being tested */
    Result = LC_SbInit();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
}

void LC_TableInit_Test_CreateResultsTablesError(void)
{
    LC_OperData.HaveActiveCDS = true;

    /* force LC_CreateResultTables to fail */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Register), -1);

    UtAssert_INT32_EQ(LC_TableInit(), -1);
}

void LC_TableInit_Test_CreateDefinitionTablesError(void)
{
    LC_OperData.HaveActiveCDS = true;

    /* force LC_CreateDefinitionTables to fail (but allow LC_CreateResultsTables
     * to succeed) */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 3, -1);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, -1);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, -1);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, -1);

    UtAssert_INT32_EQ(LC_TableInit(), -1);
}

/* Hits the uncovered branches related to LoadDefaultTables */
void LC_TableInit_Test_LoadDefaultTables(void)
{
    LC_OperData.HaveActiveCDS = true;

    /* Only recover WDT */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 3, CFE_TBL_INFO_RECOVERED_TBL);

    /* Setup LC_CreateTaskCDS to succeed */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RegisterCDS), CFE_ES_CDS_ALREADY_EXISTS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RestoreFromCDS), CFE_SUCCESS);
    LC_AppData.CDSSavedOnExit = LC_CDS_SAVED;

    /* Hit table info updated case for LC_LoadDefaultTables */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 4, CFE_TBL_INFO_UPDATED);

    UtAssert_INT32_EQ(LC_TableInit(), CFE_SUCCESS);

    /* Ensure correct table state */
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_WRT_CDS_RESTORED, LC_WRT_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_ART_CDS_RESTORED, LC_ART_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_APP_CDS_RESTORED, LC_APP_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_WDT_TBL_RESTORED, LC_WDT_TBL_RESTORED);
    UtAssert_UINT32_NEQ(LC_OperData.TableResults & LC_ADT_TBL_RESTORED, LC_ADT_TBL_RESTORED);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_CDS_UPDATED_INF_EID);

    /* Reset SendEvent and don't recover WDT */
    UT_ResetState(UT_KEY(CFE_EVS_SendEvent));
    LC_OperData.TableResults = 0;

    UtAssert_INT32_EQ(LC_TableInit(), CFE_SUCCESS);

    /* Ensure correct table state */
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_WRT_CDS_RESTORED, LC_WRT_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_ART_CDS_RESTORED, LC_ART_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_APP_CDS_RESTORED, LC_APP_CDS_RESTORED);
    UtAssert_UINT32_NEQ(LC_OperData.TableResults & LC_WDT_TBL_RESTORED, LC_WDT_TBL_RESTORED);
    UtAssert_UINT32_NEQ(LC_OperData.TableResults & LC_ADT_TBL_RESTORED, LC_ADT_TBL_RESTORED);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_CDS_UPDATED_INF_EID);

    /* Reset all states and only restore WRT and ART CDS */
    UT_ResetState(0);
    LC_OperData.TableResults = 0;
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDS), 1, CFE_ES_CDS_ALREADY_EXISTS);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDS), 1, CFE_ES_CDS_ALREADY_EXISTS);

    UtAssert_INT32_EQ(LC_TableInit(), CFE_SUCCESS);

    /* Ensure correct table state */
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_WRT_CDS_RESTORED, LC_WRT_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_ART_CDS_RESTORED, LC_ART_CDS_RESTORED);
    UtAssert_UINT32_NEQ(LC_OperData.TableResults & LC_APP_CDS_RESTORED, LC_APP_CDS_RESTORED);
    UtAssert_UINT32_NEQ(LC_OperData.TableResults & LC_WDT_TBL_RESTORED, LC_WDT_TBL_RESTORED);
    UtAssert_UINT32_NEQ(LC_OperData.TableResults & LC_ADT_TBL_RESTORED, LC_ADT_TBL_RESTORED);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_CDS_UPDATED_INF_EID);

    /* Reset all states and only restore WRT */
    UT_ResetState(0);
    LC_OperData.TableResults = 0;
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDS), 1, CFE_ES_CDS_ALREADY_EXISTS);

    UtAssert_INT32_EQ(LC_TableInit(), CFE_SUCCESS);

    /* Ensure correct table state */
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_WRT_CDS_RESTORED, LC_WRT_CDS_RESTORED);
    UtAssert_UINT32_NEQ(LC_OperData.TableResults & LC_ART_CDS_RESTORED, LC_ART_CDS_RESTORED);
    UtAssert_UINT32_NEQ(LC_OperData.TableResults & LC_APP_CDS_RESTORED, LC_APP_CDS_RESTORED);
    UtAssert_UINT32_NEQ(LC_OperData.TableResults & LC_WDT_TBL_RESTORED, LC_WDT_TBL_RESTORED);
    UtAssert_UINT32_NEQ(LC_OperData.TableResults & LC_ADT_TBL_RESTORED, LC_ADT_TBL_RESTORED);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_CDS_UPDATED_INF_EID);
}

void LC_TableInit_Test_GetWDTAddressError(void)
{
    LC_OperData.HaveActiveCDS = true;

    /* Recover all but the first two tables so LC_CreateResultTables will succeed */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Register), CFE_TBL_INFO_RECOVERED_TBL);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_SUCCESS);

    /* Setup LC_CreateTaskCDS to succeed */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RegisterCDS), CFE_ES_CDS_ALREADY_EXISTS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RestoreFromCDS), CFE_SUCCESS);
    LC_AppData.CDSSavedOnExit = LC_CDS_SAVED;

    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 3, -1);

    UtAssert_INT32_EQ(LC_TableInit(), -1);

    /* Ensure correct table state */
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_WRT_CDS_RESTORED, LC_WRT_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_ART_CDS_RESTORED, LC_ART_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_APP_CDS_RESTORED, LC_APP_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_WDT_TBL_RESTORED, LC_WDT_TBL_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_ADT_TBL_RESTORED, LC_ADT_TBL_RESTORED);

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WDT_GETADDR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void LC_TableInit_Test_GetWDTAddressUpdated(void)
{
    LC_OperData.HaveActiveCDS = true;

    /* Recover all but the first two tables so LC_CreateResultTables will succeed */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Register), CFE_TBL_INFO_RECOVERED_TBL);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_SUCCESS);

    /* Setup LC_CreateTaskCDS to succeed */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RegisterCDS), CFE_ES_CDS_ALREADY_EXISTS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RestoreFromCDS), CFE_SUCCESS);
    LC_AppData.CDSSavedOnExit = LC_CDS_SAVED;

    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 3, CFE_TBL_INFO_UPDATED);

    UtAssert_INT32_EQ(LC_TableInit(), CFE_SUCCESS);

    /* Ensure correct table state */
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_WRT_CDS_RESTORED, LC_WRT_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_ART_CDS_RESTORED, LC_ART_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_APP_CDS_RESTORED, LC_APP_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_WDT_TBL_RESTORED, LC_WDT_TBL_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_ADT_TBL_RESTORED, LC_ADT_TBL_RESTORED);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_CDS_RESTORED_INF_EID);
}

void LC_TableInit_Test_GetADTAddressError(void)
{
    LC_OperData.HaveActiveCDS = true;

    /* Recover all but the first two tables so LC_CreateResultTables will succeed */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Register), CFE_TBL_INFO_RECOVERED_TBL);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_SUCCESS);

    /* Setup LC_CreateTaskCDS to succeed */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RegisterCDS), CFE_ES_CDS_ALREADY_EXISTS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RestoreFromCDS), CFE_SUCCESS);
    LC_AppData.CDSSavedOnExit = LC_CDS_SAVED;

    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 4, -1);

    UtAssert_INT32_EQ(LC_TableInit(), -1);

    /* Ensure correct table state */
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_WRT_CDS_RESTORED, LC_WRT_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_ART_CDS_RESTORED, LC_ART_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_APP_CDS_RESTORED, LC_APP_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_WDT_TBL_RESTORED, LC_WDT_TBL_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_ADT_TBL_RESTORED, LC_ADT_TBL_RESTORED);

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_ADT_GETADDR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void LC_TableInit_Test_GetADTAddressUpdated(void)
{
    LC_OperData.HaveActiveCDS = true;

    /* Recover all but the first two tables so LC_CreateResultTables will succeed */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Register), CFE_TBL_INFO_RECOVERED_TBL);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_SUCCESS);

    /* Setup LC_CreateTaskCDS to succeed */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RegisterCDS), CFE_ES_CDS_ALREADY_EXISTS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RestoreFromCDS), CFE_SUCCESS);
    LC_AppData.CDSSavedOnExit = LC_CDS_SAVED;

    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 4, CFE_TBL_INFO_UPDATED);

    UtAssert_INT32_EQ(LC_TableInit(), CFE_SUCCESS);

    /* Ensure correct table state */
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_WRT_CDS_RESTORED, LC_WRT_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_ART_CDS_RESTORED, LC_ART_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_APP_CDS_RESTORED, LC_APP_CDS_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_WDT_TBL_RESTORED, LC_WDT_TBL_RESTORED);
    UtAssert_UINT32_EQ(LC_OperData.TableResults & LC_ADT_TBL_RESTORED, LC_ADT_TBL_RESTORED);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_CDS_RESTORED_INF_EID);
}

void LC_TableInit_Test_CreateTaskCDSError(void)
{
    CFE_Status_t Result;
    LC_OperData.HaveActiveCDS = true;

    /* force LC_CreateTaskCDS to fail */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RegisterCDS), -1);

    Result = LC_TableInit();

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True(LC_OperData.HaveActiveCDS == false, "LC_OperData.HaveActiveCDS == false");
}

void LC_TableInit_Test_Nominal(void)
{
    CFE_Status_t Result;

    LC_OperData.HaveActiveCDS = true;

    /* Setup LC_CreateTaskCDS to succeed */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RegisterCDS), CFE_ES_CDS_ALREADY_EXISTS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RestoreFromCDS), CFE_SUCCESS);
    LC_AppData.CDSSavedOnExit = LC_CDS_SAVED;

    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 3, CFE_TBL_INFO_RECOVERED_TBL);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_TBL_INFO_RECOVERED_TBL);

    Result = LC_TableInit();

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(((LC_OperData.TableResults & LC_WRT_CDS_CREATED) == LC_WRT_CDS_CREATED),
                  "LC_OperData.TableResult & LC_WRT_CDS_CREATED == LC_WRT_CDS_CREATED");
    UtAssert_True(((LC_OperData.TableResults & LC_ART_CDS_CREATED) == LC_ART_CDS_CREATED),
                  "LC_OperData.TableResult & LC_ART_CDS_CREATED == LC_ART_CDS_CREATED");
    UtAssert_True(((LC_OperData.TableResults & LC_APP_CDS_CREATED) == LC_APP_CDS_CREATED),
                  "LC_OperData.TableResult & LC_APP_CDS_CREATED == LC_APP_CDS_CREATED");
    UtAssert_True(((LC_OperData.TableResults & LC_WRT_CDS_RESTORED) == LC_WRT_CDS_RESTORED),
                  "LC_OperData.TableResult & LC_WRT_CDS_RESTORED == LC_WRT_CDS_RESTORED");
    UtAssert_True(((LC_OperData.TableResults & LC_ART_CDS_RESTORED) == LC_ART_CDS_RESTORED),
                  "LC_OperData.TableResult & LC_ART_CDS_RESTORED == LC_ART_CDS_RESTORED");
    UtAssert_True(((LC_OperData.TableResults & LC_APP_CDS_RESTORED) == LC_APP_CDS_RESTORED),
                  "LC_OperData.TableResult & LC_APP_CDS_RESTORED == LC_APP_CDS_RESTORED");
}

void LC_CreateResultTables_Test_Nominal(void)
{
    CFE_Status_t Result;

    LC_OperData.TableResults = 0;

    /* Execute the function being tested */
    Result = LC_CreateResultTables();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True(LC_OperData.TableResults == (LC_WRT_TBL_CREATED | LC_ART_TBL_CREATED),
                  "LC_OperData.TableResults == (LC_WRT_TBL_CREATED | LC_ART_TBL_CREATED)");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_CreateResultTables_Test_WRTRegisterError(void)
{
    CFE_Status_t Result;

    LC_OperData.TableResults = 0;

    /* Set to generate error message LC_WRT_REGISTER_ERR_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Register), -1);

    /* Execute the function being tested */
    Result = LC_CreateResultTables();

    /* Verify results */
    UtAssert_True(LC_OperData.TableResults == 0, "LC_OperData.TableResults == 0");
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
}

void LC_CreateResultTables_Test_WRTGetAddressError(void)
{
    CFE_Status_t Result;

    LC_OperData.TableResults = 0;

    /* Set to generate error message LC_WRT_GETADDR_ERR_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), -1);

    /* Execute the function being tested */
    Result = LC_CreateResultTables();

    /* Verify results */
    UtAssert_True(LC_OperData.TableResults == 0, "LC_OperData.TableResults == 0");
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
}

void LC_CreateResultTables_Test_ARTRegisterError(void)
{
    CFE_Status_t Result;

    LC_OperData.TableResults = 0;

    /* Set to generate error message LC_ART_REGISTER_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 2, -1);

    /* Execute the function being tested */
    Result = LC_CreateResultTables();

    /* Verify results */
    UtAssert_True(LC_OperData.TableResults == LC_WRT_TBL_CREATED, "LC_OperData.TableResults == LC_WRT_TBL_CREATED");
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
}

void LC_CreateResultTables_Test_ARTGetAddressError(void)
{
    CFE_Status_t Result;

    LC_OperData.TableResults = 0;

    /* Set to generate error message LC_ART_GETADDR_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, -1);

    /* Execute the function being tested */
    Result = LC_CreateResultTables();

    /* Verify results */
    UtAssert_True(LC_OperData.TableResults == LC_WRT_TBL_CREATED, "LC_OperData.TableResults == LC_WRT_TBL_CREATED");
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
}

void LC_CreateDefinitionTables_Test_NominalCDSActive(void)
{
    CFE_Status_t Result;

    LC_OperData.TableResults  = 0;
    LC_OperData.HaveActiveCDS = true;

    /* Set to reach all nominal code branches */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Register), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_CreateDefinitionTables();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True(LC_OperData.TableResults == (LC_WDT_CRITICAL_TBL | LC_ADT_CRITICAL_TBL),
                  "LC_OperData.TableResults == (LC_WDT_CRITICAL_TBL | LC_ADT_CRITICAL_TBL)");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_CreateDefinitionTables_Test_NominalCDSInactive(void)
{
    CFE_Status_t Result;

    LC_OperData.TableResults  = 0;
    LC_OperData.HaveActiveCDS = false;

    /* Set to reach all nominal code branches */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Register), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_CreateDefinitionTables();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True(LC_OperData.TableResults == (LC_WDT_NOT_CRITICAL | LC_ADT_NOT_CRITICAL),
                  "LC_OperData.TableResults == (LC_WDT_NOT_CRITICAL | LC_ADT_NOT_CRITICAL)");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_CreateDefinitionTables_Test_WDTReRegisterThenInfoRecovered(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Error registering WDT as critical table, retrying with default options, RC=0x%%08X");

    LC_OperData.TableResults  = 0;
    LC_OperData.HaveActiveCDS = true;

    /* Satisfies condition "(LC_OperData.HaveActiveCDS) && ((Result != CFE_TBL_INFO_RECOVERED_TBL) && (Result !=
       CFE_SUCCESS))" and then satisfies condition "Result == CFE_TBL_INFO_RECOVERED_TBL" */
    LC_APP_TEST_CFE_TBL_RegisterHookCount = 0;
    UT_SetHookFunction(UT_KEY(CFE_TBL_Register), &LC_APP_TEST_CFE_TBL_RegisterHook1, NULL);

    /* Execute the function being tested */
    Result = LC_CreateDefinitionTables();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True(LC_OperData.HaveActiveCDS == false, "LC_OperData.HaveActiveCDS == false");
    UtAssert_True(LC_OperData.TableResults ==
                      (LC_WDT_CRITICAL_TBL | LC_WDT_TBL_RESTORED | LC_ADT_CRITICAL_TBL | LC_ADT_TBL_RESTORED),
                  "LC_OperData.TableResults == (LC_WDT_CRITICAL_TBL | LC_WDT_TBL_RESTORED | LC_ADT_CRITICAL_TBL | "
                  "LC_ADT_TBL_RESTORED)");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WDT_REGISTER_CRIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_CreateDefinitionTables_Test_ADTReRegisterThenInfoRecovered(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Error registering ADT as critical table, retrying with default options, RC=0x%%08X");

    LC_OperData.TableResults  = 0;
    LC_OperData.HaveActiveCDS = true;

    /* Satisfies condition "(LC_OperData.HaveActiveCDS) && ((Result != CFE_TBL_INFO_RECOVERED_TBL) && (Result !=
       CFE_SUCCESS))" and then satisfies condition "Result == CFE_TBL_INFO_RECOVERED_TBL" */
    LC_APP_TEST_CFE_TBL_RegisterHookCount = 0;
    UT_SetHookFunction(UT_KEY(CFE_TBL_Register), &LC_APP_TEST_CFE_TBL_RegisterHook2, NULL);

    /* Execute the function being tested */
    Result = LC_CreateDefinitionTables();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True(LC_OperData.HaveActiveCDS == false, "LC_OperData.HaveActiveCDS == false");
    UtAssert_True(LC_OperData.TableResults ==
                      (LC_WDT_CRITICAL_TBL | LC_WDT_TBL_RESTORED | LC_ADT_CRITICAL_TBL | LC_ADT_TBL_RESTORED),
                  "LC_OperData.TableResults == (LC_WDT_CRITICAL_TBL | LC_WDT_TBL_RESTORED | LC_ADT_CRITICAL_TBL | "
                  "LC_ADT_TBL_RESTORED)");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_ADT_REGISTER_CRIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_CreateDefinitionTables_Test_WDTCriticalADTNoncritical(void)
{
    CFE_Status_t Result;

    LC_OperData.TableResults  = 0;
    LC_OperData.HaveActiveCDS = false;

    /* Satisfies condition "((LC_OperData.TableResults & LC_WDT_CRITICAL_TBL) == LC_WDT_CRITICAL_TBL) &&
       ((LC_OperData.TableResults & LC_ADT_NOT_CRITICAL) == LC_ADT_NOT_CRITICAL)" */
    LC_APP_TEST_CFE_TBL_RegisterHookCount = 0;
    UT_SetHookFunction(UT_KEY(CFE_TBL_Register), &LC_APP_TEST_CFE_TBL_RegisterHook3, NULL);

    /* Execute the function being tested */
    Result = LC_CreateDefinitionTables();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True(LC_OperData.TableResults ==
                      (LC_WDT_CRITICAL_TBL | LC_WDT_TBL_RESTORED | LC_ADT_NOT_CRITICAL | LC_WDT_NOT_CRITICAL),
                  "LC_OperData.TableResults == (LC_WDT_CRITICAL_TBL | LC_WDT_TBL_RESTORED | LC_ADT_NOT_CRITICAL | "
                  "LC_WDT_NOT_CRITICAL)");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_CreateDefinitionTables_Test_WDTReRegisterError(void)
{
    LC_OperData.TableResults  = 0;
    LC_OperData.HaveActiveCDS = false;

    /* Satisfies condition "((LC_OperData.TableResults & LC_WDT_CRITICAL_TBL) == LC_WDT_CRITICAL_TBL) &&
       ((LC_OperData.TableResults & LC_ADT_NOT_CRITICAL) == LC_ADT_NOT_CRITICAL)"
       and then FAILS condition "Result == CFE_SUCCESS", in order to generate error message LC_WDT_REREGISTER_ERR_EID */
    LC_APP_TEST_CFE_TBL_RegisterHookCount = 0;
    UT_SetHookFunction(UT_KEY(CFE_TBL_Register), &LC_APP_TEST_CFE_TBL_RegisterHook4, NULL);

    /* Execute the function being tested */
    UtAssert_INT32_EQ(LC_CreateDefinitionTables(), -1);

    /* Verify results */
    UtAssert_True(LC_OperData.TableResults == (LC_WDT_CRITICAL_TBL | LC_WDT_TBL_RESTORED | LC_ADT_NOT_CRITICAL),
                  "LC_OperData.TableResults == (LC_WDT_CRITICAL_TBL | LC_WDT_TBL_RESTORED | LC_ADT_NOT_CRITICAL)");

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WDT_REREGISTER_ERR_EID);
}

void LC_CreateDefinitionTables_Test_WDTRegisterError(void)
{
    LC_OperData.TableResults  = 0;
    LC_OperData.HaveActiveCDS = false;

    /* Set to generate error message LC_WDT_REGISTER_ERR_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Register), -1);

    /* Execute the function being tested */
    UtAssert_INT32_EQ(LC_CreateDefinitionTables(), -1);

    /* Verify results */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WDT_REGISTER_ERR_EID);
}

void LC_CreateDefinitionTables_Test_ADTRegisterError(void)
{
    LC_OperData.TableResults  = 0;
    LC_OperData.HaveActiveCDS = false;

    /* Set to generate error message LC_ADT_REGISTER_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 2, -1);

    /* Execute the function being tested */
    UtAssert_INT32_EQ(LC_CreateDefinitionTables(), -1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_ADT_REGISTER_ERR_EID);
}

void LC_LoadDefaultTables_Test_NominalActiveCDS(void)
{
    CFE_Status_t Result;

    LC_OperData.HaveActiveCDS = true;

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_Load */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_GetAddress */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_LoadDefaultTables();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(LC_OperData.TableResults == (LC_WDT_DEFAULT_TBL | LC_ADT_DEFAULT_TBL | LC_WRT_DEFAULT_DATA |
                                               LC_ART_DEFAULT_DATA | LC_APP_DEFAULT_DATA | LC_CDS_UPDATED),
                  "LC_OperData.TableResults == (LC_WDT_DEFAULT_TBL | LC_ADT_DEFAULT_TBL | LC_WRT_DEFAULT_DATA | "
                  "LC_ART_DEFAULT_DATA | LC_APP_DEFAULT_DATA | LC_CDS_UPDATED)");

    UtAssert_True(LC_AppData.CurrentLCState == LC_STATE_POWER_ON_RESET,
                  "LC_AppData.CurrentLCState == LC_STATE_POWER_ON_RESET");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_LoadDefaultTables_Test_ActiveCDSNotSuccess(void)
{
    CFE_Status_t Result;

    LC_OperData.HaveActiveCDS = true;

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_Load */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_GetAddress */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Set to make subfunction LC_UpdateTaskCDS return -1, in order to satisfy condition "LC_UpdateTaskCDS() ==
     * CFE_SUCCESS" */
    UT_SetDefaultReturnValue(UT_KEY(LC_UpdateTaskCDS), -1);

    /* Execute the function being tested */
    Result = LC_LoadDefaultTables();

    /* Verify results */
    UtAssert_INT32_EQ(Result, CFE_SUCCESS);

    UtAssert_INT32_EQ(LC_OperData.TableResults, (LC_WDT_DEFAULT_TBL | LC_ADT_DEFAULT_TBL | LC_WRT_DEFAULT_DATA |
                                                 LC_ART_DEFAULT_DATA | LC_APP_DEFAULT_DATA));

    UtAssert_INT32_EQ(LC_AppData.CurrentLCState, LC_STATE_POWER_ON_RESET);

    UtAssert_INT32_EQ(LC_OperData.HaveActiveCDS, false);

    /* Generates one event message we don't care about in this test */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_LoadDefaultTables_Test_NominalInactiveCDS(void)
{
    CFE_Status_t Result;
    LC_OperData.HaveActiveCDS = false;

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_Load */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_GetAddress */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_LoadDefaultTables();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(LC_OperData.TableResults == (LC_WDT_DEFAULT_TBL | LC_ADT_DEFAULT_TBL | LC_WRT_DEFAULT_DATA |
                                               LC_ART_DEFAULT_DATA | LC_APP_DEFAULT_DATA),
                  "LC_OperData.TableResults == (LC_WDT_DEFAULT_TBL | LC_ADT_DEFAULT_TBL | LC_WRT_DEFAULT_DATA | "
                  "LC_ART_DEFAULT_DATA | LC_APP_DEFAULT_DATA)");

    UtAssert_True(LC_AppData.CurrentLCState == LC_STATE_POWER_ON_RESET,
                  "LC_AppData.CurrentLCState == LC_STATE_POWER_ON_RESET");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_LoadDefaultTables_Test_LoadWDTError(void)
{
    CFE_Status_t Result;

    LC_OperData.HaveActiveCDS = false;

    /* Set to generate error message LC_WDT_LOAD_ERR_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), -1);

    /* Execute the function being tested */
    Result = LC_LoadDefaultTables();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
}

void LC_LoadDefaultTables_Test_GetWDTAddressError(void)
{
    CFE_Status_t Result;

    LC_OperData.HaveActiveCDS = false;

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_Load */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to generate error message LC_WDT_GETADDR_ERR_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), -1);

    /* Execute the function being tested */
    Result = LC_LoadDefaultTables();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
}

void LC_LoadDefaultTables_Test_LoadADTError(void)
{
    CFE_Status_t Result;

    LC_OperData.HaveActiveCDS = false;

    /* Set to satisfy condition "Result == CFE_SUCCESS" after first call to CFE_TBL_Load */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Load), 2, -1);

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_GetAddress */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_LoadDefaultTables();

    /* Verify results */
    UtAssert_INT32_EQ(Result, -1);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
}

void LC_LoadDefaultTables_Test_GetADTAddressError(void)
{
    LC_OperData.HaveActiveCDS = false;

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_Load */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to satisfy condition "Result == CFE_SUCCESS" after first call to CFE_TBL_GetAddress */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, -1);

    /* Execute the function being tested */
    UtAssert_INT32_EQ(LC_LoadDefaultTables(), -1);

    /* Verify results */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void LC_LoadDefaultTables_Test_InfoUpdated(void)
{
    CFE_Status_t Result;

    LC_OperData.HaveActiveCDS = true;

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_Load */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_GetAddress */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);

    /* Execute the function being tested */
    Result = LC_LoadDefaultTables();

    /* Verify results */
    UtAssert_True(Result == CFE_TBL_INFO_UPDATED, "Result == CFE_TBL_INFO_UPDATED");

    UtAssert_True(LC_OperData.TableResults == (LC_WDT_DEFAULT_TBL | LC_ADT_DEFAULT_TBL | LC_WRT_DEFAULT_DATA |
                                               LC_ART_DEFAULT_DATA | LC_APP_DEFAULT_DATA | LC_CDS_UPDATED),
                  "LC_OperData.TableResults == (LC_WDT_DEFAULT_TBL | LC_ADT_DEFAULT_TBL | LC_WRT_DEFAULT_DATA | "
                  "LC_ART_DEFAULT_DATA | LC_APP_DEFAULT_DATA | LC_CDS_UPDATED)");

    UtAssert_True(LC_AppData.CurrentLCState == LC_STATE_POWER_ON_RESET,
                  "LC_AppData.CurrentLCState == LC_STATE_POWER_ON_RESET");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_CreateTaskCDS_Test_Nominal(void)
{
    CFE_Status_t Result;

    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RegisterCDS), CFE_SUCCESS);

    Result = LC_CreateTaskCDS();

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);

    UtAssert_True(((LC_OperData.TableResults & LC_WRT_CDS_CREATED) == LC_WRT_CDS_CREATED),
                  "LC_OperData.TableResult & LC_WRT_CDS_CREATED == LC_WRT_CDS_CREATED");
    UtAssert_True(((LC_OperData.TableResults & LC_ART_CDS_CREATED) == LC_ART_CDS_CREATED),
                  "LC_OperData.TableResult & LC_ART_CDS_CREATED == LC_ART_CDS_CREATED");
    UtAssert_True(((LC_OperData.TableResults & LC_APP_CDS_CREATED) == LC_APP_CDS_CREATED),
                  "LC_OperData.TableResult & LC_APP_CDS_CREATED == LC_APP_CDS_CREATED");
}

void LC_CreateTaskCDS_Test_WRTRegisterCDSError(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDS), 1, -1);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Error registering WRT CDS Area, RC=0x%%08X");

    Result = LC_CreateTaskCDS();

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WRT_CDS_REGISTER_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_CreateTaskCDS_Test_ARTRegisterCDSError(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDS), 2, -1);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Error registering ART CDS Area, RC=0x%%08X");

    Result = LC_CreateTaskCDS();

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_ART_CDS_REGISTER_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_CreateTaskCDS_Test_AppDataRegisterCDSError(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDS), 3, -1);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Error registering application data CDS Area, RC=0x%%08X");

    Result = LC_CreateTaskCDS();

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 1);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APP_CDS_REGISTER_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_CreateTaskCDS_Test_RestoreSuccess(void)
{
    CFE_Status_t Result;

    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RegisterCDS), CFE_ES_CDS_ALREADY_EXISTS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RestoreFromCDS), CFE_SUCCESS);

    LC_AppData.CDSSavedOnExit = LC_CDS_SAVED;

    Result = LC_CreateTaskCDS();

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);

    UtAssert_True(((LC_OperData.TableResults & LC_WRT_CDS_CREATED) == LC_WRT_CDS_CREATED),
                  "LC_OperData.TableResult & LC_WRT_CDS_CREATED == LC_WRT_CDS_CREATED");
    UtAssert_True(((LC_OperData.TableResults & LC_ART_CDS_CREATED) == LC_ART_CDS_CREATED),
                  "LC_OperData.TableResult & LC_ART_CDS_CREATED == LC_ART_CDS_CREATED");
    UtAssert_True(((LC_OperData.TableResults & LC_APP_CDS_CREATED) == LC_APP_CDS_CREATED),
                  "LC_OperData.TableResult & LC_APP_CDS_CREATED == LC_APP_CDS_CREATED");
    UtAssert_True(((LC_OperData.TableResults & LC_WRT_CDS_RESTORED) == LC_WRT_CDS_RESTORED),
                  "LC_OperData.TableResult & LC_WRT_CDS_RESTORED == LC_WRT_CDS_RESTORED");
    UtAssert_True(((LC_OperData.TableResults & LC_ART_CDS_RESTORED) == LC_ART_CDS_RESTORED),
                  "LC_OperData.TableResult & LC_ART_CDS_RESTORED == LC_ART_CDS_RESTORED");
    UtAssert_True(((LC_OperData.TableResults & LC_APP_CDS_RESTORED) == LC_APP_CDS_RESTORED),
                  "LC_OperData.TableResult & LC_APP_CDS_RESTORED == LC_APP_CDS_RESTORED");
}

void LC_CreateTaskCDS_Test_RestoreSuccessCDSNotSaved(void)
{
    CFE_Status_t Result;

    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RegisterCDS), CFE_ES_CDS_ALREADY_EXISTS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RestoreFromCDS), CFE_SUCCESS);

    LC_AppData.CDSSavedOnExit = !LC_CDS_SAVED;

    Result = LC_CreateTaskCDS();

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);

    UtAssert_True(((LC_OperData.TableResults & LC_WRT_CDS_CREATED) == LC_WRT_CDS_CREATED),
                  "LC_OperData.TableResult & LC_WRT_CDS_CREATED == LC_WRT_CDS_CREATED");
    UtAssert_True(((LC_OperData.TableResults & LC_ART_CDS_CREATED) == LC_ART_CDS_CREATED),
                  "LC_OperData.TableResult & LC_ART_CDS_CREATED == LC_ART_CDS_CREATED");
    UtAssert_True(((LC_OperData.TableResults & LC_APP_CDS_CREATED) == LC_APP_CDS_CREATED),
                  "LC_OperData.TableResult & LC_APP_CDS_CREATED == LC_APP_CDS_CREATED");
    UtAssert_True(((LC_OperData.TableResults & LC_WRT_CDS_RESTORED) == LC_WRT_CDS_RESTORED),
                  "LC_OperData.TableResult & LC_WRT_CDS_RESTORED == LC_WRT_CDS_RESTORED");
    UtAssert_True(((LC_OperData.TableResults & LC_ART_CDS_RESTORED) == LC_ART_CDS_RESTORED),
                  "LC_OperData.TableResult & LC_ART_CDS_RESTORED == LC_ART_CDS_RESTORED");
    UtAssert_True(((LC_OperData.TableResults & LC_APP_CDS_RESTORED) == 0),
                  "LC_OperData.TableResult & LC_APP_CDS_RESTORED == 0");
}

void LC_CreateTaskCDS_Test_RestoreFail(void)
{
    CFE_Status_t Result;

    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RegisterCDS), CFE_ES_CDS_ALREADY_EXISTS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RestoreFromCDS), -1);

    LC_AppData.CDSSavedOnExit = LC_CDS_SAVED;

    Result = LC_CreateTaskCDS();

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)), 0);

    UtAssert_True(((LC_OperData.TableResults & LC_WRT_CDS_CREATED) == LC_WRT_CDS_CREATED),
                  "LC_OperData.TableResult & LC_WRT_CDS_CREATED == LC_WRT_CDS_CREATED");
    UtAssert_True(((LC_OperData.TableResults & LC_ART_CDS_CREATED) == LC_ART_CDS_CREATED),
                  "LC_OperData.TableResult & LC_ART_CDS_CREATED == LC_ART_CDS_CREATED");
    UtAssert_True(((LC_OperData.TableResults & LC_APP_CDS_CREATED) == LC_APP_CDS_CREATED),
                  "LC_OperData.TableResult & LC_APP_CDS_CREATED == LC_APP_CDS_CREATED");
    UtAssert_True(((LC_OperData.TableResults & LC_WRT_CDS_RESTORED) == 0),
                  "LC_OperData.TableResult & LC_WRT_CDS_RESTORED == 0");
    UtAssert_True(((LC_OperData.TableResults & LC_ART_CDS_RESTORED) == 0),
                  "LC_OperData.TableResult & LC_ART_CDS_RESTORED == 0");
    UtAssert_True(((LC_OperData.TableResults & LC_APP_CDS_RESTORED) == 0),
                  "LC_OperData.TableResult & LC_APP_CDS_RESTORED == 0");
}

void UtTest_Setup(void)
{
    UtTest_Add(LC_AppMain_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_AppMain_Test_Nominal");

    UtTest_Add(LC_AppMain_Test_NominalCDSSave, LC_Test_Setup, LC_Test_TearDown, "LC_AppMain_Test_NominalCDSSave");
    UtTest_Add(LC_AppMain_Test_NominalCDSNoSave, LC_Test_Setup, LC_Test_TearDown, "LC_AppMain_Test_NominalCDSNoSave");

    UtTest_Add(LC_AppMain_Test_AppInitFail, LC_Test_Setup, LC_Test_TearDown, "LC_AppMain_Test_AppInitFail");
    UtTest_Add(LC_AppMain_Test_SbError, LC_Test_Setup, LC_Test_TearDown, "LC_AppMain_Test_SbError");

    UtTest_Add(LC_AppMain_Test_SbTimeout, LC_Test_Setup, LC_Test_TearDown, "LC_AppMain_Test_SbTimeout");

#ifndef LC_SAVE_TO_CDS /* default config unit test */
    UtTest_Add(LC_AppInit_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_AppInit_Test_Nominal");
#endif

    UtTest_Add(LC_AppInit_Test_EVSInitError, LC_Test_Setup, LC_Test_TearDown, "LC_AppInit_Test_EVSInitError");
    UtTest_Add(LC_AppInit_Test_SBInitError, LC_Test_Setup, LC_Test_TearDown, "LC_AppInit_Test_SBInitError");
    UtTest_Add(LC_AppInit_Test_TableInitError, LC_Test_Setup, LC_Test_TearDown, "LC_AppInit_Test_TableInitError");

    UtTest_Add(LC_EvsInit_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_EvsInit_Test_Nominal");
    UtTest_Add(LC_EvsInit_Test_EVSRegisterError, LC_Test_Setup, LC_Test_TearDown, "LC_EvsInit_Test_EVSRegisterError");

    UtTest_Add(LC_SbInit_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_SbInit_Test_Nominal");
    UtTest_Add(LC_SbInit_Test_CreatePipeError, LC_Test_Setup, LC_Test_TearDown, "LC_SbInit_Test_CreatePipeError");
    UtTest_Add(LC_SbInit_Test_SubscribeHKReqError, LC_Test_Setup, LC_Test_TearDown,
               "LC_SbInit_Test_SubscribeHKReqError");
    UtTest_Add(LC_SbInit_Test_SubscribeGndCmdError, LC_Test_Setup, LC_Test_TearDown,
               "LC_SbInit_Test_SubscribeGndCmdError");
    UtTest_Add(LC_SbInit_Test_SubscribeSampleCmdError, LC_Test_Setup, LC_Test_TearDown,
               "LC_SbInit_Test_SubscribeSampleCmdError");

    UtTest_Add(LC_TableInit_Test_CreateTaskCDSError, LC_Test_Setup, LC_Test_TearDown,
               "LC_TableInit_Test_CreateTaskCDSError");
    UtTest_Add(LC_TableInit_Test_CreateResultsTablesError, LC_Test_Setup, LC_Test_TearDown,
               "LC_TableInit_Test_CreateResultsTablesError");
    UtTest_Add(LC_TableInit_Test_CreateDefinitionTablesError, LC_Test_Setup, LC_Test_TearDown,
               "LC_TableInit_Test_CreateDefinitionTablesError");

    UtTest_Add(LC_TableInit_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_TableInit_Test_Nominal");
    UtTest_Add(LC_TableInit_Test_LoadDefaultTables, LC_Test_Setup, LC_Test_TearDown,
               "LC_TableInit_Test_LoadDefaultTables");
    UtTest_Add(LC_TableInit_Test_GetWDTAddressError, LC_Test_Setup, LC_Test_TearDown,
               "LC_TableInit_Test_GetWDTAddressError");
    UtTest_Add(LC_TableInit_Test_GetWDTAddressUpdated, LC_Test_Setup, LC_Test_TearDown,
               "LC_TableInit_Test_GetWDTAddressUpdated");
    UtTest_Add(LC_TableInit_Test_GetADTAddressError, LC_Test_Setup, LC_Test_TearDown,
               "LC_TableInit_Test_GetADTAddressError");
    UtTest_Add(LC_TableInit_Test_GetADTAddressUpdated, LC_Test_Setup, LC_Test_TearDown,
               "LC_TableInit_Test_GetADTAddressUpdated");

    UtTest_Add(LC_CreateResultTables_Test_Nominal, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateResultTables_Test_Nominal");
    UtTest_Add(LC_CreateResultTables_Test_WRTRegisterError, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateResultTables_Test_WRTRegisterError");
    UtTest_Add(LC_CreateResultTables_Test_WRTGetAddressError, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateResultTables_Test_WRTGetAddressError");
    UtTest_Add(LC_CreateResultTables_Test_ARTRegisterError, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateResultTables_Test_ARTRegisterError");
    UtTest_Add(LC_CreateResultTables_Test_ARTGetAddressError, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateResultTables_Test_ARTGetAddressError");

    UtTest_Add(LC_CreateDefinitionTables_Test_NominalCDSActive, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateDefinitionTables_Test_NominalCDSActive");
    UtTest_Add(LC_CreateDefinitionTables_Test_NominalCDSInactive, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateDefinitionTables_Test_NominalCDSInactive");
    UtTest_Add(LC_CreateDefinitionTables_Test_WDTReRegisterThenInfoRecovered, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateDefinitionTables_Test_WDTReRegisterThenInfoRecovered");
    UtTest_Add(LC_CreateDefinitionTables_Test_ADTReRegisterThenInfoRecovered, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateDefinitionTables_Test_ADTReRegisterThenInfoRecovered");
    UtTest_Add(LC_CreateDefinitionTables_Test_WDTReRegisterError, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateDefinitionTables_Test_WDTReRegisterError");
    UtTest_Add(LC_CreateDefinitionTables_Test_WDTCriticalADTNoncritical, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateDefinitionTables_Test_WDTCriticalADTNoncritical");
    UtTest_Add(LC_CreateDefinitionTables_Test_WDTRegisterError, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateDefinitionTables_Test_WDTRegisterError");
    UtTest_Add(LC_CreateDefinitionTables_Test_ADTRegisterError, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateDefinitionTables_Test_ADTRegisterError");

    UtTest_Add(LC_LoadDefaultTables_Test_NominalActiveCDS, LC_Test_Setup, LC_Test_TearDown,
               "LC_LoadDefaultTables_Test_NominalActiveCDS");
    UtTest_Add(LC_LoadDefaultTables_Test_ActiveCDSNotSuccess, LC_Test_Setup, LC_Test_TearDown,
               "LC_LoadDefaultTables_Test_ActiveCDSNotSuccess");
    UtTest_Add(LC_LoadDefaultTables_Test_NominalInactiveCDS, LC_Test_Setup, LC_Test_TearDown,
               "LC_LoadDefaultTables_Test_NominalInactiveCDS");
    UtTest_Add(LC_LoadDefaultTables_Test_LoadWDTError, LC_Test_Setup, LC_Test_TearDown,
               "LC_LoadDefaultTables_Test_LoadWDTError");
    UtTest_Add(LC_LoadDefaultTables_Test_GetWDTAddressError, LC_Test_Setup, LC_Test_TearDown,
               "LC_LoadDefaultTables_Test_GetWDTAddressError");
    UtTest_Add(LC_LoadDefaultTables_Test_LoadADTError, LC_Test_Setup, LC_Test_TearDown,
               "LC_LoadDefaultTables_Test_LoadADTError");
    UtTest_Add(LC_LoadDefaultTables_Test_GetADTAddressError, LC_Test_Setup, LC_Test_TearDown,
               "LC_LoadDefaultTables_Test_GetADTAddressError");
    UtTest_Add(LC_LoadDefaultTables_Test_InfoUpdated, LC_Test_Setup, LC_Test_TearDown,
               "LC_LoadDefaultTables_Test_InfoUpdated");

    UtTest_Add(LC_CreateTaskCDS_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_CreateTaskCDS_Test_Nominal");
    UtTest_Add(LC_CreateTaskCDS_Test_WRTRegisterCDSError, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateTaskCDS_Test_WRTRegisterCDSError");
    UtTest_Add(LC_CreateTaskCDS_Test_ARTRegisterCDSError, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateTaskCDS_Test_ARTRegisterCDSError");
    UtTest_Add(LC_CreateTaskCDS_Test_AppDataRegisterCDSError, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateTaskCDS_Test_AppDataRegisterCDSError");
    UtTest_Add(LC_CreateTaskCDS_Test_RestoreSuccess, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateTaskCDS_Test_RestoreSuccess");
    UtTest_Add(LC_CreateTaskCDS_Test_RestoreSuccessCDSNotSaved, LC_Test_Setup, LC_Test_TearDown,
               "LC_CreateTaskCDS_Test_RestoreSuccessCDSNotSaved");
    UtTest_Add(LC_CreateTaskCDS_Test_RestoreFail, LC_Test_Setup, LC_Test_TearDown, "LC_CreateTaskCDS_Test_RestoreFail");
}
