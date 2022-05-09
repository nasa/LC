/*************************************************************************
** File:
**   $Id: lc_action.h 1.4 2017/01/22 17:24:52EST sstrege Exp  $
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
**   handle actionpoint processing
**
** Notes:
**
**
**************************************************************************/
#ifndef _lc_action_
#define _lc_action_

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in lc_action header
 */

#include "lc_action.h"
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
 * Generated stub function for LC_EvaluateRPN()
 * ----------------------------------------------------
 */
uint8 LC_EvaluateRPN(uint16 APNumber)
{
    UT_GenStub_SetupReturnBuffer(LC_EvaluateRPN, uint8);

    UT_GenStub_AddParam(LC_EvaluateRPN, uint16, APNumber);

    UT_GenStub_Execute(LC_EvaluateRPN, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_EvaluateRPN, uint8);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_SampleAPs()
 * ----------------------------------------------------
 */
void LC_SampleAPs(uint16 StartIndex, uint16 EndIndex)
{
    UT_GenStub_AddParam(LC_SampleAPs, uint16, StartIndex);
    UT_GenStub_AddParam(LC_SampleAPs, uint16, EndIndex);

    UT_GenStub_Execute(LC_SampleAPs, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_SampleSingleAP()
 * ----------------------------------------------------
 */
void LC_SampleSingleAP(uint16 APNumber)
{
    UT_GenStub_AddParam(LC_SampleSingleAP, uint16, APNumber);

    UT_GenStub_Execute(LC_SampleSingleAP, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_ValidateADT()
 * ----------------------------------------------------
 */
int32 LC_ValidateADT(void *TableData)
{
    UT_GenStub_SetupReturnBuffer(LC_ValidateADT, int32);

    UT_GenStub_AddParam(LC_ValidateADT, void *, TableData);

    UT_GenStub_Execute(LC_ValidateADT, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_ValidateADT, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_ValidateRPN()
 * ----------------------------------------------------
 */
int32 LC_ValidateRPN(const uint16 *RPNPtr, int32 *IndexValue, int32 *StackDepthValue)
{
    UT_GenStub_SetupReturnBuffer(LC_ValidateRPN, int32);

    UT_GenStub_AddParam(LC_ValidateRPN, const uint16 *, RPNPtr);
    UT_GenStub_AddParam(LC_ValidateRPN, int32 *, IndexValue);
    UT_GenStub_AddParam(LC_ValidateRPN, int32 *, StackDepthValue);

    UT_GenStub_Execute(LC_ValidateRPN, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_ValidateRPN, int32);
}

#endif
