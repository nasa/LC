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
 *   CFS Limit Checker (LC) Application Mission Configuration Header File
 */
#ifndef LC_MISSION_CFG_H
#define LC_MISSION_CFG_H

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

/**\}*/

#endif
