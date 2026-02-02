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
 *   CFS Limit Checker (LC) command dispatch routines
 */

/************************************************************************
** Includes
*************************************************************************/
#include "lc_app.h"
#include "lc_cmds.h"
#include "lc_dispatch.h"
#include "lc_msgids.h"
#include "lc_eventids.h"
#include "lc_platform_cfg.h"
#include "lc_watch.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Verify message packet length                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool LC_VerifyMsgLength(const CFE_MSG_Message_t *msgPtr, size_t ExpectedLength)
{
    bool              result       = true;
    CFE_MSG_FcnCode_t CommandCode  = 0;
    size_t            ActualLength = 0;
    CFE_SB_MsgId_t    MsgId        = CFE_SB_INVALID_MSG_ID;

    /* Static MID values for efficient comparison */
    static CFE_SB_MsgId_t SEND_HK_MID   = CFE_SB_MSGID_RESERVED;
    static CFE_SB_MsgId_t SAMPLE_AP_MID = CFE_SB_MSGID_RESERVED;

    /* Cache the local MID Values here, this avoids repeat lookups */
    if (!CFE_SB_IsValidMsgId(SEND_HK_MID))
    {
        SEND_HK_MID   = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
        SAMPLE_AP_MID = CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID);
    }

    CFE_MSG_GetSize(msgPtr, &ActualLength);

    /*
    ** Verify the message packet length...
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(msgPtr, &MsgId);
        CFE_MSG_GetFcnCode(msgPtr, &CommandCode);

        if (CFE_SB_MsgId_Equal(MsgId, SEND_HK_MID))
        {
            /*
            ** For a bad HK request, just send the event. We only increment
            ** the error counter for ground commands and not internal messages.
            */
            CFE_EVS_SendEvent(LC_HKREQ_LEN_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Invalid HK request msg length: ID = 0x%08lX, CC = %d, Len = %d, Expected = %d",
                              (unsigned long)CFE_SB_MsgIdToValue(MsgId),
                              CommandCode,
                              (int)ActualLength,
                              (int)ExpectedLength);
        }
        else if (CFE_SB_MsgId_Equal(MsgId, SAMPLE_AP_MID))
        {
            /*
            ** Same thing as previous for a bad actionpoint sample request
            */
            CFE_EVS_SendEvent(LC_APSAMPLE_LEN_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Invalid AP sample msg length: ID = 0x%08lX, CC = %d, Len = %d, Expected = %d",
                              (unsigned long)CFE_SB_MsgIdToValue(MsgId),
                              CommandCode,
                              (int)ActualLength,
                              (int)ExpectedLength);
        }
        else
        {
            /*
            ** All other cases, increment error counter
            */
            CFE_EVS_SendEvent(LC_CMD_LEN_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Invalid msg length: ID = 0x%08lX, CC = %d, Len = %d, Expected = %d",
                              (unsigned long)CFE_SB_MsgIdToValue(MsgId),
                              CommandCode,
                              (int)ActualLength,
                              (int)ExpectedLength);
            LC_AppData.CmdErrCount++;
        }

        result = false;
    }

    return result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Sample Actionpoints Request                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_SampleAPVerifyDispatch(const CFE_SB_Buffer_t *BufPtr)
{
    size_t ExpectedLength = sizeof(LC_SampleAPCmd_t);

    /*
    ** Verify message packet length
    */
    if (LC_VerifyMsgLength(&BufPtr->Msg, ExpectedLength))
    {
        LC_SampleAPReq(BufPtr);
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Housekeeping request                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_SendHkVerifyDispatch(const CFE_SB_Buffer_t *BufPtr)
{
    size_t ExpectedLength = sizeof(LC_SendHkCmd_t);

    /*
    ** Verify message packet length
    */
    if (LC_VerifyMsgLength(&BufPtr->Msg, ExpectedLength))
    {
        LC_SendHkCmd((LC_SendHkCmd_t *)BufPtr);
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Noop command                                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_NoopVerifyDispatch(const CFE_SB_Buffer_t *BufPtr)
{
    size_t ExpectedLength = sizeof(LC_NoopCmd_t);

    if (LC_VerifyMsgLength(&BufPtr->Msg, ExpectedLength))
    {
        LC_NoopCmd((LC_NoopCmd_t *)BufPtr);
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset counters command                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ResetCountersVerifyDispatch(const CFE_SB_Buffer_t *BufPtr)
{
    size_t ExpectedLength = sizeof(LC_ResetCountersCmd_t);

    if (LC_VerifyMsgLength(&BufPtr->Msg, ExpectedLength))
    {
        LC_ResetCountersCmd((LC_ResetCountersCmd_t *)BufPtr);
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Set LC state command                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_SetLCStateVerifyDispatch(const CFE_SB_Buffer_t *BufPtr)
{
    size_t ExpectedLength = sizeof(LC_SetLCStateCmd_t);

    if (LC_VerifyMsgLength(&BufPtr->Msg, ExpectedLength))
    {
        LC_SetLCStateCmd((LC_SetLCStateCmd_t *)BufPtr);
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Set actionpoint state command                                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_SetAPStateVerifyDispatch(const CFE_SB_Buffer_t *BufPtr)
{
    size_t ExpectedLength = sizeof(LC_SetAPStateCmd_t);

    if (LC_VerifyMsgLength(&BufPtr->Msg, ExpectedLength))
    {
        LC_SetAPStateCmd((LC_SetAPStateCmd_t *)BufPtr);
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Set actionpoint permanently off command                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_SetAPPermOffVerifyDispatch(const CFE_SB_Buffer_t *BufPtr)
{
    size_t ExpectedLength = sizeof(LC_SetAPPermOffCmd_t);

    if (LC_VerifyMsgLength(&BufPtr->Msg, ExpectedLength))
    {
        LC_SetAPPermOffCmd((LC_SetAPPermOffCmd_t *)BufPtr);
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset actionpoint statistics command                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ResetAPStatsVerifyDispatch(const CFE_SB_Buffer_t *BufPtr)
{
    size_t ExpectedLength = sizeof(LC_ResetAPStatsCmd_t);

    if (LC_VerifyMsgLength(&BufPtr->Msg, ExpectedLength))
    {
        LC_ResetAPStatsCmd((LC_ResetAPStatsCmd_t *)BufPtr);
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset watchpoint statistics command                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ResetWPStatsVerifyDispatch(const CFE_SB_Buffer_t *BufPtr)
{
    size_t ExpectedLength = sizeof(LC_ResetWPStatsCmd_t);

    if (LC_VerifyMsgLength(&BufPtr->Msg, ExpectedLength))
    {
        LC_ResetWPStatsCmd((LC_ResetWPStatsCmd_t *)BufPtr);
    }
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Process LC ground commands                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_AppProcessCmd(const CFE_SB_Buffer_t *BufPtr)
{
    CFE_SB_MsgId_t    MessageID   = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&BufPtr->Msg, &CommandCode);

    switch (CommandCode)
    {
        case LC_NOOP_CC:
            LC_NoopVerifyDispatch(BufPtr);
            break;

        case LC_RESET_CC:
            LC_ResetCountersVerifyDispatch(BufPtr);
            break;

        case LC_SET_LC_STATE_CC:
            LC_SetLCStateVerifyDispatch(BufPtr);
            break;

        case LC_SET_AP_STATE_CC:
            LC_SetAPStateVerifyDispatch(BufPtr);
            break;

        case LC_SET_AP_PERM_OFF_CC:
            LC_SetAPPermOffVerifyDispatch(BufPtr);
            break;

        case LC_RESET_AP_STATS_CC:
            LC_ResetAPStatsVerifyDispatch(BufPtr);
            break;

        case LC_RESET_WP_STATS_CC:
            LC_ResetWPStatsVerifyDispatch(BufPtr);
            break;

        default:
            CFE_MSG_GetMsgId(&BufPtr->Msg, &MessageID);
            CFE_EVS_SendEvent(LC_CC_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Invalid command code: ID = 0x%08lX, CC = %d",
                              (unsigned long)CFE_SB_MsgIdToValue(MessageID),
                              CommandCode);
            LC_AppData.CmdErrCount++;
            break;
    }
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Process a command pipe message                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_AppPipe(const CFE_SB_Buffer_t *BufPtr)
{
    static CFE_SB_MsgId_t CMD_MID       = CFE_SB_MSGID_RESERVED;
    static CFE_SB_MsgId_t SEND_HK_MID   = CFE_SB_MSGID_RESERVED;
    static CFE_SB_MsgId_t SAMPLE_AP_MID = CFE_SB_MSGID_RESERVED;

    CFE_SB_MsgId_t MessageID = CFE_SB_INVALID_MSG_ID;

    /* Cache the local MID values here, this avoids repeat lookups */
    if (!CFE_SB_IsValidMsgId(CMD_MID))
    {
        CMD_MID       = CFE_SB_ValueToMsgId(LC_CMD_MID);
        SEND_HK_MID   = CFE_SB_ValueToMsgId(LC_SEND_HK_MID);
        SAMPLE_AP_MID = CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID);
    }

    CFE_MSG_GetMsgId(&BufPtr->Msg, &MessageID);

    if (CFE_SB_MsgId_Equal(MessageID, SAMPLE_AP_MID))
    {
        /*
        ** Sample actionpoints request
        */
        LC_SampleAPVerifyDispatch(BufPtr);
    }
    else if (CFE_SB_MsgId_Equal(MessageID, SEND_HK_MID))
    {
        /*
        ** Housekeeping telemetry request
        ** (only routine that can return a critical error indicator)
        */
        LC_SendHkVerifyDispatch(BufPtr);
    }
    else if (CFE_SB_MsgId_Equal(MessageID, CMD_MID))
    {
        /*
        ** LC application commands...
        */
        LC_AppProcessCmd(BufPtr);
    }
    else
    {
        /*
        ** All other message ID's should be monitor
        ** packets
        */
        LC_CheckMsgForWPs(MessageID, BufPtr);
    }
}