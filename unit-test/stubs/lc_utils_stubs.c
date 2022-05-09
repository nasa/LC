/************************************************************************
** File: lc_utils.h
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
**   CFS Limit Checker (LC) utility functions
**
**
*************************************************************************/
#ifndef _lc_utils_h_
#define _lc_utils_h_

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

#include <sys/fcntl.h>
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

#endif
