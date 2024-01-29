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
 *   message data types.
 *
 * Provides default definitions for message structures
 *
 * @note This file may be overridden/superceded by mission-provided definitions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 *
 * @note
 *   Constants and enumerated types related to these message structures
 *   are defined in lc_msgdefs.h. They are kept separate to allow easy
 *   integration with ASIST RDL files which can't handle typedef
 *   declarations (see the main comment block in lc_msgdefs.h for more
 *   info).
 */
#ifndef LC_MSGSTRUCT_H
#define LC_MSGSTRUCT_H

/************************************************************************
 * Includes
 ************************************************************************/
#include "common_types.h"
#include "lc_msgdefs.h"
#include "cfe_msg_hdr.h"

/************************************************************************
 * Type Definitions
 ************************************************************************/

/**
 * \defgroup cfslccmdstructs CFS Limit Checker Command Structures
 * \{
 */

/**
 *  \brief Send HK Command
 *
 *  For command details see #LC_SEND_HK_MID
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command Header */
} LC_SendHkCmd_t;

/**
 *  \brief No-op Command
 *
 *  For command details see #LC_NOOP_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command Header */
} LC_NoopCmd_t;

/**
 *  \brief Reset Counters Command
 *
 *  For command details see #LC_RESET_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command Header */
} LC_ResetCountersCmd_t;

/**
 *  \brief Set LC Application State Payload
 *
 *  For command details see #LC_SET_LC_STATE_CC
 */
typedef struct
{
    uint16 NewLCState; /**< \brief New LC application state    */
    uint16 Padding;    /**< \brief Structure padding           */
} LC_SetLCState_Payload_t;

/**
 *  \brief Set LC Application State Command
 *
 *  For command details see #LC_SET_LC_STATE_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command Header */

    LC_SetLCState_Payload_t Payload;
} LC_SetLCStateCmd_t;

/**
 *  \brief Set AP (Actionpoint) State Payload
 *
 *  For command details see #LC_SET_AP_STATE_CC
 */
typedef struct
{
    uint16 APNumber;   /**< \brief Which actionpoint(s) to change */
    uint16 NewAPState; /**< \brief New actionpoint state          */
} LC_SetAPState_Payload_t;

/**
 *  \brief Set AP (Actionpoint) State Command
 *
 *  For command details see #LC_SET_AP_STATE_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command Header */

    LC_SetAPState_Payload_t Payload;
} LC_SetAPStateCmd_t;

/**
 *  \brief Set AP (Actionpoint) Permanently Off Payload
 *
 *  For command details see #LC_SET_AP_PERM_OFF_CC
 */
typedef struct
{
    uint16 APNumber; /**< \brief Which actionpoint to change */
    uint16 Padding;  /**< \brief Structure padding           */
} LC_SetAPPermOff_Payload_t;

/**
 *  \brief Set AP (Actionpoint) Permanently Off Command
 *
 *  For command details see #LC_SET_AP_PERM_OFF_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command Header */

    LC_SetAPPermOff_Payload_t Payload;
} LC_SetAPPermOffCmd_t;

/**
 *  \brief Reset AP (Actionpoint) Statistics Payload
 *
 *  For command details see #LC_RESET_AP_STATS_CC
 */
typedef struct
{
    uint16 APNumber; /**< \brief Which actionpoint(s) to change */
    uint16 Padding;  /**< \brief Structure padding              */
} LC_ResetAPStats_Payload_t;

/**
 *  \brief Reset AP (Actionpoint) Statistics Command
 *
 *  For command details see #LC_RESET_AP_STATS_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command Header */

    LC_ResetAPStats_Payload_t Payload;
} LC_ResetAPStatsCmd_t;

/**
 *  \brief Reset WP (Watchpoint) Statistics Payload
 *
 *  For command details see #LC_RESET_WP_STATS_CC
 */
