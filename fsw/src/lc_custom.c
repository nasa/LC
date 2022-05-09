/*************************************************************************
** File:
**   $Id: lc_custom.c 1.3 2017/01/22 17:24:49EST sstrege Exp  $
**
**  Copyright (c) 2007-2020 United States Government as represented by the
**  Administrator of the National Aeronautics and Space Administration.
**  All Other Rights Reserved.
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be
**  used, distributed and modified only pursuant to the terms of that
**  agreement.
**
** Purpose:
**   CFS Limit Checker (LC) mission specific code, including the
**   custom function template.
**
**
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "lc_custom.h"
#include "lc_tbldefs.h"
#include "lc_events.h"
#include "lc_mission_cfg.h"
#include "lc_msg.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Initiate an RTS request                                         */
/*                                                                 */
/* NOTE: For complete prolog information, see 'lc_custom.h'        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ExecuteRTS(uint16 RTSId)
{
    LC_RTSRequest_t RTSRequest;
    /**************************************************************/

    CFE_MSG_Init(&RTSRequest.CmdHeader.Msg, LC_RTS_REQ_MID, sizeof(LC_RTSRequest_t));

    CFE_MSG_SetFcnCode(&RTSRequest.CmdHeader.Msg, LC_RTS_REQ_CC);

    RTSRequest.RTSId = RTSId;

    CFE_SB_TransmitMsg(&RTSRequest.CmdHeader.Msg, true);

    return;

} /* end LC_ExecuteRTS */

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

    return (EvalResult);

} /* end LC_CustomFunction */

/************************/
/*  End of File Comment */
/************************/
