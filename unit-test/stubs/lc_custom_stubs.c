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
 * Auto-Generated stub implementations for functions defined in lc_custom header
 */

#include "lc_custom.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for LC_CustomFunction()
 * ----------------------------------------------------
 */
uint8 LC_CustomFunction(uint16 WatchIndex, uint32 ProcessedWPData, const CFE_SB_Buffer_t *BufPtr,
                        uint32 WDTCustomFuncArg)
{
    UT_GenStub_SetupReturnBuffer(LC_CustomFunction, uint8);

    UT_GenStub_AddParam(LC_CustomFunction, uint16, WatchIndex);
    UT_GenStub_AddParam(LC_CustomFunction, uint32, ProcessedWPData);
    UT_GenStub_AddParam(LC_CustomFunction, const CFE_SB_Buffer_t *, BufPtr);
    UT_GenStub_AddParam(LC_CustomFunction, uint32, WDTCustomFuncArg);

    UT_GenStub_Execute(LC_CustomFunction, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_CustomFunction, uint8);
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
