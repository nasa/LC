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
 *   Specification for the CFS Limit Checker (LC) mission specific
 *   custom function template
 */
#ifndef LC_CUSTOM_H
#define LC_CUSTOM_H

/*************************************************************************
 * Includes
 *************************************************************************/
#include "cfe.h"

/*************************************************************************
 * Exported Functions
 *************************************************************************/

/**
 * \brief Execute RTS
 *
 *  \par Description
 *       Support function for actionpoint processing that is called
 *       to send an RTS request when an actionpoint evaluation
 *       determines it has failed
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]   RTSId        ID of the RTS to request
 */
void LC_ExecuteRTS(uint16 RTSId);

/**
 * \brief Mission specific custom function
 *
 *  \par Description
 *       This is the mission specific custom function entry point.
 *       It gets called whenever the OperatorID in a watchpoint
 *       definition table entry is set to #LC_OPER_CUSTOM and
 *       must return one of the defined watchpoint evaluation
 *       result types
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in] WatchIndex         The watchpoint number (zero based
 *                                 watchpoint definition table index) for
 *                                 the watchpoint definition that caused
 *                                 the call
 *
 *  \param [in] ProcessedWPData    The watchpoint data extracted from
 *                                 the message that it was contained
 *                                 in. This is the data after any
 *                                 sizing, bit-masking, and endianess
 *                                 fixing that LC might have done
 *                                 according to the watchpoint definition
 *
 *  \param [in] BufPtr         A #CFE_SB_Buffer_t* pointer that
 *                                 references the software bus message that
 *                                 contained the watchpoint data. If the
 *                                 custom function needs the raw watchpoint
 *                                 data, it can use this pointer and the
 *                                 watchpoint definition to extract it.
 *
 *  \param [in] WDTCustomFuncArg   This is the custom function argument
 *                                 for this watchpoint from the watchpoint
 *                                 definition table. It can be used for
 *                                 whatever purpose the mission developers
 *                                 want. LC doesn't use it.
 *
 *  \return Watchpoint evaluation result
 *  \retval #LC_WATCH_TRUE  \copydoc LC_WATCH_TRUE
 *  \retval #LC_WATCH_FALSE \copydoc LC_WATCH_FALSE
 *  \retval #LC_WATCH_ERROR \copydoc LC_WATCH_ERROR
 *
 *  \sa #LC_WDTEntry_t
 */
uint8 LC_CustomFunction(uint16 WatchIndex, uint32 ProcessedWPData, const CFE_SB_Buffer_t *BufPtr,
                        uint32 WDTCustomFuncArg);

#endif
