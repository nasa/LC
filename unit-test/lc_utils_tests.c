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

#include "lc_cmds.h"
#include "lc_utils.h"
#include "lc_app.h"
#include "lc_msg.h"
#include "lc_tbldefs.h"
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

void LC_ManageTables_Test_Nominal(void)
{
    CFE_Status_t Result;
    uint8        call_count_LC_ResetResultsWP;
    uint8        call_count_LC_ResetResultsAP;

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_ManageTables();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);

    call_count_LC_ResetResultsWP = UT_GetStubCount(UT_KEY(LC_ResetResultsWP));
    call_count_LC_ResetResultsAP = UT_GetStubCount(UT_KEY(LC_ResetResultsAP));

    UtAssert_INT32_EQ(call_count_LC_ResetResultsWP, 0);
    UtAssert_INT32_EQ(call_count_LC_ResetResultsAP, 0);
}

void LC_ManageTables_Test_InfoUpdated(void)
{
    CFE_Status_t Result;
    uint8        call_count_LC_ResetResultsWP;
    uint8        call_count_LC_ResetResultsAP;

    /* Set to satisfy all instances of condition "Result == CFE_TBL_INFO_UPDATED" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);

    /* Execute the function being tested */
    Result = LC_ManageTables();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);

    call_count_LC_ResetResultsWP = UT_GetStubCount(UT_KEY(LC_ResetResultsWP));
    call_count_LC_ResetResultsAP = UT_GetStubCount(UT_KEY(LC_ResetResultsAP));

    UtAssert_INT32_EQ(call_count_LC_ResetResultsWP, 1);
    UtAssert_INT32_EQ(call_count_LC_ResetResultsAP, 1);
}

void LC_ManageTables_Test_WDTGetAddressError(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    uint8        call_count_LC_ResetResultsWP;
    uint8        call_count_LC_ResetResultsAP;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Error getting WDT address, RC=0x%%08X");

    /* Set to generate event message LC_WDT_GETADDR_ERR_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), -1);

    /* Execute the function being tested */
    Result = LC_ManageTables();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WDT_GETADDR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_LC_ResetResultsWP = UT_GetStubCount(UT_KEY(LC_ResetResultsWP));
    call_count_LC_ResetResultsAP = UT_GetStubCount(UT_KEY(LC_ResetResultsAP));

    UtAssert_INT32_EQ(call_count_LC_ResetResultsWP, 0);
    UtAssert_INT32_EQ(call_count_LC_ResetResultsAP, 0);
}

void LC_ManageTables_Test_ADTGetAddressError(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    uint8        call_count_LC_ResetResultsAP;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Error getting ADT address, RC=0x%%08X");

    /* Generates event message LC_ADT_GETADDR_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, -1);

    /* Execute the function being tested */
    Result = LC_ManageTables();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_ADT_GETADDR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_LC_ResetResultsAP = UT_GetStubCount(UT_KEY(LC_ResetResultsAP));

    UtAssert_INT32_EQ(call_count_LC_ResetResultsAP, 0);
}

