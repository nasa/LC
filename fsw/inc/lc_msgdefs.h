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
 *   Specification for the CFS Limit Checker (LC) command and telemetry
 *   message constant definitions.
 *
 * @note
 *   These Macro definitions have been put in this file (instead of
 *   lc_msg.h) so this file can be included directly into ASIST build
 *   test scripts. ASIST RDL files can accept C language \#defines but
 *   can't handle type definitions. As a result: DO NOT PUT ANY
 *   TYPEDEFS OR STRUCTURE DEFINITIONS IN THIS FILE!
 *   ADD THEM TO lc_msg.h IF NEEDED!
 */
#ifndef LC_MSGDEFS_H
#define LC_MSGDEFS_H

/************************************************************************
 * Includes
 ************************************************************************/
#include <lc_platform_cfg.h>

/************************************************************************
 * Macro Definitions
 ************************************************************************/

/**
 * \name LC Application States
 * \{
 */
#define LC_STATE_ACTIVE   1 /**< \brief LC Application State Active */
#define LC_STATE_PASSIVE  2 /**< \brief LC Application State Pasive */
#define LC_STATE_DISABLED 3 /**< \brief LC Application State Disabled */
#define LC_STATE_FROM_CDS 4 /**< \brief Used for reset processing, not valid state */
/**\}*/

/**
 * \name Actionpoint States
 * \{
 */
#define LC_APSTATE_NOT_USED 0xFF /**< \brief Actionpoint unused, not valid command argument */
#define LC_APSTATE_ACTIVE   1    /**< \brief Actionpoint state active */
#define LC_APSTATE_PASSIVE  2    /**< \brief Actionpoint state passive */
#define LC_APSTATE_DISABLED 3    /**< \brief Actionpoint state disabled */
#define LC_APSTATE_PERMOFF  4    /**< \brief Actionpoint state permanently off, see #LC_SET_AP_PERM_OFF_CC */
/**\}*/

/**
 * \name Special Values for Commands
 * \{
 */
#define LC_ALL_ACTIONPOINTS 0xFFFF /**< \brief Special number indicating all actionpoints */
#define LC_ALL_WATCHPOINTS  0xFFFF /**< \brief Special number indicating all watchpoints */
/**\}*/

/**
 * \name Housekeeping Packed Watch Results
 * \{
 */
#define LC_HKWR_FALSE 0x00 /**< \brief Two bit value used for FALSE        */
#define LC_HKWR_TRUE  0x01 /**< \brief Two bit value used for TRUE         */
#define LC_HKWR_ERROR 0x02 /**< \brief Two bit value used for ERROR        */
#define LC_HKWR_STALE 0x03 /**< \brief Two bit value used for STALE        */
/**\}*/

/**
 * \name Housekeeping Packed Action Results
 * \{
 */
#define LC_HKAR_PASS  0x00 /**< \brief Two bit value used for PASS         */
#define LC_HKAR_FAIL  0x01 /**< \brief Two bit value used for FAIL         */
#define LC_HKAR_ERROR 0x02 /**< \brief Two bit value used for ERROR        */
#define LC_HKAR_STALE 0x03 /**< \brief Two bit value used for STALE        */
/**\}*/

/**
 * \name Housekeeping Packed Action Results, State Identifiers
 * \{
 */
#define LC_HKAR_STATE_NOT_USED 0x00 /**< \brief Two bit value used for NOT USED and PERMOFF */
#define LC_HKAR_STATE_ACTIVE   0x01 /**< \brief Two bit value used for ACTIVE       */
#define LC_HKAR_STATE_PASSIVE  0x02 /**< \brief Two bit value used for PASSIVE      */
#define LC_HKAR_STATE_DISABLED 0x03 /**< \brief Two bit value used for DISABLED     */
/**\}*/

/**
 * \name Housekeeping Packed Results, Array Sizes
 * \{
 */
#define LC_HKWR_NUM_BYTES                                                         \
    (((LC_MAX_WATCHPOINTS + 15) / 16) * 4) /**< \brief 2 bits per WP and keeping  \
                                            *          array on longword boundary \
                                            */

#define LC_HKAR_NUM_BYTES                                                        \
    (((LC_MAX_ACTIONPOINTS + 7) / 8) * 4) /**< \brief 4 bits per AP and keeping  \
                                           *          array on longword boundary \
                                           */
/**\}*/

/**
 * \defgroup cfslccmdcodes CFS Limit Checker Command Codes
 * \{
 */

/**
 * \brief Noop
 *
 *  \par Description
 *       Implements the Noop command that insures the LC task is alive
 *
 *  \par Command Structure
 *       #LC_NoopCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #LC_HkPacket_t.CmdCount will increment
 *       - The #LC_NOOP_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #LC_HkPacket_t.CmdErrCount will increment
 *       - Error specific event message #LC_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #LC_RESET_CC
 */
