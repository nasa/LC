#include "lc_cmds.h"
#include "lc_utils.h"
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

void LC_VerifyMsgLength_Test_HKRequestLengthError(void)
{
    bool              Result;
    LC_NoArgsCmd_t    CmdPacket;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Invalid HK request msg length: ID = 0x%%08X, CC = %%d, Len = %%d, Expected = %%d");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = LC_SEND_HK_MID;
    FcnCode   = LC_NOOP_CC;
    MsgSize   = sizeof(LC_NoArgsCmd_t) + 1;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    Result = LC_VerifyMsgLength((CFE_MSG_Message_t *)(&CmdPacket), sizeof(CmdPacket));

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, LC_HKREQ_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

} /* end LC_VerifyMsgLength_Test_HKRequestLengthError */

void LC_VerifyMsgLength_Test_APSampleLengthError(void)
{
    bool              Result;
    LC_NoArgsCmd_t    CmdPacket;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Invalid AP sample msg length: ID = 0x%%08X, CC = %%d, Len = %%d, Expected = %%d");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = LC_SAMPLE_AP_MID;
    FcnCode   = LC_NOOP_CC;
    MsgSize   = sizeof(CmdPacket) + 1;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    Result = LC_VerifyMsgLength((CFE_MSG_Message_t *)(&CmdPacket), sizeof(CmdPacket));

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, LC_APSAMPLE_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

} /* end LC_VerifyMsgLength_Test_APSampleLengthError */

void LC_VerifyMsgLength_Test_GenericLengthError(void)
{
    bool              Result;
    LC_NoArgsCmd_t    CmdPacket;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Invalid msg length: ID = 0x%%08X, CC = %%d, Len = %%d, Expected = %%d");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = 1;
    FcnCode   = LC_NOOP_CC;
    MsgSize   = sizeof(CmdPacket) + 1;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    Result = LC_VerifyMsgLength((CFE_MSG_Message_t *)(&CmdPacket), sizeof(CmdPacket));

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    UtAssert_True(LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, LC_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

} /* end LC_VerifyMsgLength_Test_GenericLengthError */

void LC_VerifyMsgLength_Test_Nominal(void)
{
    bool              Result;
    LC_NoArgsCmd_t    CmdPacket;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;

    TestMsgId = 1;
    FcnCode   = LC_NOOP_CC;
    MsgSize   = sizeof(LC_NoArgsCmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    Result = LC_VerifyMsgLength((CFE_MSG_Message_t *)(&CmdPacket), sizeof(CmdPacket));

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);

} /* end LC_VerifyMsgLength_Test_Nominal */

void LC_ManageTables_Test_Nominal(void)
{
    int32 Result;

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = LC_ManageTables();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);

    uint8 call_count_LC_ResetResultsWP = UT_GetStubCount(UT_KEY(LC_ResetResultsWP));
    uint8 call_count_LC_ResetResultsAP = UT_GetStubCount(UT_KEY(LC_ResetResultsAP));

    UtAssert_INT32_EQ(call_count_LC_ResetResultsWP, 0);
    UtAssert_INT32_EQ(call_count_LC_ResetResultsAP, 0);
}

void LC_ManageTables_Test_InfoUpdated(void)
{
    int32 Result;

    /* Set to satisfy all instances of condition "Result == CFE_TBL_INFO_UPDATED" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);

    /* Execute the function being tested */
    Result = LC_ManageTables();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);

    uint8 call_count_LC_ResetResultsWP = UT_GetStubCount(UT_KEY(LC_ResetResultsWP));
    uint8 call_count_LC_ResetResultsAP = UT_GetStubCount(UT_KEY(LC_ResetResultsAP));

    UtAssert_INT32_EQ(call_count_LC_ResetResultsWP, 1);
    UtAssert_INT32_EQ(call_count_LC_ResetResultsAP, 1);

} /* end LC_ManageTables_Test_InfoUpdated */

void LC_ManageTables_Test_WDTGetAddressError(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Error getting WDT address, RC=0x%%08X");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Set to generate event message LC_WDT_GETADDR_ERR_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), -1);

    /* Execute the function being tested */
    Result = LC_ManageTables();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, LC_WDT_GETADDR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    uint8 call_count_LC_ResetResultsWP = UT_GetStubCount(UT_KEY(LC_ResetResultsWP));
    uint8 call_count_LC_ResetResultsAP = UT_GetStubCount(UT_KEY(LC_ResetResultsAP));

    UtAssert_INT32_EQ(call_count_LC_ResetResultsWP, 0);
    UtAssert_INT32_EQ(call_count_LC_ResetResultsAP, 0);

} /* end LC_ManageTables_Test_WDTGetAddressError */

