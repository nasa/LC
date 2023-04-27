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
 *   handle watchpoint processing
 */
#ifndef LC_WATCH_H
#define LC_WATCH_H

/*************************************************************************
 * Includes
 *************************************************************************/
#include "cfe.h"
#include "lc_app.h"

/*************************************************************************
 * Constants
 *************************************************************************/

/**
 * \name Values used when converting watchpoint values to correctly sized data
 * \{
 */
#define LC_16BIT_BE_VAL 0x0001     /**< \brief 16 bit big endian conversion value */
#define LC_16BIT_LE_VAL 0x0100     /**< \brief 16 bit little endian conversion value */
#define LC_32BIT_BE_VAL 0x00010203 /**< \brief 32 bit big endian conversion value */
#define LC_32BIT_LE_VAL 0x03020100 /**< \brief 32 bit little endian conversion value */
/**\}*/

/**
 * \name Fields of the single-precision IEEE-754 floating point format
 * \{
 */
#define LC_IEEE_EXPONENT_MASK 0x7F800000 /**< \brief IEEE-754 floating point exponent mask */
#define LC_IEEE_FRACTION_MASK 0x007FFFFF /**< \brief IEEE-754 floating point fraction mask */
/**\}*/

/*************************************************************************
 * Exported Functions
 *************************************************************************/

/**
 * \brief Check message for watchpoints
 *
 *  \par Description
 *       Processes a single software bus command pipe message that
 *       doesn't match any LC predefined command or message ids,
 *       which indicates it's probably a watchpoint message.
 *       It will search the watchpoint definition table for matches
 *       to this MessageID and handle them as needed.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] MessageID Message ID
 *  \param[in] BufPtr    Pointer to Software Bus buffer
 *
 *  \sa #LC_ProcessWP
 */
void LC_CheckMsgForWPs(CFE_SB_MsgId_t MessageID, const CFE_SB_Buffer_t *BufPtr);

/**
 * \brief Validate watchpoint definition table (WDT)
 *
 *  \par Description
 *       This function is called by table services when a validation of
 *       the watchpoint definition table is required
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] TableData Pointer to the table data to validate
 *
 *  \return Watchpoint definition table validation result
 *  \retval #CFE_SUCCESS            \copydoc CFE_SUCCESS
 *  \retval #LC_WDTVAL_ERR_DATATYPE \copydoc LC_WDTVAL_ERR_DATATYPE
 *  \retval #LC_WDTVAL_ERR_OPER     \copydoc LC_WDTVAL_ERR_OPER
 *  \retval #LC_WDTVAL_ERR_MID      \copydoc LC_WDTVAL_ERR_MID
 *  \retval #LC_WDTVAL_ERR_FPNAN    \copydoc LC_WDTVAL_ERR_FPNAN
 *  \retval #LC_WDTVAL_ERR_FPINF    \copydoc LC_WDTVAL_ERR_FPINF
 *
 *  \sa #LC_ValidateADT
 */
int32 LC_ValidateWDT(void *TableData);

/**
 * \brief Create watchpoint hash table
 *
 *  \par Description
 *       Creates a hash table to optimize the process of getting direct
 *       access to all the watchpoint table entries that reference a
 *       particular MessageID without having to search the entire table.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \sa #LC_GetHashTableIndex, #LC_AddWatchpoint
 */
void LC_CreateHashTable(void);

/**
 * \brief Process a single watchpoint
 *
 *  \par Description
 *       Support function for watchpoint processing that will
 *       evaluate a single watchpoint
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]   WatchIndex  The watchpoint number to evaluate (zero
 *                            based watchpoint definition table index)
 *
 *  \param [in]   BufPtr      Pointer to Software Bus buffer
 *
 *  \param [in]   Timestamp   A #CFE_TIME_SysTime_t timestamp to use
 *                            to update the watchpoint results data
 *                            if a state transition is detected
 */
void LC_ProcessWP(uint16 WatchIndex, const CFE_SB_Buffer_t *BufPtr, CFE_TIME_SysTime_t Timestamp);

/**
 * \brief Operator comparison
 *
 *  \par Description
 *       Support function for watchpoint processing that will perform
 *       the watchpoint data comparison based upon the operator and
 *       data type specified in the watchpoint definition table
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in] WatchIndex         The watchpoint number to compare (zero
 *                                 based watchpoint definition table index)
 *
 *  \param [in] ProcessedWPData    The watchpoint data extracted from
 *                                 the message that it was contained
 *                                 in. This is the data after any
 *                                 sizing, bit-masking, and endianess
 *                                 fixing that LC might have done
 *                                 according to the watchpoint definition
 *
 *  \return Operation comparison result from #LC_SignedCompare, #LC_UnsignedCompare, #LC_FloatCompare
 *  \retval #LC_WATCH_ERROR \copydoc LC_WATCH_ERROR
 */
