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
 *   Specification for the CFS Limit Checker (LC) routines that
 *   handle command processing
 */
#ifndef LC_CMDS_H
#define LC_CMDS_H

/*************************************************************************
 * Includes
 *************************************************************************/
#include "cfe.h"

/*************************************************************************
 * Exported Functions
 *************************************************************************/

/**
 * \brief Reset HK counters
 *
 *  \par Description
 *       Utility function that resets housekeeping counters to zero
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \sa #LC_ResetCountersCmd
 */
void LC_ResetCounters(void);

/**
 * \brief Reset AP results
 *
 *  \par Description
 *       Utility function that resets selected entries in actionpoint results table
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]   StartIndex    Start of area to reset (base zero)
 *  \param [in]   EndIndex      End of area to reset (base zero)
 *  \param [in]   ResetStatsCmd Reset AP stats command does not reset all fields
 *
 *  \sa #LC_ResetAPStatsCmd
 */
void LC_ResetResultsAP(uint32 StartIndex, uint32 EndIndex, bool ResetStatsCmd);

/**
 * \brief Reset WP results
 *
 *  \par Description
 *       Utility function that resets selected entries in watchpoint results table
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]   StartIndex    Start of area to reset (base zero)
 *  \param [in]   EndIndex      End of area to reset (base zero)
 *  \param [in]   ResetStatsCmd Reset WP stats command does not reset all fields
 *
 *  \sa #LC_ResetWPStatsCmd
 */
void LC_ResetResultsWP(uint32 StartIndex, uint32 EndIndex, bool ResetStatsCmd);

/**
 * \brief Sample actionpoints request
 *
 *  \par Description
 *       Processes an on-board sample actionpoints request message.
 *
 *  \par Assumptions, External Events, and Notes:
 *       This message does not affect the command execution counter
 *
 *  \param[in] BufPtr Pointer to Software Bus buffer
 */
void LC_SampleAPReq(const CFE_SB_Buffer_t *BufPtr);

/**
 * \brief Housekeeping request
 *
 *  \par Description
 *       Processes an on-board housekeeping request message.
 *
 *  \par Assumptions, External Events, and Notes:
 *       This message does not affect the command execution counter
 *
 *  \param[in] MsgPtr Pointer to command header
 *
 */
CFE_Status_t LC_SendHkCmd(const CFE_MSG_CommandHeader_t *MsgPtr);

/**
 * \brief Noop command
 *
 *  \par Description
 *       Processes a noop ground command.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] BufPtr Pointer to Software Bus buffer
 *
 *  \sa #LC_NOOP_CC
 */
void LC_NoopCmd(const CFE_SB_Buffer_t *BufPtr);

/**
 * \brief Reset counters command
 *
 *  \par Description
 *       Processes a reset counters ground command which will reset
 *       the following LC application counters to zero:
 *         - Command counter
 *         - Command error counter
 *         - Actionpoint sample counter
 *         - Monitored message counter
 *         - RTS execution counter
 *         - Passive RTS execution counter
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] BufPtr Pointer to Software Bus buffer
 *
 *  \sa #LC_RESET_CC
 */
void LC_ResetCountersCmd(const CFE_SB_Buffer_t *BufPtr);

/**
 * \brief Set LC state command
 *
 *  \par Description
 *       Processes a set LC application state ground command.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] BufPtr Pointer to Software Bus buffer
 *
 *  \sa #LC_SET_LC_STATE_CC
 */
void LC_SetLCStateCmd(const CFE_SB_Buffer_t *BufPtr);

/**
 * \brief Set AP state command
 *
 *  \par Description
 *       Processes a set actionpoint state ground command.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] BufPtr Pointer to Software Bus buffer
 *
 *  \sa #LC_SET_AP_STATE_CC
 */
void LC_SetAPStateCmd(const CFE_SB_Buffer_t *BufPtr);

/**
 * \brief Set AP permanently off command
 *
 *  \par Description
 *       Processes a set actionpoint permanently off ground command.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] BufPtr Pointer to Software Bus buffer
 *
 *  \sa #LC_SET_AP_PERM_OFF_CC
 */
void LC_SetAPPermOffCmd(const CFE_SB_Buffer_t *BufPtr);

/**
 * \brief Reset AP statistics command
 *
 *  \par Description
 *       Processes a reset actionpoint statistics ground command.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] BufPtr Pointer to Software Bus buffer
 *
 *  \sa #LC_RESET_AP_STATS_CC
 */
void LC_ResetAPStatsCmd(const CFE_SB_Buffer_t *BufPtr);

/**
 * \brief Reset WP statistics command
 *
 *  \par Description
 *       Processes a reset watchpoint statistics ground command.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] BufPtr Pointer to Software Bus buffer
 *
 *  \sa #LC_RESET_WP_STATS_CC
 */
void LC_ResetWPStatsCmd(const CFE_SB_Buffer_t *BufPtr);

#endif
