/*************************************************************************
** File:
**   $Id: lc_custom.h 1.3 2017/01/22 17:24:48EST sstrege Exp  $
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
**   Specification for the CFS Limit Checker (LC) mission specific
**   custom function template
**
** Notes:
**
**
**************************************************************************/
#ifndef _lc_custom_
#define _lc_custom_

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in lc_custom header
 */

#include "lc_custom.h"
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
 * Generated stub function for LC_CustomFunction()
 * ----------------------------------------------------
 */
uint8 LC_CustomFunction(uint16 WatchIndex, uint32 ProcessedWPData, const CFE_SB_Buffer_t *BufPtr,
                        uint32 WDTCustomFuncArg)
{
    /*UT_GenStub_SetupReturnBuffer(LC_CustomFunction, uint8);

    UT_GenStub_AddParam(LC_CustomFunction, uint16 , WatchIndex);
    UT_GenStub_AddParam(LC_CustomFunction, uint32 , ProcessedWPData);
    UT_GenStub_AddParam(LC_CustomFunction, const CFE_SB_Buffer_t* , BufPtr);
    UT_GenStub_AddParam(LC_CustomFunction, uint32 , WDTCustomFuncArg);

    UT_GenStub_Execute(LC_CustomFunction, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_CustomFunction, uint8);*/
    return UT_DEFAULT_IMPL(LC_CustomFunction);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_ExecuteRTS()
 * ----------------------------------------------------
 */
void LC_ExecuteRTS(uint16 RTSId)
{
    UT_GenStub_AddParam(LC_ExecuteRTS, uint16, RTSId);

    UT_GenStub_Execute(LC_ExecuteRTS, Basic, NULL);
}

#endif
