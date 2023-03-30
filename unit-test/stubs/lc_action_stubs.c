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
 * Auto-Generated stub implementations for functions defined in lc_action header
 */

#include "lc_action.h"
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
