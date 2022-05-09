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
 *   Specification for the CFS Limit Checker (LC) mission specific
 *   custom function template
 */

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