uint8 LC_OperatorCompare(uint16 WatchIndex, uint32 ProcessedWPData);

/**
 * \brief Signed comparison
 *
 *  \par Description
 *       Support function for watchpoint processing that will perform
 *       a signed watchpoint data comparison based upon the operator
 *       specified in the watchpoint definition table
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in] WatchIndex    The watchpoint number to compare (zero
 *                            based watchpoint definition table index)
 *
 *  \param [in] WPValue       The watchpoint data extracted from
 *                            the message that it was contained
 *                            in. This is the data after any
 *                            sizing, bit-masking, and endianess
 *                            fixing that LC might have done
 *                            according to the watchpoint definition
 *
 *  \param [in] CompareValue  The comparison value specified in the
 *                            watchpoint definition table (sign
 *                            extended, if needed, in an int32)
 *
 *  \return Comparison result
 *  \retval #LC_WATCH_TRUE  \copydoc LC_WATCH_TRUE
 *  \retval #LC_WATCH_FALSE \copydoc LC_WATCH_FALSE
 *  \retval #LC_WATCH_ERROR \copydoc LC_WATCH_ERROR
 */
uint8 LC_SignedCompare(uint16 WatchIndex, int32 WPValue, int32 CompareValue);

/**
 * \brief Unsigned comparison
 *
 *  \par Description
 *       Support function for watchpoint processing that will perform
 *       an unsigned watchpoint data comparison based upon the operator
 *       specified in the watchpoint definition table
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in] WatchIndex    The watchpoint number to compare (zero
 *                            based watchpoint definition table index)
 *
 *  \param [in] WPValue       The watchpoint data extracted from
 *                            the message that it was contained
 *                            in. This is the data after any
 *                            sizing, bit-masking, and endianess
 *                            fixing that LC might have done
 *                            according to the watchpoint definition
 *
 *  \param [in] CompareValue  The comparison value specified in the
 *                            watchpoint definition table (zero
 *                            extended, if needed, in an uint32)
 *
 *  \return Comparison result
 *  \retval #LC_WATCH_TRUE  \copydoc LC_WATCH_TRUE
 *  \retval #LC_WATCH_FALSE \copydoc LC_WATCH_FALSE
 *  \retval #LC_WATCH_ERROR \copydoc LC_WATCH_ERROR
 */
uint8 LC_UnsignedCompare(uint16 WatchIndex, uint32 WPValue, uint32 CompareValue);

/**
 * \brief Float comparison
 *
 *  \par Description
 *       Support function for watchpoint processing that will perform
 *       a floating point watchpoint data comparison based upon the operator
 *       specified in the watchpoint definition table
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in] WatchIndex        The watchpoint number to compare (zero
 *                                based watchpoint definition table index)
 *
 *  \param [in] WPMultiType       The watchpoint data extracted from
 *                                the message that it was contained
 *                                in. Stored in a multi-type union.
 *                                This is the data after any sizing,
 *                                bit-masking, and endianess fixing
 *                                that LC might have done according
 *                                to the watchpoint definition
 *
 *  \param [in] CompareMultiType  The comparison value specified in the
 *                                watchpoint definition table. Stored
 *                                in a muti-type union so it can easily
 *                                be accessed as a uint32 for validity
 *                                checks
 *
 *  \return Comparison result
 *  \retval #LC_WATCH_TRUE  \copydoc LC_WATCH_TRUE
 *  \retval #LC_WATCH_FALSE \copydoc LC_WATCH_FALSE
 *  \retval #LC_WATCH_ERROR \copydoc LC_WATCH_ERROR
 */
uint8 LC_FloatCompare(uint16 WatchIndex, LC_MultiType_t *WPMultiType, LC_MultiType_t *CompareMultiType);

/**
 * \brief Watchpoint offset valid
 *
 *  \par Description
 *       Support function for watchpoint processing that will check if
 *       the watchpoint offset specified in the definition table would
 *       extend past the message that contains the watchpoint data
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]   WatchIndex  The watchpoint number to check (zero
 *                            based watchpoint definition table index)
 *
 *  \param [in]   BufPtr      Pointer to Software Bus buffer
 *
 *  \return Offset validation result
 *  \retval true Offset is within the message size
 *  \retval false Offset extends past message end
 */
