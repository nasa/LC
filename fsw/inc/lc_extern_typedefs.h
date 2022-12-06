/************************************************************************
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 *
 * Copyright (c) 2019 United States Government as represented by the
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
 *
 * Declarations and prototypes for fm_extern_typedefs module
 */

#ifndef LC_EXTERN_TYPEDEFS_H
#define LC_EXTERN_TYPEDEFS_H

#include "cfe.h"

/**
 * \name LC Table Name Strings
 * \{
 */
#define LC_WDT_TABLENAME "LC_WDT"
#define LC_ADT_TABLENAME "LC_ADT"
#define LC_WRT_TABLENAME "LC_WRT"
#define LC_ART_TABLENAME "LC_ART"
/**\}*/

/**
 *  \brief Actionpoint Definition Table (ADT) Entry
 */
typedef struct
{
    uint8 DefaultState;       /**< \brief Default state for this AP (enumerated)
                                          States are defined in lc_msgdefs.h        */
    uint8 MaxPassiveEvents;   /**< \brief Max number of events before filter
                                          - RTS not started because AP is passive   */
    uint8 MaxPassFailEvents;  /**< \brief Max number of events before filter
                                          - AP result transition from pass to fail  */
    uint8 MaxFailPassEvents;  /**< \brief Max number of events before filter
                                          - AP result transition from fail to pass  */
    uint16 RTSId;             /**< \brief RTS to request if this AP fails           */
    uint16 MaxFailsBeforeRTS; /**< \brief How may consecutive failures before
                                          an RTS request is issued                  */

    uint16 RPNEquation[LC_MAX_RPN_EQU_SIZE]; /**< \brief Reverse Polish Equation that
                                                         specifies when this actionpoint
                                                         should fail                     */

    uint16 EventType; /**< \brief Event type used for event msg if AP fails:
                                  #CFE_EVS_EventType_INFORMATION,
                                  #CFE_EVS_EventType_DEBUG,
                                  #CFE_EVS_EventType_ERROR,
                                  or #CFE_EVS_EventType_CRITICAL      */

    uint16 EventID; /**< \brief Event ID used for event msg if AP fails
                                See lc_events.h for those already in use  */

    char EventText[LC_MAX_ACTION_TEXT]; /**< \brief Text used for the event msg when
                                                    this AP fails                   */
} LC_ADTEntry_t;

/**
 * \brief Alignment union
 *
 * A union type provides a way to have many different data types occupy
 * the same memory and legally alias each other.
 *
 * This is used to store the watch data points, as they may be 8, 16, or 32
 * bits and this is defined in the table / not known until runtime.
 */
typedef union
{
    uint32 Unsigned32;
    int32  Signed32;
    float  Float32;
    uint16 Unsigned16;
    int16  Signed16;
    uint8  Unsigned8;
    int8   Signed8;
    uint8  RawByte[4];
} LC_MultiType_t;

/**
 *  \brief Watchpoint Definition Table (WDT) Entry
 */
typedef struct
{
    uint8          DataType;        /**< \brief Watchpoint Data Type (enumerated)     */
    uint8          OperatorID;      /**< \brief Comparison type (enumerated)          */
    CFE_SB_MsgId_t MessageID;       /**< \brief Message ID for the message containing
                                                the watchpoint                        */
    uint32 WatchpointOffset;        /**< \brief Byte offset from the beginning of
                                                the message (including any headers)
                                                to the watchpoint                     */
    uint32 BitMask;                 /**< \brief Value to be masked with watchpoint
                                                data prior to comparison              */
    LC_MultiType_t ComparisonValue; /**< \brief Value watchpoint data is compared
                                                against                               */
    uint32 ResultAgeWhenStale;      /**< \brief Number of LC Sample Actionpoint
                                                commands that must be processed after
                                                comparison before result goes stale   */
    uint32 CustomFuncArgument;      /**< \brief Data passed to the custom function
                                                when Operator_ID is set to
                                                #LC_OPER_CUSTOM                       */
} LC_WDTEntry_t;

#endif /* LC_EXTERN_TYPEDEFS_H */