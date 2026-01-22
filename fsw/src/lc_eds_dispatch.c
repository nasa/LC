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

/**
 * @file
 *   This file contains the source code for the CFS Limit Checker (LC).
 */

/************************************************************************
** Includes
*************************************************************************/

#include "lc_app.h"
#include "lc_dispatch.h"
#include "lc_cmds.h"
#include "lc_eventids.h"
#include "lc_msgids.h"
#include "lc_msg.h"

#include "lc_eds_dispatcher.h"
#include "lc_eds_dictionary.h"

/*
 * Define a lookup table for LC app command codes
 */
/* clang-format off */
static const EdsDispatchTable_EdsComponent_LC_Application_CFE_SB_Telecommand_t LC_TC_DISPATCH_TABLE = 
{
    .CMD =
        {
            .NoopCmd_indication              = LC_NoopCmd,
            .ResetCountersCmd_indication     = LC_ResetCountersCmd,
            .SetLCStateCmd_indication        = LC_SetLCStateCmd,
            .SetAPStateCmd_indication        = LC_SetAPStateCmd,
            .SetAPPermOffCmd_indication      = LC_SetAPPermOffCmd,
            .ResetAPStatsCmd_indication      = LC_ResetAPStatsCmd,
            .ResetWPStatsCmd_indication      = LC_ResetWPStatsCmd,
        },
    .SEND_HK =
    {
        .indication = LC_SendHkCmd
    }
};
/* clang-format on */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* LC_AppPipe() -- Process command pipe message                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_AppPipe(const CFE_SB_Buffer_t *BufPtr)
{
    CFE_Status_t      Status;
    CFE_SB_MsgId_t    MsgId;
    CFE_MSG_Size_t    MsgSize;
    CFE_MSG_FcnCode_t MsgFc;

    Status = EdsDispatch_EdsComponent_LC_Application_Telecommand(BufPtr, &LC_TC_DISPATCH_TABLE);

    if (Status != CFE_SUCCESS)
    {
        CFE_MSG_GetMsgId(&BufPtr->Msg, &MsgId);
        CFE_MSG_GetSize(&BufPtr->Msg, &MsgSize);
        CFE_MSG_GetFcnCode(&BufPtr->Msg, &MsgFc);
        ++LC_AppData.CmdErrCount;

        if (Status == CFE_STATUS_UNKNOWN_MSG_ID)
        {
            CFE_EVS_SendEvent(LC_CC_ERR_EID, CFE_EVS_EventType_ERROR,
                              "SAMPLE: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
        }
        else if (Status == CFE_STATUS_WRONG_MSG_LENGTH)
        {
            CFE_EVS_SendEvent(LC_CMD_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)MsgFc, (unsigned int)MsgSize);
        }
        else
        {
            CFE_EVS_SendEvent(LC_CC_ERR_EID, CFE_EVS_EventType_ERROR,
                              "SAMPLE: Invalid ground command code: CC = %d", (int)MsgFc);
        }
    }
}