bool LC_WPOffsetValid(uint16 WatchIndex, const CFE_SB_Buffer_t *BufPtr);

/**
 * \brief Get sized data
 *
 *  \par Description
 *       Support function for watchpoint processing that will extract
 *       the watchpoint data from a software bus message based upon the
 *       data type specified in the watchpoint definition table and
 *       store it in a uint32. If there are any endian differences between
 *       LC and the watchpoint data, this is where it will get fixed up.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]  WatchIndex     The watchpoint number to extract (zero
 *                              based watchpoint definition table index)
 *
 *  \param [in]  WPDataPtr      A pointer to the first byte of the
 *                              watchpoint data as it exists in the
 *                              software bus message it was received in
 *
 *  \param [in]  SizedDataPtr   A pointer to where the extracted watchpoint
 *                              data should be stored
 *
 *  \return Boolean execution status result
 *  \retval true No error
 *  \retval false An error occurred
 */
bool LC_GetSizedWPData(uint16 WatchIndex, const uint8 *WPDataPtr, uint32 *SizedDataPtr);

/**
 * \brief Convert messageID into hash table index
 *
 *  \par Description
 *       Utility function for watchpoint processing that converts a
 *       messageID into an index into the watchpoint hash table.
 *
 *       The following code supports use of the watchpoint hash table:
 *
 *       1) #LC_GetHashTableIndex - convert messageID to hash table index
 *       2) #LC_CreateHashTable   - after load Watchpoint Definition Table
 *       3) #LC_AddWatchpoint     - add one watchpoint to hash table
 *       4) #LC_CheckMsgForWPs    - process all WP's that reference messageID
 *
 *       The following data structures support the hash table:
 *
 *       1) Hash table (256 entries)
 *       2) Array of links for messageID linked lists (LC_MAX_WATCHPOINTS)
 *       3) Array of links for watchpoint linked lists (LC_MAX_WATCHPOINTS)
 *
 *       Rather than search the entire Watchpoint Definition Table to find
 *       the watchpoints that reference a particular messageID, LC does
 *       the following:
 *
 *       1) Call hash table function (convert messageID to hash table index)
 *       2) Get messageID linked list from indexed hash table entry
 *       3) Search messageID list (max 8) for matching messageID
 *       4) Get watchpoint linked list from matching messageID link
 *       5) Done - only watchpoints that reference messageID are in list
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]  MessageID   SoftwareBus packet message ID
 *
 *  \return Index into watchpoint hash table
 */
uint32 LC_GetHashTableIndex(CFE_SB_MsgId_t MessageID);

/**
 * \brief Add one watchpoint linked list entry during creation of hash table
 *
 *  \par Description
 *       Utility function that adds another link to the watchpoint linked list
 *       for the specified messageID. The function will also add a messageID
 *       linked list entry to the hash table if this is the first reference
 *       to that messageID. The function will also subscribe to the messageID
 *       if this is the first reference to that messageID. The function will
 *       return a pointer to the watchpoint linked list entry just added.
 *
 *       The following code supports use of the watchpoint hash table:
 *
 *       1) #LC_GetHashTableIndex - convert messageID to hash table index
 *       2) #LC_CreateHashTable   - after load Watchpoint Definition Table
 *       3) #LC_AddWatchpoint     - add one watchpoint to hash table
 *       4) #LC_CheckMsgForWPs    - process all WP's that reference messageID
 *
 *       The following data structures support the hash table:
 *
 *       1) Hash table (256 entries)
 *       2) Array of links for messageID linked lists (LC_MAX_WATCHPOINTS)
 *       3) Array of links for watchpoint linked lists (LC_MAX_WATCHPOINTS)
 *
 *       Rather than search the entire Watchpoint Definition Table to find
 *       the watchpoints that reference a particular messageID, LC does
 *       the following:
 *
 *       1) Call hash table function (convert messageID to hash table index)
 *       2) Get messageID linked list from indexed hash table entry
 *       3) Search messageID list (max 8) for matching messageID
 *       4) Get watchpoint linked list from matching messageID link
 *       5) Done - only watchpoints that reference messageID are in list
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]  MessageID   SoftwareBus packet message ID
 *
 *  \return Pointer to the watchpoint linked list entry just added
 */
LC_WatchPtList_t *LC_AddWatchpoint(const CFE_SB_MsgId_t MessageID);

#endif
