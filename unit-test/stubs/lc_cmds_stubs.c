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
 *
 * Auto-Generated stub implementations for functions defined in lc_cmds header
 */

#include "lc_cmds.h"
#include "utgenstub.h"

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
 * Generated stub function for LC_ResetCounters()
 * ----------------------------------------------------
 */
void LC_ResetCounters(void)
{

    UT_GenStub_Execute(LC_ResetCounters, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_ResetCountersCmd()
 * ----------------------------------------------------
 */
void LC_ResetCountersCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_GenStub_AddParam(LC_ResetCountersCmd, const CFE_SB_Buffer_t *, BufPtr);

    UT_GenStub_Execute(LC_ResetCountersCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_ResetResultsAP()
 * ----------------------------------------------------
 */
void LC_ResetResultsAP(uint32 StartIndex, uint32 EndIndex, bool ResetStatsCmd)
{
    UT_GenStub_AddParam(LC_ResetResultsAP, uint32, StartIndex);
    UT_GenStub_AddParam(LC_ResetResultsAP, uint32, EndIndex);
    UT_GenStub_AddParam(LC_ResetResultsAP, bool, ResetStatsCmd);

    UT_GenStub_Execute(LC_ResetResultsAP, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_ResetResultsWP()
 * ----------------------------------------------------
 */
void LC_ResetResultsWP(uint32 StartIndex, uint32 EndIndex, bool ResetStatsCmd)
{
    UT_GenStub_AddParam(LC_ResetResultsWP, uint32, StartIndex);
    UT_GenStub_AddParam(LC_ResetResultsWP, uint32, EndIndex);
    UT_GenStub_AddParam(LC_ResetResultsWP, bool, ResetStatsCmd);

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
 * Generated stub function for LC_SendHkCmd()
 * ----------------------------------------------------
 */
CFE_Status_t LC_SendHkCmd(const CFE_MSG_CommandHeader_t *MsgPtr)
{
    UT_GenStub_SetupReturnBuffer(LC_SendHkCmd, CFE_Status_t);

    UT_GenStub_AddParam(LC_SendHkCmd, const CFE_MSG_CommandHeader_t *, MsgPtr);

    UT_GenStub_Execute(LC_SendHkCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_SendHkCmd, CFE_Status_t);
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