typedef struct
{
    uint16 WPNumber; /**< \brief Which watchpoint(s) to change */
    uint16 Padding;  /**< \brief Structure padding             */
} LC_ResetWPStats_Payload_t;

/**
 *  \brief Reset WP (Watchpoint) Statistics Command
 *
 *  For command details see #LC_RESET_WP_STATS_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command Header */

    LC_ResetWPStats_Payload_t Payload;
} LC_ResetWPStatsCmd_t;

/**
 *  \brief Sample AP (Actionpoint) Payload
 *
 *  See #LC_SAMPLE_AP_MID
 */
typedef struct
{
    uint16 StartIndex; /**< \brief Start actionpoint to sample */
    uint16 EndIndex;   /**< \brief End actionpoint to sample */

    uint16 UpdateAge; /**< \brief Update WP results age (T or F) */
    uint16 Padding;   /**< \brief Structure padding              */
} LC_SampleAP_Payload_t;

/**
 *  \brief Sample AP (Actionpoint) Request
 *
 *  See #LC_SAMPLE_AP_MID
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command Header */

    LC_SampleAP_Payload_t Payload;
} LC_SampleAPCmd_t;

/**
 * \brief Payload to Start a Stored Command RTS
 */
typedef struct
{
    uint16 RTSId;   /**< \brief RTS Id to start */
    uint16 Padding; /**< \brief Padding */
} LC_RTSRequest_Payload_t;

/**
 * \brief Send Command to Start a Stored Command RTS
 *
 * This is a local declaration of the command message structure
 * to initiate an RTS and has been placed here to allow the
 * the LC application to be built without including headers from
 * any other applications (like Stored Commanding).
 * A mission may choose to remove this and use a message
 * structure declared elsewhere instead.
 *
 * This also applies to the LC_RTS_REQ_MID and LC_RTS_REQ_CC
 * constants (see lc_mission_cfg.h).
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command Header */

    LC_RTSRequest_Payload_t Payload;
} LC_RTSRequestCmd_t;

/**\}*/

/**
 * \defgroup cfslctlm CFS Limit Checker Telemetry
 * \{
 */

/**
 *  \brief Housekeeping Payload Structure
 */
typedef struct
{
    uint8 CmdCount;       /**< \brief LC Application Command Counter */
    uint8 CmdErrCount;    /**< \brief LC Application Command Error Counter */
    uint8 CurrentLCState; /**< \brief Current LC application operating state */

    uint8 Pad8; /**< \brief Structure padding */

    uint8 WPResults[LC_HKWR_NUM_BYTES]; /**< \brief Packed watchpoint results data, 2 bits per watchpoint */
    uint8 APResults[LC_HKAR_NUM_BYTES]; /**< \brief Packed actionpoint results data, 4 bits per actionpoint */

    uint16 PassiveRTSExecCount; /**< \brief Total count of RTS sequences not initiated because the LC state is
                                 *          set to #LC_STATE_PASSIVE or the state of the actionpoint that failed
                                 *          is set to #LC_APSTATE_PASSIVE
                                 */

    uint16 WPsInUse;  /**< \brief How many watchpoints are currently in effect */
    uint16 ActiveAPs; /**< \brief How many actionpoints are currently active */

    uint16 Pad16; /**< \brief Structure padding */

    uint32 APSampleCount;     /**< \brief Total count of Actionpoints sampled */
    uint32 MonitoredMsgCount; /**< \brief Total count of messages monitored for watchpoints */
    uint32 RTSExecCount;      /**< \brief Total count of RTS sequences initiated */
} LC_HkTlm_Payload_t;

/**
 *  \brief Housekeeping Telemetry Structure Structure
 */
typedef struct
{
    CFE_MSG_TelemetryHeader_t TelemetryHeader; /**< \brief Telemetry Header */

    LC_HkTlm_Payload_t Payload;
} LC_HkPacket_t;

/**\}*/

#endif
