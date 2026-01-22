/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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

/* lc testing includes */
#include "lc_test_utils.h"
#include "lc_dispatch.h"
#include "lc_eds_dispatcher.h"
#include "lc_cmds.h"
#include "lc_msgids.h"
#include "lc_eventids.h"


/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

void lc_dispatch_tests_Setup(void)
{
    LC_Test_Setup();
}

void lc_dispatch_tests_Teardown(void)
{
    LC_Test_TearDown();
}

/*
**********************************************************************************
**          TEST CASE FUNCTIONS
**********************************************************************************
*/

void Test_LC_AppPipe(void)
{
    /*
     * Test Case For:
     * void LC_AppPipe
     */
    CFE_SB_Buffer_t UtBuf;

    UT_SetDeferredRetcode(UT_KEY(CFE_EDSMSG_Dispatch), 1, CFE_SUCCESS);

    memset(&UtBuf, 0, sizeof(UtBuf));
    UtAssert_VOIDCALL(LC_AppPipe(&UtBuf));
}

/*
 * Register the test cases to execute with the unit test tool
 */
void UtTest_Setup(void)
{
    UtTest_Add(Test_LC_AppPipe, lc_dispatch_tests_Setup, lc_dispatch_tests_Teardown, "Test_LC_AppPipe");
}