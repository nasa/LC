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
 *   CFS Limit Checker (LC) mission specific code, including the
 *   custom function template.
 */

/*************************************************************************
** Includes
*************************************************************************/
#include "lc_custom.h"
#include "lc_tbldefs.h"
#include "lc_eventids.h"
#include "lc_mission_cfg.h"
#include "lc_msg.h"
#include "lc_msgids.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Initiate an RTS request                                         */
/*                                                                 */
/* NOTE: For complete prolog information, see 'lc_custom.h'        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ExecuteRTS(uint16 RTSId)
{
    LC_RTSRequestCmd_t RTSRequest;

    memset(&RTSRequest, 0, sizeof(RTSRequest));

    CFE_MSG_Init(CFE_MSG_PTR(RTSRequest.CommandHeader), CFE_SB_ValueToMsgId(LC_RTS_REQ_MID), sizeof(RTSRequest));
    CFE_MSG_SetFcnCode(CFE_MSG_PTR(RTSRequest.CommandHeader), LC_RTS_REQ_CC);

    RTSRequest.Payload.RTSId = RTSId;

    CFE_SB_TransmitMsg(CFE_MSG_PTR(RTSRequest.CommandHeader), true);

    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Mission specific custom function entry point                    */
/*                                                                 */
/* NOTE: For complete prolog information, see 'lc_custom.h'        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint8 LC_CustomFunction(uint16 WatchIndex, uint32 ProcessedWPData, const CFE_SB_Buffer_t *BufPtr,
                        uint32 WDTCustomFuncArg)
{
    uint8 EvalResult = LC_WATCH_FALSE;

    /*
    ** This function is the entry point for all watchpoints where
    ** the OperatorID in the watchpoint definition table entry
    ** is set to LC_OPER_CUSTOM.
    **
    ** For this reason The first step would normally be to
    ** switch on the WatchIndex to figure out what watchpoint got
    ** us here. As an alternate, a mission may choose to use the
    ** WDTCustomFuncArg for this instead.
    */
    switch (WatchIndex)
    {
        case 0x0000:
        case 0x0001:
        default:
            CFE_EVS_SendEvent(LC_CFCALL_ERR_EID, CFE_EVS_EventType_ERROR, "Unexpected LC_CustomFunction call: WP = %d",
                              WatchIndex);
            break;

    } /* end WatchIndex switch */

    return EvalResult;
}
