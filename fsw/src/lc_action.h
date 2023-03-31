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
 *   handle actionpoint processing
 */
#ifndef LC_ACTION_H
#define LC_ACTION_H

/*************************************************************************
 * Includes
 *************************************************************************/
#include "cfe.h"
#include "lc_tbl.h"

/************************************************************************
 * Macro Definitions
 ************************************************************************/

/**
 * \name LC Actionpoint Event Trailer
 * \{
 */

#define LC_AP_EVENT_TAIL_STR ": AP = %d, FailCount = %d, RTS = %d" /**< \brief AP event trailer string */

#define LC_AP_EVENT_TAIL_LEN 36 /**< \brief Length of string including NUL */

/**\}*/

/*************************************************************************
 * Exported Functions
 *************************************************************************/

/**
 * \brief Sample actionpoints
 *
 *  \par Description
 *       Support function for #LC_SampleAPReq that will sample the
 *       selected actionpoints.  The start and end arguments define
 *       which actionpoint(s) the command will sample.  If both the
 *       start and end arguments are set to #LC_ALL_ACTIONPOINTS,
 *       the command will be interpreted as a request to sample all
 *       actionpoints (heritage).  Otherwise, the start index must
 *       be less than or equal to the end index, and both must be
 *       within the bounds of the actionpoint table.  The calling
 *       function ensures that the start index and the end index are
 *       within the bounds of the actionpoint table.  If StartIndex >
 *       EndIndex, the function will (intentionally) silently fail.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]   StartIndex   The first actionpoint to sample
 *                             (zero based actionpoint table index)
 *
 *  \param [in]   EndIndex     The last actionpoint to sample
 *                             (zero based actionpoint table index)
 *
 *  \sa #LC_SampleAPReq
 */
void LC_SampleAPs(uint16 StartIndex, uint16 EndIndex);

/**
 * \brief Validate actionpoint definition table (ADT)
 *
 *  \par Description
 *       This function is called by table services when a validation of
 *       the actionpoint definition table is required
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]   *TableData     Pointer to the table data to validate
 *
 *  \return Actionpoint definition table validation status
 *  \retval #CFE_SUCCESS            \copydoc CFE_SUCCESS
 *  \retval #LC_ADTVAL_ERR_DEFSTATE \copydoc LC_ADTVAL_ERR_DEFSTATE
 *  \retval #LC_ADTVAL_ERR_RTSID    \copydoc LC_ADTVAL_ERR_RTSID
 *  \retval #LC_ADTVAL_ERR_FAILCNT  \copydoc LC_ADTVAL_ERR_FAILCNT
 *  \retval #LC_ADTVAL_ERR_EVTTYPE  \copydoc LC_ADTVAL_ERR_EVTTYPE
 *  \retval #LC_ADTVAL_ERR_RPN      \copydoc LC_ADTVAL_ERR_RPN
 *
 *  \sa #LC_ValidateWDT
 */
int32 LC_ValidateADT(void *TableData);

/**
 * \brief Sample single actionpoint
 *
 *  \par Description
 *       Support function for actionpoint processing that will sample
 *       a single actionpoint and handle the result as needed.
 *
 *       The sample is only performed if the actionpoint is active or
 *       passive.  Other states are ignored since this routine is
 *       called in a loop to process ALL actionpoints.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]   APNumber     The actionpoint number to sample (zero
 *                             based actionpoint definition table index)
 */
void LC_SampleSingleAP(uint16 APNumber);

/**
 * \brief Evaluate RPN
 *
 *  \par Description
 *       Support function for actionpoint processing that evaluates
 *       the reverse polish notation (RPN) equation for the specified
 *       actionpoint and returns the result
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]   APNumber     The actionpoint number to evaluate (zero
 *                             based actionpoint definition table index)
 *
 *  \return Reverse polish notation result
 *  \retval #LC_ACTION_PASS  \copydoc LC_ACTION_PASS
 *  \retval #LC_ACTION_FAIL  \copydoc LC_ACTION_FAIL
 *  \retval #LC_ACTION_STALE \copydoc LC_ACTION_STALE
 *  \retval #LC_ACTION_ERROR \copydoc LC_ACTION_ERROR
 */
uint8 LC_EvaluateRPN(uint16 APNumber);

/**
 * \brief Validate RPN expression
 *
 *  \par Description
 *       Support function for actionpoint definition table validation
 *       that checks a reverse polish notation (RPN) equation for
 *       possible errors.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]   RPNPtr            Pointer to the RPN equation
 *
 *  \param [in]   IndexValue        A pointer where to store the equation
 *                                  index value if an error is detected
 *
 *  \param [in]   StackDepthValue   A pointer where to store the equation
 *                                  stack depth value if an error is detected
 *
 *  \return Actionpoint definition table reverse polish notation validation status
 *  \retval #LC_ADTVAL_ERR_NONE  \copydoc LC_ADTVAL_ERR_NONE
 *  \retval #LC_ADTVAL_ERR_RPN \copydoc LC_ADTVAL_ERR_RPN
 *
 *  \sa #LC_ValidateADT
 */
int32 LC_ValidateRPN(const uint16 *RPNPtr, int32 *IndexValue, int32 *StackDepthValue);

#endif