#define LC_NOOP_CC 0

/**
 * \brief Reset Counters
 *
 *  \par Description
 *       Resets the LC housekeeping counters
 *
 *  \par Command Structure
 *       #LC_ResetCountersCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #LC_HkPacket_t.CmdCount will be cleared
 *       - The #LC_RESET_DBG_EID debug event message will be
 *         generated when the command is executed
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #LC_HkPacket_t.CmdErrCount will increment
 *       - Error specific event message #LC_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #LC_NOOP_CC
 */
#define LC_RESET_CC 1

/**
 * \brief Set LC Application State
 *
 *  \par Description
 *       Sets the operational state of the LC application
 *
 *  \par Command Structure
 *       #LC_SetLCStateCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #LC_HkPacket_t.CmdCount will increment
 *       - #LC_HkPacket_t.CurrentLCState will be set to the new state
 *       - The #LC_LCSTATE_INF_EID informational event message will be
 *         generated when the command is executed
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - Invalid new state specified in command message
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #LC_HkPacket_t.CmdErrCount will increment
 *       - Error specific event message #LC_LEN_ERR_EID
 *       - Error specific event message #LC_LCSTATE_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 */
#define LC_SET_LC_STATE_CC 2

/**
 * \brief Set AP State
 *
 *  \par Description
 *       Set actionpoint state
 *
 *  \par Command Structure
 *       #LC_SetAPStateCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #LC_HkPacket_t.CmdCount will increment
 *       - The #LC_APSTATE_INF_EID informational event message will be
 *         generated when the command is executed
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - Invalid actionpoint state specified in command message
 *       - Actionpoint number specified in command message is
 *         out of range
 *       - Actionpoint current state is either #LC_APSTATE_NOT_USED
 *         or #LC_APSTATE_PERMOFF
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #LC_HkPacket_t.CmdErrCount will increment
 *       - Error specific event message #LC_LEN_ERR_EID
 *       - Error specific event message #LC_APSTATE_NEW_ERR_EID
 *       - Error specific event message #LC_APSTATE_APNUM_ERR_EID
 *       - Error specific event message #LC_APSTATE_CURR_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 */
#define LC_SET_AP_STATE_CC 3

/**
 * \brief Set AP Permanently Off
 *
 *  \par Description
 *       Set the specified actionpoint's state to #LC_APSTATE_PERMOFF
 *
 *  \par Command Structure
 *       #LC_SetAPPermOffCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #LC_HkPacket_t.CmdCount will increment
 *       - The #LC_APOFF_INF_EID informational event message will be
 *         generated when the command is executed
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - Actionpoint number specified in command message is
 *         out of range
 *       - Actionpoint current state is not #LC_APSTATE_DISABLED
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #LC_HkPacket_t.CmdErrCount will increment
 *       - Error specific event message #LC_LEN_ERR_EID
 *       - Error specific event message #LC_APOFF_APNUM_ERR_EID
 *       - Error specific event message #LC_APOFF_CURR_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 */
#define LC_SET_AP_PERM_OFF_CC 4

/**
 * \brief Reset AP Statistics
 *
 *  \par Description
 *       Resets actionpoint statistics
 *
 *  \par Command Structure
 *       #LC_ResetAPStatsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #LC_HkPacket_t.CmdCount will increment
 *       - The #LC_APSTATS_INF_EID informational event message will be
 *         generated when the command is executed
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - Actionpoint number specified in command message is
 *         out of range
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #LC_HkPacket_t.CmdErrCount will increment
 *       - Error specific event message #LC_LEN_ERR_EID
 *       - Error specific event message #LC_APSTATS_APNUM_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #LC_RESET_WP_STATS_CC
 */
#define LC_RESET_AP_STATS_CC 5

/**
 * \brief Reset WP Statistics
 *
 *  \par Description
 *       Resets watchpoint statistics
 *
 *  \par Command Structure
 *       #LC_ResetWPStatsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #LC_HkPacket_t.CmdCount will increment
 *       - The #LC_WPSTATS_INF_EID informational event message will be
 *         generated when the command is executed
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - Watchpoint number specified in command message is
 *         out of range
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #LC_HkPacket_t.CmdErrCount will increment
 *       - Error specific event message #LC_LEN_ERR_EID
 *       - Error specific event message #LC_WPSTATS_WPNUM_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #LC_RESET_AP_STATS_CC
 */
#define LC_RESET_WP_STATS_CC 6

/**\}*/

#ifndef LC_OMIT_DEPRECATED
#define LC_SET_AP_PERMOFF_CC LC_SET_AP_PERM_OFF_CC
#define LC_ACTION_NOT_USED   LC_APSTATE_NOT_USED
#endif

#endif
