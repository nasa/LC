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
 *   CFS Limit Checker (LC) command handling routines
 */

/************************************************************************
** Includes
*************************************************************************/
#include "lc_app.h"
#include "lc_cmds.h"
#include "lc_msgids.h"
#include "lc_eventids.h"
#include "lc_version.h"
#include "lc_action.h"
#include "lc_watch.h"
#include "lc_platform_cfg.h"
#include "lc_utils.h"

#define LC_GET_CMD_PAYLOAD(ptr, type) (&((const type *)(ptr))->Payload)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Sample Actionpoints Request                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_SampleAPReq(const CFE_SB_Buffer_t *BufPtr)
{
    const LC_SampleAP_Payload_t *LC_SampleAP;
    uint16                       WatchIndex;
    bool                         ValidSampleCmd = false;

    LC_SampleAP = LC_GET_CMD_PAYLOAD(BufPtr, LC_SampleAPCmd_t);

    /*
    ** Ignore AP sample requests if disabled at the application level
    */
    if (LC_AppData.CurrentLCState != LC_STATE_DISABLED)
    {
        /*
        ** Range check the actionpoint array index arguments
        */
        if ((LC_SampleAP->StartIndex == LC_ALL_ACTIONPOINTS) && (LC_SampleAP->EndIndex == LC_ALL_ACTIONPOINTS))
        {
            /*
            ** Allow special "sample all" heritage values
            */
            LC_SampleAPs(0, LC_MAX_ACTIONPOINTS - 1);
            ValidSampleCmd = true;
        }
        else if ((LC_SampleAP->StartIndex <= LC_SampleAP->EndIndex) && (LC_SampleAP->EndIndex < LC_MAX_ACTIONPOINTS))
        {
            /*
            ** Start is less or equal to end, and end is within the array
            */
            LC_SampleAPs(LC_SampleAP->StartIndex, LC_SampleAP->EndIndex);
            ValidSampleCmd = true;
        }
        else
        {
            /*
            ** At least one actionpoint array index is out of range
            */
            CFE_EVS_SendEvent(LC_APSAMPLE_APNUM_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Sample AP error: invalid AP number, start = %d, end = %d", LC_SampleAP->StartIndex,
                              LC_SampleAP->EndIndex);
        }

        /*
        ** Optionally update the age of watchpoint results
        */
        if ((LC_SampleAP->UpdateAge != 0) && (ValidSampleCmd))
        {
            for (WatchIndex = 0; WatchIndex < LC_MAX_WATCHPOINTS; WatchIndex++)
            {
                if (LC_OperData.WRTPtr[WatchIndex].CountdownToStale > 0)
                {
                    LC_OperData.WRTPtr[WatchIndex].CountdownToStale--;

                    if (LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0)
                    {
                        LC_OperData.WRTPtr[WatchIndex].WatchResult = LC_WATCH_STALE;
                    }
                }
            }
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Housekeeping request                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t LC_SendHkCmd(const CFE_MSG_CommandHeader_t *MsgPtr)
{
    uint16 TableIndex;
    uint16 HKIndex;
    uint8  ByteData = 0;
    uint8  Shift;
    uint8  TempByteData;
    uint8  i;

    LC_HkTlm_Payload_t *PayloadPtr;

    /* Update HK variables */
    PayloadPtr = &LC_OperData.HkPacket.Payload;

    PayloadPtr->CmdCount            = LC_AppData.CmdCount;
    PayloadPtr->CmdErrCount         = LC_AppData.CmdErrCount;
    PayloadPtr->APSampleCount       = LC_AppData.APSampleCount;
    PayloadPtr->MonitoredMsgCount   = LC_AppData.MonitoredMsgCount;
    PayloadPtr->RTSExecCount        = LC_AppData.RTSExecCount;
    PayloadPtr->PassiveRTSExecCount = LC_AppData.PassiveRTSExecCount;
    PayloadPtr->CurrentLCState      = LC_AppData.CurrentLCState;
    PayloadPtr->WPsInUse            = LC_OperData.WatchpointCount;

    /* Clear out the active actionpoint count, it will get recomputed below */
    PayloadPtr->ActiveAPs = 0;

    /* Update packed watch results (4 watch results in one 8-bit byte) */
    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex += 4)
    {
        HKIndex = TableIndex / 4;

        /* Pack in the results 2-bits at a time to ByteData */
        for (i = 0; i < 4; i++)
        {
            Shift = 6 - (i * 2);
            switch (LC_OperData.WRTPtr[TableIndex + 3 - i].WatchResult)
            {
                case LC_WATCH_STALE:
                    TempByteData = LC_HKWR_STALE << Shift;
                    break;

                case LC_WATCH_FALSE:
                    TempByteData = LC_HKWR_FALSE << Shift;
                    break;

                case LC_WATCH_TRUE:
                    TempByteData = LC_HKWR_TRUE << Shift;
                    break;

                /* We should never get an undefined watch result, but set an error result if we do */
                case LC_WATCH_ERROR:
                default:
                    TempByteData = LC_HKWR_ERROR << Shift;
                    break;
            }

            /* Directly assign 1st result, but logical OR the 2nd, 3rd and 4th results */
            if (i == 0)
                ByteData = TempByteData;
            else
                ByteData |= TempByteData;
        }

        /* Update housekeeping watch results array */
        /* SAD: HKIndex is derived from TableIndex, ensuring it stays within the bounds of the WPResults array */
        PayloadPtr->WPResults[HKIndex] = ByteData;
    }

    /* Update packed action results (2 action state/result pairs (4 bits each) in one 8-bit byte) */
    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex += 2)
    {
        HKIndex = TableIndex / 2;

        /* Pack in 1st and 2nd actionpoints, current state */
        for (i = 0; i < 2; i++)
        {
            Shift = 6 - (i * 4);
            switch (LC_OperData.ARTPtr[TableIndex + 1 - i].CurrentState)
            {
                case LC_APSTATE_NOT_USED:
                    TempByteData = LC_HKAR_STATE_NOT_USED << Shift;
                    break;

                case LC_APSTATE_ACTIVE:
                    TempByteData = LC_HKAR_STATE_ACTIVE << Shift;
                    PayloadPtr->ActiveAPs++;
                    break;

                case LC_APSTATE_PASSIVE:
                    TempByteData = LC_HKAR_STATE_PASSIVE << Shift;
                    break;

                case LC_APSTATE_DISABLED:
                    TempByteData = LC_HKAR_STATE_DISABLED << Shift;
                    break;

                /*
                ** Permanantly disabled actionpoints get reported as unused. We should
                ** never get an undefined action state, but set to NOT_USED if we do.
                */
                case LC_APSTATE_PERMOFF:
                default:
                    TempByteData = LC_HKAR_STATE_NOT_USED << Shift;
                    break;
            }

            /* Directly assign 1st state, but logical OR the 2nd state */
            if (i == 0)
                ByteData = TempByteData;
            else
                ByteData |= TempByteData;

            /* Pack in 1st and 2nd actionpoints, action result */
            Shift -= 2;
            switch (LC_OperData.ARTPtr[TableIndex + 1 - i].ActionResult)
            {
                case LC_ACTION_STALE:
                    ByteData |= LC_HKAR_STALE << Shift;
                    break;

                case LC_ACTION_PASS:
                    ByteData |= LC_HKAR_PASS << Shift;
                    break;

                case LC_ACTION_FAIL:
                    ByteData |= LC_HKAR_FAIL << Shift;
                    break;

                /* We should never get an undefined action result, but set an error result if we do */
                case LC_ACTION_ERROR:
                default:
                    ByteData |= LC_HKAR_ERROR << Shift;
                    break;
            }
        }

        /* Update housekeeping action results array */
        /* SAD: HKIndex is derived from TableIndex, ensuring it stays within the bounds of the APResults array */
        PayloadPtr->APResults[HKIndex] = ByteData;
    }

    /* Timestamp and send housekeeping packet */
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(LC_OperData.HkPacket.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(LC_OperData.HkPacket.TelemetryHeader), true);

    return LC_PerformMaintenance();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Noop command                                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_NoopCmd(const CFE_SB_Buffer_t *BufPtr)
{
    LC_AppData.CmdCount++;

    CFE_EVS_SendEvent(LC_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "No-op command: Version %d.%d.%d.%d",
                      LC_MAJOR_VERSION, LC_MINOR_VERSION, LC_REVISION, LC_MISSION_REV);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset counters command                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ResetCountersCmd(const CFE_SB_Buffer_t *BufPtr)
{
    LC_ResetCounters();

    CFE_EVS_SendEvent(LC_RESET_INF_EID, CFE_EVS_EventType_INFORMATION, "Reset counters command");
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset housekeeping counters                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ResetCounters(void)
{
    LC_AppData.CmdCount    = 0;
    LC_AppData.CmdErrCount = 0;

    LC_AppData.APSampleCount       = 0;
    LC_AppData.MonitoredMsgCount   = 0;
    LC_AppData.RTSExecCount        = 0;
    LC_AppData.PassiveRTSExecCount = 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Set LC state command                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_SetLCStateCmd(const CFE_SB_Buffer_t *BufPtr)
{
    const LC_SetLCState_Payload_t *CmdPtr;

    CmdPtr = LC_GET_CMD_PAYLOAD(BufPtr, LC_SetLCStateCmd_t);

    switch (CmdPtr->NewLCState)
    {
        case LC_STATE_ACTIVE:
        case LC_STATE_PASSIVE:
        case LC_STATE_DISABLED:
            LC_AppData.CurrentLCState = CmdPtr->NewLCState;
            LC_AppData.CmdCount++;

            CFE_EVS_SendEvent(LC_LCSTATE_INF_EID, CFE_EVS_EventType_INFORMATION, "Set LC state command: new state = %d",
                              CmdPtr->NewLCState);
            break;

        default:
            CFE_EVS_SendEvent(LC_LCSTATE_ERR_EID, CFE_EVS_EventType_ERROR, "Set LC state error: invalid state = %d",
                              CmdPtr->NewLCState);

            LC_AppData.CmdErrCount++;
            break;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Set actionpoint state command                                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_SetAPStateCmd(const CFE_SB_Buffer_t *BufPtr)
{
    const LC_SetAPState_Payload_t *CmdPtr;
    uint32                         TableIndex;
    uint8                          CurrentAPState;
    bool                           ValidState = true;
    bool                           CmdSuccess = false;

    CmdPtr = LC_GET_CMD_PAYLOAD(BufPtr, LC_SetAPStateCmd_t);

    /*
    ** Do a sanity check on the new actionpoint state
    ** specified.
    */
    switch (CmdPtr->NewAPState)
    {
        case LC_APSTATE_ACTIVE:
        case LC_APSTATE_PASSIVE:
        case LC_APSTATE_DISABLED:
            break;

        default:
            ValidState = false;
            CFE_EVS_SendEvent(LC_APSTATE_NEW_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Set AP state error: AP = %d, Invalid new state = %d", CmdPtr->APNumber,
                              CmdPtr->NewAPState);

            LC_AppData.CmdErrCount++;
            break;
    }

    /*
    ** Do the rest based on the actionpoint ID we were given
    */
    if (ValidState == true)
    {
        if ((CmdPtr->APNumber) == LC_ALL_ACTIONPOINTS)
        {
            /*
            ** Set all actionpoints to the new state except those that are not
            ** used or set permanently off
            */
            for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
            {
                CurrentAPState = LC_OperData.ARTPtr[TableIndex].CurrentState;

                if ((CurrentAPState != LC_APSTATE_NOT_USED) && (CurrentAPState != LC_APSTATE_PERMOFF))
                {
                    LC_OperData.ARTPtr[TableIndex].CurrentState = CmdPtr->NewAPState;
                }
            }

            /*
            ** Set flag that we succeeded
            */
            CmdSuccess = true;
        }
        else
        {
            if ((CmdPtr->APNumber) < LC_MAX_ACTIONPOINTS)
            {
                TableIndex     = CmdPtr->APNumber;
                CurrentAPState = LC_OperData.ARTPtr[TableIndex].CurrentState;

                if ((CurrentAPState != LC_APSTATE_NOT_USED) && (CurrentAPState != LC_APSTATE_PERMOFF))
                {
                    /*
                    ** Update state for single actionpoint specified
                    */
                    LC_OperData.ARTPtr[TableIndex].CurrentState = CmdPtr->NewAPState;

                    CmdSuccess = true;
                }
                else
                {
                    /*
                    ** Actionpoints that are not used or set permanently
                    ** off can only be changed by a table load
                    */
                    CFE_EVS_SendEvent(LC_APSTATE_CURR_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "Set AP state error: AP = %d, Invalid current AP state = %d", CmdPtr->APNumber,
                                      CurrentAPState);

                    LC_AppData.CmdErrCount++;
                }
            }
            else
            {
                /*
                **  Actionpoint number is out of range
                **  (it's zero based, since it's a table index)
                */
                CFE_EVS_SendEvent(LC_APSTATE_APNUM_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Set AP state error: Invalid AP number = %d", CmdPtr->APNumber);

                LC_AppData.CmdErrCount++;
            }
        }

        /*
        ** Update the command counter and send out event if command
        ** executed
        */
        if (CmdSuccess == true)
        {
            LC_AppData.CmdCount++;

            CFE_EVS_SendEvent(LC_APSTATE_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Set AP state command: AP = %d, New state = %d", CmdPtr->APNumber, CmdPtr->NewAPState);
        }

    } /* end ValidState if */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Set actionpoint permanently off command                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_SetAPPermOffCmd(const CFE_SB_Buffer_t *BufPtr)
{
    const LC_SetAPPermOff_Payload_t *CmdPtr;
    uint32                           TableIndex;
    uint8                            CurrentAPState;

    CmdPtr = LC_GET_CMD_PAYLOAD(BufPtr, LC_SetAPPermOffCmd_t);

    if (((CmdPtr->APNumber) == LC_ALL_ACTIONPOINTS) || ((CmdPtr->APNumber) >= LC_MAX_ACTIONPOINTS))
    {
        /*
        **  Invalid actionpoint number
        **  (This command can't be invoked for all actionpoints)
        */
        CFE_EVS_SendEvent(LC_APOFF_APNUM_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Set AP perm off error: Invalid AP number = %d", CmdPtr->APNumber);

        LC_AppData.CmdErrCount++;
    }
    else
    {
        TableIndex     = CmdPtr->APNumber;
        CurrentAPState = LC_OperData.ARTPtr[TableIndex].CurrentState;

        if (CurrentAPState != LC_APSTATE_DISABLED)
        {
            /*
            ** Actionpoints can only be turned permanently off if
            ** they are currently disabled
            */
            CFE_EVS_SendEvent(LC_APOFF_CURR_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Set AP perm off error, AP NOT Disabled: AP = %d, Current state = %d", CmdPtr->APNumber,
                              CurrentAPState);

            LC_AppData.CmdErrCount++;
        }
        else
        {
            /*
            ** Update state for actionpoint specified
            */
            LC_OperData.ARTPtr[TableIndex].CurrentState = LC_APSTATE_PERMOFF;

            LC_AppData.CmdCount++;

            CFE_EVS_SendEvent(LC_APOFF_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Set AP permanently off command: AP = %d", CmdPtr->APNumber);
        }

    } /* end CmdPtr -> APNumber else */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset actionpoint statistics command                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ResetAPStatsCmd(const CFE_SB_Buffer_t *BufPtr)
{
    const LC_ResetAPStats_Payload_t *CmdPtr;
    bool                             CmdSuccess = false;

    CmdPtr = LC_GET_CMD_PAYLOAD(BufPtr, LC_ResetAPStatsCmd_t);

    /* arg may be one or all AP's */
    if (CmdPtr->APNumber == LC_ALL_ACTIONPOINTS)
    {
        LC_ResetResultsAP(0, LC_MAX_ACTIONPOINTS - 1, true);
        CmdSuccess = true;
    }
    else if (CmdPtr->APNumber < LC_MAX_ACTIONPOINTS)
    {
        LC_ResetResultsAP(CmdPtr->APNumber, CmdPtr->APNumber, true);
        CmdSuccess = true;
    }
    else
    {
        /* arg is out of range (zero based table index) */
        LC_AppData.CmdErrCount++;

        CFE_EVS_SendEvent(LC_APSTATS_APNUM_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Reset AP stats error: invalid AP number = %d", CmdPtr->APNumber);
    }

    if (CmdSuccess == true)
    {
        LC_AppData.CmdCount++;

        CFE_EVS_SendEvent(LC_APSTATS_INF_EID, CFE_EVS_EventType_INFORMATION, "Reset AP stats command: AP = %d",
                          CmdPtr->APNumber);
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset selected AP statistics (utility function)                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ResetResultsAP(uint32 StartIndex, uint32 EndIndex, bool ResetStatsCmd)
{
    uint32 TableIndex;

    /* reset selected entries in actionpoint results table */
    for (TableIndex = StartIndex; TableIndex <= EndIndex; TableIndex++)
    {
        if (!ResetStatsCmd)
        {
            /* reset AP stats command does not modify AP state or most recent test result */
            LC_OperData.ARTPtr[TableIndex].ActionResult = LC_ACTION_STALE;
            LC_OperData.ARTPtr[TableIndex].CurrentState = LC_OperData.ADTPtr[TableIndex].DefaultState;
        }

        LC_OperData.ARTPtr[TableIndex].PassiveAPCount  = 0;
        LC_OperData.ARTPtr[TableIndex].FailToPassCount = 0;
        LC_OperData.ARTPtr[TableIndex].PassToFailCount = 0;

        LC_OperData.ARTPtr[TableIndex].ConsecutiveFailCount    = 0;
        LC_OperData.ARTPtr[TableIndex].CumulativeFailCount     = 0;
        LC_OperData.ARTPtr[TableIndex].CumulativeRTSExecCount  = 0;
        LC_OperData.ARTPtr[TableIndex].CumulativeEventMsgsSent = 0;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset watchpoint statistics command                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ResetWPStatsCmd(const CFE_SB_Buffer_t *BufPtr)
{
    const LC_ResetWPStats_Payload_t *CmdPtr;
    bool                             CmdSuccess = false;

    CmdPtr = LC_GET_CMD_PAYLOAD(BufPtr, LC_ResetWPStatsCmd_t);

    /* arg may be one or all WP's */
    if (CmdPtr->WPNumber == LC_ALL_WATCHPOINTS)
    {
        LC_ResetResultsWP(0, LC_MAX_WATCHPOINTS - 1, true);
        CmdSuccess = true;
    }
    else if (CmdPtr->WPNumber < LC_MAX_WATCHPOINTS)
    {
        LC_ResetResultsWP(CmdPtr->WPNumber, CmdPtr->WPNumber, true);
        CmdSuccess = true;
    }
    else
    {
        /* arg is out of range (zero based table index) */
        LC_AppData.CmdErrCount++;

        CFE_EVS_SendEvent(LC_WPSTATS_WPNUM_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Reset WP stats error: invalid WP number = %d", CmdPtr->WPNumber);
    }

    if (CmdSuccess == true)
    {
        LC_AppData.CmdCount++;

        CFE_EVS_SendEvent(LC_WPSTATS_INF_EID, CFE_EVS_EventType_INFORMATION, "Reset WP stats command: WP = %d",
                          CmdPtr->WPNumber);
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset selected WP statistics (utility function)                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ResetResultsWP(uint32 StartIndex, uint32 EndIndex, bool ResetStatsCmd)
{
    uint32 TableIndex;

    /* reset selected entries in watchpoint results table */
    for (TableIndex = StartIndex; TableIndex <= EndIndex; TableIndex++)
    {
        if (!ResetStatsCmd)
        {
            /* reset WP stats command does not modify most recent test result */
            LC_OperData.WRTPtr[TableIndex].WatchResult      = LC_WATCH_STALE;
            LC_OperData.WRTPtr[TableIndex].CountdownToStale = 0;
        }

        LC_OperData.WRTPtr[TableIndex].EvaluationCount      = 0;
        LC_OperData.WRTPtr[TableIndex].FalseToTrueCount     = 0;
        LC_OperData.WRTPtr[TableIndex].ConsecutiveTrueCount = 0;
        LC_OperData.WRTPtr[TableIndex].CumulativeTrueCount  = 0;

        LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Value                = 0;
        LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Seconds    = 0;
        LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Subseconds = 0;

        LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Value                = 0;
        LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Seconds    = 0;
        LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Subseconds = 0;
    }
}
