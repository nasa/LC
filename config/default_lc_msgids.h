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
 *   CFS Limit Checker (LC) Application Message IDs
 */
#ifndef DEFAULT_LC_MSGIDS_H
#define DEFAULT_LC_MSGIDS_H

/* ======== */
/* Includes */
/* ======== */

#include "cfe_core_api_base_msgids.h"
#include "lc_msgid_values.h"

/**
 * \defgroup cfslccmdmid CFS Limit Checker Command Message IDs
 * \{
 */

#define LC_CMD_MID       LC_CMD_PLATFORM_MIDVAL(CMD) /**< \brief Msg ID for cmds to LC                */
#define LC_SEND_HK_MID   LC_CMD_PLATFORM_MIDVAL(SEND_HK) /**< \brief Msg ID to request LC housekeeping    */
#define LC_SAMPLE_AP_MID LC_CMD_PLATFORM_MIDVAL(SAMPLE_AP) /**< \brief Msg ID to request actionpoint sample */

/**\}*/

/**
 * \defgroup cfslctlmmid CFS Limit Checker Telemetry Message IDs
 * \{
 */

#define LC_HK_TLM_MID LC_TLM_PLATFORM_MIDVAL(HK_TLM) /**< \brief LC Housekeeping Telemetry */

/**\}*/

#endif
