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
#ifndef LC_DISPATCH_H
#define LC_DISPATCH_H

/*************************************************************************
 * Includes
 *************************************************************************/
#include "cfe_sb_api_typedefs.h"

/*************************************************************************
 * Exported Functions
 *************************************************************************/

/**
 * \brief Process a command pipe message
 *
 *  \par Description
 *       Processes a single software bus command pipe message. Checks
 *       the message and command IDs and calls the appropriate routine
 *       to handle the message.
 *
 *       All messageIDs other than #LC_CMD_MID, #LC_SEND_HK_MID, and
 *       #LC_SAMPLE_AP_MID are assumed to be monitor packets.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] BufPtr Pointer to Software Bus buffer
 *
 *  \return Execution status, see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
CFE_Status_t LC_AppPipe(const CFE_SB_Buffer_t *BufPtr);

#endif
