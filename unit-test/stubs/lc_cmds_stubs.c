/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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
CFE_Status_t LC_NoopCmd(const LC_NoopCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(LC_NoopCmd, CFE_Status_t);

    UT_GenStub_AddParam(LC_NoopCmd, const LC_NoopCmd_t *, msg);

    UT_GenStub_Execute(LC_NoopCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_NoopCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_ResetAPStatsCmd()
 * ----------------------------------------------------
 */
CFE_Status_t LC_ResetAPStatsCmd(const LC_ResetAPStatsCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(LC_ResetAPStatsCmd, CFE_Status_t);

    UT_GenStub_AddParam(LC_ResetAPStatsCmd, const LC_ResetAPStatsCmd_t *, msg);

    UT_GenStub_Execute(LC_ResetAPStatsCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_ResetAPStatsCmd, CFE_Status_t);
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
CFE_Status_t LC_ResetCountersCmd(const LC_ResetCountersCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(LC_ResetCountersCmd, CFE_Status_t);

    UT_GenStub_AddParam(LC_ResetCountersCmd, const LC_ResetCountersCmd_t *, msg);

    UT_GenStub_Execute(LC_ResetCountersCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_ResetCountersCmd, CFE_Status_t);
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
CFE_Status_t LC_ResetWPStatsCmd(const LC_ResetWPStatsCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(LC_ResetWPStatsCmd, CFE_Status_t);

    UT_GenStub_AddParam(LC_ResetWPStatsCmd, const LC_ResetWPStatsCmd_t *, msg);

    UT_GenStub_Execute(LC_ResetWPStatsCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_ResetWPStatsCmd, CFE_Status_t);
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
CFE_Status_t LC_SendHkCmd(const LC_SendHkCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(LC_SendHkCmd, CFE_Status_t);

    UT_GenStub_AddParam(LC_SendHkCmd, const LC_SendHkCmd_t *, msg);

    UT_GenStub_Execute(LC_SendHkCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_SendHkCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_SetAPPermOffCmd()
 * ----------------------------------------------------
 */
CFE_Status_t LC_SetAPPermOffCmd(const LC_SetAPPermOffCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(LC_SetAPPermOffCmd, CFE_Status_t);

    UT_GenStub_AddParam(LC_SetAPPermOffCmd, const LC_SetAPPermOffCmd_t *, msg);

    UT_GenStub_Execute(LC_SetAPPermOffCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_SetAPPermOffCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_SetAPStateCmd()
 * ----------------------------------------------------
 */
CFE_Status_t LC_SetAPStateCmd(const LC_SetAPStateCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(LC_SetAPStateCmd, CFE_Status_t);

    UT_GenStub_AddParam(LC_SetAPStateCmd, const LC_SetAPStateCmd_t *, msg);

    UT_GenStub_Execute(LC_SetAPStateCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_SetAPStateCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_SetLCStateCmd()
 * ----------------------------------------------------
 */
CFE_Status_t LC_SetLCStateCmd(const LC_SetLCStateCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(LC_SetLCStateCmd, CFE_Status_t);

    UT_GenStub_AddParam(LC_SetLCStateCmd, const LC_SetLCStateCmd_t *, msg);

    UT_GenStub_Execute(LC_SetLCStateCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_SetLCStateCmd, CFE_Status_t);
}
