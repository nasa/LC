/************************************************************************
 * NASA Docket No. GSC-18,921-1, and identified as “CFS Limit Checker
 * Application version 2.2.0”
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
 *   Specification for the CFS Limit Checker (LC) routines that
 *   handle command processing
 */

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in lc_cmds header
 */

#include "lc_cmds.h"
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
 * Generated stub function for LC_AppPipe()
 * ----------------------------------------------------
 */
int32 LC_AppPipe(const CFE_SB_Buffer_t *BufPtr)
{
    UT_GenStub_SetupReturnBuffer(LC_AppPipe, int32);

    UT_GenStub_AddParam(LC_AppPipe, const CFE_SB_Buffer_t *, BufPtr);

    UT_GenStub_Execute(LC_AppPipe, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_AppPipe, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_HousekeepingReq()
 * ----------------------------------------------------
 */
int32 LC_HousekeepingReq(const CFE_MSG_CommandHeader_t *MsgPtr)
{
    UT_GenStub_SetupReturnBuffer(LC_HousekeepingReq, int32);

    UT_GenStub_AddParam(LC_HousekeepingReq, const CFE_MSG_CommandHeader_t *, MsgPtr);

    UT_GenStub_Execute(LC_HousekeepingReq, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_HousekeepingReq, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_NoopCmd()
 * ----------------------------------------------------
 */
void LC_NoopCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_GenStub_AddParam(LC_NoopCmd, const CFE_SB_Buffer_t *, BufPtr);

    UT_GenStub_Execute(LC_NoopCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_ResetAPStatsCmd()
 * ----------------------------------------------------
 */
void LC_ResetAPStatsCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_GenStub_AddParam(LC_ResetAPStatsCmd, const CFE_SB_Buffer_t *, BufPtr);

    UT_GenStub_Execute(LC_ResetAPStatsCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_ResetCmd()
 * ----------------------------------------------------
 */
void LC_ResetCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_GenStub_AddParam(LC_ResetCmd, const CFE_SB_Buffer_t *, BufPtr);

    UT_GenStub_Execute(LC_ResetCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_ResetCounters()
 * ----------------------------------------------------
 */
void LC_ResetCounters(void)
{

    UT_GenStub_Execute(LC_ResetCounters, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_ResetResultsAP()
 * ----------------------------------------------------
 */
void LC_ResetResultsAP(uint32 StartIndex, uint32 EndIndex, bool ResetCmd)
{
    UT_GenStub_AddParam(LC_ResetResultsAP, uint32, StartIndex);
    UT_GenStub_AddParam(LC_ResetResultsAP, uint32, EndIndex);
    UT_GenStub_AddParam(LC_ResetResultsAP, bool, ResetCmd);

    UT_GenStub_Execute(LC_ResetResultsAP, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_ResetResultsWP()
 * ----------------------------------------------------
 */
void LC_ResetResultsWP(uint32 StartIndex, uint32 EndIndex, bool ResetCmd)
{
    UT_GenStub_AddParam(LC_ResetResultsWP, uint32, StartIndex);
    UT_GenStub_AddParam(LC_ResetResultsWP, uint32, EndIndex);
    UT_GenStub_AddParam(LC_ResetResultsWP, bool, ResetCmd);

    UT_GenStub_Execute(LC_ResetResultsWP, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_ResetWPStatsCmd()
 * ----------------------------------------------------
 */
void LC_ResetWPStatsCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_GenStub_AddParam(LC_ResetWPStatsCmd, const CFE_SB_Buffer_t *, BufPtr);

    UT_GenStub_Execute(LC_ResetWPStatsCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_SampleAPReq()
 * ----------------------------------------------------
 */
void LC_SampleAPReq(const CFE_SB_Buffer_t *BufPtr)
{
    UT_GenStub_AddParam(LC_SampleAPReq, const CFE_SB_Buffer_t *, BufPtr);

    UT_GenStub_Execute(LC_SampleAPReq, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_SetAPPermOffCmd()
 * ----------------------------------------------------
 */
void LC_SetAPPermOffCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_GenStub_AddParam(LC_SetAPPermOffCmd, const CFE_SB_Buffer_t *, BufPtr);

    UT_GenStub_Execute(LC_SetAPPermOffCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_SetAPStateCmd()
 * ----------------------------------------------------
 */
void LC_SetAPStateCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_GenStub_AddParam(LC_SetAPStateCmd, const CFE_SB_Buffer_t *, BufPtr);

    UT_GenStub_Execute(LC_SetAPStateCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_SetLCStateCmd()
 * ----------------------------------------------------
 */
void LC_SetLCStateCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_GenStub_AddParam(LC_SetLCStateCmd, const CFE_SB_Buffer_t *, BufPtr);

    UT_GenStub_Execute(LC_SetLCStateCmd, Basic, NULL);
}
