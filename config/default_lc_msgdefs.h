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
#ifndef DEFAULT_LC_MSGDEFS_H
#define DEFAULT_LC_MSGDEFS_H

/************************************************************************
 * Includes
 ************************************************************************/
#include "common_types.h"

#include "lc_interface_cfg.h"
#include "lc_fcncodes.h"

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

#ifndef LC_OMIT_DEPRECATED
#define LC_ACTION_NOT_USED LC_APSTATE_NOT_USED
#endif

/**
 *  \brief Reset AP (Actionpoint) Statistics Payload
 *
 *  For command details see #LC_RESET_AP_STATS_CC,
 *  #LC_SET_AP_PERM_OFF_CC, #LC_SET_AP_STATE_CC
 */
typedef struct
{
    uint16 APNumber; /**< \brief Which actionpoint(s) to change */
    uint16 Padding;  /**< \brief Structure padding */
} LC_APNumber_Payload_t;

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
 *  \brief Set LC Application State Payload
 *
 *  For command details see #LC_SET_LC_STATE_CC
 */
typedef struct
{
    uint16 NewLCState; /**< \brief New LC application state */
    uint16 Padding;    /**< \brief Structure padding */
} LC_SetLCState_Payload_t;

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
 *  \brief Reset WP (Watchpoint) Statistics Payload
 *
 *  For command details see #LC_RESET_WP_STATS_CC
 */
typedef struct
{
    uint16 WPNumber; /**< \brief Which watchpoint(s) to change */
    uint16 Padding;  /**< \brief Structure padding */
} LC_ResetWPStats_Payload_t;

/**
 * \brief Payload to Start a Stored Command RTS
 */
typedef struct
{
    uint16 RTSId;   /**< \brief RTS Id to start */
    uint16 Padding; /**< \brief Padding */
} LC_RTSRequest_Payload_t;

/**
 * \defgroup cfslctlm CFS Limit Checker Telemetry
 * \{
 */

/**
 *  \brief Housekeeping Payload Structure
 */
typedef struct
{
    uint8 CommandCounter;    /**< \brief LC Application Command Counter */
    uint8 CommandErrCounter; /**< \brief LC Application Command Error Counter */
    uint8 CurrentLCState;    /**< \brief Current LC application operating state */

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

/**\}*/

#endif
