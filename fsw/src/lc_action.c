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
 *   Functions used for CFS Limit Checker actionpoint processing
 */

/*************************************************************************
** Includes
*************************************************************************/
#include "lc_app.h"
#include "lc_action.h"
#include "lc_msgids.h"
#include "lc_eventids.h"
#include "lc_custom.h"
#include "lc_platform_cfg.h"

#include <string.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Sample one or all actionpoints                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_SampleAPs(uint16 StartIndex, uint16 EndIndex)
{
    uint16 TableIndex;
    uint8  CurrentAPState;

    /*
    ** Make sure the current state of the starting actionpoint
    ** in the sample is valid for a sample request
    */
    CurrentAPState = LC_OperData.ARTPtr[StartIndex].CurrentState;

    if ((CurrentAPState != LC_ActionPoint_NOT_USED) && (CurrentAPState != LC_ActionPointState_PERMOFF))
    {
        /*
         ** Sample selected actionpoints
         */
        for (TableIndex = StartIndex; TableIndex <= EndIndex; TableIndex++)
        {
            LC_SampleSingleAP(TableIndex);
        }
    }
    else
    {
        /*
        **  Actionpoint isn't currently operational
        */
        CFE_EVS_SendEvent(LC_APSAMPLE_CURR_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Sample AP error, invalid current AP state: AP = %d, State = %d", StartIndex, CurrentAPState);
    }

    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Sample a single actionpoint                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_SampleSingleAP(uint16 APNumber)
{
    uint8 CurrentAPState;
    uint8 PreviousResult;
    uint8 CurrentResult;
    char  EventText[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    /*
    ** We only do the sample if the actionpoint is active
    ** or passive. Other states are ignored since this
    ** routine is called in a loop to process ALL actionpoints.
    */
    CurrentAPState = LC_OperData.ARTPtr[APNumber].CurrentState;

    if ((CurrentAPState == LC_ActionPointState_ACTIVE) || (CurrentAPState == LC_ActionPointState_PASSIVE))
    {
        /*
        ** Evaluate the actionpoint and update the results
        ** as needed
        */
        LC_AppData.APSampleCount++;

        PreviousResult = LC_OperData.ARTPtr[APNumber].ActionResult;

        CurrentResult = LC_EvaluateRPN(APNumber);

        LC_OperData.ARTPtr[APNumber].ActionResult = CurrentResult;

        /*****************************************
        ** If actionpoint fails current evaluation
        ******************************************/
        if (CurrentResult == LC_ACTION_FAIL)
        {
            LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount++;
            LC_OperData.ARTPtr[APNumber].CumulativeFailCount++;

            if (PreviousResult == LC_ACTION_PASS)
            {
                /*
                **  We failed this time, but we passed last time
                */
                LC_OperData.ARTPtr[APNumber].PassToFailCount++;

                /*
                **  Send only a limited number of Pass to Fail events
                */
                if (LC_OperData.ARTPtr[APNumber].PassToFailCount <= LC_OperData.ADTPtr[APNumber].MaxPassFailEvents)
                {
                    CFE_EVS_SendEvent(LC_AP_PASSTOFAIL_INF_EID, CFE_EVS_EventType_INFORMATION,
                                      "AP state change from PASS to FAIL: AP = %d", APNumber);
                    LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent++;
                }
            }

            if (LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount >= LC_OperData.ADTPtr[APNumber].MaxFailsBeforeRTS)
            {
                /*
                ** We have failed enough times to request the RTS
                */
                if (CurrentAPState == LC_ActionPointState_ACTIVE)
                {
                    /*
                    ** Actions go to passive after they've failed
                    */
                    LC_OperData.ARTPtr[APNumber].CurrentState = LC_ActionPointState_PASSIVE;

                    if (LC_AppData.CurrentLCState == LC_AppState_ACTIVE)
                    {
                        /*
                        ** If the LC application state is active, request the
                        ** specified RTS be executed
                        */
                        LC_ExecuteRTS(LC_OperData.ADTPtr[APNumber].RTSId);

                        LC_OperData.ARTPtr[APNumber].CumulativeRTSExecCount++;

                        LC_AppData.RTSExecCount++;

                        /*
                        ** Copy event text specific to this action and
                        ** add our trailer with AP specific info making
                        ** sure we won't exceed our character buffer.
                        */
                        snprintf(EventText, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "%s %s",
                                 LC_OperData.ADTPtr[APNumber].EventText, LC_AP_EVENT_TAIL_STR);

                        CFE_EVS_SendEvent(LC_OperData.ADTPtr[APNumber].EventID, LC_OperData.ADTPtr[APNumber].EventType,
                                          EventText, APNumber, LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount,
                                          LC_OperData.ADTPtr[APNumber].RTSId);

                        LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent++;
                    }
                    else
                    {
                        /*
                        ** The LC application state is passive so we don't
                        ** do the RTS request. We bump the passive RTS execution
                        ** counter and send out a generic event message
                        */
                        LC_AppData.PassiveRTSExecCount++;

                        CFE_EVS_SendEvent(LC_PASSIVE_FAIL_DBG_EID, CFE_EVS_EventType_DEBUG,
                                          "AP failed while LC App passive: AP = %d, FailCount = %d, RTS = %d", APNumber,
                                          (int)LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount,
                                          LC_OperData.ADTPtr[APNumber].RTSId);
                    }
                }
                else
                {
                    /*
                    ** The actionpoint failed while the actionpoint state is passive
                    */
                    LC_OperData.ARTPtr[APNumber].PassiveAPCount++;
                    LC_AppData.PassiveRTSExecCount++;

                    /*
                    **  Send only a limited number of AP is Passive events
                    */
                    if (LC_OperData.ARTPtr[APNumber].PassiveAPCount <= LC_OperData.ADTPtr[APNumber].MaxPassiveEvents)
                    {
                        CFE_EVS_SendEvent(LC_AP_PASSIVE_FAIL_INF_EID, CFE_EVS_EventType_INFORMATION,
                                          "AP failed while passive: AP = %d, FailCount = %d, RTS = %d", APNumber,
                                          (int)LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount,
                                          LC_OperData.ADTPtr[APNumber].RTSId);
                        LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent++;
                    }
                }

            } /* end (ConsecutiveFailCount >= MaxFailsBeforeRTS) if */

        } /* end (CurrentResult == LC_ACTION_FAIL) if */

        /******************************************
        ** If actionpoint passes current evaluation
        *******************************************/
        else if (CurrentResult == LC_ACTION_PASS)
        {
            if (PreviousResult == LC_ACTION_FAIL)
            {
                /*
                **  We passed this time, but we failed last time
                */
                LC_OperData.ARTPtr[APNumber].FailToPassCount++;

                /*
                **  Send only a limited number of Fail to Pass events
                */
                if (LC_OperData.ARTPtr[APNumber].FailToPassCount <= LC_OperData.ADTPtr[APNumber].MaxFailPassEvents)
                {
                    CFE_EVS_SendEvent(LC_AP_FAILTOPASS_INF_EID, CFE_EVS_EventType_INFORMATION,
                                      "AP state change from FAIL to PASS: AP = %d", APNumber);
                    LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent++;
                }
            }
            /*
            ** Clear consecutive failure counter for this AP
            */
            LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount = 0;
        }

        /*
        ** If actionpoint is not measured or has gone stale
        */
        else if (CurrentResult == LC_ACTION_STALE)
        {
            /*
            ** Make sure the consecutive fail count is zeroed
            */
            LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount = 0;
        }
        else
        {
            /*
            ** We got back a LC_ACTION_ERROR result, send event
            */
            CFE_EVS_SendEvent(LC_ACTION_ERROR_ERR_EID, CFE_EVS_EventType_ERROR,
                              "AP evaluated to error: AP = %d, Result = %d", APNumber, CurrentResult);
        }

    } /* end CurrentAPState if */

    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Evaluate the Reverse Polish Notation (RPN) equation for an      */
/* actionpoint                                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * StackPtr is an index into an array RPNStack (see LC_EvaluateRPN)
 * which contains values to be used in an RPN function.  This macro
 * validates the StackPtr to confirm that another operand is present
 * in the RPMStack array, then returns the next element.
 */
#define POP_RPN_DATA ((StackPtr <= 0) ? (IllegalRPN = true) : (RPNStack[--StackPtr]))
uint8 LC_EvaluateRPN(uint16 APNumber)
{
    bool   Done;
    bool   IllegalRPN;
    bool   IllegalOperand;
    uint8  EvalResult = LC_ACTION_ERROR;
    int32  RPNEquationPtr;
    int32  StackPtr;
    uint16 RPNData;
    uint16 RPNStack[LC_MAX_RPN_EQU_SIZE];
    uint16 Operand1;
    uint16 Operand2;

    StackPtr       = 0;
    RPNEquationPtr = 0;
    IllegalRPN     = false;
    IllegalOperand = false;
    Done           = false;

    /*
    ** Keep going till we're done or till we get an error
    */
    while ((Done == false) && (IllegalRPN == false) && (IllegalOperand == false))
    {
        /*
        **  Pick off each piece of the RPN equation and handle
        **  one at a time
        */
        RPNData = LC_OperData.ADTPtr[APNumber].RPNEquation[RPNEquationPtr++];

        /*
        **  The data is either an RPN operator or a watchpoint number
        */
        switch (RPNData)
        {
            /*
            **  If the data is an RPN operator then each operand is
            **  either a watchpoint evaluation result or it is the
            **  result of a previous RPN operation
            */
            case LC_RPN_AND:
                Operand2 = POP_RPN_DATA;
                Operand1 = POP_RPN_DATA;
                if ((Operand1 == LC_WATCH_FALSE) || (Operand2 == LC_WATCH_FALSE))
                {
                    RPNStack[StackPtr++] = LC_WATCH_FALSE;
                }
                else if ((Operand1 == LC_WATCH_ERROR) || (Operand2 == LC_WATCH_ERROR))
                {
                    RPNStack[StackPtr++] = LC_WATCH_ERROR;
                }
                else if ((Operand1 == LC_WATCH_STALE) || (Operand2 == LC_WATCH_STALE))
                {
                    RPNStack[StackPtr++] = LC_WATCH_STALE;
                }
                else
                {
                    RPNStack[StackPtr++] = LC_WATCH_TRUE;
                }
                break;

            case LC_RPN_OR:
                Operand2 = POP_RPN_DATA;
                Operand1 = POP_RPN_DATA;
                if ((Operand1 == LC_WATCH_TRUE) || (Operand2 == LC_WATCH_TRUE))
                {
                    RPNStack[StackPtr++] = LC_WATCH_TRUE;
                }
                else if ((Operand1 == LC_WATCH_ERROR) || (Operand2 == LC_WATCH_ERROR))
                {
                    RPNStack[StackPtr++] = LC_WATCH_ERROR;
                }
                else if ((Operand1 == LC_WATCH_STALE) || (Operand2 == LC_WATCH_STALE))
                {
                    RPNStack[StackPtr++] = LC_WATCH_STALE;
                }
                else
                {
                    RPNStack[StackPtr++] = LC_WATCH_FALSE;
                }
                break;

            case LC_RPN_XOR:
                Operand2 = POP_RPN_DATA;
                Operand1 = POP_RPN_DATA;
                if ((Operand1 == LC_WATCH_ERROR) || (Operand2 == LC_WATCH_ERROR))
                {
                    RPNStack[StackPtr++] = LC_WATCH_ERROR;
                }
                else if ((Operand1 == LC_WATCH_STALE) || (Operand2 == LC_WATCH_STALE))
                {
                    RPNStack[StackPtr++] = LC_WATCH_STALE;
                }
                else
                {
                    RPNStack[StackPtr++] = (Operand1 != Operand2);
                }
                break;

            case LC_RPN_NOT:
                Operand1 = POP_RPN_DATA;
                if (Operand1 == LC_WATCH_ERROR)
                {
                    RPNStack[StackPtr++] = LC_WATCH_ERROR;
                }
                else if (Operand1 == LC_WATCH_STALE)
                {
                    RPNStack[StackPtr++] = LC_WATCH_STALE;
                }
                else
                {
                    RPNStack[StackPtr++] = (Operand1 == LC_WATCH_FALSE);
                }
                break;

            case LC_RPN_EQUAL:
                EvalResult = POP_RPN_DATA;
                if ((EvalResult == LC_WATCH_ERROR) || (EvalResult == LC_WATCH_STALE))
                {
                    IllegalOperand = true;
                }
                else if (StackPtr == 0)
                {
                    Done = true;
                }
                else
                {
                    IllegalRPN = true;
                }
                break;

            /*
            **  If the data is a watchpoint number then the operand is
            **  the current value of that watchpoint result
            */
            default:
                if (RPNData < LC_MAX_WATCHPOINTS)
                {
                    RPNStack[StackPtr++] = LC_OperData.WRTPtr[RPNData].WatchResult;
                }
                else
                {
                    IllegalRPN = true;
                }
                break;

        } /* end switch */

        /*
        ** If still not done and have no errors - check for the end of the buffer
        */
        if ((Done == false) && (IllegalRPN == false) && (IllegalOperand == false))
        {
            if (RPNEquationPtr >= LC_MAX_RPN_EQU_SIZE)
            {
                IllegalRPN = true;
            }
        }

    } /* end while */

    if (IllegalRPN == true)
    {
        CFE_EVS_SendEvent(LC_INVALID_RPN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "AP has illegal RPN expression: AP = %d, LastOperand = %d, StackPtr = %d", APNumber,
                          (int)(RPNEquationPtr - 1), (int)StackPtr);

        EvalResult = LC_ACTION_ERROR;
    }
    else if (EvalResult == LC_WATCH_ERROR)
    {
        EvalResult = LC_ACTION_ERROR;
    }
    else if (EvalResult == LC_WATCH_STALE)
    {
        EvalResult = LC_ACTION_STALE;
    }
    else if (EvalResult == LC_WATCH_FALSE)
    {
        EvalResult = LC_ACTION_PASS;
    }
    else if (EvalResult == LC_WATCH_TRUE)
    {
        EvalResult = LC_ACTION_FAIL;
    }

    return EvalResult;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Validate the actionpoint definition table (ADT)                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 LC_ValidateADT(void *TableData)
{
    LC_ADTEntry_t *TableArray = (LC_ADTEntry_t *)TableData;

    int32 EntryResult = LC_ADTVAL_ERR_NONE;
    int32 TableResult = CFE_SUCCESS;
    int32 TableIndex;

    uint8  DefaultState;
    uint16 RTSId;
    uint16 MaxFailsBeforeRTS;
    uint16 EventType;

    uint16 *RPNPtr;
    int32   RPNIndex      = 0;
    int32   RPNStackDepth = 0;

    int32 GoodCount   = 0;
    int32 BadCount    = 0;
    int32 UnusedCount = 0;

    /*
    ** Verify each entry in the pending actionpoint definition table
    */
    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        DefaultState      = TableArray[TableIndex].DefaultState;
        RTSId             = TableArray[TableIndex].RTSId;
        MaxFailsBeforeRTS = TableArray[TableIndex].MaxFailsBeforeRTS;
        RPNPtr            = TableArray[TableIndex].RPNEquation;
        EventType         = TableArray[TableIndex].EventType;

        if (DefaultState == LC_ActionPoint_NOT_USED)
        {
            /*
            ** Unused table entry
            */
            UnusedCount++;
        }
        else if ((DefaultState != LC_ActionPointState_ACTIVE) && (DefaultState != LC_ActionPointState_PASSIVE) &&
                 (DefaultState != LC_ActionPointState_DISABLED) && (DefaultState != LC_ActionPointState_PERMOFF))
        {
            /*
            ** Invalid default state
            */
            BadCount++;
            EntryResult = LC_ADTVAL_ERR_DEFSTATE;
        }
        else if (RTSId > LC_MAX_VALID_ADT_RTSID)
        {
            /*
            ** Bad RTS ID (limit set by configuration parameter,
            ** see lc_platform_cfg.h)
            */
            BadCount++;
            EntryResult = LC_ADTVAL_ERR_RTSID;
        }
        else if (MaxFailsBeforeRTS == 0)
        {
            /*
            ** Bad fail count
            */
            BadCount++;
            EntryResult = LC_ADTVAL_ERR_FAILCNT;
        }
        else if ((EventType != CFE_EVS_EventType_DEBUG) && (EventType != CFE_EVS_EventType_INFORMATION) &&
                 (EventType != CFE_EVS_EventType_ERROR) && (EventType != CFE_EVS_EventType_CRITICAL))
        {
            /*
            ** Invalid event type
            */
            BadCount++;
            EntryResult = LC_ADTVAL_ERR_EVTTYPE;
        }
        else
        {
            /*
            ** Validate reverse polish equation syntax
            */
            EntryResult = LC_ValidateRPN(RPNPtr, &RPNIndex, &RPNStackDepth);

            if (EntryResult != LC_ADTVAL_ERR_NONE)
            {
                BadCount++;
            }
            else
            {
                GoodCount++;
            }
        }

        /*
        ** Generate detailed event for "first" error
        */
        if ((EntryResult != LC_ADTVAL_ERR_NONE) && (TableResult == CFE_SUCCESS))
        {
            if (EntryResult == LC_ADTVAL_ERR_RPN)
            {
                CFE_EVS_SendEvent(LC_ADTVAL_RPNERR_EID, CFE_EVS_EventType_ERROR,
                                  "ADT verify RPN err: AP = %d, Index = %d, StackDepth = %d", (int)TableIndex,
                                  (int)RPNIndex, (int)RPNStackDepth);
            }
            else
            {
                CFE_EVS_SendEvent(LC_ADTVAL_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "ADT verify err: AP = %d, Err = %d, State = %d, RTS = %d, FailCnt = %d, EvtType = %d",
                                  (int)TableIndex, (int)EntryResult, DefaultState, RTSId, MaxFailsBeforeRTS, EventType);
            }

            TableResult = EntryResult;
        }

    } /* end TableIndex for */

    /*
    ** Generate informational event with error totals
    */
    CFE_EVS_SendEvent(LC_ADTVAL_INF_EID, CFE_EVS_EventType_INFORMATION,
                      "ADT verify results: good = %d, bad = %d, unused = %d", (int)GoodCount, (int)BadCount,
                      (int)UnusedCount);

    return TableResult;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Validate a reverse polish notation (RPN) equation               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 LC_ValidateRPN(const uint16 *RPNPtr, int32 *IndexValue, int32 *StackDepthValue)
{
    int32  Result = LC_ADTVAL_ERR_NONE;
    int32  BufferIndex;
    int32  StackDepth;
    uint16 BufferItem;

    /*
    ** Each equation consists only of watchpoint ID numbers
    ** and polish symbols (and, or, equal, etc).
    **
    ** Each watchpoint ID increases the stack depth and
    ** each polish symbol (except "not" which has no
    ** effect) decreases the stack depth.
    **
    ** The stack depth must never become negative and the
    ** depth must equal 1 when the "=" symbol is found.
    **
    ** The last symbol in the equation is assumed to be
    ** the "=" symbol, and the test does not examine any
    ** further data.
    */
    StackDepth = 0;

    for (BufferIndex = 0; BufferIndex < LC_MAX_RPN_EQU_SIZE; BufferIndex++)
    {
        BufferItem = RPNPtr[BufferIndex];

        if ((BufferItem == LC_RPN_AND) || (BufferItem == LC_RPN_OR) || (BufferItem == LC_RPN_XOR))
        {
            /*
            ** Depth test will fail if we haven't already counted 2
            ** watchpoint ID values
            */
            StackDepth--;
        }
        else if (BufferItem == LC_RPN_NOT)
        {
            /*
            ** Depth test will fail if this is 1st symbol
            */
        }
        else if (BufferItem == LC_RPN_EQUAL)
        {
            /*
            ** Equation ends when LC_RPN_EQUAL is found
            */
            break;
        }
        else if (BufferItem < LC_MAX_WATCHPOINTS)
        {
            /*
            ** Valid watchpoint ID
            */
            StackDepth++;
        }
        else
        {
            /*
            ** Not a valid polish symbol or watchpoint ID
            */
            break;
        }

        /*
        ** Must have more watchpoint ID's than polish symbols
        */
        if (StackDepth <= 0)
        {
            break;
        }
    }

    /*
    ** Check for valid Reverse Polish Notation equation
    */
    if ((BufferItem == LC_RPN_EQUAL) && (StackDepth == 1))
    {
        Result = LC_ADTVAL_ERR_NONE;
    }
    else
    {
        Result = LC_ADTVAL_ERR_RPN;

        /*
        ** Provide additional error information
        */
        *IndexValue      = BufferIndex;
        *StackDepthValue = StackDepth;
    }

    return Result;
}
