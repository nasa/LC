/************************************************************************
** File:
**   $Id: lc_app.h 1.4 2017/01/22 17:24:19EST sstrege Exp  $
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
**   Unit specification for the Core Flight System (CFS)
**   Limit Checker (LC) Application.
**
** Notes:
**
**
*************************************************************************/
#ifndef _lc_app_stubs_
#define _lc_app_stubs_

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in lc_app header
 */

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

/************************************************************************
** LC Global Data
*************************************************************************/
LC_OperData_t LC_OperData;
LC_AppData_t  LC_AppData;

/*
 * ----------------------------------------------------
 * Generated stub function for LC_AppInit()
 * ----------------------------------------------------
 */
int32 LC_AppInit(void)
{
    UT_GenStub_SetupReturnBuffer(LC_AppInit, int32);

    UT_GenStub_Execute(LC_AppInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_AppInit, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_AppMain()
 * ----------------------------------------------------
 */
void LC_AppMain(void)
{

    UT_GenStub_Execute(LC_AppMain, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_CreateDefinitionTables()
 * ----------------------------------------------------
 */
int32 LC_CreateDefinitionTables(void)
{
    UT_GenStub_SetupReturnBuffer(LC_CreateDefinitionTables, int32);

    UT_GenStub_Execute(LC_CreateDefinitionTables, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_CreateDefinitionTables, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_CreateResultTables()
 * ----------------------------------------------------
 */
int32 LC_CreateResultTables(void)
{
    UT_GenStub_SetupReturnBuffer(LC_CreateResultTables, int32);

    UT_GenStub_Execute(LC_CreateResultTables, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_CreateResultTables, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_CreateTaskCDS()
 * ----------------------------------------------------
 */
int32 LC_CreateTaskCDS(void)
{
    UT_GenStub_SetupReturnBuffer(LC_CreateTaskCDS, int32);

    UT_GenStub_Execute(LC_CreateTaskCDS, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_CreateTaskCDS, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_EvsInit()
 * ----------------------------------------------------
 */
int32 LC_EvsInit(void)
{
    UT_GenStub_SetupReturnBuffer(LC_EvsInit, int32);

    UT_GenStub_Execute(LC_EvsInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_EvsInit, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_LoadDefaultTables()
 * ----------------------------------------------------
 */
int32 LC_LoadDefaultTables(void)
{
    UT_GenStub_SetupReturnBuffer(LC_LoadDefaultTables, int32);

    UT_GenStub_Execute(LC_LoadDefaultTables, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_LoadDefaultTables, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_SbInit()
 * ----------------------------------------------------
 */
int32 LC_SbInit(void)
{
    UT_GenStub_SetupReturnBuffer(LC_SbInit, int32);

    UT_GenStub_Execute(LC_SbInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_SbInit, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_TableInit()
 * ----------------------------------------------------
 */
int32 LC_TableInit(void)
{
    UT_GenStub_SetupReturnBuffer(LC_TableInit, int32);

    UT_GenStub_Execute(LC_TableInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_TableInit, int32);
}

#endif
