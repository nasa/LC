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
 *   Contains CFS Limit Checker (LC) macros that run preprocessor checks
 *   on mission and platform configurable parameters
 */
#ifndef LC_VERIFY_H
#define LC_VERIFY_H

#include <stdint.h>

/*************************************************************************
 * Macro Definitions - defined in lc_mission_cfg.h
 *************************************************************************/

/*
 * RTS request message ID
 */
#ifndef LC_RTS_REQ_MID
#error LC_RTS_REQ_MID must be defined!
#elif LC_RTS_REQ_MID < 1
#error LC_RTS_REQ_MID must be greater than zero
#elif LC_RTS_REQ_MID > CFE_PLATFORM_SB_HIGHEST_VALID_MSGID
#error LC_RTS_REQ_MID must not exceed CFE_PLATFORM_SB_HIGHEST_VALID_MSGID
#endif

/*
 * RTS request command code
 */
#ifndef LC_RTS_REQ_CC
#error LC_RTS_REQ_CC must be defined!
#elif LC_RTS_REQ_CC < 0
#error LC_RTS_REQ_CC must not be less than zero
#elif LC_RTS_REQ_CC > 127
#error LC_RTS_REQ_CC must not exceed 127
#endif

/*************************************************************************
 * Macro Definitions - defined in lc_platform_cfg.h
 *************************************************************************/

/*
 * Application name
 */
#ifndef LC_APP_NAME
#error LC_APP_NAME must be defined!
#endif

/*
 * Command pipe depth
 */
#ifndef LC_PIPE_DEPTH
#error LC_PIPE_DEPTH must be defined!
#elif LC_PIPE_DEPTH < 1
#error LC_PIPE_DEPTH must not be less than 1
#elif LC_PIPE_DEPTH > UINT16_MAX
#error LC_PIPE_DEPTH must not exceed UINT16_MAX
#endif

/*
 * Maximum number of watchpoints
 */
#ifndef LC_MAX_WATCHPOINTS
#error LC_MAX_WATCHPOINTS must be defined!
#elif LC_MAX_WATCHPOINTS < 1
#error LC_MAX_WATCHPOINTS must not be less than 1
#elif LC_MAX_WATCHPOINTS > 65520
#error LC_MAX_WATCHPOINTS must not exceed 65520 (OxFFF0)
#elif LC_MAX_WATCHPOINTS % 4 != 0
#error LC_MAX_WATCHPOINTS must be a multiple of 4
#endif

/*
 * Maximum number of actionpoints
 */
#ifndef LC_MAX_ACTIONPOINTS
#error LC_MAX_ACTIONPOINTS must be defined!
#elif LC_MAX_ACTIONPOINTS < 1
#error LC_MAX_ACTIONPOINTS must not be less than 1
#elif LC_MAX_ACTIONPOINTS > UINT16_MAX
#error LC_MAX_ACTIONPOINTS must not exceed UINT16_MAX
#elif LC_MAX_ACTIONPOINTS % 2 != 0
#error LC_MAX_ACTIONPOINTS must be a multiple of 2
#endif

/*
 * LC state after power-on reset
 */
#ifndef LC_STATE_POWER_ON_RESET
#error LC_STATE_POWER_ON_RESET must be defined!
#elif (LC_STATE_POWER_ON_RESET != LC_AppState_ACTIVE) && (LC_STATE_POWER_ON_RESET != LC_AppState_PASSIVE) && \
    (LC_STATE_POWER_ON_RESET != LC_AppState_DISABLED)
#error LC_STATE_POWER_ON_RESET must be defined as a supported enumerated type
#endif

/*
 * LC state when CDS is restored
 */
#ifndef LC_STATE_WHEN_CDS_RESTORED
#error LC_STATE_WHEN_CDS_RESTORED must be defined!
#elif (LC_STATE_WHEN_CDS_RESTORED != LC_AppState_ACTIVE) && (LC_STATE_WHEN_CDS_RESTORED != LC_AppState_PASSIVE) && \
    (LC_STATE_WHEN_CDS_RESTORED != LC_AppState_DISABLED) && (LC_STATE_WHEN_CDS_RESTORED != LC_AppState_FROM_CDS)
#error LC_STATE_WHEN_CDS_RESTORED must be defined as a supported enumerated type
#endif

/*
 * Default watchpoint definition table filename
 */
#ifndef LC_WDT_FILENAME
#error LC_WDT_FILENAME must be defined!
#endif

/*
 * Default actionpoint definition table filename
 */
#ifndef LC_ADT_FILENAME
#error LC_ADT_FILENAME must be defined!
#endif

/*
 * RPN equation buffer size (in 16 bit words)
 */
#ifndef LC_MAX_RPN_EQU_SIZE
#error LC_MAX_RPN_EQU_SIZE must be defined!
#elif LC_MAX_RPN_EQU_SIZE < 2
#error LC_MAX_RPN_EQU_SIZE must not be less than 2
#elif LC_MAX_RPN_EQU_SIZE > 32
#error LC_MAX_RPN_EQU_SIZE must not exceed 32
#endif

/*
 * Maximum actionpoint event text string size
 */
#ifndef LC_MAX_ACTION_TEXT
#error LC_MAX_ACTION_TEXT must be defined!
#elif LC_MAX_ACTION_TEXT < 0
#error LC_MAX_ACTION_TEXT must not be less than zero
#elif LC_MAX_ACTION_TEXT > CFE_MISSION_EVS_MAX_MESSAGE_LENGTH
#error LC_MAX_ACTION_TEXT must not exceed CFE_MISSION_EVS_MAX_MESSAGE_LENGTH
#endif

/* Note: LC_AP_EVENT_TAIL_LEN is defined in lc_action.h */
#ifndef LC_AP_EVENT_TAIL_LEN
#error LC_AP_EVENT_TAIL_LEN must be defined!
#elif LC_AP_EVENT_TAIL_LEN < 0
#error LC_AP_EVENT_TAIL_LEN must not be less than zero
#elif LC_AP_EVENT_TAIL_LEN > CFE_MISSION_EVS_MAX_MESSAGE_LENGTH
#error LC_AP_EVENT_TAIL_LEN must not exceed CFE_MISSION_EVS_MAX_MESSAGE_LENGTH
#endif

#if (LC_MAX_ACTION_TEXT + LC_AP_EVENT_TAIL_LEN) > CFE_MISSION_EVS_MAX_MESSAGE_LENGTH
#error The sum of LC_MAX_ACTION_TEXT + LC_AP_EVENT_TAIL_LEN must not exceed CFE_MISSION_EVS_MAX_MESSAGE_LENGTH
#endif

/*
 * Maximum valid actionpoint definition table RTS ID
 */
#ifndef LC_MAX_VALID_ADT_RTSID
#error LC_MAX_VALID_ADT_RTSID must be defined!
#elif LC_MAX_VALID_ADT_RTSID < 0
#error LC_MAX_VALID_ADT_RTSID must not be less than zero
#elif LC_MAX_VALID_ADT_RTSID > UINT16_MAX
#error LC_MAX_VALID_ADT_RTSID must not exceed UINT16_MAX
#endif

#endif
