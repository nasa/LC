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
 *   Unit specification for the Core Flight System (CFS)
 *   Limit Checker (LC) Application.
 */

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
