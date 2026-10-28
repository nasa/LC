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
#ifndef DEFAULT_LC_MSGSTRUCT_H
#define DEFAULT_LC_MSGSTRUCT_H

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
