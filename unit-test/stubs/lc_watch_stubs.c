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
 * Auto-Generated stub implementations for functions defined in lc_watch header
 */

#include "lc_watch.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for LC_AddWatchpoint()
 * ----------------------------------------------------
 */
LC_WatchPtList_t *LC_AddWatchpoint(const CFE_SB_MsgId_t MessageID)
{
    UT_GenStub_SetupReturnBuffer(LC_AddWatchpoint, LC_WatchPtList_t *);

    UT_GenStub_AddParam(LC_AddWatchpoint, const CFE_SB_MsgId_t, MessageID);

    UT_GenStub_Execute(LC_AddWatchpoint, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_AddWatchpoint, LC_WatchPtList_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_CheckMsgForWPs()
 * ----------------------------------------------------
 */
void LC_CheckMsgForWPs(CFE_SB_MsgId_t MessageID, const CFE_SB_Buffer_t *BufPtr)
{
    UT_GenStub_AddParam(LC_CheckMsgForWPs, CFE_SB_MsgId_t, MessageID);
    UT_GenStub_AddParam(LC_CheckMsgForWPs, const CFE_SB_Buffer_t *, BufPtr);

    UT_GenStub_Execute(LC_CheckMsgForWPs, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_CreateHashTable()
 * ----------------------------------------------------
 */
void LC_CreateHashTable(void)
{

    UT_GenStub_Execute(LC_CreateHashTable, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_FloatCompare()
 * ----------------------------------------------------
 */
uint8 LC_FloatCompare(uint16 WatchIndex, LC_MultiType_t *WPMultiType, LC_MultiType_t *CompareMultiType)
{
    UT_GenStub_SetupReturnBuffer(LC_FloatCompare, uint8);

    UT_GenStub_AddParam(LC_FloatCompare, uint16, WatchIndex);
    UT_GenStub_AddParam(LC_FloatCompare, LC_MultiType_t *, WPMultiType);
    UT_GenStub_AddParam(LC_FloatCompare, LC_MultiType_t *, CompareMultiType);

    UT_GenStub_Execute(LC_FloatCompare, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_FloatCompare, uint8);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_GetHashTableIndex()
 * ----------------------------------------------------
 */
uint32 LC_GetHashTableIndex(CFE_SB_MsgId_t MessageID)
{
    UT_GenStub_SetupReturnBuffer(LC_GetHashTableIndex, uint32);

    UT_GenStub_AddParam(LC_GetHashTableIndex, CFE_SB_MsgId_t, MessageID);

    UT_GenStub_Execute(LC_GetHashTableIndex, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_GetHashTableIndex, uint32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_GetSizedWPData()
 * ----------------------------------------------------
 */
bool LC_GetSizedWPData(uint16 WatchIndex, const uint8 *WPDataPtr, uint32 *SizedDataPtr)
{
    UT_GenStub_SetupReturnBuffer(LC_GetSizedWPData, bool);

    UT_GenStub_AddParam(LC_GetSizedWPData, uint16, WatchIndex);
    UT_GenStub_AddParam(LC_GetSizedWPData, const uint8 *, WPDataPtr);
    UT_GenStub_AddParam(LC_GetSizedWPData, uint32 *, SizedDataPtr);

    UT_GenStub_Execute(LC_GetSizedWPData, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_GetSizedWPData, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_OperatorCompare()
 * ----------------------------------------------------
 */
uint8 LC_OperatorCompare(uint16 WatchIndex, uint32 ProcessedWPData)
{
    UT_GenStub_SetupReturnBuffer(LC_OperatorCompare, uint8);

    UT_GenStub_AddParam(LC_OperatorCompare, uint16, WatchIndex);
    UT_GenStub_AddParam(LC_OperatorCompare, uint32, ProcessedWPData);

    UT_GenStub_Execute(LC_OperatorCompare, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_OperatorCompare, uint8);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_ProcessWP()
 * ----------------------------------------------------
 */
void LC_ProcessWP(uint16 WatchIndex, const CFE_SB_Buffer_t *BufPtr, CFE_TIME_SysTime_t Timestamp)
{
    UT_GenStub_AddParam(LC_ProcessWP, uint16, WatchIndex);
    UT_GenStub_AddParam(LC_ProcessWP, const CFE_SB_Buffer_t *, BufPtr);
    UT_GenStub_AddParam(LC_ProcessWP, CFE_TIME_SysTime_t, Timestamp);

    UT_GenStub_Execute(LC_ProcessWP, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_SignedCompare()
 * ----------------------------------------------------
 */
uint8 LC_SignedCompare(uint16 WatchIndex, int32 WPValue, int32 CompareValue)
{
    UT_GenStub_SetupReturnBuffer(LC_SignedCompare, uint8);

    UT_GenStub_AddParam(LC_SignedCompare, uint16, WatchIndex);
    UT_GenStub_AddParam(LC_SignedCompare, int32, WPValue);
    UT_GenStub_AddParam(LC_SignedCompare, int32, CompareValue);

    UT_GenStub_Execute(LC_SignedCompare, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_SignedCompare, uint8);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_UnsignedCompare()
 * ----------------------------------------------------
 */
uint8 LC_UnsignedCompare(uint16 WatchIndex, uint32 WPValue, uint32 CompareValue)
{
    UT_GenStub_SetupReturnBuffer(LC_UnsignedCompare, uint8);

    UT_GenStub_AddParam(LC_UnsignedCompare, uint16, WatchIndex);
    UT_GenStub_AddParam(LC_UnsignedCompare, uint32, WPValue);
    UT_GenStub_AddParam(LC_UnsignedCompare, uint32, CompareValue);

    UT_GenStub_Execute(LC_UnsignedCompare, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_UnsignedCompare, uint8);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_ValidateWDT()
 * ----------------------------------------------------
 */
int32 LC_ValidateWDT(void *TableData)
{
    UT_GenStub_SetupReturnBuffer(LC_ValidateWDT, int32);

    UT_GenStub_AddParam(LC_ValidateWDT, void *, TableData);

    UT_GenStub_Execute(LC_ValidateWDT, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_ValidateWDT, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for LC_WPOffsetValid()
 * ----------------------------------------------------
 */
bool LC_WPOffsetValid(uint16 WatchIndex, const CFE_SB_Buffer_t *BufPtr)
{
    UT_GenStub_SetupReturnBuffer(LC_WPOffsetValid, bool);

    UT_GenStub_AddParam(LC_WPOffsetValid, uint16, WatchIndex);
    UT_GenStub_AddParam(LC_WPOffsetValid, const CFE_SB_Buffer_t *, BufPtr);

    UT_GenStub_Execute(LC_WPOffsetValid, Basic, NULL);

    return UT_GenStub_GetReturnValue(LC_WPOffsetValid, bool);
}
