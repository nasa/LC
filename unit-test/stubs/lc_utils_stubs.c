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
 *   CFS Limit Checker (LC) utility functions
 */

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in lc_utils header
 */

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

#include <unistd.h>
#include <stdlib.h>

#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for LC_ManageTables()
 * ----------------------------------------------------
 */
int32 LC_ManageTables(void)
{
    UT_GenStub_SetupReturnBuffer(LC_ManageTables, int32);

    UT_GenStub_Execute(LC_ManageTables, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_ManageTables, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_PerformMaintenance()
 * ----------------------------------------------------
 */
int32 LC_PerformMaintenance(void)
{
    UT_GenStub_SetupReturnBuffer(LC_PerformMaintenance, int32);

    UT_GenStub_Execute(LC_PerformMaintenance, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_PerformMaintenance, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_UpdateTaskCDS()
 * ----------------------------------------------------
 */
int32 LC_UpdateTaskCDS(void)
{
    UT_GenStub_SetupReturnBuffer(LC_UpdateTaskCDS, int32);

    UT_GenStub_Execute(LC_UpdateTaskCDS, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_UpdateTaskCDS, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_VerifyMsgLength()
 * ----------------------------------------------------
 */
bool LC_VerifyMsgLength(const CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
{
    // UT_GenStub_SetupReturnBuffer(LC_VerifyMsgLength, bool);

    // UT_GenStub_AddParam(LC_VerifyMsgLength, const CFE_MSG_Message_t* , MsgPtr);
    // UT_GenStub_AddParam(LC_VerifyMsgLength, size_t , ExpectedLength);

    // UT_GenStub_Execute(LC_VerifyMsgLength, Basic, NULL);

    // return UT_GenStub_GetReturnValue(LC_VerifyMsgLength, bool);
    return UT_DEFAULT_IMPL(LC_VerifyMsgLength) != 0;
}