void LC_ManageTables_Test_ADTGetAddressError(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Error getting ADT address, RC=0x%%08X");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Generates event message LC_ADT_GETADDR_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, -1);

    /* Execute the function being tested */
    Result = LC_ManageTables();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, LC_ADT_GETADDR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    uint8 call_count_LC_ResetResultsAP = UT_GetStubCount(UT_KEY(LC_ResetResultsAP));

    UtAssert_INT32_EQ(call_count_LC_ResetResultsAP, 0);

} /* end LC_ManageTables_Test_ADTGetAddressError */

void LC_UpdateTaskCDS_Test_UpdateWatchpointError(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Unable to update watchpoint results in CDS, RC=0x%%08X");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Set to generate error message LC_WRT_NO_SAVE_ERR_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_CopyToCDS), -1);

    /* Execute the function being tested */
    Result = LC_UpdateTaskCDS();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, LC_WRT_NO_SAVE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

} /* end LC_UpdateTaskCDS_Test_UpdateWatchpointError */

void LC_UpdateTaskCDS_Test_UpdateActionpointError(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Unable to update actionpoint results in CDS, RC=0x%%08X");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Set to generate error message LC_ART_NO_SAVE_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 2, -1);

    /* Execute the function being tested */
    Result = LC_UpdateTaskCDS();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, LC_ART_NO_SAVE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

} /* end LC_UpdateTaskCDS_Test_UpdateActionpointError */

void LC_UpdateTaskCDS_Test_UpdateAppDataError(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Unable to update application data in CDS, RC=0x%%08X");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Set to generate error message LC_APP_NO_SAVE_START_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 3, -1);

    /* Execute the function being tested */
    Result = LC_UpdateTaskCDS();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");
    UtAssert_True(LC_AppData.CDSSavedOnExit == LC_CDS_SAVED, "LC_AppData.CDSSavedOnExit == LC_CDS_SAVED");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, LC_APP_NO_SAVE_START_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

} /* end LC_UpdateTaskCDS_Test_UpdateAppDataError */

void LC_UpdateTaskCDS_Test_Nominal(void)
{
    int32 Result;

    /* Execute the function being tested */
    Result = LC_UpdateTaskCDS();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True(LC_AppData.CDSSavedOnExit == LC_CDS_SAVED, "LC_AppData.CDSSavedOnExit == LC_CDS_SAVED");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_INT32_EQ(call_count_CFE_EVS_SendEvent, 0);

} /* end LC_UpdateTaskCDS_Test_Nominal */

void LC_PerformMaintenance_Test_NominalNoCDS(void)
{
    int32 Result;

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
    int32 Result;

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
    int32 Result;

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
    int32 Result;

    LC_OperData.HaveActiveCDS = false;
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), -1);

    /* Execute the function being tested */
    Result = LC_PerformMaintenance();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");
}

void UtTest_Setup(void)
{
    UtTest_Add(LC_VerifyMsgLength_Test_HKRequestLengthError, LC_Test_Setup, LC_Test_TearDown,
               "LC_VerifyMsgLength_Test_HKRequestLengthError");
    UtTest_Add(LC_VerifyMsgLength_Test_APSampleLengthError, LC_Test_Setup, LC_Test_TearDown,
               "LC_VerifyMsgLength_Test_APSampleLengthError");
    UtTest_Add(LC_VerifyMsgLength_Test_GenericLengthError, LC_Test_Setup, LC_Test_TearDown,
               "LC_VerifyMsgLength_Test_GenericLengthError");
    UtTest_Add(LC_VerifyMsgLength_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_VerifyMsgLength_Test_Nominal");

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
               "LC_PerfomMaintenance_Test_NominalNoCDS");
    UtTest_Add(LC_PerformMaintenance_Test_NominalCDS, LC_Test_Setup, LC_Test_TearDown,
               "LC_PerfomMaintenance_Test_NominalCDS");
    UtTest_Add(LC_PerformMaintenance_Test_UpdateCDSFail, LC_Test_Setup, LC_Test_TearDown,
               "LC_PerfomMaintenance_Test_UpdateCDSFail");
    UtTest_Add(LC_PerformMaintenance_Test_ManageTablesFail, LC_Test_Setup, LC_Test_TearDown,
               "LC_PerfomMaintenance_Test_ManageTablesFail");
}
