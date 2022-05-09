/*************************************************************************
** File:
**   $Id: lc_cmds.h 1.4 2017/01/22 17:24:53EST sstrege Exp  $
**
**  Copyright (c) 2007-2020 United States Government as represented by the
**  Administrator of the National Aeronautics and Space Administration.
**  All Other Rights Reserved.
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be
**  used, distributed and modified only pursuant to the terms of that
**  agreement.
**
** Purpose:
**   Specification for the CFS Limit Checker (LC) routines that
**   handle command processing
**
** Notes:
**
**
**************************************************************************/
#ifndef _lc_cmds_
#define _lc_cmds_

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

#include <sys/fcntl.h>
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

#endif
