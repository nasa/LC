/*************************************************************************
** File:
**   $Id: lc_cmds.h 1.4 2017/01/22 17:24:53EST sstrege Exp  $
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
**   Specification for the CFS Limit Checker (LC) routines that
**   handle command processing
**
** Notes:
**
**
**************************************************************************/
#ifndef _lc_cmds_
#define _lc_cmds_

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/*************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief Process a command pipe message
**
**  \par Description
**       Processes a single software bus command pipe message. Checks
**       the message and command IDs and calls the appropriate routine
**       to handle the message.
**
**       All messageIDs other than #LC_CMD_MID, #LC_SEND_HK_MID, and
**       #LC_SAMPLE_AP_MID are assumed to be monitor packets.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   BufPtr   A #CFE_SB_Buffer_t* pointer that
**                             references the software bus message
**
**  \returns
**  \retcode #CFE_SUCCESS   \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #LC_HousekeepingReq        \endcode
**  \endreturns
**
**  \sa #CFE_SB_RcvMsg
**
*************************************************************************/
int32 LC_AppPipe(const CFE_SB_Buffer_t *BufPtr);

/************************************************************************/
/** \brief Reset HK counters
**
**  \par Description
**       Utility function that resets housekeeping counters to zero
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \sa #LC_ResetCmd
**
*************************************************************************/
void LC_ResetCounters(void);

/************************************************************************/
/** \brief Reset AP results
**
**  \par Description
**       Utility function that resets selected entries in actionpoint results table
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   StartIndex   Start of area to reset (base zero)
**  \param [in]   EndIndex     End of area to reset (base zero)
**  \param [in]   ResetCmd     Reset AP stats command does not reset all fields
**
**  \sa #LC_ResetAPStatsCmd
**
*************************************************************************/
void LC_ResetResultsAP(uint32 StartIndex, uint32 EndIndex, bool ResetCmd);

/************************************************************************/
/** \brief Reset WP results
**
**  \par Description
**       Utility function that resets selected entries in watchpoint results table
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   StartIndex   Start of area to reset (base zero)
**  \param [in]   EndIndex     End of area to reset (base zero)
**  \param [in]   ResetCmd     Reset WP stats command does not reset all fields
**
**  \sa #LC_ResetWPStatsCmd
**
*************************************************************************/
void LC_ResetResultsWP(uint32 StartIndex, uint32 EndIndex, bool ResetCmd);

/************************************************************************/
/** \brief Sample actionpoints request
**
**  \par Description
**       Processes an on-board sample actionpoints request message.
**
**  \par Assumptions, External Events, and Notes:
**       This message does not affect the command execution counter
**
**  \param [in]   BufPtr   A #CFE_SB_Buffer_t* pointer that
**                             references the software bus message
**
*************************************************************************/
void LC_SampleAPReq(const CFE_SB_Buffer_t *BufPtr);

/************************************************************************/
/** \brief Housekeeping request
**
**  \par Description
**       Processes an on-board housekeeping request message.
**
**  \par Assumptions, External Events, and Notes:
**       This message does not affect the command execution counter
**
**  \param [in]   MsgPtr   A #CFE_MSG_CommandHeader_t* pointer that
**                             references the software bus message
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #LC_AcquirePointers     \endcode
**  \endreturns
**
*************************************************************************/
int32 LC_HousekeepingReq(const CFE_MSG_CommandHeader_t *MsgPtr);

/************************************************************************/
/** \brief Noop command
**
**  \par Description
**       Processes a noop ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   BufPtr   A #CFE_SB_Buffer_t* pointer that
**                             references the software bus message
**
**  \sa #LC_NOOP_CC
**
*************************************************************************/
void LC_NoopCmd(const CFE_SB_Buffer_t *BufPtr);

/************************************************************************/
/** \brief Reset counters command
**
**  \par Description
**       Processes a reset counters ground command which will reset
**       the following LC application counters to zero:
**         - Command counter
**         - Command error counter
**         - Actionpoint sample counter
**         - Monitored message counter
**         - RTS execution counter
**         - Passive RTS execution counter
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   BufPtr   A #CFE_SB_Buffer_t* pointer that
**                             references the software bus message
**
**  \sa #LC_RESET_CC
**
*************************************************************************/
void LC_ResetCmd(const CFE_SB_Buffer_t *BufPtr);

/************************************************************************/
/** \brief Set LC state command
**
**  \par Description
**       Processes a set LC application state ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   BufPtr   A #CFE_SB_Buffer_t* pointer that
**                             references the software bus message
**
**  \sa #LC_SET_LC_STATE_CC
**
*************************************************************************/
void LC_SetLCStateCmd(const CFE_SB_Buffer_t *BufPtr);

/************************************************************************/
/** \brief Set AP state command
**
**  \par Description
**       Processes a set actionpoint state ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   BufPtr   A #CFE_SB_Buffer_t* pointer that
**                             references the software bus message
**
**  \sa #LC_SET_AP_STATE_CC
**
*************************************************************************/
void LC_SetAPStateCmd(const CFE_SB_Buffer_t *BufPtr);

/************************************************************************/
/** \brief Set AP permanently off command
**
**  \par Description
**       Processes a set actionpoint permanently off ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   BufPtr   A #CFE_SB_Buffer_t* pointer that
**                             references the software bus message
**
**  \sa #LC_SET_AP_PERMOFF_CC
**
*************************************************************************/
void LC_SetAPPermOffCmd(const CFE_SB_Buffer_t *BufPtr);

/************************************************************************/
/** \brief Reset AP statistics command
**
**  \par Description
**       Processes a reset actionpoint statistics ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   BufPtr   A #CFE_SB_Buffer_t* pointer that
**                             references the software bus message
**
**  \sa #LC_RESET_AP_STATS_CC
**
*************************************************************************/
void LC_ResetAPStatsCmd(const CFE_SB_Buffer_t *BufPtr);

/************************************************************************/
/** \brief Reset WP statistics command
**
**  \par Description
**       Processes a reset watchpoint statistics ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   BufPtr   A #CFE_SB_Buffer_t* pointer that
**                             references the software bus message
**
**  \sa #LC_RESET_WP_STATS_CC
**
*************************************************************************/
void LC_ResetWPStatsCmd(const CFE_SB_Buffer_t *BufPtr);

#endif /* _lc_cmds_ */

/************************/
/*  End of File Comment */
/************************/
