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
#ifndef LC_MSGID_VALUES_H
#define LC_MSGID_VALUES_H

#include "lc_topicids.h"
#include "cfe_core_api_base_msgids.h"

#define LC_CMD_PLATFORM_MIDVAL(x) CFE_PLATFORM_CMD_TOPICID_TO_MIDV(LC_MISSION_##x##_TOPICID)
#define LC_TLM_PLATFORM_MIDVAL(x) CFE_PLATFORM_TLM_TOPICID_TO_MIDV(LC_MISSION_##x##_TOPICID)

#endif
