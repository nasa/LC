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
 *   CFS Limit Checker (LC) Application Public Definitions
 *
 * This provides default values for configurable items that affect
 * the interface(s) of this module.  This includes the CMD/TLM message
 * interface, tables definitions, and/or the public API, if applicable.
 *
 * @note This file may be overridden/superceded by mission-provided definitions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */
#ifndef LC_INTERFACE_CFG_H
#define LC_INTERFACE_CFG_H

/**
 * \defgroup cfslcmissioncfg CFS Limit Checker Mission Configuration
 * \{
 */

/**
 * \brief RTS Request Message ID
 *
 *  \par Description:
 *       Message ID that will be used by the #LC_ExecuteRTS function
 *       to construct the RTS request message.
 *
 *       We define this here to allow the LC application to be built
 *       without including headers from the application (e.g. SC) that
 *       will receive the command.  Obviously, this definition must
 *       match the message ID expectyed by the receiving application.
 *
 *       A mission may choose to modify the #LC_ExecuteRTS function not
 *       to use this or define it using a message ID constant that is
 *       defined numerically elsewhere.
 *
 *  \par Limits:
 *       This parameter shouldn't be larger than the value set for
 *       #CFE_PLATFORM_SB_HIGHEST_VALID_MSGID.
 */
#define LC_RTS_REQ_MID 0x18A9

/**
 * \brief RTS Request Command Code
 *
 *  \par Description:
 *       Command code that will be used by the #LC_ExecuteRTS function
 *       to construct the RTS request message.
 *
 *       We define this here to allow the LC application to be built
 *       without including headers from the application (e.g. SC) that
 *       will receive the command.  Obviously, this definition must
 *       match the command code expected by the receiving application.
 *
 *  \par Limits:
 *       This parameter can't be larger than 127 (the maximum value of
 *       7-bit number).
 */
#define LC_RTS_REQ_CC 4

/**
 * \brief Maximum number of watchpoints
 *
 *  \par Description:
 *       Maximum number of watchpoints that can be defined in the
 *       Watchpoint Definition Table (WDT)
 *
 *  \par Limits:
 *       This parameter can't be larger than 65520 (0xFFF0) because
 *       higher values are reserved for use as Reverse Polish
 *       operators. It must be a multiple of 4 to avoid
 *       indexing past the end of the array as LC indexes
 *       ahead to build the packed status bytes.
 *
 *       This parameter will dictate the size of the Watchpoint
 *       Definition Table:
 *
 *       WDT Size = LC_MAX_WATCHPOINTS * sizeof(#LC_WDTEntry_t)
 *
 *       The total size of this table should not exceed the
 *       cFE size limit for a single buffered table set by the
 *       #CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE parameter
 */
#define LC_MAX_WATCHPOINTS 176

/**
 * \brief Maximum number of actionpoints
 *
 *  \par Description:
 *       Maximum number of actionpoints that can be defined in the
 *       Actionpoint Definition Table (ADT)
 *
 *  \par Limits:
 *       This parameter can't be larger than an unsigned 16 bit
 *       integer (65535). It must be a multiple of 2 to avoid
 *       indexing past the end of the array as LC indexes
 *       ahead to build the packed status bytes.
 *
 *       This parameter will dictate the size of the Actionpoint
 *       Definition Table:
 *
 *       ADT Size = LC_MAX_ACTIONPOINTS * sizeof(#LC_ADTEntry_t)
 *
 *       The total size of this table should not exceed the
 *       cFE size limit for a single buffered table set by the
 *       #CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE parameter
 */
#define LC_MAX_ACTIONPOINTS 176

/**
 * \brief Maximum actionpoint event text string size
 *
 *  \par Description:
 *       Maximum length of the event message string that can specified
 *       in an actionpoint definition (including NUL terminator)
 *
 *  \par Limits:
 *       LC appends the trailer text #LC_AP_EVENT_TAIL_STR to this
 *       string when reporting actionpoint failures. The size of this
 *       string is #LC_AP_EVENT_TAIL_LEN
 *
 *       The total value of LC_MAX_ACTION_TEXT + #LC_AP_EVENT_TAIL_LEN
 *       should be less than #CFE_MISSION_EVS_MAX_MESSAGE_LENGTH to avoid
 *       event message truncation
 *
 *       Raising this value will also increase the size of the
 *       Actionpoint Definition Table (ADT)
 */
#define LC_MAX_ACTION_TEXT 32

/**
 * \brief Maximum reverse polish (RPN) equation size
 *
 *  \par Description:
 *       Maximum combined number of operators and operands that may
 *       exist in an actionpoint definition's reverse polish equation
 *
 *  \par Limits:
 *       The LC app limits this parameter to 32.
 *       Increasing this value will increase the size of the
 *       Actionpoint Definition Table (ADT)
 */
#define LC_MAX_RPN_EQU_SIZE 20

/**\}*/

#endif
