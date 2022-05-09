/************************************************************************
 * NASA Docket No. GSC-18,921-1, and identified as “CFS Limit Checker
 * Application version 2.2.0”
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
 *   CFS Limit Checker (LC) utility functions
 */
#ifndef LC_UTILS_H
#define LC_UTILS_H

/************************************************************************
 * Includes
 ************************************************************************/
#include "cfe.h"

/**
 * \brief Verify message length
 *
 *  \par Description
 *       Checks if the actual length of a software bus message matches
 *       the expected length and sends an error event if a mismatch
 *       occures
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] MsgPtr         Pointer to Message
 *  \param[in] ExpectedLength The expected length of the message
 *                            based upon the command code
 *
 *  \return Boolean message length verification result
 *  \retval true  Length verification passed
 *  \retval false Length verification failed
 *
 *  \sa #LC_LEN_ERR_EID
 */
bool LC_VerifyMsgLength(const CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

/**
 * \brief Manage LC application tables
 *
 *  \par Description
 *       Checks the status of the LC application tables and provides
 *       an opportunity to dump or reload tables.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status, see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 *
 *  \sa #LC_WDT_GETADDR_ERR_EID, #LC_ADT_GETADDR_ERR_EID
 */
int32 LC_ManageTables(void);

/**
 * \brief Write to Critical Data Store (CDS)
 *
 *  \par Description
 *       This function updates the CDS areas containing the watchpoint
 *       results table, the actionpoint results table and the LC
 *       application global data.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status, see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
int32 LC_UpdateTaskCDS(void);

/**
 * \brief Perform Routine Maintenance
 *
 *  \par Description
 *       This function manages LC tables and updates the Task CDS.
 *
 *  \par Assumptions, External Events, and Notes:
 *       This function is called during the housekeeping cycle and
 *       if the software bus times out receiving a message.  The CDS
 *       is updated only at these times.  One alternative was to do
 *       the CDS update every time the results tables were modified
 *       but that would result in the update ocurring several times
 *       per second.  By doing the update in the housekeeping and
 *       timeout cases, we cut down on the update frequency at the
 *       cost of the stored data being a couple of seconds old when
 *       a processor reset does occur.
 *
 *  \return Execution status, see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
int32 LC_PerformMaintenance(void);

#endif
