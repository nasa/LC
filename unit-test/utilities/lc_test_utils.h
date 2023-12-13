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
 *   This file contains the function prototypes and global variables for
 *   the unit test utilities for the LC application.
 */
#ifndef LC_TEST_UTILS_H
#define LC_TEST_UTILS_H

#include "lc_app.h"
#include "utstubs.h"
#include "cfe_msgids.h"

/*
 * Allow UT access to the global "LC_AppData" object.
 */
extern LC_AppData_t  LC_AppData;
extern LC_OperData_t LC_OperData;

/* Global table variables for table pointers contained in LC_OperData */
extern LC_WDTEntry_t WDTable[LC_MAX_WATCHPOINTS];
extern LC_ADTEntry_t ADTable[LC_MAX_ACTIONPOINTS];
extern LC_WRTEntry_t WRTable[LC_MAX_WATCHPOINTS];
extern LC_ARTEntry_t ARTable[LC_MAX_ACTIONPOINTS];

/*
 * Global context structures
 */
typedef struct
{
    uint16 EventID;
    uint16 EventType;
    char   Spec[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
} CFE_EVS_SendEvent_context_t;

typedef struct
{
    char Spec[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
} CFE_ES_WriteToSysLog_context_t;

extern CFE_EVS_SendEvent_context_t    context_CFE_EVS_SendEvent[];
extern CFE_ES_WriteToSysLog_context_t context_CFE_ES_WriteToSysLog;

/* Command buffer typedef for any handler */
typedef union
{
    CFE_SB_Buffer_t Buf;

    LC_SendHkCmd_t        SendHkCmd;
    LC_NoopCmd_t          NoopCmd;
    LC_ResetCountersCmd_t ResetCountersCmd;
    LC_SetLCStateCmd_t    SetLCStateCmd;
    LC_SetAPStateCmd_t    SetAPStateCmd;
    LC_SetAPPermOffCmd_t  SetAPPermOffCmd;
    LC_ResetAPStatsCmd_t  ResetAPStatsCmd;
    LC_ResetWPStatsCmd_t  ResetWPStatsCmd;
    LC_SampleAPCmd_t      SampleAPCmd;
    LC_RTSRequestCmd_t    RTSRequestCmd;
} UT_CmdBuf_t;

extern UT_CmdBuf_t UT_CmdBuf;

/*
 * Macro to add a test case to the list of tests to execute
 */
#define ADD_TEST(test) UtTest_Add((Test_##test), LC_UT_Setup, LC_UT_TearDown, #test)

/* Unit test MID */
#define LC_UT_MID_1 CFE_SB_ValueToMsgId(1)
#define LC_UT_MID_2 CFE_SB_ValueToMsgId(2)

/*
 * Setup function prior to every test
 */
void LC_Test_Setup(void);

/*
 * Teardown function after every test
 */
void LC_Test_TearDown(void);

#endif
