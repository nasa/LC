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
 * Auto-Generated stub implementations for functions defined in lc_utils header
 */

#include "lc_utils.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for LC_ManageTables()
 * ----------------------------------------------------
 */
CFE_Status_t LC_ManageTables(void)
{
    UT_GenStub_SetupReturnBuffer(LC_ManageTables, CFE_Status_t);

    UT_GenStub_Execute(LC_ManageTables, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_ManageTables, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_PerformMaintenance()
 * ----------------------------------------------------
 */
CFE_Status_t LC_PerformMaintenance(void)
{
    UT_GenStub_SetupReturnBuffer(LC_PerformMaintenance, CFE_Status_t);

    UT_GenStub_Execute(LC_PerformMaintenance, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_PerformMaintenance, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_UpdateTaskCDS()
 * ----------------------------------------------------
 */
CFE_Status_t LC_UpdateTaskCDS(void)
{
    UT_GenStub_SetupReturnBuffer(LC_UpdateTaskCDS, CFE_Status_t);

    UT_GenStub_Execute(LC_UpdateTaskCDS, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_UpdateTaskCDS, CFE_Status_t);
}