void LC_UpdateTaskCDS_Test_UpdateWatchpointError(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Unable to update watchpoint results in CDS, RC=0x%%08X");

    /* Set to generate error message LC_WRT_NO_SAVE_ERR_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_CopyToCDS), -1);

    /* Execute the function being tested */
    Result = LC_UpdateTaskCDS();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_WRT_NO_SAVE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_UpdateTaskCDS_Test_UpdateActionpointError(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Unable to update actionpoint results in CDS, RC=0x%%08X");

    /* Set to generate error message LC_ART_NO_SAVE_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 2, -1);

    /* Execute the function being tested */
    Result = LC_UpdateTaskCDS();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_ART_NO_SAVE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_UpdateTaskCDS_Test_UpdateAppDataError(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Unable to update application data in CDS, RC=0x%%08X");

    /* Set to generate error message LC_APP_NO_SAVE_START_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 3, -1);

    /* Execute the function being tested */
    Result = LC_UpdateTaskCDS();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");
    UtAssert_True(LC_AppData.CDSSavedOnExit == LC_CDS_SAVED, "LC_AppData.CDSSavedOnExit == LC_CDS_SAVED");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, LC_APP_NO_SAVE_START_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);
}

void LC_UpdateTaskCDS_Test_Nominal(void)
{
    CFE_Status_t Result;

    /* Execute the function being tested */
    Result = LC_UpdateTaskCDS();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True(LC_AppData.CDSSavedOnExit == LC_CDS_SAVED, "LC_AppData.CDSSavedOnExit == LC_CDS_SAVED");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_PerformMaintenance_Test_NominalNoCDS(void)
{
    CFE_Status_t Result;

    LC_OperData.HaveActiveCDS = false;
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_PerformMaintenance();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_PerformMaintenance_Test_NominalCDS(void)
{
    CFE_Status_t Result;

    LC_OperData.HaveActiveCDS = true;
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_PerformMaintenance();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);
}

void LC_PerformMaintenance_Test_UpdateCDSFail(void)
{
    CFE_Status_t Result;

    LC_OperData.HaveActiveCDS = true;

    /* force LC_ManageTables to succeed */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* force LC_UpdateTaskCDS to fail */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_CopyToCDS), -1);

    /* Execute the function being tested */
    Result = LC_PerformMaintenance();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(LC_OperData.HaveActiveCDS == false, "LC_OperData.HaveActiveCDS == false");
}

void LC_PerformMaintenance_Test_ManageTablesFail(void)
{
    CFE_Status_t Result;

    LC_OperData.HaveActiveCDS = false;
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), -1);

    /* Execute the function being tested */
    Result = LC_PerformMaintenance();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");
}

void UtTest_Setup(void)
{
    UtTest_Add(LC_ManageTables_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_ManageTables_Test_Nominal");
    UtTest_Add(LC_ManageTables_Test_InfoUpdated, LC_Test_Setup, LC_Test_TearDown, "LC_ManageTables_Test_InfoUpdated");
    UtTest_Add(LC_ManageTables_Test_WDTGetAddressError, LC_Test_Setup, LC_Test_TearDown,
               "LC_ManageTables_Test_WDTGetAddressError");
    UtTest_Add(LC_ManageTables_Test_ADTGetAddressError, LC_Test_Setup, LC_Test_TearDown,
               "LC_ManageTables_Test_ADTGetAddressError");

    UtTest_Add(LC_UpdateTaskCDS_Test_UpdateWatchpointError, LC_Test_Setup, LC_Test_TearDown,
               "LC_UpdateTaskCDS_Test_UpdateWatchpointError");
    UtTest_Add(LC_UpdateTaskCDS_Test_UpdateActionpointError, LC_Test_Setup, LC_Test_TearDown,
               "LC_UpdateTaskCDS_Test_UpdateActionpointError");
    UtTest_Add(LC_UpdateTaskCDS_Test_UpdateAppDataError, LC_Test_Setup, LC_Test_TearDown,
               "LC_UpdateTaskCDS_Test_UpdateAppDataError");
    UtTest_Add(LC_UpdateTaskCDS_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_UpdateTaskCDS_Test_Nominal");

    UtTest_Add(LC_PerformMaintenance_Test_NominalNoCDS, LC_Test_Setup, LC_Test_TearDown,
               "LC_PerformMaintenance_Test_NominalNoCDS");
    UtTest_Add(LC_PerformMaintenance_Test_NominalCDS, LC_Test_Setup, LC_Test_TearDown,
               "LC_PerformMaintenance_Test_NominalCDS");
    UtTest_Add(LC_PerformMaintenance_Test_UpdateCDSFail, LC_Test_Setup, LC_Test_TearDown,
               "LC_PerformMaintenance_Test_UpdateCDSFail");
    UtTest_Add(LC_PerformMaintenance_Test_ManageTablesFail, LC_Test_Setup, LC_Test_TearDown,
               "LC_PerformMaintenance_Test_ManageTablesFail");
}
